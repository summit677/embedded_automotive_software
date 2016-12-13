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
 * CVS: $Id: ee_evset.c,v 1.4 2005/11/03 09:39:48 pj Exp $
 */

#include "ee_internal.h"

/* SetEvent:
   - can be called from ISR and from task level
   - if the task was waiting for one of the events -> wake up it!
   - returns (only extended state)
       E_OS_ID     if task id is invalid
       E_OS_ACCESS if the referenced task is not an extended task
       E_OS_STATE  events cannot be set because the task is in the
                   suspended state

   Note: The implementation of this function can be a little bit
   optimized.  That is, we should check in which queue the task really
   goes, then eventually inserting it into the ready queue...

   Note: part of this code is present also in altick.c!
*/


#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
#ifndef __PRIVATE_SETEVENT__

StatusType EE_oo_SetEvent(TaskType TaskID, EventMaskType Mask)
{
  /* Error Value */
  register StatusType ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_SETEVENT);

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
  /* SetEvent is callable by Task and ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

#if defined(__RN_EVENT__) || defined(EE_AS_RPC__)
  if ( EE_IS_TID_REMOTE(TaskID) )
  {
#ifdef EE_AS_RPC__
    /* Tmp Tid (introduced to meet MISRA requirements) */
    EE_TID tmp_tid;

    EE_os_param unmarked_tid;
    EE_os_param as_mask;

    /* Two steps macro assignment to meet MISRA 10.3 required rule */
    tmp_tid = EE_UNMARK_REMOTE_TID(TaskID);
    unmarked_tid.value_param = (EE_UREG)tmp_tid;
    as_mask.value_param      = Mask;

    /* Forward the request to another CPU in synchronous way */
    ev = EE_as_rpc(OSServiceId_SetEvent, unmarked_tid, as_mask,
      EE_OS_INVALID_PARAM);
#else /* EE_AS_RPC__ */
    /* Forward the request to another CPU when the task does
       not belong to the current CPU */
    register EE_TYPERN_PARAM par;
    par.ev = Mask;
    (void)EE_rn_send((EE_SREG)EE_MARK_REMOTE_TID(TaskID),
      EE_RN_EVENT, par);
    ev = E_OK;
#endif /* EE_AS_RPC__ */
  } else {
#endif /* __RN_EVENT__ || EE_AS_RPC__ */

#ifdef __OO_EXTENDED_STATUS__
    /* Check if the TASK Id is valid */
    if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
      ev = E_OS_ID;
    } else if ( EE_th_is_extended[TaskID] == 0U ) {
      ev = E_OS_ACCESS;
    } else
#endif /* __OO_EXTENDED_STATUS__ */

    if ( EE_th_status[TaskID] == SUSPENDED ) {
      ev = E_OS_STATE;
    } else {
      /* Set the event mask only if the task is not suspended */
      EE_th_event_active[TaskID] |= Mask;

      /* Check if the TASK was waiting for an event we just set
       *
       * WARNING:
       * the test with status==WAITING is FUNDAMENTAL to avoid double
       * insertion of the task in the ready queue!!! Example, when I call
       * two times the same setevent... the first time the task must go in
       * the ready queue, the second time NOT!!!
       */
      if ( ((EE_th_event_waitmask[TaskID] & Mask) != 0U) &&
        (EE_th_status[TaskID] == WAITING) )
      {
        /* [SWS_Os_00472] The Operating System module shall start an
            OsTaskTimeFrame when a task is released successfully.
            (SRS_Os_11008) */
        /* [SWS_Os_00467] If an attempt is made to release a task before
            the end of an OsTaskTimeFrame then the Operating System module
            shall not perform the release AND shall call the
            ProtectionHook() with E_OS_PROTECTION_ARRIVAL AND the EVENT
            SHALL BE SET. */
        if ( EE_as_tp_handle_interarrival(EE_AS_TP_ID_FROM_TASK(TaskID)) ) {
          /* If yes, the task must go back into the READY state */
          EE_th_status[TaskID] = READY;
          /* Insert the task in the ready queue... */
          EE_rq_insert(TaskID);

          /* If I am at task level, check for preemption... */
          if ( EE_hal_get_IRQ_nesting_level() == 0U ) {
            /* We are inside a task */
            EE_oo_preemption_point();
          }
        }
      }
      ev = E_OK;
    }
#if defined(__RN_EVENT__) || defined(EE_AS_RPC__)
  }
#endif /* __RN_EVENT__ || EE_AS_RPC__ */

  if ( ev != E_OK ) {
    EE_ORTI_set_lasterror(ev);
    EE_oo_notify_error_SetEvent(TaskID, Mask, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_SETEVENT);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

#endif /* __PRIVATE_SETEVENT__ */
#endif /* defined(__OO_ECC1__) || defined(__OO_ECC2__) */

