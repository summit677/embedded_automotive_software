/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2011  Evidence Srl
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
 * Basic Autosar API, common to all SCs
 * Author: 2011, Bernardo  Dal Seno
 */

#include "ee_internal.h"

#ifdef EE_SERVICE_PROTECTION__
/* Used by the kernel to flag in witch context is executing */
/* Initialized to Idle so no API other than StartOS and Start(NonAutosar)Core
   can be called i main function */
EE_TYPECONTEXT EE_as_execution_context = Idle_Context;
#endif /* EE_SERVICE_PROTECTION__ */

#ifdef EE_AS_OSAPPLICATIONS__

/* Store the actual active OS-Application */
ApplicationType EE_as_active_app;

ISRType EE_as_GetISRID( void )
{
  /* Error Value Flag */
  register StatusType     ev;
  register ISRType        irq;

#if defined(EE_MAX_ISR2) && (EE_MAX_ISR2 > 0)
  register EE_UREG        irqnest = EE_hal_get_IRQ_nesting_level();
#endif /* EE_MAX_ISR2 > 0 */

  EE_ORTI_set_service_in(EE_SERVICETRACE_GETISRID);

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      “invalid value” of  the service. (BSW11009, BSW11013) */
  /* GetISRID is callable by Task, ISR2, ErrorHook and ProtectionHook */
  if ( EE_as_execution_context > ProtectionHook_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

#if defined(EE_MAX_ISR2) && (EE_MAX_ISR2 > 0)
  if ( irqnest > 0U ) {
    /* Inside an IRQ handler */
    irq = EE_as_ISR_stack[irqnest - 1U].ISR_ID;
    ev = E_OK;
  } else {
    irq = INVALID_ISR;
    ev = E_OK;
  }
#else  /* EE_MAX_ISR2 > 0 */
  {
    irq = INVALID_ISR;
    ev = E_OK;
  }
#endif /* EE_MAX_ISR2 > 0 */

  if ( ev != E_OK ) {
    EE_OS_ERROR_PARAMETERS_INIT(EE_OS_INVALID_PARAM,EE_OS_INVALID_PARAM,
      EE_OS_INVALID_PARAM);
    EE_os_notify_error_from_us(OSServiceId_GetISRID, &error_parameters,
      ev);
    EE_ORTI_set_service_out(EE_SERVICETRACE_GETISRID);
    irq = INVALID_ISR;
  } else {
    EE_ORTI_set_service_out(EE_SERVICETRACE_GETISRID);
  }

  return irq;
}

/* Supposed to be called by EE_as_TerminateISR2 to restore EE_as_active_app and
   terminate stacked task, eventually. */
void EE_as_after_IRQ_interrupted_app ( ApplicationType interrupted_app )
{
  EE_as_active_app = interrupted_app;
  /* Check the OSApplication that is going to be reactivated if it is not
     restarting terminate the current TASK */
  if ( (EE_as_Application_RAM[interrupted_app].ApplState !=
      APPLICATION_ACCESSIBLE) &&
    (EE_stk_queryfirst() !=
      EE_as_Application_ROM[interrupted_app].restart_task) )
  {
    EE_as_terminate_current_app_task();
  }
}
#endif /* EE_AS_OSAPPLICATIONS__ */

/*******************************************************************************
 *                     Stack Monitoring Internal Support
 ******************************************************************************/
#ifdef EE_STACK_MONITORING__
/* Used Internally in Kernel primitives */
void EE_as_monitoring_the_stack( void )
{
#ifdef EE_AS_OSAPPLICATIONS__
  if ( EE_IRQ_nesting_level > 0U ) {
    EE_as_check_and_handle_stack_overflow( EE_as_active_app,
      EE_as_Application_ROM[EE_as_active_app].ISRTOS );
  } else {
    EE_as_check_and_handle_stack_overflow( EE_as_active_app,
      EE_std_thread_tos[EE_stk_queryfirst() + 1]);
  }
#else /* EE_AS_OSAPPLICATIONS__ */
#ifdef __IRQ_STACK_NEEDED__
  /* I cannot monitor ISR2 Stack In this Case */
  /* TODO EG: Make RT-Druid Handle this configuration */
#warning Stack Monitoring Enabled without OSApplication: In this State ISR2\
  Stack is not monitorable
  if ( EE_IRQ_nesting_level > 0U ) {
    return;
  }
#endif /* __IRQ_STACK_NEEDED__ */
  EE_as_check_and_handle_stack_overflow(
    EE_std_thread_tos[EE_stk_queryfirst() + 1] );
#endif /* EE_AS_OSAPPLICATIONS__ */
}
#endif /* EE_STACK_MONITORING__ */
