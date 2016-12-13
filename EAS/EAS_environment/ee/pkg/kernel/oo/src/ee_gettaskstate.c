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
 * Author: 2004 Paolo Gai
 * CVS: $Id: ee_gettaskstate.c,v 1.1 2005/07/16 12:23:42 pj Exp $
 */

#include "ee_internal.h"

/***************************************************************************
 * 13.2 Task management 
 ***************************************************************************/

/* 13.2.3.6: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_GETTASKSTATE__

StatusType EE_oo_GetTaskState(TaskType TaskID, TaskStateRefType State)
{
  register StatusType ev;

  EE_ORTI_set_service_in(EE_SERVICETRACE_GETTASKSTATE);

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
  /* GetTaskID is callable by Task, ISR2, ErrorHook, ProtectionHook, Pre and
     Post TaskHook */
  if ( (EE_as_execution_context > ErrorHook_Context) &&
       (EE_as_execution_context != PreTaskHook_Context) &&
       (EE_as_execution_context != PostTaskHook_Context) )
  {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

  /* [OS566]: The Operating System API shall check in extended mode all pointer
     argument for NULL pointer and return OS_E_PARAMETER_POINTER
     if such argument is NULL. 
     + 
     MISRA dictate NULL check for pointers always. */
  if ( State == NULL ) {
    ev = E_OS_PARAM_POINTER;
  } else
#if defined(EE_AS_OSAPPLICATIONS__) && (defined(EE_SERVICE_PROTECTION__) &&\
  defined(__EE_MEMORY_PROTECTION__))
  /* [SWS_Os_00051]: If an invalid address (address is not writable by this
      OS-Application) is passed as an out-parameter to an Operating System
      service, the Operating System module shall return the status code
      E_OS_ILLEGAL_ADDRESS. (SRS_Os_11009, SRS_Os_11013) */
  if ( !OSMEMORY_IS_WRITEABLE( EE_hal_get_app_mem_access(EE_as_active_app,
    State, sizeof(*State))) )
  {
    ev = E_OS_ILLEGAL_ADDRESS;
  } else
#endif /* EE_AS_OSAPPLICATIONS__ && EE_SERVICE_PROTECTION__ &&
  __EE_MEMORY_PROTECTION__ */

#ifdef EE_AS_RPC__
  if ( EE_IS_TID_REMOTE(TaskID) )
  {
    /* Tmp Tid (introduced to meet MISRA requirements) */
    EE_TID tmp_tid;
    EE_os_param as_state;
    EE_os_param unmarked_tid;
	/* Two steps macro assignment to meet MISRA 10.3 required rule */
    tmp_tid = EE_UNMARK_REMOTE_TID(TaskID);
    unmarked_tid.value_param = (EE_UREG)tmp_tid;
    as_state.task_state_ref = State;

    /* forward the request to another CPU in synchronous way */
    ev = EE_as_rpc_from_us(OSServiceId_GetTaskState, unmarked_tid, as_state,
      EE_OS_INVALID_PARAM);
  } else {
#endif /* EE_AS_RPC__ */

#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
    /* check if the task Id is valid */
    if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
      ev = E_OS_ID;
    } else if ( EE_TASK_ACCESS_ERR(TaskID, EE_as_active_app) ) {
      ev = E_OS_ACCESS;
    } else
#elif defined(__OO_EXTENDED_STATUS__)
    /* check if the task Id is valid */
    if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
      ev = E_OS_ID;
    } else
#endif /* EE_AS_OSAPPLICATIONS__ || E_SERVICE_PROTECTION__ ||
__OO_EXTENDED_STATUS__ */
    {
    /* XXX: This SHALL be atomic. Check this architectures other than TriCore */
      *State = EE_th_status[TaskID];
      ev = E_OK;
    }
#ifdef EE_AS_RPC__
  }
#endif /*  EE_AS_RPC__ */

  if ( ev != E_OK ) {
    EE_OS_ERROR_PARAMETERS();
    EE_OS_ERROR_PARAMETERS_PARAM1_REF(task_state_ref,State);

    EE_os_notify_error_from_us(OSServiceId_GetTaskState, &error_parameters, ev);
    EE_ORTI_set_service_out(EE_SERVICETRACE_GETTASKSTATE);
  } else {
    EE_ORTI_set_service_out(EE_SERVICETRACE_GETTASKSTATE);
  }

  return ev;
}
#endif /* __PRIVATE_GETTASKSTATE__ */

