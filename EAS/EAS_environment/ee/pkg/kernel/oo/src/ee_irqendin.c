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
 * CVS: $Id: ee_irqendin.c,v 1.2 2006/01/24 10:21:14 pj Exp $
 */

#include "ee_internal.h"

#if defined(__OO_ISR2_RESOURCES__) || defined(EE_AS_USER_SPINLOCKS__)
/* Index used to give ISR2 Temporary TID value and to access at
   EE_isr2_nesting_level array */
EE_UREG EE_isr2_index = EE_UREG_MINUS1;

/* Assign a fake TID to an ISR2 to eventually handle resources clean-up */
EE_TID EE_oo_assign_TID_to_ISR2( void ) {
  EE_UREG const actual_nesting = EE_hal_get_IRQ_nesting_level();

  if ( EE_isr2_index == EE_UREG_MINUS1 ) {
    EE_isr2_index = 0U;
    EE_isr2_nesting_level[0] = actual_nesting;
  } else if ( EE_isr2_nesting_level[EE_isr2_index] < actual_nesting ) {
    ++EE_isr2_index;
    EE_isr2_nesting_level[EE_isr2_index] = actual_nesting;
  } else {
    /* Nothing to do added just for MISRA 2004 Required Rule 14.10 */
  }

  return (EE_MAX_TASK + (EE_TID)EE_isr2_index);
}

/* [OS369]: If a Category 2 OsIsr calls GetResource() and ends (returns)
    without calling the corresponding ReleaseResource(), the Operating System
    shall perform the ReleaseResource() call and shall call the ErrorHook()
    (if configured) with the status E_OS_RESOURCE. */
static void EE_IRQ_release_all_items( void ) {
  /* Check if the index is valid -> at least one ISR2 got a resource */
  if ( EE_isr2_index != EE_UREG_MINUS1 ) {
    /* N.B This method MUST be called at the end of IRQ post-stub but BEFORE
       decrementing nesting. */
    EE_UREG const actual_nesting = EE_hal_get_IRQ_nesting_level();

    /* Check if this is the right level where do clean-up */
    if ( EE_isr2_nesting_level[EE_isr2_index] == actual_nesting ) {
      EE_TID current_isr2_tid = (EE_TID)EE_isr2_index + EE_MAX_TASK;
#ifdef __OO_ISR2_RESOURCES__
      /* OS369 */
      if ( EE_oo_release_all_resources(current_isr2_tid) != EE_UREG_MINUS1 ) {
        EE_ORTI_set_lasterror(E_OS_RESOURCE);
        EE_oo_notify_error_service(OSId_ISR2Body, E_OS_RESOURCE);
      }
#endif /* __OO_ISR2_RESOURCES__ */
#ifdef EE_AS_USER_SPINLOCKS__
      /* Force Spinlock Release */
      if ( EE_as_release_all_spinlocks(current_isr2_tid) !=
        INVALID_SPINLOCK )
      {
        EE_ORTI_set_lasterror(E_OS_SPINLOCK);
        EE_oo_notify_error_service(OSId_ISR2Body, E_OS_SPINLOCK);
      }
#endif /* EE_AS_USER_SPINLOCKS__ */

      /* Decrement ISR2 index (from 0U to EE_UREG_MINUS1 is handled by
         unsigned wraparound) */
      --EE_isr2_index;
    }
  }
}
#else /* __OO_ISR2_RESOURCES__ || EE_AS_USER_SPINLOCKS__ */
#define EE_IRQ_release_all_items()    ((void)0)
#endif /* __OO_ISR2_RESOURCES__ || EE_AS_USER_SPINLOCKS__ */

#ifndef __PRIVATE_IRQ_END_INSTANCE__

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
static void EE_IRQ_run_next_task( void )
{
  register EE_TID next;
  next = EE_rq2stk_exchange();
  if ( EE_th_waswaiting[next] ) {
    EE_th_waswaiting[next] = 0U;
    EE_oo_call_PreTaskHook();
    EE_hal_IRQ_stacked(next);
  } else {
    EE_hal_IRQ_ready(next);
  }
}
#else /* __OO_ECC1__ || __OO_ECC2__ */
static void EE_IRQ_run_next_task( void )
{
  EE_hal_IRQ_ready(EE_rq2stk_exchange());
}
#endif /* __OO_ECC1__ || __OO_ECC2__ */

