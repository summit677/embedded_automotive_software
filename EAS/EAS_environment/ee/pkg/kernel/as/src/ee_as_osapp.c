/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2011  Evidence Srl
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

 /**  @file   ee_as_osapp.c
  *   @brief  API related to OS-applications
  *   @author Bernardo  Dal Seno
  *   @date 2012
  *   @author Errico Guidieri
  *   @date 2013
  */

#include "ee_internal.h"

#if 0 /* Begin Comment */
ApplicationType EE_as_GetApplicationID_internal ( void )
{
  ApplicationType app;
#if defined(EE_MAX_ISR2) && (EE_MAX_ISR2 > 0)
  EE_UREG irqnest;
  irqnest = EE_hal_get_IRQ_nesting_level();
  if ( irqnest > 0U ) {
    /* Inside an IRQ handler */
    ISRType irq;
    irq = EE_as_ISR_stack[irqnest - 1U].ISR_ID;
    app = EE_as_ISR_ROM[irq].ApplID;
  } else
#endif /* defined(EE_MAX_ISR2) && (EE_MAX_ISR2 > 0) */
  {
    EE_TID t = EE_stk_queryfirst();
    /* if t == INVALID_TASK the result is still correct */
    app = EE_th_app[t + 1];
  }
  return app;
}
#endif /* End Comment */

ApplicationType EE_as_GetApplicationID ( void )
{
  register StatusType       ev;
  register ApplicationType  app;

  EE_ORTI_set_service_in(EE_SERVICETRACE_GETAPPLICATIONID);

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of the service. (BSW11009, BSW11013) */
  /* GetApplicationID is callable by Task, ISR2, ErrorHook, ProtectionHook,
     Pre/Post Task Hook, Startup Hook & ShutdownHook */
  if ( EE_as_execution_context > ShutdownHook_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */
  {
    app = EE_as_active_app;
    ev = E_OK;
  }

  if ( ev != E_OK ) {
    EE_OS_ERROR_PARAMETERS_INIT(EE_OS_INVALID_PARAM,EE_OS_INVALID_PARAM,
      EE_OS_INVALID_PARAM);
    EE_os_notify_error_from_us(OSServiceId_GetApplicationID,
      &error_parameters, ev);

    EE_ORTI_set_service_out(EE_SERVICETRACE_GETAPPLICATIONID);
    app = INVALID_OSAPPLICATION;
  } else {
    EE_ORTI_set_service_out(EE_SERVICETRACE_GETAPPLICATIONID);
  }
  return app;
}

ObjectAccessType EE_as_CheckObjectAccess_internal(ApplicationType ApplID,
  ObjectTypeType ObjectType, EE_os_param_id ObjectID)
{
  /* [OS504] The Operating System module shall deny access to Operating System
      objects from other OS-Applications to an OS-Application which is not in
      state APPLICATION_ACCESSIBLE. */
  ObjectAccessType access = NO_ACCESS;

  if ( (ApplID < EE_MAX_APP) &&
    (EE_as_Application_RAM[ApplID].ApplState == APPLICATION_ACCESSIBLE) )
  {
#ifdef EE_SERVICE_PROTECTION__
    switch ( ObjectType ) {
      case OBJECT_TASK:
        if ( (EE_TID)ObjectID < EE_MAX_TASK ) {
          access = (EE_TASK_ACCESS(ObjectID, ApplID))? ACCESS: NO_ACCESS;
        }
        break;
#if defined(EE_MAX_ISR2) && (EE_MAX_ISR2 > 0)
      case OBJECT_ISR:
        if ( ObjectID < EE_MAX_ISR_ID ) {
          access = (EE_ISR_ACCESS(ObjectID, ApplID))? ACCESS: NO_ACCESS;
        }
        break;
#endif /* EE_MAX_ISR2 > 0 */
#ifndef __OO_NO_RESOURCES__
      case OBJECT_RESOURCE:
        if ( ObjectID < EE_MAX_RESOURCE ) {
          access = (EE_RESOURCE_ACCESS(ObjectID, ApplID))? ACCESS:
            NO_ACCESS;
        }
        break;
#endif /* !__OO_NO_RESOURCES__ */
#if defined(EE_MAX_ALARM) && (EE_MAX_ALARM > 0)
      case OBJECT_ALARM:
        if ( ObjectID < EE_MAX_ALARM ) {
          access = (EE_ALARM_ACCESS(ObjectID, ApplID))? ACCESS: NO_ACCESS;
        }
        break;

      case OBJECT_COUNTER:
        if ( ObjectID < EE_MAX_COUNTER ) {
          access = (EE_COUNTER_ACCESS(ObjectID, ApplID))? ACCESS: NO_ACCESS;
        }
#endif /* EE_MAX_ALARM > 0 */
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)
      case OBJECT_SCHEDULETABLE:
        if ( ObjectID < EE_MAX_SCHEDULETABLE ) {
          access = (EE_SCHED_TABLE_ACCESS(ObjectID, ApplID))? ACCESS:
            NO_ACCESS;
        }
#endif /* EE_MAX_SCHEDULETABLE > 0 */
      default:
        access = NO_ACCESS;
        break;
    }
#else  /* EE_SERVICE_PROTECTION__ */
    access = ACCESS;
#endif /* EE_SERVICE_PROTECTION__ */
  }

  return access;
}

