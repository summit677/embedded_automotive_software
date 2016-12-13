/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2012  Evidence Srl
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

 /** @file      ee_as_multicore.c
  *  @brief     Basic Autosar API and data structures for multicore management
  *  @author    Errico Guidieri
  *  @date      2012
  */

#include "ee_internal.h"

#ifndef __PRIVATE_STARTOS__
/* Flag set by StartOs to signal that the OS is started */
extern EE_UREG  volatile EE_oo_started;

#if defined (EE_MAX_CPU) && (EE_MAX_CPU > 1)
/** @brief The following contains cores start addresses */
extern EE_ADDR const EE_SHARED_CDATA EE_as_core_start_addresses[EE_MAX_CPU - 1];
#endif /* EE_MAX_CPU */

#ifdef EE_MASTER_CPU
/** @brief The following contains cores application modes: N.B it is used inside
      StartOS */
AppModeType volatile EE_SHARED_UDATA EE_as_os_application_mode[EE_MAX_CPU];

/** @brief mask for Autosar cores started */
EE_UREG volatile EE_SHARED_IDATA
  EE_as_core_mask = ((EE_UREG)1U << OS_CORE_ID_MASTER);

/** @brief counter for Autosar cores started (OS_CORE_ID_MASTER is always an
      AUTOSAR by default) */
EE_UREG volatile EE_SHARED_IDATA EE_as_core_started = 1U;

/** @brief mask for non Autosar cores started */
EE_UREG volatile  EE_SHARED_UDATA EE_as_not_as_core_mask;

#endif /* EE_MASTER_CPU */

void EE_as_StartCore( CoreIdType CoreID, StatusType *Status )
{
  /* Error Value */
  register StatusType ev;
  /* Primitive Lock Procedure */
  register const EE_FREG flag = EE_hal_begin_nested_primitive();
  /* Variable introduced to meet MISRA 12.1 in the next else if statement */
  register EE_UREG EE_as_core_mask_local = EE_as_core_mask;

  /* Get CPU0 Spinlocks to serialize core starting procedure. As
     implementation choice we serialize core activation on core0 spinlock
     that is the one with max priority */
  EE_hal_spin_in(EE_SPINLOCK_CORE0);

  EE_ORTI_set_service_in(EE_SERVICETRACE_STARTCORE);

  if ( CoreID >= EE_MAX_CPU ) {
    /* [OS677] The function StartCore shall start one core that shall run under
        the control of the AUTOSAR OS. (BSW4080006, BSW4080026, BSW4080027)
     */
    /* [OS681] There is no call to the ErrorHook() if an error occurs during
        StartCore(); (BSW4080006, BSW4080026, BSW4080027) */
    /*  XXX: AS IMPLEMENTATION CHOICE THE CONFIGURATOR ASSIGN CORES IN ASCENDING
          ORDER OF ID AUTOMATICALLY. THERE IS NO WAY FOR USER TO SELECT
          EXPLICITLY WICH CORE ASSIGN TO AN SPECIFIC SLAVE CONFIGURATION.
          So if an ID is lower than EE_MAX_CPU is valid to call StartCore on.
        FIXME: Probably this behavior could be not perfectly compliant so it
          should be checked */
      ev = E_OS_ID;
  } else if ( EE_oo_started != 0U ) {
    /* [OS606] The AUTOSAR specification does not support the activation of
        AUTOSAR cores after calling StartOS on that core. If StartCore is called
        after StartOS it shall return with E_OS_ACCESS in extended status.
        (BSW4080001) */
    /* [OS678] Calls to the StartCore function after StartOS() shall return with
        E_OS_ACCESS and the core shall not be started.
        (BSW4080006, BSW4080026, BSW4080027) */
      ev = E_OS_ACCESS;
  } else if ( ((EE_as_core_mask_local | EE_as_not_as_core_mask) &
    ((EE_UREG)1U << CoreID)) != 0U ) {
    /* [OS679] If the parameter CoreIDs refers to a core that was already
        started by the function StartCore the related core is ignored and
        E_OS_STATE shall be returned. (BSW4080006, BSW4080026, BSW4080027) */
    /* [OS680] If the parameter CoreID refers to a core that was already
        started by the function StartNonAutosarCore the related core is
        ignored and E_OS_STATE shall be returned.
        (BSW4080006, BSW4080026, BSW4080027) */
    ev = E_OS_STATE;
  } else {
    /* Really start the core if we are not in MASTER core */
    if ( CoreID != OS_CORE_ID_MASTER ) {
      /* Flag that core is started as Autosar core */
      EE_as_core_mask |= ((EE_UREG)1U << CoreID);
      /* Increment the Autosar Cores counter */
      ++EE_as_core_started;

      EE_hal_start_core(CoreID, EE_as_core_start_addresses[CoreID - 1]);
    }

    ev = E_OK;
  }

  /* Restore the initial conditions */
  EE_hal_spin_out(EE_SPINLOCK_CORE0);

  EE_ORTI_set_service_out(EE_SERVICETRACE_STARTCORE);

  EE_hal_end_nested_primitive(flag);

  if ( Status != NULL ) {
    *Status = ev;
  }

  return;
}