/* This primitive shall be atomic.
   This primitive shall be inserted as the last function in an IRQ handler.
   If the HAL allow IRQ nesting the C_end_instance should work as follows:
   - it must implement the preemption test only if it is the last IRQ on the
     stack
   - if there are other interrupts on the stack the IRQ end_instance should do
     nothing
*/
void EE_IRQ_end_instance( void )
{
  register EE_TID rqfirst, current;

  rqfirst = EE_rq_queryfirst();
  current = EE_stk_queryfirst();

  if ( (rqfirst != EE_NIL) && (EE_sys_ceiling < EE_th_ready_prio[rqfirst]) ) {
    /* We have to schedule a READY thread */

    if ( current != EE_NIL ) {
      /* There is a post task hook only if a TASK that was RUNNING */
      EE_oo_call_PostTaskHook();
      /* The RUNNING TASK is now preempted-> READY status + still in stacked
         queue */
      EE_th_status[current] = READY;
    }

    /* Another TASK is put into the RUNNING state */
    EE_th_status[rqfirst] = RUNNING;

    EE_sys_ceiling |= EE_th_dispatch_prio[rqfirst];

    EE_ORTI_set_th_eq_dispatch_prio(rqfirst);

    /* "Press TP start for the first time" for this new activation or realese
        from wait of the TASK */
    EE_as_tp_active_start_on_TASK_stacking(rqfirst);
    /* Set the next running TASK, it doesn't directly RUN it */
    EE_IRQ_run_next_task();
  } else {
    if ( current == EE_NIL ) {
      /* We are switching back to the Idle loop */
      EE_as_set_execution_context( Idle_Context );
      EE_as_tp_active_start_idle();
    } else {
      /* We are switching back to a preempted TASK */
      EE_as_set_execution_context( TASK_Context );
      /* Enable the Task Timing Protection Set */
      EE_as_tp_active_set_from_TASK(current);
    }
    /* Set the Context Switch up, it doesn't directly switch */
    EE_hal_IRQ_stacked(current);
  }
}

/* This primitive shall be atomic.
   This primitive shall be inserted as the last function in an IRQ post-stub.
   This primitive do needed clean-up as resetting kernel interrupt nested
   counter and release gotten resources if application forget to do that as
   specified by Autosar standard.
*/
void EE_IRQ_end_post_stub( void ) {
  /* [OS368]: If a Category 2 OsIsr calls DisableAllInterupts()/
      SuspendAllInterrupts()/SuspendOSInterrupts() and ends (returns)
      without calling the corresponding EnableAllInterrupts()/
      ResumeAllInterrupts() / ResumeOSInterrupts(),
      the Operating System shall perform the missing service and shall call
      the ErrorHook() (if configured) with the status E_OS_DISABLEDINT. */

  /* Only check and reset ISR flags an counters because ISR re-enabling is done
     in CPU layer */
  if ( EE_oo_check_disableint_error() )
  {
    EE_ORTI_set_lasterror(E_OS_DISABLEDINT);
    EE_oo_notify_error_service(OSId_ISR2Body, E_OS_DISABLEDINT);
    /* Reset ISRs counter */
    EE_oo_IRQ_disable_count = 0U;
  }

  /* [SWS_Os_00474] The Operating System module shall reset an ISR’s
      OsIsrExecutionBudget when the ISR returns control to the OS or terminates.
      (SRS_Os_11008) */
  /* Reset ALL TP Budgets, safest approach */
  EE_as_tp_active_reset_budgets();

  /* [OS369]: If a Category 2 OsIsr calls GetResource() and ends (returns)
      without calling the corresponding ReleaseResource(), the Operating System
      shall perform the ReleaseResource() call and shall call the ErrorHook()
      (if configured) with the status E_OS_RESOURCE. */
  /* [OS109]: If the Operating System module forcibly terminates an interrupt
      service routine, it clears the interrupt request, aborts the interrupt
      service routine (The interrupt source stays in the current state.)
      and releases all OSEK resources the interrupt service routine has
      allocated and calls EnableAllInterrupts() / ResumeOSInterrupts() /
      ResumeAllInterrupts() if the interrupt called DisableAllInterrupts() /
      SuspendOSInterrupts() / SuspendAllInterrupts() before without the
      corresponding EnableAllInterrupts() / ResumeOSInterrupts() /
      ResumeAllInterrupts() call. */
  EE_IRQ_release_all_items();
}
#endif /* __PRIVATE_IRQ_END_INSTANCE__ */

