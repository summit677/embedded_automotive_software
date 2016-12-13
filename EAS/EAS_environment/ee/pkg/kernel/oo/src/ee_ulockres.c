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
 * CVS: $Id: ee_ulockres.c,v 1.3 2006/12/03 22:07:50 pj Exp $
 */

#include "ee_internal.h"

/* ReleaseResource:
   - release a resource
   - lock/unlock on the same function level
   - no point of rescheduling inside critical sections!!!
   - returns (only extended state)
       E_OS_ID     if resource number is invalid
       E_OS_ACCESS if resource already locked or interrupt routine 
                   greater than the ceiling priority

   Extended Status: Count for locked resources!!!!
*/

#ifndef __PRIVATE_RELEASERESOURCE__

StatusType EE_oo_ReleaseResource(ResourceType ResID)
{
  /* Error Value */
  register StatusType ev;
  /* Primitive Lock Variable */
#if (!defined(__EE_MEMORY_PROTECTION__)) || defined(__OO_ISR2_RESOURCES__)
  register EE_FREG flag;
#endif /* !__EE_MEMORY_PROTECTION__ || __OO_ISR2_RESOURCES__ */
#ifdef __MSRP__
  register EE_UREG isGlobal;
#endif /* __MSRP__ */
#if defined(__OO_EXTENDED_STATUS__) || defined(__OO_ISR2_RESOURCES__) ||\
    defined(__OO_ORTI_PRIORITY__)
  register EE_TID current = EE_stk_queryfirst();
#endif /* __OO_EXTENDED_STATUS__ || __OO_ISR2_RESOURCES__ ||
  __OO_ORTI_PRIORITY__ */
  /* To cache inside task info */
  register EE_SREG inside_task = (EE_hal_get_IRQ_nesting_level() == 0U);
  /* Primitive Lock Procedure */
  EE_OS_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_RELEASERESOURCE);

  EE_as_monitoring_the_stack();

#ifdef __MSRP__
  isGlobal = EE_oo_isGlobal(ResID);
  ResID = ResID & ~EE_GLOBAL_MUTEX;
#endif /* __MSRP__ */

#ifdef __OO_ISR2_RESOURCES__
  /* If actually we are inside an ISR2 get the fake TID to access stack */
  if ( inside_task == 0 ) {
    current = EE_oo_get_ISR2_TID();
  }
#endif /* __OO_ISR2_RESOURCES__ */

#ifdef EE_SERVICE_PROTECTION__
  /*  [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
       Task/OsIsr calls any OS service (excluding the interrupt services)
       then the Operating System shall ignore the service AND shall return
       E_OS_DISABLEDINT if the service returns a StatusType value. */
  /*  [OS088]: If an OS-Application makes a service call from the wrong context
       AND is currently not inside a Category 1 ISR the Operating System module
       shall not perform the requested action (the service call shall have no
       effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
       "invalid value" of the service. (BSW11009, BSW11013) */
  /* ReleaseResource is callable by Task and ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
  /* no comparison for ResID < 0, the type is unsigned! */
  if ( ResID >= EE_MAX_RESOURCE ) {
    ev = E_OS_ID;
  } else if ( EE_RESOURCE_ACCESS_ERR(ResID, EE_as_active_app) ) {
    ev = E_OS_ACCESS;
  } else
#elif defined(__OO_EXTENDED_STATUS__)
  /* no comparison for ResID < 0, the type is unsigned! */
  if ( ResID >= EE_MAX_RESOURCE ) {
    ev = E_OS_ID;
  } else
#endif /* EE_AS_OSAPPLICATIONS__ || E_SERVICE_PROTECTION__ ||
__OO_EXTENDED_STATUS__ */

#ifdef __OO_EXTENDED_STATUS__
  if ( (EE_resource_locked[ResID] == 0U) ||
        (EE_th_resource_last[current] != ResID) ) {
    ev = E_OS_NOFUNC;
  } else if ( (inside_task != 0) && ((current == EE_NIL) ||
      (EE_th_ready_prio[current] > EE_resource_ceiling[ResID])) ) {
    ev = E_OS_ACCESS;
  } else
#ifdef __OO_ISR2_RESOURCES__
  /* Check if interrupt controller priority is greater than priority
     of resource to be realeased */
  if ( (inside_task == 0) &&
    (EE_hal_check_int_prio_if_higher(EE_resource_isr2_priority[ResID]) !=
     0U) )
  {
    ev = E_OS_ACCESS;
  } else
#else /* __OO_ISR2_RESOURCES__ */
  if ( inside_task == 0 ) {
    ev = E_OS_ACCESS;
  } else
#endif /* __OO_ISR2_RESOURCES__ */
#endif /* __OO_EXTENDED_STATUS__ */
  {
    /* Stop TP Lock Budget, if needed */
    EE_as_tp_active_stop_budget(EE_RESOURCE_LOCK_BUDGET, ResID, EE_FALSE);

#ifdef __OO_ISR2_RESOURCES__
    /* Restore old ISR2 priority */
    flag = EE_hal_change_int_prio(EE_isr2_oldpriority[ResID], flag);
#endif /* __OO_ISR2_RESOURCES__ */

#if defined(__OO_EXTENDED_STATUS__) || defined(__OO_ISR2_RESOURCES__)
    /* remove the last entry from the data structure */
    EE_th_resource_last[current] = 
      EE_resource_stack[EE_th_resource_last[current]];
#endif /* __OO_EXTENDED_STATUS__ || __OO_ISR2_RESOURCES__ */

#if defined(__OO_EXTENDED_STATUS__) || defined(__OO_ORTI_RES_ISLOCKED__)
    /* ok, we have to free that resource! */
    EE_resource_locked[ResID] = 0U;
#endif /* __OO_EXTENDED_STATUS__ || __OO_ORTI_RES_ISLOCKED__ */

#ifdef __MSRP__
    /* if this is a global resource, unlock the others CPUs */
    if ( isGlobal ) {
      EE_hal_spin_out((EE_TYPESPIN)ResID);
    }
#endif /* __MSRP__ */

    /* there is no need to store that the resource has no more lockers,
       because we inserted an if expression into the ORTI File */
    /* #ifdef __OO_ORTI_RES_LOCKER_TASK__ */
    /*  EE_ORTI_res_locker[ResID] = EE_NIL; */
    /* #endif */

    EE_sys_ceiling = EE_resource_oldceiling[ResID];

#ifdef __OO_ORTI_PRIORITY__
    EE_ORTI_th_priority[current] = EE_ORTI_resource_oldpriority[ResID];
#endif /* __OO_ORTI_PRIORITY__ */

    /* Check if there is a preemption
       this test has to be done only if we are inside a task */
    if ( inside_task ) {
      /* we are inside a task */
      EE_oo_preemption_point();
    }
    ev = E_OK;
  }

  if ( ev != E_OK ) {
    EE_ORTI_set_lasterror(ev);
    EE_oo_notify_error_ReleaseResource(ResID, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_RELEASERESOURCE);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

#endif /* __PRIVATE_RELEASERESOURCE__ */
