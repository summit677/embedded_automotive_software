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
 * APIs and functions used for memory protection
 * Author: 2011 Bernardo  Dal Seno
 */

#include <ee_internal.h>

/*
 * Functions to enable and disable interrupts used when memory protection is
 * active. Interrupts must be enabled/disabled when returning from the syscall
 * made from user space.
 */

/* The variables below have the "EE_oo" prefix because they could be used also
 * for the OO kernel. */

/* IRQ state at the time DisableAllInterrupts() is called.  This is global,
 * because it's used by other APIs to check if they are called inside a
 * Disable/Enable section. */
EE_FREG EE_oo_all_irq_prev_state = EE_HAL_IRQSTATE_INVALID;

/* IRQ state at the time the first SuspendAllInterrupts() is called */
static EE_FREG EE_oo_res_all_irq_prev_state;

EE_FREG EE_as_DisableAllInterrupts(EE_FREG prev)
{
  EE_FREG next;
  EE_ORTI_set_service_in(EE_SERVICETRACE_DISABLEALLINTERRUPTS);
  EE_oo_all_irq_prev_state = EE_hal_set_irq_valid_flag(prev);
  next = EE_hal_clear_irq_flag(prev);
  ++EE_oo_IRQ_disable_count;

  /* Enable DisableAllInterrupts TP budget, if needed */
  if ( EE_oo_IRQ_disable_count == 1U ) {
    EE_as_tp_active_activate_budget(EE_ALL_INTERRUPT_LOCK_BUDGET,
      INVALID_OBJECTID, EE_TRUE);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_DISABLEALLINTERRUPTS);
  return next;
}

EE_FREG EE_as_EnableAllInterrupts(EE_FREG prev)
{
  EE_FREG next;
  EE_FREG prev_state;
  EE_ORTI_set_service_in(EE_SERVICETRACE_ENABLEALLINTERRUPTS);
  next = prev;
  prev_state = EE_oo_all_irq_prev_state;
  if ( prev_state == EE_HAL_IRQSTATE_INVALID ) {
    /* No previous DisableAllInterrupts(): do nothing */
  } else {
    EE_oo_all_irq_prev_state = EE_HAL_IRQSTATE_INVALID;
    next = EE_hal_copy_irq_flag(prev_state, next);
    --EE_oo_IRQ_disable_count;
  }

  /* Stop DisableAllInterrupts TP budget, if needed */
  if ( EE_oo_IRQ_disable_count == 0U ) {
    EE_as_tp_active_stop_budget(EE_ALL_INTERRUPT_LOCK_BUDGET, INVALID_OBJECTID,
      EE_TRUE);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_ENABLEALLINTERRUPTS);
  return next;
}

EE_FREG EE_as_SuspendAllInterrupts(EE_FREG prev)
{
  EE_FREG next;
  EE_ORTI_set_service_in(EE_SERVICETRACE_SUSPENDALLINTERRUPTS);
  next = prev;
  if ( EE_oo_IRQ_disable_count == 0U ) {
    next = EE_hal_clear_irq_flag(next);
    EE_oo_res_all_irq_prev_state = prev;
  }
  ++EE_oo_IRQ_disable_count;

  /* Enable DisableAllInterrupts TP budget, if needed */
  if ( EE_oo_IRQ_disable_count == 1U ) {
    EE_as_tp_active_activate_budget(EE_ALL_INTERRUPT_LOCK_BUDGET,
      INVALID_OBJECTID, EE_TRUE);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_SUSPENDALLINTERRUPTS);
  return next;
}

EE_FREG EE_as_ResumeAllInterrupts(EE_FREG prev)
{
  EE_FREG next;
  EE_ORTI_set_service_in(EE_SERVICETRACE_RESUMEALLINTERRUPTS);
  next = prev;
  if ( EE_oo_IRQ_disable_count == 0U ) {
    /* No previous SuspendAllInterrupts(): do nothing */
  } else {
    --EE_oo_IRQ_disable_count;
    if ( EE_oo_IRQ_disable_count == 0U ) {
      next = EE_hal_copy_irq_flag(
      EE_oo_res_all_irq_prev_state, next);
      /* Stop DisableAllInterrupts TP budget, if needed */
      EE_as_tp_active_stop_budget(EE_ALL_INTERRUPT_LOCK_BUDGET,
        INVALID_OBJECTID, EE_TRUE);
    }
  }
  EE_ORTI_set_service_out(EE_SERVICETRACE_RESUMEALLINTERRUPTS);
  return next;
}

