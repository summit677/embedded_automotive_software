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
 * CVS: $Id: ee_activate.c,v 1.5 2006/01/05 14:37:22 pj Exp $
 */

#include "ee_internal.h"

/* ActivateTask:
   - The task is moved from the suspended state to the ready state
   - called from interrupts, from tasks, from StartupHook
   - returns E_OS_LIMIT if too many activations are issued
      E_OK otherwise
      E_OS_ID if the taskID is invalid (Extended status)
   - clears the events of a task (extended task)

   NOTE: part of this source code is copied into altick.c!
*/

#ifndef __PRIVATE_ACTIVATETASK__
StatusType EE_oo_ActivateTask(TaskType TaskID)
{
  /* Error Value */
  register StatusType ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_ACTIVATETASK);

  EE_as_monitoring_the_stack();

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
  /* ActivateTask is callable by Task and ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

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
    ev = EE_as_rpc( OSServiceId_ActivateTask, unmarked_tid,
      EE_OS_INVALID_PARAM, EE_OS_INVALID_PARAM );
#else /* EE_AS_RPC__ */
    EE_TYPERN_PARAM par;
    par.pending = 1U;
    /* Forward the request to another CPU in asynchronous way */
    (void)EE_rn_send(EE_UNMARK_REMOTE_TID(TaskID), EE_RN_TASK, par);
    ev = E_OK;
#endif /* EE_AS_RPC__ */
  } else {
#endif /* __RN_TASK__ || EE_AS_RPC__ */

#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
    /* Check if the task Id is valid */
    if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
      ev = E_OS_ID;
    } else if ( EE_TASK_ACCESS_ERR(TaskID, EE_as_active_app) ) {
      ev = E_OS_ACCESS;
    } else
#elif defined(__OO_EXTENDED_STATUS__)
    /* Check if the task Id is valid */
    if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
      ev = E_OS_ID;
    } else
#endif /* EE_AS_OSAPPLICATIONS__ || E_SERVICE_PROTECTION__ ||
__OO_EXTENDED_STATUS__ */
    /* Check for pending activations */
    if ( EE_th_rnact[TaskID] == 0U ) {
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
        /* Put the TASK in ready queue */
        EE_oo_task_in_ready_queue(TaskID);

        /* Check for preemption: 
           this test has to be done only if we are inside a task */ 
        if ( EE_hal_get_IRQ_nesting_level() == 0U ) {
          /* we are inside a task */
          EE_oo_preemption_point();
        }
      }
      ev = E_OK;
    }
#if defined(__RN_TASK__) || defined(EE_AS_RPC__)
  }
#endif /* __RN_TASK__ || EE_AS_RPC__ */

  if ( ev != E_OK ) {
    EE_ORTI_set_lasterror(ev);
    EE_oo_notify_error_ActivateTask(TaskID, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_ACTIVATETASK);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}
#endif /* !__PRIVATE_ACTIVATETASK__ */

