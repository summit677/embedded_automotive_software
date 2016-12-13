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
 * CVS: $Id: ee_terminat.c,v 1.1 2005/07/16 12:23:42 pj Exp $
 */

#include "ee_internal.h"

/* TerminateTask:
   - The task is moved from the running state to the suspended state
   - automatic release of internal resource
   - no check for pending resources left by the task
   - Rescheduling is issued
   - returns (only extended state)
       E_OS_RESOURCE if the task still occupy resources
       E_OS_CALLLEVEL if called at interrupt level
*/

#ifndef __PRIVATE_TERMINATETASK__
StatusType EE_oo_TerminateTask(void)
{
  /* Error Value */
  register StatusType ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_TERMINATETASK);

  EE_as_monitoring_the_stack();

#ifdef EE_SERVICE_PROTECTION__
  /*  [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */
#if defined(__OO_EXTENDED_STATUS__) || defined(EE_SERVICE_PROTECTION__)
  /*  [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      “invalid value” of  the service. (BSW11009, BSW11013) */
  /* check for a call at interrupt level
   * This must be the FIRST Check!!!
   */
  if ( (EE_hal_get_IRQ_nesting_level() != 0U)
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
  if ( EE_th_resource_last[EE_stk_queryfirst()] != EE_UREG_MINUS1 ) {
    ev = E_OS_RESOURCE;
  } else
#endif /* __OO_NO_RESOURCES__ */

#ifdef EE_AS_USER_SPINLOCKS__
  /* [OS612]: In extended status TerminateTask / ChainTask shall return with an
      error (E_OS_SPINLOCK), which can be evaluated in the application.
      (BSW4080021) */
  if (EE_as_spinlocks_last[EE_CURRENTCPU] != INVALID_SPINLOCK) {
    ev = E_OS_SPINLOCK;
  } else
#endif /* EE_AS_USER_SPINLOCKS__ */
#endif /* __OO_EXTENDED_STATUS__ */
  {
#ifndef __OO_NO_CHAINTASK__
    EE_th_terminate_nextask[EE_stk_queryfirst()] = EE_NIL;
#endif /* __OO_NO_CHAINTASK__ */
    ev = E_OK;
  }

  if ( ev != E_OK ) {
    EE_ORTI_set_lasterror(ev);
    EE_oo_notify_error_service(OSServiceId_TerminateTask, ev);
  } else {
    /* The following won't never return */
    EE_ORTI_set_service_out(EE_SERVICETRACE_TERMINATETASK);
    EE_hal_terminate_task(EE_stk_queryfirst());  
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_TERMINATETASK);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}
#endif /* __PRIVATE_TERMINATETASK__ */
