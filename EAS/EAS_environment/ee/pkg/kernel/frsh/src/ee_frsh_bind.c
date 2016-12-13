/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2008  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation, 
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

/*
 * Author: 2009 Paolo Gai
 * CVS: $Id: ee_cap.c,v 1.6 2008/07/18 09:53:55 tiberipa Exp $
 */

#include "ee_internal.h"
#include "frsh_core_types.h"
#include "frsh_error.h"

/*
frsh_thread_bind()

This operation associates a thread with a vres, which means that it starts consuming the vres's budget and
is executed according to the contract established for that vres. If the thread is already bound to another
vres, it is effectively unbound from it and bound to the specified one.

It fails if the vres's policy is different than FRSH_NONE, or if there is already a thread bound to this vres.

Returns:
- 0 if successful
- FRSH_ERR_BAD_ARGUMENT : if the vres value does not complain with the
  expected format or valid range or the given thread does not exist
- FRSH_ERR_NOT_CONTRACTED_VRES : if the referenced vres is not valid
- FRSH_ERR_ALREADY_BOUND : if the given vres has a thread already bound
*/
#ifndef __PRIVATE_BINDTASK__
int EE_frsh_BindTask(const frsh_vres_id_t vres, const frsh_thread_id_t thread)
{
  register EE_FREG flag;
  register EE_TIME tmp_time;
  register EE_TID tmp_exec;

  /* consistency check on the parameters. these checks does not require interrupt disabling */
  if (vres<0 || vres >= EE_MAX_CONTRACT)
    return FRSH_ERR_NOT_CONTRACTED_VRES;

  if (thread<0 || thread >= EE_MAX_TASK)
    return FRSH_ERR_BAD_ARGUMENT;

#ifdef __RN_BIND__
  if (thread & EE_REMOTE_TID) {
    register EE_TYPERN_PARAM par;
    par.vres = vres;
    /* forward the request to another CPU */
    if (EE_rn_send(thread & ~EE_REMOTE_TID, EE_RN_BIND, par )) {
      /* a bind or unbind operation is currently pending; maybe we should use a custom return value */
      return FRSH_ERR_ALREADY_BOUND;
    } else {
      return FRSH_NO_ERROR;
    }
  }
#endif

  flag = EE_hal_begin_nested_primitive();

  if (EE_vres[vres].task != EE_NIL) {
    EE_hal_end_nested_primitive(flag);
    return FRSH_ERR_ALREADY_BOUND;
  }

  /* this part is very similar to ActivateTask */

  tmp_time = EE_hal_gettime();

  /* save the current running task into a temporary variable */
  tmp_exec = EE_exec;
  /* --- */
  
  /* check_slice: checks the elapsed time on the exec task, putting it into the right
     queue (recharging or ready). at the end EE_exec is EE_NIL */
  EE_frsh_check_slice(tmp_time);
  /* --- */

  /* implement the bind behavior */

  /* detach the current VRES, and depending on the result do the thread handling */
  /* works also if the thread has not a VRES attached (that is EE_VRES_NIL) */
  if (EE_frsh_bind_detach_thread(thread)) {
    /* The VRES has been detached
       - as a result of detach_vres the task is not inserted in the ready queue
       - we can give the task the new VRES now*/
    EE_vres[vres].task = thread;
    EE_th[thread].vres = vres;

    /* at this point, 

       the new VRES is linked to the task, but the task needs to be fixed.

       The new VRES can be in one of the following statuses
       FREEZED if it has never binded to anyone
       FREEZED, INACTIVE or RECHARGING after a bind/unbind

       (
       in fact, bind / unbind leave a VRES in one of the following statuses

       before bind/unbind       after
       freezed                  no change
       inactive                 no change
       active  ---->>>>         inactive
       recharging               no change

       That is, the new VRES can be either freezed, inactive, or recharging
       )

       The task is in a given status. 
       Remember that the task is NOT inserted in the ready queue.
       If it was in the stacked queue, it remained there.
       Depending on the status we have to do some actions among the following (thread status
       uppercase, vres status lowercase):

       SUSPENDED
         freezed    --> nothing
	 inactive   --> nothing
	 recharging --> nothing. at the recharging time process_recharging will put the vres in inactive
       READY
         freezed    --> updatecapacity, eventually insert into the ready queue
	 inactive   --> updatecapacity, eventually insert into the ready queue
	 recharging --> nothing, it will be inserted in the ready queue when the recharging time will happen
	                we ALWAYS CALL in any case updatecapacity which has a test on the recharging state.
       STACKED
         IT IS NOT POSSIBLE that we are here and the atsk is STARCKED.
	 if it is tacked, in fact, the bind is deferred.

       BLOCKED
         freezed    --> nothing
	 inactive   --> nothing
	 recharging --> nothing. at the recharging time process_recharging will put the vres in inactive
    */

    if (EE_th[thread].status & EE_TASK_READY) {
      if(EE_frsh_updatecapacity(thread, tmp_time) == EE_UC_InsertRDQueue) {
	EE_rq_insert(thread);
      }
    }

  } else {
    /* The VRES has NOT been detached. Store in the detached variable
       that the task has to be attached to the new VRES */
    EE_th[thread].vres_deferred = vres;

    /* this value will make the test after begin_nested_primitive fail! */
    EE_vres[vres].task = EE_TID_DEFERRED;
  }
  
  /* --- */

  
  /* check_recharging: if ready and stacked queue are empty pulls from the recharging queue */
  EE_frsh_check_recharging(tmp_time);
  /* --- */
  
  /* at this point, exec is for sure EE_NIL (it is set by check_slice) */
  /* select the first task from the ready or stacked queue */
  /* the function set the EE_exec value, removing the task from the queue
     the status is untouched */
  
  EE_frsh_select_exec();
  /* --- */
  
  EE_frsh_run_exec(tmp_exec);
  
  EE_hal_end_nested_primitive(flag);

  return FRSH_NO_ERROR;
}
#endif