ObjectAccessType EE_as_CheckObjectAccess(ApplicationType ApplID,
  ObjectTypeType ObjectType, EE_os_param_id ObjectID)
{
  register StatusType       ev;
  register ObjectAccessType access;

  EE_ORTI_set_service_in(EE_SERVICETRACE_CHECKOBJECTACCESS);

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of the service. (BSW11009, BSW11013) */
  /* CheckObjectAccess is callable by Task, ISR2, ErrorHook and ProtectionHook */
  if ( EE_as_execution_context > ProtectionHook_Context )
  {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */
  {
    access = EE_as_CheckObjectAccess_internal(ApplID, ObjectType, ObjectID);
    ev = E_OK;
  }

  if ( ev != E_OK) {
    EE_OS_ERROR_PARAMETERS_INIT({ApplID},{ObjectType},{ObjectID});
    EE_os_notify_error_from_us(OSServiceId_CheckObjectAccess,
      &error_parameters, ev);
    EE_ORTI_set_service_out(EE_SERVICETRACE_CHECKOBJECTACCESS);
    access = NO_ACCESS;
  } else {
    EE_ORTI_set_service_out(EE_SERVICETRACE_CHECKOBJECTACCESS);
  }

  return access;

}

#ifdef EE_SERVICE_PROTECTION__
ApplicationType EE_as_CheckObjectOwnership_internal(ObjectTypeType ObjectType,
  EE_os_param_id ObjectID)
{
  ApplicationType app = INVALID_OSAPPLICATION;

  switch ( ObjectType ) {
    case OBJECT_TASK:
      if ( (EE_TID)ObjectID < EE_MAX_TASK ) {
        app = EE_th_app[(ObjectID + 1U)];
      }
      break;

#if defined(EE_MAX_ISR_ID) && (EE_MAX_ISR_ID > 0)
    case OBJECT_ISR:
      if ( ObjectID < EE_MAX_ISR_ID ) {
        app = EE_as_ISR_ROM[ObjectID].ApplID;
      }
      break;
#endif /* EE_MAX_ISR_ID > 0 */

#if defined(EE_MAX_ALARM) && (EE_MAX_ALARM > 0)
      case OBJECT_ALARM:
      if ( ObjectID < EE_MAX_ALARM ) {
        app = EE_alarm_ROM[ObjectID].ApplID;
      }
      break;

    case OBJECT_COUNTER:
      if ( ObjectID < EE_MAX_COUNTER ) {
        app = EE_counter_ROM[ObjectID].ApplID;
      }
#endif /* EE_MAX_ALARM > 0 */

#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)
    case OBJECT_SCHEDULETABLE:
      if ( ObjectID < EE_MAX_SCHEDULETABLE ) {
        app = EE_as_Schedule_Table_ROM[ObjectID].ApplID;
      }
#endif /* EE_MAX_SCHEDULETABLE > 0 */
    default:
      break;
  }

  return app;
}

ApplicationType EE_as_CheckObjectOwnership(ObjectTypeType ObjectType,
  EE_os_param_id ObjectID)
{
  register StatusType       ev;
  register ApplicationType  app;

  EE_ORTI_set_service_in(EE_SERVICETRACE_CHECKOBJECTOWNERSHIP);

  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of the service. (BSW11009, BSW11013) */
  /* CheckObjectOwnership is callable by Task, ISR2, ErrorHook and
     ProtectionHook */
  if ( EE_as_execution_context > ProtectionHook_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else {
    app = EE_as_CheckObjectOwnership_internal(ObjectType, ObjectID);
    ev = E_OK;
  }

  if ( ev != E_OK ) {
    EE_OS_ERROR_PARAMETERS_INIT({ObjectType},{ObjectID},EE_OS_INVALID_PARAM);

    EE_os_notify_error_from_us(OSServiceId_CheckObjectOwnership,
      &error_parameters, ev);
    EE_ORTI_set_service_out(EE_SERVICETRACE_CHECKOBJECTOWNERSHIP);
    app = INVALID_OSAPPLICATION;
  } else {
    EE_ORTI_set_service_out(EE_SERVICETRACE_CHECKOBJECTOWNERSHIP);
  }

  return app;
}
#endif /* EE_SERVICE_PROTECTION__ */

StatusType EE_as_GetApplicationState( ApplicationType Application,
  ApplicationStateRefType Value )
{
  /* Error Value */
  register StatusType ev;

  EE_ORTI_set_service_in(EE_SERVICETRACE_GETAPPLICATIONSTATE);

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of the service. (BSW11009, BSW11013) */
  /* GetApplicationState is callable by Task, ISR2, ErrorHook, ProtectionHook,
     Pre- and Post- TaskHook, StartupHook and ShutdownHook */
  if ( EE_as_execution_context > ShutdownHook_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */
  /* [OS566]: The Operating System API shall check in extended mode all
      pointer argument for NULL pointer and return OS_E_PARAM_POINTER
      if such argument is NULL. +
    MISRA dictate NULL check for pointers always. */
  if ( Value == NULL ) {
    ev = E_OS_PARAM_POINTER;
  } else
#if defined(EE_SERVICE_PROTECTION__) && defined(__EE_MEMORY_PROTECTION__)
  /* [SWS_Os_00051]: If an invalid address (address is not writable by this
      OS-Application) is passed as an out-parameter to an Operating System
      service, the Operating System module shall return the status code
      E_OS_ILLEGAL_ADDRESS. (SRS_Os_11009, SRS_Os_11013) */
  if ( !OSMEMORY_IS_WRITEABLE(EE_hal_get_app_mem_access(EE_as_active_app, Value,
      sizeof(*Value))) ) {
    ev = E_OS_ILLEGAL_ADDRESS;
  } else
#endif /* EE_SERVICE_PROTECTION__ && __EE_MEMORY_PROTECTION__ */

#ifdef EE_AS_RPC__
  if ( EE_AS_ID_REMOTE(Application) ) {
    EE_os_param as_app_state_ref;
    EE_os_param const unmarked_app_id = { EE_AS_UNMARK_REMOTE_ID(Application) };
    as_app_state_ref.application_state_ref = Value;
    /* Forward the request to another CPU in synchronous way */
    ev = EE_as_rpc_from_us(OSServiceId_GetApplicationState, unmarked_app_id,
      as_app_state_ref, EE_OS_INVALID_PARAM);
  } else {
#endif /* EE_AS_RPC__ */
#ifdef __OO_EXTENDED_STATUS__
    /* [OS495]: If the <Application> in a call of GetApplicationState() is not
        valid GetApplicationState() shall return E_OS_ID. */
    if ( Application >= EE_MAX_APP ) {
      ev = E_OS_ID;
    } else
#endif /* __OO_EXTENDED_STATUS__ */
    {
      /* XXX: ApplicationStateType is an enum so an integer, so a
          read it SHOULD Atomic (Check this in Architectures other than TriCore) */
      (*Value) = EE_as_Application_RAM[Application].ApplState;
      ev = E_OK;
    }
#ifdef EE_AS_RPC__
  }
#endif /* EE_AS_RPC__ */

  if ( ev != E_OK ) {
    EE_OS_ERROR_PARAMETERS();
    EE_OS_ERROR_PARAMETERS_PARAM1_VALUE(Application);
    EE_OS_ERROR_PARAMETERS_PARAM2_REF(application_state_ref,Value);

    /* Begin the critical section here */
    EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();
    EE_os_notify_error_from_us(OSServiceId_GetApplicationState,
      &error_parameters, ev);
    EE_ORTI_set_service_out(EE_SERVICETRACE_GETAPPLICATIONSTATE);
    EE_OS_EXIT_CRITICAL_SECTION();
  } else {
    EE_ORTI_set_service_out(EE_SERVICETRACE_GETAPPLICATIONSTATE);
  }

  return ev;
}

/* ==========================================================================
                        OSApplication Termination
   ========================================================================== */

void EE_as_terminate_current_app_task( void )
{
  register EE_TID current_task = EE_stk_queryfirst();
  /* If current task still holds OSEK Resources, release them. */
  (void)EE_oo_release_all_resources(current_task);

  /* Force the spinlock release for current task */
  (void)EE_as_release_all_spinlocks(current_task);

#ifndef __OO_NO_CHAINTASK__
  EE_th_terminate_nextask[current_task] = EE_NIL;
#endif /* __OO_NO_CHAINTASK__ */

#if defined(__OO_BCC2__) || defined(__OO_ECC2__)
  /* This Trick will fix the problem of MAX activation that grows if the
     TASK is RUNNING and another instance of the same TASK is READY in the
     moment of termination of its OSApplication */
  if ( EE_th_rnact[current_task] == EE_th_rnact_max[current_task] ) {
    --EE_th_rnact[current_task];
  }
#endif /* __OO_BCC2__ || __OO_ECC2__ */

  EE_hal_terminate_task(current_task);
}

static void EE_as_task_other_cleanup( EE_TID tid ) {
  /* Reset remaining activations...*/
#if defined(__OO_BCC1__) || defined(__OO_ECC1__)
  EE_th_rnact[tid] = 1;
#else /* __OO_BCC1__ || __OO_ECC1__ */
  EE_th_rnact[tid] = EE_th_rnact_max[tid];
#endif /* __OO_BCC1__ || __OO_ECC1__ */

  /* Reset ORTI priority if needed */
  EE_ORTI_set_th_priority(tid, 0U);

  /* Put TASK status as Suspended */
  EE_th_status[tid] = SUSPENDED;
#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
  /* Remove events eventually set (I could check but if it is an extended
     TASK with EE_th_is_extended[] array, but it would be only cycles time). */
  EE_th_event_active[tid] = 0U;
#endif /* __OO_ECC1__ || __OO_ECC2__ */

  /* [SWS_Os_00473]: The Operating System module shall reset a task’s
      OsTaskExecutionBudget on a transition to the SUSPENDED or WAITING states.
      (SRS_Os_11008) */
  /* The TASK shall be as it was when the OS Started: Reset Everything */
  EE_as_tp_reset_budgets(EE_AS_TP_ID_FROM_TASK(tid));
  EE_as_tp_stop_interarrival_frame (EE_AS_TP_ID_FROM_TASK(tid));
}

static void EE_as_terminate_preempted_task( EE_TID tid ) {
  /* If task still holds OSEK Resources, release them. */
  (void)EE_oo_release_all_resources(tid);

  /* Force the spinlock release for task */
  (void)EE_as_release_all_spinlocks(tid);

  /* Reset the thread priority bit in the system_ceiling. */
  EE_sys_ceiling &= ~EE_th_dispatch_prio[tid];

  /* Make the clean-up of other data structures related to scheduler */
  EE_as_task_other_cleanup(tid);
  /* TASK stack rewind */
  EE_hal_terminate_other_task(tid);
}

/* Terminate stacked TASKs belonging to app */
static void EE_as_app_terminate_stacked_tasks( ApplicationType app )
{
  /* Indexes used to traverse the stacked queue */
  EE_TID current, previous, next;

  /* Remove TASKs from the stacked queue starting from the first preempted
     task. (Current stacked TASK eventually will be terminated at the end of
     primitive) */
  previous  = EE_stk_queryfirst();
  current   = EE_th_next[previous];
  while ( current != EE_NIL ) {
    /* Get the next */
    next = EE_th_next[current];
    /* Check if the stacked task have to be terminated */
    if ( EE_th_app[current + 1] == app ) {
      /* Terminate the TASK */
      EE_as_terminate_preempted_task(current);
      /* Removing the task from the queue */
      EE_th_next[previous] = next;
    } else {
      /* Otherwise current will be next previous */
      previous = current;
    }
    current = next;
  }
}

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
static void EE_as_app_terminate_waiting_tasks( ApplicationType app )
{
  /* Index used to cycle over TASKs' status */
  EE_TID tid;
  for ( tid = 0; tid < EE_MAX_TASK; ++tid ) {
    /* If the TASK and belong to the given app... */
    if ( EE_th_app[tid + 1] == app ) {
        /* and it is waiting... */
      if ( EE_th_status[tid] == WAITING ) {
        /* It have to be terminated. ( Do not check for resources and spinlocks
           release, a TASK MUST NOT hold any resource/spinlock before wait ) */

        /* Remove the wait flag, mask and active events */
        EE_th_waswaiting[tid]     = 0U;
        EE_th_event_waitmask[tid] = 0U;
        EE_th_event_active[tid]   = 0U;

        /* Reset remaining activations...*/
#ifdef __OO_ECC1__
        EE_th_rnact[tid] = 1;
#else
        EE_th_rnact[tid] = EE_th_rnact_max[tid];
#endif /* __OO_ECC1__ */

        /* Reset ORTI priority if needed */
        EE_ORTI_set_th_priority(tid, 0U);
        /* Put TASK status as Suspended */
        EE_th_status[tid] = SUSPENDED;

        /* Make the clean-up of other data structures related to scheduler */
        EE_as_task_other_cleanup(tid);
        /* TASK stack rewind */
        EE_hal_terminate_other_task(tid);
      }
    }
  }
}
#else /* __OO_ECC1__ || __OO_ECC2__ */
/* No waiting TASKs */
#define EE_as_app_terminate_waiting_tasks(app) ((void)0U)
#endif /* __OO_ECC1__ || __OO_ECC2__ */
/**
 * This function remove tasks belonging to an application from the ready queue
 */
#if defined(__OO_BCC1__) || defined(__OO_ECC1__)
static void EE_as_terminate_app_tasks( ApplicationType app )
{
  /* Indexes used to traverse the ready queue */
  EE_TID current, previous;

  /* Terminate stacked tasks belonging to app */
  EE_as_app_terminate_stacked_tasks(app);
  /* If needed I have to cycle to search eventual TASK in waiting status to be
     Terminated */
  EE_as_app_terminate_waiting_tasks(app);

  /* Move the head of the the ready queue until the queue is not EMPTY and the
     first element doesn't belong to app */
  while ( (EE_rq_first != EE_NIL) && (EE_th_app[EE_rq_first + 1] == app) ) {
    /* Terminate the task */
    EE_as_task_other_cleanup(EE_rq_first);
    EE_rq_first = EE_th_next[EE_rq_first];
  }

  /* Remove elements inside ready queue, if the queue is still NOT EMPTY */
  current = EE_rq_first;
  previous = current;

  while ( current != EE_NIL ) {
    /* Get the next */
    EE_TID next = EE_th_next[current];
    /* If the current have to be removed */
    if ( EE_th_app[current + 1] == app ) {
      /* Make the clean-up of other data structures related to ready queue */
      EE_as_task_other_cleanup(current);
      /* Assign previous-next to next */
      EE_th_next[previous] = next;
    } else {
      /* Otherwise current will be next previous */
      previous = current;
    }
    /* Move the current to next at the end */
    current = next;
  }
}
#elif defined(__OO_BCC2__) || defined(__OO_ECC2__)

static void EE_as_terminate_app_tasks( ApplicationType app )
{

  /* First non-empty queue */
  EE_INT8 x;
  /* Indexes used to traverse the priority-ready queue */
  EE_TYPEPAIR current, previous, next;
  /* Hold "current" tid */
  EE_TID tid;

  /* Terminate stacked tasks belonging to app */
  EE_as_app_terminate_stacked_tasks(app);
  /* If needed I have to cycle to search eventual TASK in waiting status to be
     Terminated */
  EE_as_app_terminate_waiting_tasks(app);

  x = EE_rq_get_first_not_empty_queue();
  while ( x != (EE_INT8)-1 ) {
    /* Move the head of the the priority ready queue until the queue is not
       EMPTY and the first element doesn't belong to app */
    current = EE_rq_queues_head[x];
    while ( current != (EE_TYPEPAIR)-1 ) {
      /* Get the next */
      next = EE_rq_pairs_next[current];
      /* Hold "current" tid */
      tid = EE_rq_pairs_tid[current];

      if (EE_th_app[tid + 1] == app) {
        /* Free the descriptor */
        EE_rq_pairs_next[current] = EE_rq_free;
        EE_rq_free = current;

        /* Move the head of ready priority queue */
        EE_rq_queues_head[x] = next;

        /* Make the clean-up of other data structures related to ready queue */
        EE_as_task_other_cleanup(tid);
      } else {
        /* current != -1 is checked in the cycle below */
        break;
      }
      /* Move the current to next at the end */
      current = next;
    }

    /* Remove elements inside priority ready queue, if the queue is still
       NOT EMPTY */
    previous = current;
    while ( current != (EE_TYPEPAIR)-1 ) {
      /* Get the next */
      next = EE_rq_pairs_next[current];
      /* Hold "current" tid */
      tid  = EE_rq_pairs_tid[current];

      if ( EE_th_app[tid + 1] == app ) {
        /* Free the descriptor */
        EE_rq_pairs_next[current] = EE_rq_free;
        EE_rq_free = current;

        /* Assign previous-next to next */
        EE_rq_pairs_next[previous] = next;
 
        /* Make the clean-up of other data structures related to ready queue */
        EE_as_task_other_cleanup(tid);
      } else {
        /* Otherwise current will be next previous */
        previous = current;
      }
      /* Move the current to next at the end */
      current = next;
    }

    /* If the actual priority ready queue is empty, clean it (even though it
       could be already clean) */
    if (EE_rq_queues_head[x] == (EE_TYPEPAIR)-1) {
      /* Adjust priority ready queue tail */
      EE_rq_queues_tail[x] = -1;
      /* Reset the (x)th bit in the bitfield (casts are for MISRA Compliance) */
      EE_rq_bitmask &= ((EE_TYPE_RQ_MASK)~((EE_TYPE_RQ_MASK)1U << x));
    }
    /* Decrement priority ready queue index until -1 */
    --x;
  }
}
#endif /* OO Kernels */

void EE_as_TerminateApplication_internal ( ApplicationType Application,
  RestartType RestartOption )
{
  /* Pointer to OS-Application to be terminated data structure */
  register EE_as_Application_RAM_type * const app_RAM_ptr =
    &EE_as_Application_RAM[Application];

  /* Terminate application TASKs */
  EE_as_terminate_app_tasks(Application);

#ifdef EE_SERVICE_PROTECTION__
#if defined(EE_MAX_ALARM) && (EE_MAX_ALARM > 0)
  {
    /* Handle alarms cancellation and counters reset */
    AlarmType alarm;
    for ( alarm = 0; alarm < EE_MAX_ALARM; ++alarm )
    {
      EE_oo_alarm_ROM_type const * const pAlarmROM = &EE_alarm_ROM[alarm];

      if ( pAlarmROM->ApplID == Application )
      {
        CounterType       const counter = EE_oo_counter_object_ROM[alarm].c;

        EE_oo_counter_object_RAM_type * const pCounterObjectRAM =
          &EE_oo_counter_object_RAM[alarm];

        /* RESET COUNTER OBJECT */
        pCounterObjectRAM->used  = 0U;
        pCounterObjectRAM->cycle = 0U;
        pCounterObjectRAM->delta = 0U;
        pCounterObjectRAM->next  = INVALID_COUNTER_OBJECT;

        if ( EE_counter_ROM[counter].ApplID == Application )
        {
          /* RESET THE COUNTER TOO */
          EE_oo_counter_RAM_type  * const pCounterRAM =
            &EE_counter_RAM[counter];

          pCounterRAM->value = 0U;
          pCounterRAM->first = INVALID_COUNTER_OBJECT;
        }
      }
    }
  }
#endif /* EE_MAX_ALARM > 0 */

#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)
  {
    /* Handle alarms cancellation and counters reset */
    ScheduleTableType sched_id;
    for ( sched_id = 0; sched_id < EE_MAX_SCHEDULETABLE; ++sched_id )
    {
      EE_as_Schedule_Table_ROM_type const * const p_schedule_table_ROM = 
        &EE_as_Schedule_Table_ROM[sched_id];

      if ( p_schedule_table_ROM->ApplID == Application )
      {
        CounterObjectType const obj_id  = EE_MAX_ALARM + sched_id;
        CounterType       const counter = EE_oo_counter_object_ROM[obj_id].c;

        EE_oo_counter_object_RAM_type * const pCounterObjectRAM =
          &EE_oo_counter_object_RAM[obj_id];
        EE_as_Schedule_Table_RAM_type * const p_schedule_table_RAM =
          &EE_as_Schedule_Table_RAM[sched_id];

        /* RESET COUNTER OBJECT */
        pCounterObjectRAM->used  = 0U;
        pCounterObjectRAM->cycle = 0U;
        pCounterObjectRAM->delta = 0U;
        pCounterObjectRAM->next  = INVALID_COUNTER_OBJECT;

        /* RESET SCHEDULE TABLE RAM TOO */
        p_schedule_table_RAM->status      = SCHEDULETABLE_STOPPED;
        p_schedule_table_RAM->position    = INVALID_SCHEDULETABLE_POSITION;
        p_schedule_table_RAM->deviation   = 0;
        p_schedule_table_RAM->next_table  = INVALID_SCHEDULETABLE;

        if ( EE_counter_ROM[counter].ApplID == Application )
        {
          /* RESET THE COUNTER TOO */
          EE_oo_counter_RAM_type  * const pCounterRAM =
            &EE_counter_RAM[counter];

          pCounterRAM->value = 0U;
          pCounterRAM->first = INVALID_COUNTER_OBJECT;
        }
      }
    }
  }
#endif /* EE_MAX_SCHEDULETABLE > 0 */
#endif /* EE_SERVICE_PROTECTION__ */

  /* Set Application in requested Status */
  if ( RestartOption == RESTART ) {
    TaskType restart_task = EE_as_Application_ROM[Application].restart_task;
    app_RAM_ptr->ApplState = APPLICATION_RESTARTING;
    if ( restart_task != EE_NIL ) {
      /* Decrement the residual number activation */
      --EE_th_rnact[restart_task];
      /* Put the task in the ready state */
      EE_oo_set_th_status_ready(restart_task);
      /* Insert the restart task in the ready queue */
      EE_rq_insert(restart_task);
    }
  } else {
    app_RAM_ptr->ApplState =  APPLICATION_TERMINATED;
  }
}

StatusType EE_as_TerminateApplication ( ApplicationType Application,
  RestartType RestartOption )
{
  /* Pointer to OS-Application to be terminated data structure */
  register EE_as_Application_RAM_type * app_RAM_ptr;
  /* Calling OS-Application ID */
  register ApplicationType              current_app;
  /* Error Value */
  register StatusType                   ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_TERMINATEAPPLICATION);

  EE_as_monitoring_the_stack();

  /* Calling OS-Application ID */
  current_app = EE_as_active_app;
  /* Get the OS-Application to be terminated data structure */
  app_RAM_ptr = &EE_as_Application_RAM[Application];

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of the service. (BSW11009, BSW11013) */
  /* TerminateApplication is callable by Task, ISR2, ErrorHook
      (only self termination) */
  if ( (EE_as_execution_context > ErrorHook_Context) || 
        ((EE_as_execution_context == ErrorHook_Context) &&
          (current_app != Application)) )
  {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

#ifdef EE_AS_RPC__
  if ( EE_AS_ID_REMOTE(Application) ) {
    EE_os_param const unmarked_app_id = { EE_AS_UNMARK_REMOTE_ID(Application) };
    EE_os_param const as_restart_option = { RestartOption };

    /* Forward the request to another CPU in synchronous way */
    ev = EE_as_rpc(OSServiceId_TerminateApplication, unmarked_app_id,
      as_restart_option, EE_OS_INVALID_PARAM);
  } else {
#endif /* EE_AS_RPC__ */
#ifdef __OO_EXTENDED_STATUS__
      /* [OS495] If the <Application> in a call of GetApplicationState() is not
         valid GetApplicationState() shall return E_OS_ID. */
      /* [OS493] If the input parameter <Application> in a call of
          TerminateApplication() is not valid. TerminateApplication() shall
          return E_OS_ID. */
      /* [OS459] If the <RestartOption> in a call of TerminateApplication() is
          invalid, TerminateApplication() shall return E_OS_VALUE. */
      /* [OS494] If the input parameter <Application> in a call of
          TerminateApplication() is valid AND the caller belongs to a
          non-trusted OS-Application AND the caller does not belong
          to <Application> TerminateApplication() shall return E_OS_ACCESS. */
      if ( Application >= EE_MAX_APP ) {
        ev = E_OS_ID;
      } else if ( Application >= EE_MAX_APP ) {
        ev = E_OS_ID;
      } else if ( (RestartOption != NO_RESTART) && (RestartOption != RESTART) )
      {
        ev = E_OS_VALUE;
      } else if ( (current_app != Application) &&
        (EE_as_Application_ROM[current_app].Mode != EE_MEMPROT_TRUST_MODE) )
      {
        ev = E_OS_ACCESS;
      } else
#endif /* __OO_EXTENDED_STATUS__ */
    /* [OS507] If the state of <Application> in a call of TerminateApplication()
       is APPLICATION_TERMINATED TerminateApplication() shall return E_OS_STATE.
    */
    /* [OS508] If the state of <Application> in a call of TerminateApplication()
       is APPLICATION_RESTARTING and the caller does not belong to the
       <Application> then TerminateApplication() shall return E_OS_STATE. */
    /* [OS548] If the state of <Application> in a call of TerminateApplication()
       is APPLICATION_RESTARTING AND the caller does belong to the <Application>
       AND the <RestartOption> is equal RESTART then TerminateApplication()
       shall return E_OS_STATE. ( ) */
    if ( (app_RAM_ptr->ApplState == APPLICATION_TERMINATED) ||
           ((app_RAM_ptr->ApplState == APPLICATION_RESTARTING) &&
           ((current_app != Application) || (RestartOption == RESTART))) )
    {
      ev = E_OS_STATE;
    } else {
      /* [OS287] If the parameters in a call of TerminateApplication() are valid
         and the above criteria are met TerminateApplication() shall terminate
         <Application> (i.e. to kill all tasks, disable the interrupt sources of
         those ISRs which belong to the OS-Application and free all other
         OS resources associated with the application) AND shall activate
         the configured OsRestartTask of <Application> if <RestartOption>
         equals RESTART. If the <Application> is restarted, its state is set
         to APPLICATION_RESTARTING otherwise to APPLICATION_TERMINATED.
         If the caller belongs to <Application> TerminateApplication()shall not
         return, otherwise it shall return E_OK. */
      EE_as_TerminateApplication_internal(Application, RestartOption);

      /* If the caller belong to the OS-Application to be terminated,
         terminate it. */
      if ( current_app == Application ) {
        EE_as_terminate_current_app();
      }
      ev = E_OK;
    }
#ifdef EE_AS_RPC__
  }
#endif /* EE_AS_RPC__ */

  if ( ev != E_OK ) {
#ifdef __OO_HAS_ERRORHOOK__
    EE_OS_PARAM(os_application);
    EE_OS_PARAM(os_restart_option);
    EE_OS_PARAM_VALUE(os_application, Application);
    EE_OS_PARAM_VALUE(os_restart_option, RestartOption);
#endif /* __OO_HAS_ERRORHOOK__ */
    EE_os_notify_error(OSServiceId_TerminateApplication, os_application,
      os_restart_option, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_TERMINATEAPPLICATION);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;  
}

StatusType EE_as_AllowAccess( void )
{
  /* Current OS-Application the one to be restarted */
  register ApplicationType              current_app;
  /* Pointer to OS-Application to be restarted data structure */
  register EE_as_Application_RAM_type * app_RAM_ptr;
  /* Error Value */
  register StatusType                   ev;
#ifdef EE_SERVICE_PROTECTION__
  /* Current Task */
  register EE_TID                       current;
#endif /* EE_SERVICE_PROTECTION__ */
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_ALLOWACCESS);

  EE_as_monitoring_the_stack();

  /* Current Application */
  current_app = EE_as_active_app;

  /* Get the OS-Application to be restarted data structure */
  app_RAM_ptr = &EE_as_Application_RAM[current_app];

#ifdef EE_SERVICE_PROTECTION__
  /* Current Task */
  current = EE_stk_queryfirst();
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of the service. (BSW11009, BSW11013) */
  /* AllowAccess is callable only by Restarting Task Task, ISR2, ErrorHook
      (only self termination) */
  if ( (EE_as_execution_context > TASK_Context) || 
        (current != EE_as_Application_ROM[current_app].restart_task) )
  {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

  /* [OS497] If the state of the OS-Application of the caller of AllowAccess()
      is not APPLICATION_RESTARTING AllowAccess() shall return E_OS_STATE. */
  if ( app_RAM_ptr->ApplState != APPLICATION_RESTARTING ) {
    ev = E_OS_STATE;
  } else {
    /* [OS498] If the state of the OS-Application of the caller of AllowAccess()
       is APPLICATION_RESTARTING, AllowAccess() shall set the state to
       APPLICATION_ACCESSIBLE and allow other OS-Applications to access the
       configured objects of the callers OS-Application. */
    app_RAM_ptr->ApplState = APPLICATION_ACCESSIBLE;

    ev = E_OK;
  }

  if ( ev != E_OK ) {
    EE_os_notify_error(OSServiceId_AllowAccess, EE_OS_INVALID_PARAM,
      EE_OS_INVALID_PARAM, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_ALLOWACCESS);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

