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
 * CVS: $Id: ee_chaintas.c,v 1.3 2005/07/19 14:01:16 pj Exp $
 */

#include "ee_internal.h"

/* ChainTask: 
  - activate a task 
    - The task is moved from the running state to the suspended state 
      and the other is activated (it can be the same task)
    - automatic release of internal resource 
    - Rescheduling is issued 
    - returns 
      E_OS_LIMIT if too many activations
      Extended status
      E_OS_ID if the task id is invalid
      E_OS_RESOURCE if the task still occupy resources
      E_OS_CALLLEVEL if called at interrupt level 
*/

#ifndef __OO_NO_CHAINTASK__
#ifndef __PRIVATE_CHAINTASK__
StatusType EE_oo_ChainTask(TaskType TaskID)
{
  register TaskType current;
  /* Error Value */
  register StatusType ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_CHAINTASK);

  EE_as_monitoring_the_stack();

  current = EE_stk_queryfirst();

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of  the service. (BSW11009, BSW11013) */
  if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

#if defined(__OO_EXTENDED_STATUS__) || defined(EE_SERVICE_PROTECTION__)
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of  the service. (BSW11009, BSW11013) */
  /* ChainTask can be callable only by Task */
  /* Check for a call at interrupt level: This must be the FIRST check! */
  if ( (EE_hal_get_IRQ_nesting_level() != 0U) || (current == EE_NIL)
#if !defined (EE_SERVICE_PROTECTION__)
  ) /* If EE_SERVICE_PROTECTION__ is not defined the succeeding
	 * check is always FALSE, hence it is not needed  */
#else
  || (EE_as_get_execution_context() > TASK_Context) )
#endif
  {
    ev = E_OS_CALLEVEL;
  } else
#endif /* __OO_EXTENDED_STATUS__ || EE_SERVICE_PROTECTION__ */

#ifdef __OO_EXTENDED_STATUS__
#ifndef __OO_NO_RESOURCES__
    /* check for busy resources */ 
    if ( EE_th_resource_last[current] != EE_UREG_MINUS1 ) {
      ev = E_OS_RESOURCE;
    } else
#endif /* !__OO_NO_RESOURCES__ */

#ifdef EE_AS_USER_SPINLOCKS__
/* [OS612] In extended status TerminateTask / ChainTask shall return with an
    error (E_OS_SPINLOCK), which can be evaluated in the application.
    (BSW4080021) */
  if ( EE_as_spinlocks_last[EE_CURRENTCPU] != INVALID_SPINLOCK ) {
    ev = E_OS_SPINLOCK;
  } else
#endif /* EE_AS_USER_SPINLOCKS__ */
#endif /* __OO_EXTENDED_STATUS__ */

#if defined(__RN_TASK__) || defined(EE_AS_RPC__)
  if ( EE_IS_TID_REMOTE(TaskID) ) {
#ifdef EE_AS_RPC__
    /* Tmp Tid (introduced to meet MISRA requirements) */
    EE_TID tmp_tid;

    EE_os_param unmarked_tid;
    /* Two steps macro assignment to meet MISRA 10.3 required rule */
    tmp_tid = EE_UNMARK_REMOTE_TID(TaskID);
    unmarked_tid.value_param = (EE_UREG)tmp_tid;
    /* Forward the request to another CPU in synchronous way */
    ev = EE_as_rpc(OSServiceId_ChainTask, unmarked_tid,
      EE_OS_INVALID_PARAM, EE_OS_INVALID_PARAM);
    /* Like in TerminateTask, we do not have to activate any other task */
    EE_th_terminate_nextask[current] = EE_NIL;
#else /* EE_AS_RPC__ */
    EE_TYPERN_PARAM par;
    par.pending = 1U;
    /* Forward the request to another CPU */
    (void)EE_rn_send((EE_SREG)EE_MARK_REMOTE_TID(TaskID),
      EE_RN_TASK, par);
    /* Like in TerminateTask, we do not have to activate any other task */
    EE_th_terminate_nextask[current] = EE_NIL;
    ev = E_OK;
#endif /* EE_AS_RPC__ */
  } else {
#endif /* __RN_TASK__ || EE_AS_RPC__ */
#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
    /* Check if the TASK Id is valid */
    if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
      ev = E_OS_ID;
    } else if ( EE_TASK_ACCESS_ERR(TaskID, EE_as_active_app) ) {
      ev = E_OS_ACCESS;
    } else
#elif defined(__OO_EXTENDED_STATUS__)
    /* Check if the TASK Id is valid */
    if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
      ev = E_OS_ID;
    } else
#endif /* EE_AS_OSAPPLICATIONS__ || E_SERVICE_PROTECTION__ ||
__OO_EXTENDED_STATUS__ */
    /*  Check for pending activations; works also if the task passed as
        parameter inside ChainTask is the calling task;
        see MODISTARC Test 9 */
    if ( (TaskID != current) && (EE_th_rnact[TaskID] == 0U) ) {
      ev = E_OS_LIMIT;
    } else {
      /* [SWS_Os_00469]: The Operating System module shall start an
          OsTaskTimeFrame when a task is activated successfully.
          (SRS_Os_11008) */
      /* [SWS_Os_00466]: If an attempt is made to activate a task before the
          end of an OsTaskTimeFrame then the Operating System module shall not
          perform the activation AND shall call the ProtectionHook()
          with E_OS_PROTECTION_ARRIVAL. */
      /* Check Interarrival Frame */
      if ( EE_as_tp_handle_interarrival(EE_AS_TP_ID_FROM_TASK(TaskID)) )
      {
        /* Note: the decrement is done now, not in the endcycle! */
        --EE_th_rnact[TaskID];
        /* We have to activate another TASK... */
        EE_th_terminate_nextask[current] = TaskID;
      }
      ev = E_OK;
    }
#if defined(__RN_TASK__) || defined(EE_AS_RPC__)
  }
#endif /* __RN_TASK__ || EE_AS_RPC__ */

  if ( ev != E_OK ) {
    EE_ORTI_set_lasterror(ev);
    EE_oo_notify_error_ChainTask(TaskID, ev);
  } else {
    /* The following won't never return */
    EE_ORTI_set_service_out(EE_SERVICETRACE_CHAINTASK);
    EE_hal_terminate_task(current);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_CHAINTASK);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

#endif /* __PRIVATE_CHAINTASK__ */
#endif /* __OO_NO_CHAINTASK__ */
