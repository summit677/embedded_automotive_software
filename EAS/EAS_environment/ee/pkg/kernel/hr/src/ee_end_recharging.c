/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2013  Evidence Srl
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

#include "ee_internal.h"

#ifndef __PRIVATE_IRQ_RECHARGING__
/*
 * This routine is called when the recharging time of a task is finished
 * It basically takes the time, and check all the recharging times which has expired
 *
 * Please note that when a server is inserted into the recharging queue
 * its budget is always greter than the minimum budget, and for this
 * reason the server (and then its served tasks) can be inserted into the ready queue without
 * problems.
 */

void EE_hr_IRQ_recharging(void)
{
  register EE_TIME tmp_time;
  register EE_FREG flag;
  register EE_TYPECONTRACT c;
  
  flag = EE_hal_begin_nested_primitive();

  /* read the current time */
  tmp_time = EE_hal_gettime();

  // Check for negative budget and in case re-insert in the rcg queue
  c = EE_rcg_queryfirst();
  while (c != EE_VRES_NIL) {
    if ((EE_STIME)(EE_vres[c].absdline - tmp_time) <= EE_TIMER_MINCAPACITY) {
      /* remove the task from the recharging queue */
      EE_rcg_getfirst();

      /* update the absolute deadline by summing the period.  doing
	 absdeadline = tmp_time+period IS WRONG, because in that way
	 we have a drift with respect to the recharging implemented
	 into EE_hr_check_recharging. 

	 Note that this function is different from
	 EE_hr_check_recharging, and they cannot be the -same-
	 function, or we cannot empty this function, because the
	 EE_hr_check_recharging only works when ready and stacked
	 queues are empty, whereas this function works in any case.*/
      EE_vres[c].absdline += EE_ct[c].period;

      /* we process the task pointed by the VRES */
      EE_hr_process_recharging(c);
    } else {
      /* the tasks are ordered by deadline. if one fails, the others are for sure in the future */
      break;
    }

    c = EE_rcg_queryfirst();
  }

  /* c points to the top of the recharging queue */

  // Program the recharging timer
  if (c == EE_VRES_NIL) {
      EE_hr_stop_recharging_timer();
  } else {
      EE_hr_set_recharging_timer(EE_vres[c].absdline - tmp_time);
  }

  EE_hal_end_nested_primitive(flag);
}

#endif
