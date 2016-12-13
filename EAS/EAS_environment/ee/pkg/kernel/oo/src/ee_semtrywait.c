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
 * CVS: $Id: ee_semtrywait.c,v 1.1 2006/05/03 05:59:55 pj Exp $
 */

#include "ee_internal.h"

/* TryWaitSem:
   - can be called from ISR2, basic and extended tasks, and the main task
   - If the semaphore counter is 0, the primitive returns a non-zero value. 
   - If the semaphore counter has a value greater than 0, then the counter is decremented and the primitive returns 0.
   - This primitive never blocks the calling task, and it never issues
     a rescheduling.
*/


#ifndef __PRIVATE_TRYWAITSEM__
int EE_oo_TryWaitSem(SemRefType Sem)
{
  /* Error Value */
  register StatusType ev;
  int returnvalue = EE_NIL;

  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_TRYWAITSEM);

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

  /* check if we have to wait */
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
      returnvalue = -1;
    } else {
#else /* EE_AS_OSAPPLICATIONS__ && __EE_MEMORY_PROTECTION__ &&
  EE_SERVICE_PROTECTION__ */
    {
#endif /* EE_AS_OSAPPLICATIONS__ && __EE_MEMORY_PROTECTION__ &&
  EE_SERVICE_PROTECTION__ */
      if ( Sem->count ) {
        Sem->count--;
        returnvalue = 0;
      }
      else {
        returnvalue = 1;
      }
      ev = E_OK;
    }
  } else {
    ev = E_OS_PARAM_POINTER;
  }

  if ( ev != E_OK )
  {
    EE_OS_PARAM(os_sem);
    EE_OS_PARAM_REF(os_sem, sem_ref, Sem);
    EE_os_notify_error(OSServiceId_TryWaitSem, os_sem,
      EE_OS_INVALID_PARAM, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_TRYWAITSEM);

  EE_OS_EXIT_CRITICAL_SECTION();

  return returnvalue;
}
#endif
