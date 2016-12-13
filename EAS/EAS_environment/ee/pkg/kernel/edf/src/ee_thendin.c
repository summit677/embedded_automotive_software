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
 * Author: 2001 Paolo Gai
 * CVS: $Id: ee_thendin.c,v 1.2 2007/06/14 10:27:12 pj Exp $
 */

#include "ee_internal.h"

#ifndef __PRIVATE_THREAD_END_INSTANCE__
void EE_thread_end_instance(void)
{
  EE_TID current;

  current = EE_stk_queryfirst();

  // decrease the pending activations... ready or stacked => (nact>0)
  EE_th_nact[current]--;

#if defined(__MULTI__) || defined(__WITH_STATUS__)
  /* The task state switch from STACKED TO READY because it end its
   * instance. Note that status=READY and nact=0 ==>> the task is
   * suspended!!! */
  EE_th_status[current] = EE_READY;
#endif

  // reset the task priority bit in the system_ceiling
  EE_sys_ceiling &= ~EE_th_dispatch_prio[current];

  // extract the task from the stk data structure
  EE_stk_getfirst();

  if (EE_th_nact[current] > 0) {
    // compute the deadline 
    EE_th_absdline[current] = EE_hal_gettime() + EE_th_reldline[current];

    // there are pending activations... 
    // we have to reinsert the task into the ready queue before rescheduling!!!
    EE_rq_insert(current);
  }

  // check if there is to schedule a ready task or pop a preempted task
  //th_absdline[stk_queryfirst()] <= th_absdline[rq_queryfirst()] 
  // see also irq_sched.c
  if (EE_rq_queryfirst() == EE_NIL ||  // note that this test work also for the main task!
      (EE_stk_queryfirst() != EE_NIL && 
        (
	 (EE_STIME)(EE_th_absdline[EE_stk_queryfirst()] - 
		    EE_th_absdline[EE_rq_queryfirst()]) <= 0
	 || EE_sys_ceiling >= EE_th_ready_prio[EE_rq_queryfirst()]
        ) 
      )
     )  {
    // we have to schedule an interrupted task (already on the stack!!!)
    // the state is already STACKED!
    EE_hal_endcycle_stacked(EE_stk_queryfirst());
    // never returns !!!
  }
  else {
    // we have to schedule a ready task
#if defined(__MULTI__)
    register int flag;
    
    flag = EE_th_status[EE_rq_queryfirst()] & EE_WASSTACKED;
#endif
    
#if defined(__MULTI__) || defined(__WITH_STATUS__)
    EE_th_status[EE_rq_queryfirst()] = EE_STACKED;
#endif

    EE_sys_ceiling |= EE_th_dispatch_prio[EE_rq_queryfirst()];

#if defined(__MULTI__)
    if (flag)
      EE_hal_endcycle_stacked(EE_rq2stk_exchange());
    else
      EE_hal_endcycle_ready(EE_rq2stk_exchange());
#else
    EE_hal_endcycle_ready(EE_rq2stk_exchange());
#endif
  }
}  
#endif
