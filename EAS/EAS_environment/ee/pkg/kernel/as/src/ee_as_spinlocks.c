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

 /** @file      ee_as_spinlocks.c
  *  @brief     Autosar API and data structures for spinlocks management
  *  @author    Errico Guidieri
  *  @date      2012
  */

#include "ee_internal.h"

#ifndef EE_PRIVATE_SPINLOCK__

StatusType EE_as_GetSpinlock( SpinlockIdType SpinlockId )
{
  /* Error Value */
  register StatusType   ev;
  /* TASK or ISR2 current id */
  register EE_TID       current;

  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_GETSPINLOCK);

  EE_as_monitoring_the_stack();

  /* If actually we are inside an ISR2 assign a fake TID to access stack */
  if ( EE_hal_get_IRQ_nesting_level() > 0U ) {
    current = EE_oo_assign_TID_to_ISR2();
  } else {
    current = EE_stk_queryfirst();
  }

#ifdef EE_SERVICE_PROTECTION__
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of  the service. (BSW11009, BSW11013) */
  /* [OS650]: GetSpinlock shall be callable from TASK level. (BSW4080018,
      BSW4080021) */
  /* [OS651]: GetSpinlock shall be callable from ISR2 level. (BSW4080021) */
  /* The behavior of GetSpinlock is undefined if called from a category 1 ISR */
  /* [OS693]: It shall be allowed to call the function GetSpinlock while
      interrupts are disabled. (BSW4080021) */
  /* [OS694]: It shall be allowed to call the function GetSpinlock while a
      RESOURCE is occupied. (BSW4080021) */
  /* GetSpinlock is callable by Task, ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
  /* [OS692]: The function GetSpinlock shall return E_OS_ACCESS if the accessing
      OS-Application was not listed in the configuration (OsSpinlock).
      (BSW4080021) */
  if ( SpinlockId >= (EE_TYPESPIN)EE_MAX_SPINLOCK_USER ) {
    ev = E_OS_ID;
  } else if ( EE_SPINLOCK_ACCESS_ERR(SpinlockId, EE_as_active_app) ) {
    ev = E_OS_ACCESS;
  } else
#elif defined(__OO_EXTENDED_STATUS__)
  if ( SpinlockId >= (EE_TYPESPIN)EE_MAX_SPINLOCK_USER ) {
    ev = E_OS_ID;
  } else
#endif /* EE_FULL_SERVICE_PROTECTION || __OO_EXTENDED_STATUS__ */
#ifdef __OO_EXTENDED_STATUS__
  /* [OS689]: The function GetSpinlock shall return E_OS_ID if the parameter
      SpinlockID refers to a spinlock that does not exist. (BSW4080021) */
  /* [OS658]: The AUTOSAR Operating System shall generate an error if a TASK
      tries to occupy a spinlock that is assigned to a TASK/ISR2 on the same
      core (including itself). (BSW4080018, BSW4080021) */
  /* [OS659]: The AUTOSAR Operating System shall generate an error if an ISR2
      tries to occupy a spinlock that is assigned to a TASK/ISR2 on the same
      core. (BSW4080018, BSW4080021) */
  /* [OS690]: The function GetSpinlock shall return E_OS_INTERFERENCE_DEADLOCK
      if the spinlock referred by the parameter SpinlockID is already occupied
      by a TASK/ISR2 on the same core. (BSW4080021) */
  /* [OS660]: A unique order in which multiple spinlocks can be occupied by a
      TASK/ISR2 should be configurable in the AUTOSAR Operating System. This
      might be realized by the configuration item
      (OsSpinlockSuccessor{NEXT_SPINLOCK}) where "NEXT_SPINLOCK" refers to the
      consecutive spinlock. (See chapter 10.2.5) (BSW4080018, BSW4080021) */
  /* [OS661]: The AUTOSAR Operating System shall generate an error if a
      TASK/ISR2 that currently holds a spinlock tries to seize another spinlock
      that has not been configured as a direct or indirect successor of the
      latest acquired spinlock (by means of the OsSpinlockSuccessor
      configuration parameter) or if no successor is configured.
      (BSW4080018, BSW4080021) */
  /* [OS691]: The function GetSpinlock shall return E_OS_NESTING_DEADLOCK if the
      sequence by which multiple spinlocks are occupied at the same time do not
      comply with the configured order. (BSW4080021) */
  if ( current == EE_NIL ) {
    ev = E_OS_ACCESS;
  } else if ( EE_as_spinlocks_locker_core[SpinlockId] == EE_CURRENTCPU ) {
    ev = E_OS_INTERFERENCE_DEADLOCK;
  } else
