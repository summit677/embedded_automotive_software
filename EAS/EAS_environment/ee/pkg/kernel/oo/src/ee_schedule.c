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
 * CVS: $Id: ee_schedule.c,v 1.1 2005/07/16 12:23:42 pj Exp $
 */

#include "ee_internal.h"

/* Schedule: 
    - the internal resource is released; 
    - no check if resources are still used by the task
    - then rescheduling takes place 
    - and then the internal resource is taken again
    - returns
      Extended status
      E_OS_CALLLEVEL if called at interrupt level 
      E_OS_RESOURCE if the calling task occupies resources
*/

#ifndef __PRIVATE_SCHEDULE__

StatusType EE_oo_Schedule(void)
{
  register EE_TID current, rq;
  /* Error Value */
  register StatusType ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_SCHEDULE);

  EE_as_monitoring_the_stack();

  current = EE_stk_queryfirst();
  rq = EE_rq_queryfirst();

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
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
  /* check for a call at interrupt level */
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
  if ( EE_th_resource_last[current] != EE_UREG_MINUS1 )
  {
    ev = E_OS_RESOURCE;
  } else
#endif /* __OO_NO_RESOURCES__ */

#ifdef EE_AS_USER_SPINLOCKS__
  /* [OS624]: The AUTOSAR Operating System Schedule API service shall check if it
      has been called while the calling TASK has occupied a spinlock. In
      extended status an error E_OS_SPINLOCK shall be returned and the scheduler
      shall not be called. (BSW4080021) */
  if ( EE_as_spinlocks_last[EE_CURRENTCPU] != INVALID_SPINLOCK ) {
    ev = E_OS_SPINLOCK;
  } else
#endif /* EE_AS_USER_SPINLOCKS__ */

#endif /* __OO_EXTENDED_STATUS__ */

#if defined(EE_AS_OSAPPLICATIONS__) && defined(__EE_MEMORY_PROTECTION__)
#if defined(EE_SYSCALL_NR) && defined(EE_MAX_SYS_SERVICEID) &&\
  (EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID)
  /*  If a TASK is inside CallTrustedFunction() and TASK
      rescheduling takes place within the same OSApplication scheduling of
      other TASKs which belong to the same OS-Application as the caller needs
      to be restricted.
      EG: To Assure that I CANNOT let a TASK release internal resource, if this
        means schedule a TASK of the same OSApplication, unless the
        OSApplication is TRUSTED. */
  if (  (EE_as_active_app == EE_th_app[rq + 1]) &&
        (EE_as_Application_RAM[EE_as_active_app].
          TrustedFunctionCallsCounter != 0U) &&
        (EE_as_Application_ROM[EE_as_active_app].Mode != EE_MEMPROT_TRUST_MODE )
     )
  {
    ev = E_OK;
  } else
#endif /* EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID */
#endif /* EE_AS_OSAPPLICATIONS__ && __EE_MEMORY_PROTECTION__ */
  /* Check if there is a preemption */
  if ( rq != EE_NIL ) {
    /* The standard says that "Schedule enables a processor assignment
       to other TASKs with lower priority than the ceiling priority of
       the internal resource and higher priority than the priority of
       the calling TASK". That means that only TASKs currently in the
       ready queue with the ready priority > than the ready priority
       of the running TASK can be executed... */
    if ( EE_th_ready_prio[current] < EE_th_ready_prio[rq] ) {
      EE_oo_call_PostTaskHook();
      /* release the internal resource */
      EE_sys_ceiling &= ~EE_th_dispatch_prio[current];

      /* set the ready priority bit. In that way we prevent preemption
       * from all the tasks with lower priority than the current task.
       *
       * NOTE: Setting the ready priority is legal because if the task
       * has been scheduled it must be that the system_ceiling <
       * ready_priority, and so (system_ceiling &ready_priority)=0!!!
       * after a task has been put in execution, the dispatch priority
       * is set. no other bits are set when this function is called
       * (all the resources must be unlocked, and all the task that
       * preempts the running task must have been finished!.
       */
      EE_sys_ceiling |= EE_th_ready_prio[current];

      /* we have to put the task in the ready status */
      EE_th_status[current] = READY;
      /* but not in the ready queue!!! 
         the task remains into the stacked queue!
      */

      /* get the new internal resource */
      EE_sys_ceiling |= EE_th_dispatch_prio[rq];
      /* put the task in running state */
      EE_th_status[rq] = RUNNING;

      EE_ORTI_set_th_eq_dispatch_prio(current);
      EE_ORTI_set_th_eq_dispatch_prio(rq);

      /* Execute context SWITCH, this method return when we have a switch
         back on the previous TASK contest. */
      EE_oo_run_next_task();

      /* release the ready priority bit and... */
      EE_sys_ceiling &= ~EE_th_ready_prio[current];
      /* ...get again the internal resource */
      EE_sys_ceiling |= EE_th_dispatch_prio[current];

      EE_ORTI_set_th_eq_dispatch_prio(current);

      /* Call PreTaskHook in the first TASK context */
      EE_oo_call_PreTaskHook();
    }
    ev = E_OK;
  } else {
    ev = E_OK;
  }

  if ( ev != E_OK ) {
    EE_ORTI_set_lasterror(ev);
    EE_oo_notify_error_service(OSServiceId_Schedule, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_SCHEDULE);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

#endif /* __PRIVATE_SCHEDULE__ */

