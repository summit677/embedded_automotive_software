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
frsh_thread_unbind()
This operation unbinds a thread from a vres. Since threads with no vres associated are not allowed to
execute, they remain in a dormant state until they are either eliminated or bound again.

If the thread is inside a critical section the effects of this call are deferred until the critical section is ended

Returns:
0 if successful
FRSH_ERR_BAD_ARGUMENT : if the given thread does not exist
FRSH_ERR_NOT_BOUND : if the given thread does not have a valid vres bound to it
*/
#ifndef __PRIVATE_UNBINDTASK__
int EE_frsh_UnbindTask(const frsh_thread_id_t thread)
{
  register EE_FREG flag;
  register EE_TIME tmp_time;
  register EE_TID tmp_exec;

  /* consistency check on the parameters. these checks does not require interrupt disabling */
  if (thread<0 || thread >= EE_MAX_TASK)
    return FRSH_ERR_BAD_ARGUMENT;

#ifdef __RN_UNBIND__
  if (thread & EE_REMOTE_TID) {
    /* forward the request to another CPU */
    if (EE_rn_send(thread & ~EE_REMOTE_TID, EE_RN_UNBIND, (EE_TYPERN_PARAM)(EE_UREG)0 )) {
      /* a bind or unbind operation is currently pending; maybe we should use a custom return value */
      return FRSH_ERR_NOT_BOUND;
    } else {
      return FRSH_NO_ERROR;
    }
  }
#endif

  flag = EE_hal_begin_nested_primitive();

  if (EE_th[thread].vres == EE_VRES_NIL) {
    EE_hal_end_nested_primitive(flag);
    return FRSH_ERR_NOT_BOUND;
  }

  if (EE_th[thread].vres_deferred != EE_VRES_NIL) {
    EE_hal_end_nested_primitive(flag);
    return FRSH_ERR_NOT_BOUND;
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

  if (EE_frsh_bind_detach_thread(thread)) {
    /* The VRES has been detached
       - as a result of detach_vres the task is not inserted in the ready queue
    
       at this point, 

       the task needs to be fixed.
    */

    /* detach the VRES */
    EE_th[thread].vres = EE_VRES_NIL;

    /*
       The task is in a given status. 
       Remember that the task is NOT inserted in the ready queue.
       If it was in the stacked queue, it remained there.
       Depending on the status we have to do some actions among the following (thread status
       uppercase, vres status lowercase):

       SUSPENDED
         do nothing
       READY
         the task has already been removed from the ready queue. do nothing
       STACKED
         the unbind is deferred later, see the else part
       BLOCKED
         the task is not in the ready queue. do nothing.
    */
  } else {
    /* The VRES has NOT been detached. Store in the detached variable
       that the task has to be attached to the new VRES */
    EE_th[thread].vres_deferred = EE_VRES_UNBOUND;
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
