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
 * Author: 2002 Paolo Gai
 * CVS: $Id: ee_sempost.c,v 1.3 2006/12/03 22:07:50 pj Exp $
 */

#include "ee_internal.h"

/* PostSem:
  - This primitive can be called from ISRs, Tasks, and from the background
    (main) task.
  - for BCC1 and BCC2, a smaller reduced version is provided.
  - The primitive implements the traditional counting Semaphore post operation.
  - If there are tasks blocked on the Semaphore, then one of these tasks is
    awaken. In that case, the call enforces rescheduling.
    If there are no tasks blocked on the Semaphore, the semaphore counter is
    incremented.
  - Error value returned
    Standard:  No error, E_OK 
    E_OS_VALUE Semaphore counter has the maximum value

    Conformance: 
    - BCC1, BCC2 (without wakeup)
    - ECC1, ECC2 (with wakeup)
*/

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
#ifndef __PRIVATE_POSTSEM__

StatusType EE_oo_PostSem(SemRefType Sem)
{
  /* Error Value */
  register StatusType ev;
  register TaskType unlocked_tmp;

  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_POSTSEM);

  EE_as_monitoring_the_stack();

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

  /* check if the post on the semaphore wakes up someone */
  if ( (Sem != NULL) && (Sem->first != EE_NIL) ) {

    /* update the semaphore queue */
    unlocked_tmp = Sem->first;
    if ( (Sem->first = EE_th_next[unlocked_tmp]) == EE_NIL ) {
      Sem->last = EE_NIL;
    }

    /* if yes, the task must go back into the READY state */
    EE_th_status[unlocked_tmp] = READY;
    /* insert the task in the ready queue */
    EE_rq_insert(unlocked_tmp);
  
    /* and if I am at task level, check for preemption... */
    if (EE_hal_get_IRQ_nesting_level() == 0U) {
      /* we are inside a task */
      EE_oo_preemption_point();
    }
    ev = E_OK;
  } else if ( Sem != NULL ) {
#if defined(EE_AS_OSAPPLICATIONS__) && (defined(EE_SERVICE_PROTECTION__) &&\
  defined(__EE_MEMORY_PROTECTION__))
    /* [SWS_Os_00051]: If an invalid address (address is not writable by this
        OS-Application) is passed as an out-parameter to an Operating System
        service, the Operating System module shall return the status code
        E_OS_ILLEGAL_ADDRESS. (SRS_Os_11009, SRS_Os_11013) */
    if ( !OSMEMORY_IS_WRITEABLE(EE_hal_get_app_mem_access(EE_as_active_app,
      Sem, sizeof(*Sem))) )
    {
      ev = E_OS_ILLEGAL_ADDRESS;
    } else
#endif /* EE_AS_OSAPPLICATIONS__ && __EE_MEMORY_PROTECTION__ &&
  EE_SERVICE_PROTECTION__ */
    if ( Sem->count == EE_MAX_SEM_COUNTER ) {
      ev = E_OS_VALUE;
    } else {
      Sem->count++;
      ev = E_OK;
    }
  } else {
    ev = E_OS_PARAM_POINTER;
  }

  if ( ev != E_OK )
  {
    EE_OS_PARAM(os_sem);
    EE_OS_PARAM_REF(os_sem, sem_ref, Sem);
    EE_os_notify_error(OSServiceId_PostSem, os_sem,
      EE_OS_INVALID_PARAM, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_POSTSEM);

  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

#endif /* __PRIVATE_POSTSEM__ */
#endif /* ECC1/ECC2 */


/* This is a simplified version of PostSem */

#if defined(__OO_BCC1__) || defined(__OO_BCC2__)
#ifndef __PRIVATE_POSTSEM__

StatusType EE_oo_PostSem(SemRefType Sem)
{
  /* Error Value */
  register StatusType ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_POSTSEM);

  /* the wake up check is removed because there is no blocking wait! */
  if ( Sem != NULL ) {
#if defined(EE_AS_OSAPPLICATIONS__) && (defined(EE_SERVICE_PROTECTION__) &&\
  defined(__EE_MEMORY_PROTECTION__))
    /* [SWS_Os_00051]: If an invalid address (address is not writable by this
        OS-Application) is passed as an out-parameter to an Operating System
        service, the Operating System module shall return the status code
        E_OS_ILLEGAL_ADDRESS. (SRS_Os_11009, SRS_Os_11013) */
    if ( !OSMEMORY_IS_WRITEABLE(EE_hal_get_app_mem_access(EE_as_active_app,
      Sem, sizeof(*Sem))) )
    {
      ev = E_OS_ILLEGAL_ADDRESS;
    } else
#endif /* EE_AS_OSAPPLICATIONS__ && __EE_MEMORY_PROTECTION__ &&
  EE_SERVICE_PROTECTION__ */
    if ( Sem->count == EE_MAX_SEM_COUNTER ) {
      ev = E_OS_VALUE;
    } else {
      Sem->count++;
      ev = E_OK;
    }
  } else {
    ev = E_OS_PARAM_POINTER;
  }

  if ( ev != E_OK )
  {
    EE_OS_PARAM(os_sem);
    EE_OS_PARAM_REF(os_sem, sem_ref, Sem);
    EE_os_notify_error(OSServiceId_PostSem, os_sem,
      EE_OS_INVALID_PARAM, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_POSTSEM);

  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

#endif /* __PRIVATE_POSTSEM__ */
#endif /* BCC1/BCC2 */
