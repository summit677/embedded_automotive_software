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
 * CVS: $Id: ee_evget.c,v 1.1 2005/07/16 12:23:42 pj Exp $
 */

#include "ee_internal.h"

/* GetEvent:
   - can be called from a task, from ISR, from error, pre e post taskhook
   - clear the events for the task
   - returns (only extended state)
       E_OS_ID       task id invalid
       E_OS_ACCESS   if the task is not an extended task
       E_OS_STATE    the task id is in the suspended state
*/


#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
#ifndef __PRIVATE_GETEVENT__

StatusType EE_oo_GetEvent(TaskType TaskID, EventMaskRefType Event)
{
  /* Error Value */
  register StatusType ev;
  EE_ORTI_set_service_in(EE_SERVICETRACE_GETEVENT);

#ifdef EE_SERVICE_PROTECTION__
  /*  [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /*  [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      “invalid value” of  the service. (BSW11009, BSW11013) */
  /* GetEvent is callable by Task, ISR2, ErrorHook Pre and Post TaskHook */
  if ( (EE_as_execution_context > ErrorHook_Context) &&
       (EE_as_execution_context != PreTaskHook_Context) &&
       (EE_as_execution_context != PostTaskHook_Context) )
  {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */


#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
#ifdef EE_AS_RPC__
  /*  [OS589] All functions that are not allowed to operate cross core shall
        return E_OS_CORE in extended status if called with parameters that
        require a cross core operation. (BSW4080013) */
  if ( EE_IS_TID_REMOTE(TaskID) ) {
    ev = E_OS_CORE;
  } else
#endif /* EE_AS_RPC__ */
  /* check if the task Id is valid */
  if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
    ev = E_OS_ID;
  } else if ( EE_TASK_ACCESS_ERR(TaskID, EE_as_active_app) ) {
    ev = E_OS_ACCESS;
  } else
#elif defined(__OO_EXTENDED_STATUS__)
#ifdef EE_AS_RPC__
  /* [OS589]: All functions that are not allowed to operate cross core shall
      return E_OS_CORE in extended status if called with parameters that
      require a cross core operation. (BSW4080013) */
  if ( EE_IS_TID_REMOTE(TaskID) ) {
    ev = E_OS_CORE;
  } else
#endif /* EE_AS_RPC__ */
  /* Check if the TASK ID is valid */
  if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
    ev = E_OS_ID;
  } else
#endif /* EE_AS_OSAPPLICATIONS__ || E_SERVICE_PROTECTION__ ||
__OO_EXTENDED_STATUS__ */
#ifdef __OO_EXTENDED_STATUS__
  if ( EE_th_is_extended[TaskID] == 0U ) {
    ev = E_OS_ACCESS;
  } else if ( EE_th_status[TaskID] == SUSPENDED ) {
    ev = E_OS_STATE;
  } else
#endif /* __OO_EXTENDED_STATUS__ */
  if ( Event == NULL ) {
    ev = E_OS_PARAM_POINTER;
  } else
#if defined(EE_AS_OSAPPLICATIONS__) && (defined(EE_SERVICE_PROTECTION__) &&\
  defined(__EE_MEMORY_PROTECTION__))
  /* [SWS_Os_00051] If an invalid address (address is not writable by this
      OS-Application) is passed as an out-parameter to an Operating System
      service, the Operating System module shall return the status code
      E_OS_ILLEGAL_ADDRESS. (SRS_Os_11009, SRS_Os_11013) */
  if ( !OSMEMORY_IS_WRITEABLE(EE_hal_get_app_mem_access(EE_as_active_app,
    Event, sizeof(*Event))) )
  {
    ev = E_OS_ILLEGAL_ADDRESS;
  } else
#endif /* EE_AS_OSAPPLICATIONS__ && __EE_MEMORY_PROTECTION__ &&
  EE_SERVICE_PROTECTION__ */
  {
    /* XXX: This SHALL be atomic. Check this architectures other than TriCore */
    *Event = EE_th_event_active[TaskID];
    ev = E_OK;
  }

  if ( ev != E_OK ) {
    EE_OS_ERROR_PARAMETERS();
    EE_OS_ERROR_PARAMETERS_PARAM1_VALUE((EE_UREG)TaskID);
    EE_OS_ERROR_PARAMETERS_PARAM2_REF(event_ref,Event);

    EE_os_notify_error_from_us(OSServiceId_GetEvent, &error_parameters, ev);
    EE_ORTI_set_service_out(EE_SERVICETRACE_GETEVENT);
  } else {
    EE_ORTI_set_service_out(EE_SERVICETRACE_GETEVENT);
  }

  return ev;
}

#endif /* __PRIVATE_GETEVENT__ */
#endif /* #ifndef __PRIVATE_GETEVENT__ */