/* FIXME: Currently ERIKA has not defined any HAL primitives to selectively
 * disable ISR2s. Therefore, SuspendOSInterrupts() suspends all interrupts. */
EE_FREG EE_as_SuspendOSInterrupts(EE_FREG prev)
{
  return EE_as_SuspendAllInterrupts(prev);
}

EE_FREG EE_as_ResumeOSInterrupts(EE_FREG prev)
{
  return EE_as_ResumeAllInterrupts(prev);
}

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define API_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

AccessType EE_as_CheckTaskMemoryAccess( TaskType TaskID,
  MemoryStartAddressType Address, MemorySizeType Size )
{
  /* This function will be called only by a syscall, interrupts will
     be handled by the syscall handler */
  register AccessType ret;
  /* Error Value Flag */
  register StatusType     ev;

  EE_ORTI_set_service_in(EE_SERVICETRACE_CHECKTASKMEMORYACCESS);

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of the service. (BSW11009, BSW11013) */
  /* CheckTaskMemoryAccess is callable by Task, ISR2, ErrorHook and
     ProtectionHook */
  if ( EE_as_execution_context > ProtectionHook_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

  if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
    /* Invalid TaskID: no permission */
    ret = NO_ACCESS;
    ev = E_OK;
  } else {
    ApplicationType app;
    app = EE_th_app[TaskID + 1];
    ret = EE_hal_get_app_mem_access(app, Address, Size);
    ev  = E_OK;
  }

  if ( ev != E_OK ) {
    EE_OS_ERROR_PARAMETERS();
    EE_OS_ERROR_PARAMETERS_PARAM1_VALUE(TaskID);
    EE_OS_ERROR_PARAMETERS_PARAM2_REF(memory_address,Address);
    EE_OS_ERROR_PARAMETERS_PARAM1_VALUE(Size);

    EE_os_notify_error_from_us(OSServiceId_CheckTaskMemoryAccess,
      &error_parameters, ev);
    EE_ORTI_set_service_out(EE_SERVICETRACE_CHECKTASKMEMORYACCESS);
    ret = NO_ACCESS;
  } else {
    EE_ORTI_set_service_out(EE_SERVICETRACE_CHECKTASKMEMORYACCESS);
  }

  return ret;
}

