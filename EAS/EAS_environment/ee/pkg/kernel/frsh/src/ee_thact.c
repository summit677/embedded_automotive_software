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
 * CVS: $Id: ee_thact.c,v 1.6 2008/07/18 09:53:55 tiberipa Exp $
 */

#include "ee_internal.h"




#ifndef __PRIVATE_THREAD_ACTIVATE__

void EE_frsh_thread_activate(EE_TID t)
{
  register EE_TID tmp_exec;
  register EE_TIME tmp_time;
  register EE_FREG flag;
  int rn_ret_val;

#ifdef __RN_TASK__
  if (t & EE_REMOTE_TID) {
    register EE_TYPERN_PARAM par;
    par.pending = 1;
    /* forward the request to another CPU */
    rn_ret_val = EE_rn_send(t & ~EE_REMOTE_TID, EE_RN_TASK, par );
    return;
  }
#endif
  
  flag = EE_hal_begin_nested_primitive();

  /* read the current time; this will be used later to compute task's
     deadlines */

  tmp_time = EE_hal_gettime();

  /* --- */

  /* We activate the task and we put it into the right queue
     we also set the scheduling parameters */

  if (EE_th[t].nact == 0) {
    /* since nact==0, the task has not been stacked before, and so it
       is safe to put it in the READY state */

    if (EE_frsh_updatecapacity(t, tmp_time) == EE_UC_InsertRDQueue){
      /* In this case, the budhet has been updated and the task is ready to be executed */
      EE_rq_insert(t);
    }
    /* otherwise, the task's VRES has been inserted in the recharging queue! */

    /* EE_frsh_updatecapacity updates the VRES status to either active or recharging */
    EE_th[t].status = EE_TASK_READY;
  }
  
  EE_th[t].nact++;
  /* --- */

  /* check if in an ISR context
   * if we are in an ISR context the slice and recharging will be done at the end of the interrupt
   */
  if (!EE_hal_get_IRQ_nesting_level()) { 

    /* save the current running task into a temporary variable */
    tmp_exec = EE_exec;
    /* --- */
    
    /* check_slice: checks the elapsed time on the exec task, putting it into the right
       queue (recharging or ready). at the end EE_exec is EE_NIL */
    EE_frsh_check_slice(tmp_time);
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
  }

  EE_hal_end_nested_primitive(flag);
}

#endif
