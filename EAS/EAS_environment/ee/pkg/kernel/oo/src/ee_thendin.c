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
 * Author: 2002-2004 Paolo Gai
 * CVS: $Id: ee_thendin.c,v 1.3 2006/01/24 10:20:20 pj Exp $
 */

#include "ee_internal.h"

#ifndef __PRIVATE_THREAD_END_INSTANCE__

#if defined(__OO_BCC2__) || defined(__OO_ECC2__)
static EE_BIT EE_thread_rnact_max(EE_TID current) {
  return (EE_th_rnact[current] == EE_th_rnact_max[current]) ? 1U : 0U;
}
#else /* __OO_BCC2__ || __OO_ECC2__ */
#define EE_thread_rnact_max(current)  (1U)
#endif /* __OO_BCC2__ || __OO_ECC2__ */

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
static void EE_thread_endcycle_next(void)
{
  register EE_TID next;
  next = EE_rq2stk_exchange();
  if (EE_th_waswaiting[next]) {
    EE_th_waswaiting[next] = 0U;
    EE_oo_call_PreTaskHook();
    EE_hal_endcycle_stacked(next);
  } else {
    EE_hal_endcycle_ready(next);
  }
  /* Remember: after hal_endcycle_XXX there MUST be NOTHING!!! */
}
#else /* __OO_ECC1__ || __OO_ECC2__ */
static void EE_thread_endcycle_next(void)
{
  EE_hal_endcycle_ready(EE_rq2stk_exchange());
  /* Remember: after hal_endcycle_XXX there MUST be NOTHING!!! */
}
#endif /* __OO_ECC1__ || __OO_ECC2__ */

void EE_thread_end_instance(void)
{
  EE_TID current, rqfirst;
  EE_TID TaskID;

  current = EE_stk_queryfirst();

  EE_oo_call_PostTaskHook();

  /* [SWS_Os_00473] The Operating System module shall reset a task’s
      OsTaskExecutionBudget on a transition to the SUSPENDED or WAITING states.
      (SRS_Os_11008) */
  /* Reset ALL TP Budgets, just in case */
  EE_as_tp_active_reset_budgets();

  /* Reset ISRs counters */
  EE_oo_IRQ_disable_count = 0U;

  /* Increase the remaining activations...*/
  EE_th_rnact[current]++;

#ifndef __OO_NO_CHAINTASK__
  /* If we called a ChainTask, 
     EE_th_terminate_nextask[current] != NIL */
  TaskID = EE_th_terminate_nextask[current];
#else /* __OO_NO_CHAINTASK__ */
  TaskID = EE_NIL;
#endif  /* __OO_NO_CHAINTASK__ */

  /* The task state switch from STACKED TO READY because it end its
   * instance. Note that status=READY and
   * rnact==maximum number of pending activations ==>> the task is
   * SUSPENDED!!! */
#if defined(__OO_BCC2__) || defined(__OO_ECC2__)
  if( (1U == EE_thread_rnact_max(current)) || (current == TaskID) ) {
    EE_th_status[current] = SUSPENDED;
  } else {   
    EE_th_status[current] = READY;
  }
#else /* __OO_BCC2__ || __OO_ECC2__ */
  EE_th_status[current] = SUSPENDED;
#endif /* __OO_BCC2__ || __OO_ECC2__ */

  /* Reset the thread priority bit in the system_ceiling */
  EE_sys_ceiling &= ~EE_th_dispatch_prio[current];

  EE_ORTI_set_th_priority(current, 0U);

  /* Extract the TASK from the stk queue and return the new head */
  current = EE_stk_getfirst();

#ifndef __OO_NO_CHAINTASK__
  /* If we called a ChainTask, 
     EE_th_terminate_nextask[current] != NIL */
  if ( TaskID != EE_NIL ) {

    /* See also activate.c
       Put the task in the ready state:
       - if the task is basic/BCC1 or extended it can be here only because
         it had rnact=1 before the call, and so it is in suspended state
       - if the task is basic/BCC2 it can be that it is ready or 
         running. In that case we have to check and queue it anyway */
    EE_oo_set_th_status_ready(TaskID);

    /* insert the task in the ready queue */
    EE_rq_insert(TaskID);
  }
#endif /* __OO_NO_CHAINTASK__ */

  /* Check if there is to schedule a ready thread or pop a preempted thread */
  rqfirst = EE_rq_queryfirst();
  if ( rqfirst == EE_NIL ) {
    /* No threads in the ready queue, return to the preempted TASK
      (maybe Idle) */
    if ( current != EE_NIL ) {
      EE_th_status[current] = RUNNING;
      /* The call the PreTaskHook is done inside EE_oo_preemption_point */
      /* EE_oo_call_PreTaskHook(); */
      /* Enable the TASK Timing Protection Set */
      EE_as_tp_active_set_from_TASK(current);
    } else {
      /* We are switching back to the Idle loop */
      EE_as_set_execution_context( Idle_Context );
      EE_as_tp_active_start_idle();
    }
    EE_hal_endcycle_stacked(current);
  } else if ( EE_sys_ceiling >= EE_th_ready_prio[rqfirst] ) {
    /* We have to schedule an interrupted thread (already on the stack!!!) */
    EE_th_status[current] = RUNNING;
    EE_oo_call_PreTaskHook();

    /* Enable the TASK Timing Protection Set */
    EE_as_tp_active_set_from_TASK(current);
    /* Prepare to Context SWITCH, It doesn't do the switch by it self. */
    EE_hal_endcycle_stacked(current);
  } else {
    /* We have to schedule a ready thread */
    EE_th_status[rqfirst] = RUNNING;
    EE_sys_ceiling |= EE_th_dispatch_prio[rqfirst];

    EE_ORTI_set_th_eq_dispatch_prio(rqfirst);

    /* "Press TP start for the first time" for this new activation or release
        from wait of the TASK */
    EE_as_tp_active_start_on_TASK_stacking(rqfirst);

    /* Prepare to Context SWITCH, It doesn't do the switch by it self. */
    EE_thread_endcycle_next();
  }
  /* Remember: after hal_endcycle_XXX there MUST be NOTHING!!! */
}
#endif /* __PRIVATE_THREAD_END_INSTANCE__ */