#if defined( EE_SPINLOCK_ORDERED )
  if ( (EE_as_spinlocks_last[EE_CURRENTCPU] != INVALID_SPINLOCK) &&
       (EE_as_spinlocks_last[EE_CURRENTCPU] >= SpinlockId) )
#else /* EE_SPINLOCK_ORDERED */
  if ( EE_as_spinlocks_last[EE_CURRENTCPU] != INVALID_SPINLOCK )
#endif /* EE_SPINLOCK_ORDERED */
  {
    ev = E_OS_NESTING_DEADLOCK;
  } else
#endif /* __OO_EXTENDED_STATUS__ */
  {
    /* [OS649]: The AUTOSAR Operating System shall provide a GetSpinlock
        function which occupies a spinlock. If the spinlock is already occupied,
        GetSpinlock shall keep on trying to occupy the spinlock until it
        succeeds. (BSW4080018, BSW4080021) Same of [OS687] */

    /* Spin until get the lock */
    EE_hal_spin_in(SpinlockId);

    /* Populate Spinlocks Stack for CURRENT CPU */
    EE_as_spinlocks_stack[SpinlockId] = EE_as_spinlocks_last[EE_CURRENTCPU];
    EE_as_spinlocks_last[EE_CURRENTCPU] = SpinlockId;

    /* Set CURRENT CPU and CURRENT THREAD (means TASK or ISR2) as
       spinlock locker */
    EE_as_spinlocks_locker_core[SpinlockId] = (EE_TYPECOREID)EE_CURRENTCPU;

    EE_as_spinlocks_locker_task_or_isr2[SpinlockId] = current;

    /* [OS688]: The function GetSpinlock shall return E_OK if no error was
        detected. The spinlock is now occupied by the calling TASK/ISR2 on the
        calling core. */
    ev = E_OK;
  }

  if ( ev != E_OK ) {
#ifdef __OO_HAS_ERRORHOOK__
    EE_OS_PARAM(os_spinlock_id);
    EE_OS_PARAM_VALUE(os_spinlock_id,SpinlockId);
#endif /* __OO_HAS_ERRORHOOK__ */
    EE_os_notify_error(OSServiceId_GetSpinlock, os_spinlock_id,
      EE_OS_INVALID_PARAM, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_GETSPINLOCK);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

StatusType EE_as_ReleaseSpinlock( SpinlockIdType SpinlockId )
{
  /* Error Value */
  register StatusType   ev;
  register TaskType     current;

  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_RELEASESPINLOCK);

  EE_as_monitoring_the_stack();

  /* If actually we are inside an ISR2 assign a fake TID to access stack */
  if ( EE_hal_get_IRQ_nesting_level() > 0U ) {
    current = EE_oo_assign_TID_to_ISR2();
  } else {
    current = EE_stk_queryfirst();
  }

#ifdef EE_SERVICE_PROTECTION__
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of  the service. (BSW11009, BSW11013) */
  /* [OS655]: The AUTOSAR Operating System shall provide a ReleaseSpinlock
        function which releases an occupied spinlock. If the spinlock is not
        occupied an error shall be returned. (BSW4080018, BSW4080021) */
  /* [OS656]: ReleaseSpinlock shall be callable from TASK level. (BSW4080018,
        BSW4080021)
     [OS657]: ReleaseSpinlock shall be callable from ISR2 level. (BSW4080018,
        BSW4080021) */

  /* ReleaseSpinlock is callable by Task, ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else
#endif /* EE_SERVICE_PROTECTION__ */
#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
  /* [OS700]: The function ReleaseSpinlock shall return E_OS_ACCESS if the TASK
      has no access to the spinlock referred by the parameter SpinlockID
      (BSW4080021) */
  if ( SpinlockId >= (EE_TYPESPIN)EE_MAX_SPINLOCK_USER ) {
    ev = E_OS_ID;
  } else if ( EE_SPINLOCK_ACCESS_ERR(SpinlockId, EE_as_active_app) ) {
    ev = E_OS_ACCESS;
  } else
#elif defined(__OO_EXTENDED_STATUS__)
  if ( SpinlockId >= (EE_TYPESPIN)EE_MAX_SPINLOCK_USER ) {
    ev = E_OS_ID;
  } else
#endif /* EE_FULL_SERVICE_PROTECTION || __OO_EXTENDED_STATUS__ */
#ifdef __OO_EXTENDED_STATUS__
  /* [OS698]: The function ReleaseSpinlock shall return E_OS_ID if the parameter
      SpinlockID refers to a spinlock that does not exist. (BSW4080021) */
  /* [OS699]: The function ReleaseSpinlock shall return E_OS_STATE if the
      parameter SpinlockID refers to a spinlock that is not occupied by the
      calling TASK. (BSW4080021) */
  /* [OS701]: The function ReleaseSpinlock shall return E_OS_NOFUNC if the TASK
      tries to release a spinlock while another spinlock has to be released
      before. No functionality shall be performed. (BSW4080021) */
  /* [OS702]: Spinlocks and RESOURCEs can only be locked and unlocked in strict
      LIFO order. Otherwise E_OS_RESOURCE shall be returned. (BSW4080021) */
  if ( current == EE_NIL ) {
    ev = E_OS_ACCESS;
  } else if ( (EE_as_spinlocks_locker_core[SpinlockId] != EE_CURRENTCPU) ||
    (EE_as_spinlocks_locker_task_or_isr2[SpinlockId] != current) )
  {
    ev = E_OS_STATE;
  } else if ( EE_as_spinlocks_last[EE_CURRENTCPU] != SpinlockId ) {
    ev = E_OS_NOFUNC;
  } else
#endif /* __OO_EXTENDED_STATUS__ */
  {
    /* Remove the last entry from the data structure */
    EE_as_spinlocks_last[EE_CURRENTCPU] = EE_as_spinlocks_stack[SpinlockId];

    /* Free the spinlock */
    EE_as_spinlocks_locker_core[SpinlockId] = INVALID_CORE_ID;
    EE_as_spinlocks_locker_task_or_isr2[SpinlockId] = EE_NIL;

    /* Release the lock */
    EE_hal_spin_out(SpinlockId);

    ev = E_OK;
  }

  /* [OS697]: The function ReleaseSpinlock shall return E_OK if no error was
      detected. The spinlock is now free and can be occupied by the same or
      other TASKs. (BSW4080021) */
  if ( ev != E_OK ) {
#ifdef __OO_HAS_ERRORHOOK__
    EE_OS_PARAM(os_spinlock_id);
    EE_OS_PARAM_VALUE(os_spinlock_id,SpinlockId);
#endif /* __OO_HAS_ERRORHOOK__ */
    EE_os_notify_error(OSServiceId_ReleaseSpinlock, os_spinlock_id,
      EE_OS_INVALID_PARAM, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_RELEASESPINLOCK);
  EE_OS_EXIT_CRITICAL_SECTION();
  return ev;
}

StatusType EE_as_TryToGetSpinlock( SpinlockIdType SpinlockId,
  TryToGetSpinlockType* Success )
{

  register EE_TID       current;
  register EE_BIT       try_to_spin_in_flag;
  /* Error Value */
  register StatusType   ev;
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_TRYTOGETSPINLOCK);

  EE_as_monitoring_the_stack();

  /* If actually we are inside an ISR2 assign a fake TID to access stack */
  if ( EE_hal_get_IRQ_nesting_level() > 0U ) {
    current = EE_oo_assign_TID_to_ISR2();
  } else {
    current = EE_stk_queryfirst();
  }

#ifdef EE_SERVICE_PROTECTION__
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of  the service. (BSW11009, BSW11013) */
  /* [OS652]: The AUTOSAR Operating System shall provide a TryToGetSpinlock
      function which occupies a spinlock. If the spinlock is already occupied
      by a TASK, TryToGetSpinlock shall return. (BSW4080018, BSW4080021) */
  /* [OS653]: TryToGetSpinlock shall be callable from TASK level.
      (BSW4080018, BSW4080021) */
  /* [OS654]: TryToGetSpinlock shall be callable from ISR2 level.
      (BSW4080018, BSW4080021) */
  /* [OS711]: It shall be allowed to call the function TryToGetSpinlock while
      interrupts are disabled. (BSW4080021) */
  /* [OS712]: It shall be allowed to call the function TryToGetSpinlock while a
      RESOURCE is occupied. (BSW4080021) */

  /* TryToGetSpinlock is callable by Task, ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
  /* [OS710]: The function TryToGetSpinlock shall return E_OS_ACCESS if the
      TASK has no access to the spinlock referred by the parameter SpinlockID
      (BSW4080021) */
  if ( SpinlockId >= (EE_TYPESPIN)EE_MAX_SPINLOCK_USER ) {
    ev = E_OS_ID;
  } else if ( EE_SPINLOCK_ACCESS_ERR(SpinlockId, EE_as_active_app) ) {
    ev = E_OS_ACCESS;
  } else
#elif defined(__OO_EXTENDED_STATUS__)
  if ( SpinlockId >= (EE_TYPESPIN)EE_MAX_SPINLOCK_USER ) {
    ev = E_OS_ID;
  } else
#endif /* EE_FULL_SERVICE_PROTECTION || __OO_EXTENDED_STATUS__ */
#ifdef __OO_EXTENDED_STATUS__

  /* [OS706]: If the function TryToGetSpinlock does not return E_OK, the OUT
      parameter "Success" shall be undefined. (BSW4080021) */
  /* [OS708]: The function TryToGetSpinlock shall return
      E_OS_INTERFERENCE_DEADLOCK if the spinlock referred by the parameter
      SpinlockID is already occupied by a TASK on the same core.(BSW4080021) */
  /* [OS660]: A unique order in which multiple spinlocks can be occupied by a
      TASK/ISR2 should be configurable in the AUTOSAR Operating System. This
      might be realized by the configuration item
      (OsSpinlockSuccessor{NEXT_SPINLOCK}) where "NEXT_SPINLOCK" refers to the
      consecutive spinlock. (See chapter 10.2.5) (BSW4080018, BSW4080021) */
  /* [OS661]: The AUTOSAR Operating System shall generate an error if a
      TASK/ISR2 that currently holds a spinlock tries to seize another spinlock
      that has not been configured as a direct or indirect successor of the
      latest acquired spinlock (by means of the OsSpinlockSuccessor
      configuration parameter) or if no successor is configured.
      (BSW4080018, BSW4080021) */
  /* [OS709]: The function TryToGetSpinlock shall return E_OS_NESTING_DEADLOCK
      if a TASK tries to occupy a spinlock while holding a different spinlock
      in a way that may cause a deadlock. */
  if ( current == EE_NIL ) {
    ev = E_OS_ACCESS;
  } else if ( EE_as_spinlocks_locker_core[SpinlockId] == EE_CURRENTCPU ) {
    ev = E_OS_INTERFERENCE_DEADLOCK;
  } else
#if defined( EE_SPINLOCK_ORDERED )
  if ( (EE_as_spinlocks_last[EE_CURRENTCPU] != INVALID_SPINLOCK) &&
       (EE_as_spinlocks_last[EE_CURRENTCPU] >= SpinlockId) )
#else /* EE_SPINLOCK_ORDERED */
  if ( EE_as_spinlocks_last[EE_CURRENTCPU] != INVALID_SPINLOCK )
#endif /* EE_SPINLOCK_ORDERED */
  {
    ev = E_OS_NESTING_DEADLOCK;
  } else
#endif /* __OO_EXTENDED_STATUS__ */
  /* [OS566]: The Operating System API shall check in extended mode all pointer
      argument for NULL pointer and return OS_E_PARAMETER_POINTER 
      if such argument is NULL. +
      MISRA dictate NULL check for pointers always. */
  if ( Success == NULL ) {
    ev = E_OS_PARAM_POINTER;
  } else {

    /* Try to Spin in, return immediately if it doesn't */
    try_to_spin_in_flag = EE_hal_try_to_spin_in(SpinlockId);

    /* [OS705]: The function TryToGetSpinlock shall set the OUT parameter
        "Success" to TRYTOGETSPINLOCK_SUCCESS if the spinlock was successfully
        occupied, and TRYTOGETSPINLOCK_NOSUCCESS if not. In both cases
        E_OK shall be returned. (BSW4080021) */
    if ( try_to_spin_in_flag == 0U )
    {
      *Success =  TRYTOGETSPINLOCK_NOSUCCESS;
    } else {
      /* Populate Spinlocks Stack for CURRENT CPU */
      EE_as_spinlocks_stack[SpinlockId] = EE_as_spinlocks_last[EE_CURRENTCPU];
      EE_as_spinlocks_last[EE_CURRENTCPU] = SpinlockId;

      /* Set CURRENT CPU and CURRENT THREAD (means TASK or ISR2) as
        Spinlock locker */
      EE_as_spinlocks_locker_core[SpinlockId] = (EE_TYPECOREID)EE_CURRENTCPU;

      EE_as_spinlocks_locker_task_or_isr2[SpinlockId] = current;

      *Success =  TRYTOGETSPINLOCK_SUCCESS;
    }
    ev = E_OK;
  }

  if ( ev != E_OK ) {
#ifdef __OO_HAS_ERRORHOOK__
    EE_OS_PARAM(os_spinlock_id);
    EE_OS_PARAM(os_success);
    EE_OS_PARAM_VALUE(os_spinlock_id,SpinlockId);
    EE_OS_PARAM_REF(os_success,try_to_get_spinlock_ref,Success);
#endif /* __OO_HAS_ERRORHOOK__ */
    EE_os_notify_error(OSServiceId_TryToGetSpinlock, os_spinlock_id,
      os_success, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_TRYTOGETSPINLOCK);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

SpinlockIdType EE_as_release_all_spinlocks( EE_TID tid )
{
  /* ALLERT! This method have to be called only inside a critical section
     (e.g. interrupt disabled) */
  register SpinlockIdType curSpin, spinId = INVALID_SPINLOCK;

  curSpin = EE_as_spinlocks_last[EE_CURRENTCPU];
  while ( (curSpin != INVALID_SPINLOCK) &&
    (EE_as_spinlocks_locker_task_or_isr2[curSpin] == tid) )
  {
    spinId = curSpin;
    /* Remove the last entry from the data structure */
    EE_as_spinlocks_last[EE_CURRENTCPU] =  EE_as_spinlocks_stack[curSpin];

    /* Free the spinlock */
    EE_as_spinlocks_locker_core[curSpin] = INVALID_CORE_ID;
    EE_as_spinlocks_locker_task_or_isr2[curSpin] = EE_NIL;

    /* Release the lock */
    EE_hal_spin_out(curSpin);

    curSpin = EE_as_spinlocks_last[EE_CURRENTCPU];
  }

  return spinId;
}

#endif /* EE_PRIVATE_SPINLOCK__ */


