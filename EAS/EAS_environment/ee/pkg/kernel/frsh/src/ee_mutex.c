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
 * Author: 2008 Paolo Tiberi & Francesco Focacci, 2009 Paolo Gai
 * CVS: $Id: ee_mutex.c,v 1.6 2008/07/18 09:53:55 tiberipa Exp $
 */

#include "ee_internal.h"

#ifndef __PRIVATE_GETRESOURCE__
void EE_frsh_GetResource(EE_TYPERESOURCE m)
{
#ifdef __MSRP__
  register EE_TYPERESOURCE tmp;
#endif
  
  register EE_FREG flag;
  flag = EE_hal_begin_nested_primitive();
  
#ifdef __MSRP__
  /* mask off the MSB, that indicates whether this is a global or a
     local resource */
  tmp = m & ~EE_GLOBAL_MUTEX;
  
  EE_resource_oldceiling[tmp] = EE_sys_ceiling;
  EE_sys_ceiling |= EE_resource_ceiling[tmp];
  
  /* if this is a global resource, lock the others CPUs */
  if (m & EE_GLOBAL_MUTEX) EE_hal_spin_in(tmp);
#else
  EE_resource_oldceiling[m] = EE_sys_ceiling;
  EE_sys_ceiling |= EE_resource_ceiling[m];
#endif
  
  EE_th[EE_exec].lockedcounter++;
  
  EE_hal_end_nested_primitive(flag);
}
#endif /* __PRIVATE_MUTEX_LOCK__ */




#ifndef __PRIVATE_RELEASERESOURCE__
void EE_frsh_ReleaseResource(EE_TYPERESOURCE m)
{
  register EE_FREG flag;
  register EE_TID tmp_exec;
  register EE_TIME tmp_time;
#ifdef __MSRP__
  register EE_TYPERESOURCE tmp;
#endif

  flag = EE_hal_begin_nested_primitive();
  
#ifdef __MSRP__
  tmp = m & ~EE_GLOBAL_MUTEX;

  /* if this is a global resource, unlock the others CPUs */
  if (m & EE_GLOBAL_MUTEX) EE_hal_spin_out(tmp);
  
  EE_sys_ceiling = EE_resource_oldceiling[tmp];
#else
  EE_sys_ceiling = EE_resource_oldceiling[m];
#endif

  EE_th[EE_exec].lockedcounter--;

  /* this part is very similar to ActivateTask */

  tmp_time = EE_hal_gettime();

  /* save the current running task into a temporary variable */
  tmp_exec = EE_exec;
  /* --- */

  /* check_slice: checks the elapsed time on the exec task, putting it into the right
     queue (recharging or ready). at the end EE_exec is EE_NIL */
  EE_frsh_check_slice(tmp_time);
  /* --- */

  /* check for deferred unbinds */
  if (!EE_th[tmp_exec].lockedcounter) {
    register EE_TYPECONTRACT vres = EE_th[tmp_exec].vres_deferred;
    EE_th[tmp_exec].vres_deferred = EE_VRES_NIL;

    if (vres == EE_VRES_UNBOUND) {
      /* The task was unbound but when unbinding it it was into a
	 resource. We implement the same as an unbind. */
      EE_frsh_bind_detach_thread(tmp_exec);
      EE_th[tmp_exec].vres = EE_VRES_NIL;

    } else if (vres != EE_VRES_NIL) {
      /* the task has been bound to another VRES. unbind it from the current VRES, 
	 and bind it to the right VRES */
      EE_frsh_bind_detach_thread(tmp_exec);
      EE_vres[vres].task = tmp_exec;
      EE_th[tmp_exec].vres = vres;
      if (EE_th[tmp_exec].status & EE_TASK_READY) {
	if(EE_frsh_updatecapacity(tmp_exec, tmp_time) == EE_UC_InsertRDQueue) {
	  EE_rq_insert(tmp_exec);
	}
      }
    }
  }

  
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
}

#endif /* __PRIVATE_MUTEX_UNLOCK__ */