void EE_as_StartNonAutosarCore( CoreIdType CoreID, StatusType *Status )
{
  /* Error Value */
  register StatusType ev;
  /* Primitive Lock Procedure */
  register const EE_FREG flag = EE_hal_begin_nested_primitive();
  /* Variable introduced to meet MISRA 12.1 in the next else if statement */
  register EE_UREG EE_as_core_mask_local = EE_as_core_mask;

  /* Get CPU0 Spinlocks to serialize core starting procedure */
  EE_hal_spin_in(EE_SPINLOCK_CORE0);

  EE_ORTI_set_service_in(EE_SERVICETRACE_STARTNONAUTOSARCORE);
  /* [OS683] The function StartNonAutosarCore shall start a core that is not
      controlled by the AUTOSAR OS. (BSW4080006, BSW4080026, BSW4080027) */

  /* EE_NUMBER_OF_CORES SHALL be defined in MCU layer (ideally in multicore
      support header */
  if ( CoreID >= EE_NUMBER_OF_CORES ) {
    /*  [OS685] If the parameter CoreID refers to an unknown core the function
        StartNonAutosarCore has effect and sets "Status" to E_OS_ID.
        (BSW4080006, BSW4080026, BSW4080027) */

      ev = E_OS_ID;
  } else if ( ((EE_as_core_mask_local | EE_as_not_as_core_mask) &
      ((EE_UREG)1U << CoreID)) != 0U ) {
    /*  [OS684] If the parameter CoreID refers to a core that was already
          started by the function StartNonAutosarCore has no effect and sets
          "Status" to E_OS_STATE. (BSW4080006, BSW4080026, BSW4080027) */
    /* XXX: Specification do not states for StartNonAutosarCore what to do if
        core has been start by StartCore, I choose to handle it has error */
    ev = E_OS_STATE;
  } else {
    /* Really start the core if we are not in MASTER core */
    if ( CoreID != OS_CORE_ID_MASTER ) {
      /* Flag that core is started as non Autosar core */
      EE_as_not_as_core_mask |= ((EE_UREG)1U << CoreID);

      EE_hal_start_core(CoreID, EE_as_core_start_addresses[CoreID - 1]);
    }

    ev = E_OK;
  }

  /* Restore the initial conditions */
  EE_hal_spin_out(EE_SPINLOCK_CORE0);

  EE_ORTI_set_service_out(EE_SERVICETRACE_STARTNONAUTOSARCORE);

  EE_hal_end_nested_primitive(flag);

  if ( Status != NULL ) {
    *Status = ev;
  }

  return;
}

#endif /* __PRIVATE_STARTOS__ */