AccessType EE_as_CheckISRMemoryAccess( ISRType ISRID,
  MemoryStartAddressType Address, MemorySizeType Size )
{
  /* This function will be called only by a syscall, interrupts will
     be handled by the syscall handler */
  /* Invalid ISRID: no permission */
  register AccessType ret;
  /* Error Value Flag */
  register StatusType ev;

  EE_ORTI_set_service_in(EE_SERVICETRACE_CHECKISRMEMORYACCESS);

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of the service. (BSW11009, BSW11013) */
  /* CheckISRMemoryAccess is callable by Task, ISR2, ErrorHook and
     ProtectionHook */
  if ( EE_as_execution_context > ProtectionHook_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

#if defined(EE_MAX_ISR_ID) && (EE_MAX_ISR_ID > 0)
  if ( ISRID < (ISRType)EE_MAX_ISR_ID ) {
    ApplicationType app;
    app = EE_as_ISR_ROM[ISRID].ApplID;
    ret = EE_hal_get_app_mem_access(app, Address, Size);
    ev  = E_OK;
  } else
#endif /* EE_MAX_ISR_ID > 0 */
  {
    ret = NO_ACCESS;
    ev  = E_OK;
  }

  if ( ev != E_OK ) {
    EE_OS_ERROR_PARAMETERS();
    EE_OS_ERROR_PARAMETERS_PARAM1_VALUE(ISRID);
    EE_OS_ERROR_PARAMETERS_PARAM2_REF(memory_address,Address);
    EE_OS_ERROR_PARAMETERS_PARAM1_VALUE(Size);

    EE_os_notify_error_from_us(OSServiceId_CheckISRMemoryAccess,
      &error_parameters, ev);
    EE_ORTI_set_service_out(EE_SERVICETRACE_CHECKISRMEMORYACCESS);
    ret = NO_ACCESS;
  } else {
    EE_ORTI_set_service_out(EE_SERVICETRACE_CHECKISRMEMORYACCESS);
  }
  return ret;
}

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define API_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#if defined(EE_SYSCALL_NR) && defined(EE_MAX_SYS_SERVICEID) &&\
  (EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID)
StatusType EE_as_CallTrustedFunction(TrustedFunctionIndexType FunctionIndex,
  TrustedFunctionParameterRefType FunctionParams)
{
  register StatusType        ev;
  /* This function will be called only by a syscall, interrupts will
     be handled by the syscall handler, but TP have to be handled here... */
  register EE_as_Application_RAM_type * const app_RAM_ptr =
    &EE_as_Application_RAM[EE_as_active_app];
  EE_ORTI_set_service_in(EE_SERVICETRACE_CALLTRUSTEDFUNCTION);

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of the service. (BSW11009, BSW11013) */
  /* CallTrustedFunction is callable by Task and ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */
  /* [OS623]: The OS API function CallTrustedFunction shall return E_OS_ACCESS
      in extended status if the target trusted function is part of an
      OS-Application on another core. (BSW4080013)
    XXX: This cannot never happens in our implementation */

  /* [SWS_Os_00100] If CallTrustedFunction() is called and the called trusted
      function is not configured the Operating System module shall call the
      ErrorHook with E_OS_SERVICEID. */
  if ( (FunctionIndex < EE_MAX_SYS_SERVICEID) &&
        (FunctionIndex >= EE_SYSCALL_NR) )
  {
    ev = E_OS_SERVICEID;
  } else {
    ++app_RAM_ptr->TrustedFunctionCallsCounter;
    /* Re-enable interrupts before call the TRUSTED Function */
    EE_hal_enableIRQ();
    ev = ((EE_TRUSTEDFUNCTYPE)EE_syscall_table[FunctionIndex])(FunctionIndex,
      FunctionParams);
    /* Disable them again */
    EE_hal_disableIRQ();
    /* Decrement TRUSTED function call counter */
    --app_RAM_ptr->TrustedFunctionCallsCounter;

    /* Monitor the stack here: after the function call & before scheduling */
    EE_as_monitoring_the_stack();
    /* A Task Activation could have been delayed */
    if ( ev == E_OK ) {
      /* Check for preemption: 
         this test has to be done only if we are inside a TASK. The check
         that we are not in a nested Trusted Functions Call, is done inside the
         preemption point */
      if ( EE_hal_get_IRQ_nesting_level() == 0U ) {
        /* we are inside a task */
        EE_oo_preemption_point();
      }
    }
  }

  if ( ev != E_OK ) {
    EE_OS_PARAM(os_function_index);
    EE_OS_PARAM(os_function_params);
    EE_OS_PARAM_VALUE(os_function_index, FunctionIndex);
    EE_OS_PARAM_REF(os_function_params, trusted_function_parameter_ref,
      FunctionParams);
    EE_os_notify_error(OSServiceId_CallTrustedFunction, os_function_index,
      os_function_params, EE_OS_INVALID_PARAM, ev);
  }

  /*  Reaction to timing protection can be defined to terminate the
      OSApplication. If a task is inside CallTrustedFunction() and task
      rescheduling takes place within the same OSApplication, the newly running
      higher priority task may cause timing protection and terminate the
      OSApplication, thus indirectly aborting the trusted function.
      To avoid this, the scheduling of other Tasks which belong to the same
      OS-Application as the caller needs to be restricted, as well as the
      availability of interrupts of the same OS-Application. */
  /* [SWS_Os_00565]: When CallTrustedFunction() is called and the caller of
      CallTrustedFunction() is supervised with timing protection, the Operating
      System shall delay any timing protection errors until the return of
      CallTrustedFunction(). */
  /* [SWS_Os_00564]: If such a violation is detected inside a nested call
      sequence of CallTrustedFunction() of a task, the delay shall last until
      the return of the last CallTrustedFunction(). */
  /* The following handle enventual TP errors. The check if we are inside a
     Trusted Function Call inside, because the following function is called at
     the end of all ERIKA Services, and the check have to be done at every
     service's call */
  EE_as_tp_active_update_budgets_and_restart();

  EE_ORTI_set_service_out(EE_SERVICETRACE_CALLTRUSTEDFUNCTION);
  return ev;
}
#endif /* EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID */

