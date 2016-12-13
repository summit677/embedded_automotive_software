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
 * Author: 2008 Paolo Tiberi & Francesco Focacci
 * CVS: $Id: ee_thendin.c,v 1.7 2008/07/18 09:53:55 tiberipa Exp $
 */

#include "ee_internal.h"

#ifndef __PRIVATE_THREAD_END_INSTANCE__

int EE_hr_recharge(EE_TIME);

// this function MUST NOT BE CALLED BY THE USER!!!
void EE_thread_end_instance(void)
{
  register EE_TIME tmp_time;
  register int wasstacked;

  tmp_time = EE_hal_gettime();

  /* decrease the pending activations... ready or stacked => (nact>0) */
  EE_th[EE_exec].nact--;

  /* this should never happen ... */
#ifdef DEBUG
  if (EE_th[EE_exec].lockedcounter || EE_th[EE_exec].sys_lockedcounter)
    for(;;);
#endif

  /* end_slice: checks the elapsed time on the exec task, putting it into the right
     queue (recharging, ready, or simply put the task suspended). at the end EE_exec is EE_NIL */
  EE_hr_end_slice(tmp_time);
  /* --- */

 
  /* check if the queues are empty and if there is someone in the recharging queue 
   * to activate
   */
  EE_hr_check_recharging(tmp_time);
    /* --- */


  /* the exec task is now EE_NIL.
     the next task to execute is either in the stacked or in the ready queue

     we have now to choose between the tasks pointed by stkfirst and
     by rqfirst 
  */
  EE_hr_select_exec();
  /* --- */


  if (EE_exec == EE_NIL) {
    /* no task to schedule, go to the main */
    EE_hr_stop_budget_timer();
    EE_hal_endcycle_stacked(EE_exec);
  } else {
    /* there is a task to schedule */
    wasstacked = EE_th[EE_exec].status & EE_TASK_WASSTACKED;
    EE_th[EE_exec].status = EE_TASK_EXEC;  
  
    /* reprogram the capacity timer for the new task */
    EE_hr_set_budget_timer(EE_vres[EE_th[EE_exec].vres].budget_avail);
    
    if (wasstacked)
      EE_hal_endcycle_stacked(EE_exec);
    else
      EE_hal_endcycle_ready(EE_exec);
  }
}
#endif
