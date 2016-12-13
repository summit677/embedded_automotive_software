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
 * CVS: $Id: ee_internal.h,v 1.6 2006/12/03 22:07:50 pj Exp $
 */

#ifndef __INCLUDE_OO_INTERNAL_H__
#define __INCLUDE_OO_INTERNAL_H__

#include "kernel/oo/inc/ee_oo_common.h"
#include "kernel/oo/inc/ee_oo_intfunc.h"
#include "kernel/oo/inc/ee_oo_irq.h"

/*******************************************************************************
 *                    New Error Handling Internal Utilities
 ******************************************************************************/

 /* Handler macro to join strings and transform label in string */
#define EE_OS_VALUE(s)                      s
#define EE_OS_STRINGIFY(s)                  #s
#define EE_OS_STRING_JOIN(s1,s2)            s1##s2
#define EE_OS_STRING_JOIN_INDIRECT(s1,s2)   EE_OS_STRING_JOIN(s1,s2)

/* Error Parameters Utilities Macros */
#define EE_OS_PARAM(param_name)   EE_os_param param_name
#define EE_OS_PARAM_VALUE(param_name, param_value)  \
  (param_name.value_param = param_value)

#define EE_OS_PARAM_REF(param_name, param_field, param_ref) \
  (EE_OS_VALUE(param_name).EE_OS_VALUE(param_field) = param_ref)

#ifdef __OO_HAS_ERRORHOOK__
/* Error Parameters Data Structure Utilities Macros from user space */
#define EE_OS_ERROR_PARAMETERS()  EE_oo_ErrorHook_parameters error_parameters

#define EE_OS_ERROR_PARAMETERS_INIT(param1_in,param2_in,param3_in)  \
  EE_oo_ErrorHook_parameters error_parameters = { param1_in, param2_in, \
    param3_in }

#define EE_OS_ERROR_PARAMETERS_PARAM1_VALUE(param1_value) \
  (error_parameters.param1.value_param = param1_value)
#define EE_OS_ERROR_PARAMETERS_PARAM2_VALUE(param2_value) \
  (error_parameters.param2.value_param = param2_value)
#define EE_OS_ERROR_PARAMETERS_PARAM3_VALUE(param3_value) \
  (error_parameters.param3.value_param = param3_value)

#define EE_OS_ERROR_PARAMETERS_PARAM1_REF(param_field,param_ref)  \
  (error_parameters.param1.EE_OS_VALUE(param_field) = param_ref)
#define EE_OS_ERROR_PARAMETERS_PARAM2_REF(param_field,param_ref)  \
  (error_parameters.param2.EE_OS_VALUE(param_field) = param_ref)
#define EE_OS_ERROR_PARAMETERS_PARAM3_REF(param_field,param_ref)  \
  (error_parameters.param3.EE_OS_VALUE(param_field) = param_ref)
#else /* __OO_HAS_ERRORHOOK__ */
#define EE_OS_ERROR_PARAMETERS()                                    ((void)0)

#define EE_OS_ERROR_PARAMETERS_INIT(param1_in,param2_in,param3_in)  ((void)0)

#define EE_OS_ERROR_PARAMETERS_PARAM1_VALUE(param1_value)           ((void)0)
#define EE_OS_ERROR_PARAMETERS_PARAM2_VALUE(param2_value)           ((void)0)
#define EE_OS_ERROR_PARAMETERS_PARAM3_VALUE(param3_value)           ((void)0)

#define EE_OS_ERROR_PARAMETERS_PARAM1_REF(param_field,param_ref)    ((void)0)
#define EE_OS_ERROR_PARAMETERS_PARAM2_REF(param_field,param_ref)    ((void)0)
#define EE_OS_ERROR_PARAMETERS_PARAM3_REF(param_field,param_ref)    ((void)0)
#endif /* __OO_HAS_ERRORHOOK__ */

/* Error Handling */
#ifdef __OO_HAS_ERRORHOOK__
#ifndef  __OO_ERRORHOOK_NOMACROS__

__INLINE__ void __ALWAYS_INLINE__ EE_os_fill_error_data( OSServiceIdType
  ServiceID, EE_os_param param1, EE_os_param param2, EE_os_param param3 )
{
  register EE_oo_ErrorHook_parameters * const
    p_errorhook_parameter = EE_oo_get_errorhook_data();
  (*EE_oo_get_errorhook_service_id()) = ServiceID;
  p_errorhook_parameter->param1 = param1;
  p_errorhook_parameter->param2 = param2;
  p_errorhook_parameter->param3 = param3;
}
#else /* !__OO_ERRORHOOK_NOMACROS__ */

#define EE_os_fill_error_data(ServiceID,param1,param2,param3) ((void)0)
#endif /* !__OO_ERRORHOOK_NOMACROS__ */

__INLINE__ void __ALWAYS_INLINE__ EE_os_notify_error( OSServiceIdType ServiceID,
  EE_os_param param1, EE_os_param param2, EE_os_param param3,
  StatusType Error )
{
  EE_ORTI_set_lasterror(Error);
  if ( EE_ErrorHook_nested_flag == 0U )
  {
    EE_os_fill_error_data(ServiceID, param1, param2, param3);
    EE_ErrorHook_nested_flag = 1U;
    EE_oo_call_ErrorHook(Error);
    EE_ErrorHook_nested_flag = 0U;
  }
}

#ifndef __EE_MEMORY_PROTECTION__
__INLINE__ void __ALWAYS_INLINE__ EE_os_notify_error_from_us(
  OSServiceIdType ServiceID, const EE_oo_ErrorHook_parameters * const
    error_parameters_ref, StatusType Error )
{
  if ( error_parameters_ref != NULL ) {
    register EE_FREG const flag = EE_hal_begin_nested_primitive();
    EE_os_notify_error(ServiceID, error_parameters_ref->param1,
      error_parameters_ref->param2, error_parameters_ref->param3, Error);
    EE_hal_end_nested_primitive(flag);
  }
}
#else /* !__EE_MEMORY_PROTECTION__ */
/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following code belong to ERIKA API section ee_api_text */
#define API_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

void EE_os_notify_error_from_us( OSServiceIdType ServiceID,
  const EE_oo_ErrorHook_parameters * const error_parameters_ref,
  StatusType Error );

#ifdef EE_SUPPORT_MEMMAP_H
#define API_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
#endif /* !__EE_MEMORY_PROTECTION__ */
#else /* __OO_HAS_ERRORHOOK__ */

/* Macro used for remapping to prevent compiler warnings on unused parameters */
#define EE_os_notify_error(k,j,x,y,z) EE_os_notify_error_impl(z)

__INLINE__ void __ALWAYS_INLINE__ EE_os_notify_error_impl(StatusType Error)
{
  EE_ORTI_set_lasterror(Error);
}

/* Macro used for remapping to prevent compiler warnings on unused parameters */
#define EE_os_notify_error_from_us(x,y,z) EE_os_notify_error_from_us_impl(z)

__INLINE__ void __ALWAYS_INLINE__ EE_os_notify_error_from_us_impl(
  StatusType Error )
{
  EE_ORTI_set_lasterror(Error);
}
#endif /* __OO_HAS_ERRORHOOK__ */

/*******************************************************************************
 *                  Kernel Critical Section Utility Macros
 ******************************************************************************/
#ifndef __EE_MEMORY_PROTECTION__

#define EE_OS_DECLARE_CRITICAL_SECTION()  register EE_FREG flag

#define EE_OS_ENTER_CRITICAL_SECTION()          \
  do {                                          \
    flag = EE_hal_begin_nested_primitive();     \
    EE_as_tp_active_pause_and_update_budgets(); \
  } while ( ( 0 ) )

#define EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION()  \
  EE_OS_DECLARE_CRITICAL_SECTION();                 \
  EE_OS_ENTER_CRITICAL_SECTION()

#define EE_OS_EXIT_CRITICAL_SECTION()             \
  do {                                            \
    EE_as_tp_active_update_budgets_and_restart(); \
    EE_hal_end_nested_primitive(flag);            \
  } while ( ( 0 ) )

#define EE_OS_FOREVER_CRITICAL_SECTION()          \
  do {                                            \
    EE_hal_disableIRQ();                          \
    EE_as_tp_active_pause_and_update_budgets();   \
  } while ( ( 0 ) )

#else /* !__EE_MEMORY_PROTECTION__ */
/* TP in any case have to be handled in Service Implementation. Syscall
   mechanism can be implement in C language and, in that case, a function call
   cannot be done in syscall handler. */
#define EE_OS_DECLARE_CRITICAL_SECTION()            ((void)0)
#define EE_OS_ENTER_CRITICAL_SECTION()              \
  EE_as_tp_active_pause_and_update_budgets()
#define EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION()  \
  EE_as_tp_active_pause_and_update_budgets()
#define EE_OS_EXIT_CRITICAL_SECTION()               \
  EE_as_tp_active_update_budgets_and_restart()
#define EE_OS_FOREVER_CRITICAL_SECTION()			\
  EE_as_tp_active_pause_and_update_budgets()
#endif /* !__EE_MEMORY_PROTECTION__ */

/* return the first stacked task (the running task) without extracting it 
   we suppose that this function can be called ALSO at interrupt ENABLED!!!
   (see WaitEvent.c) */
#ifndef __PRIVATE_STK_QUERYFIRST__
__INLINE__ EE_TID __ALWAYS_INLINE__ EE_stk_queryfirst(void)
{
    return EE_stkfirst;  
}
#endif /* __PRIVATE_STK_QUERYFIRST__ */

/* This function notifies a tick to a hardware counter.
   That is, the counter is
   incremented by 1.  It must be called into an ISR2 or into a
   task notify that the event linked to the counter occurred.

   The function will also implement the notification of expired alarms
   (calling an alarm callback, setting an event, or activating a
   task).

   The function can be considered atomic, and NO RESCHEDULING will
   take place after the execution of this function. To implement the
   rescheduling at task level, you can use the Schedule() or the
   ForceSchedule() functions just after this notification.

   see also internal.h
*/
#if (!defined(__OO_NO_ALARMS__)) || defined(EE_AS_SCHEDULETABLES__)

StatusType EE_oo_IncrementCounterHardware(CounterType CounterID);
void       EE_oo_IncrementCounterImplementation(CounterType CounterID);

/* kernel internal API */
#define IncrementCounterHardware EE_oo_IncrementCounterHardware

/* Helper function (0 as increment means next tick)*/
void EE_oo_counter_object_insert( CounterObjectType ObjectID, TickType
  increment );

#if defined (EE_COUNTER_OBJECTS_ROM_SIZE)
#if (EE_COUNTER_OBJECTS_ROM_SIZE > 0)
__INLINE__ void __ALWAYS_INLINE__
  EE_oo_handle_abs_counter_object_insertion( CounterObjectType ObjectID,
    TickType start, TickType cycle ) 
{
  /* These are used to evaluate alarm time handling wrap around */
  register TickType           alarm_time;
  register TickType           start_rel;
  register CounterType const  c = EE_oo_counter_object_ROM[ObjectID].c;

  /* first, use the alarm and set the cycle */
  EE_oo_counter_object_RAM[ObjectID].used = 1U;
  EE_oo_counter_object_RAM[ObjectID].cycle = cycle;

  /* Handling wrap around for alarm time */
  start_rel = start - EE_counter_RAM[c].value;

  /*  When will be here start value will be already checked against counter
      max allowed value */
  if ( start_rel == 0U ) {
    /* start_rel == 0U -> the alarm should start now or next time that counter
       has this value. Has been chosen the second option */
    alarm_time = EE_counter_ROM[c].maxallowedvalue;
  } else if ( start_rel < EE_TYPETICK_HALF_VALUE ) {
    /* Normal behavior */
    alarm_time = start_rel - 1U;
  } else {
    /* start_rel is "negative" in this case (unsigned wrap around do the
       work) */
    alarm_time = EE_counter_ROM[c].maxallowedvalue + start_rel;
  }

  /* Set alarm with a relative amount of time (alarm_time already is a "0 as
     next tick" value)*/
  EE_oo_counter_object_insert(ObjectID, alarm_time);
}

__INLINE__ void __ALWAYS_INLINE__
  EE_oo_handle_rel_counter_object_insertion( CounterObjectType ObjectID,
    TickType increment, TickType cycle )
{

  /* first, use the alarm and set the cycle */
  EE_oo_counter_object_RAM[ObjectID].used = 1U;
  EE_oo_counter_object_RAM[ObjectID].cycle = cycle;

  /* then, insert the task into the delta queue with an increment equal
     (increment - 1U) increment equal to 0 means next tick */
  EE_oo_counter_object_insert(ObjectID, (increment - 1U));
}

/* cancellation */
__INLINE__ void __ALWAYS_INLINE__ EE_oo_handle_counter_object_cancellation(
  CounterObjectType ObjectID )
{
  register CounterObjectType current, previous;
  register CounterType       c      = EE_oo_counter_object_ROM[ObjectID].c;

  /* to compute the relative value in ticks, we have to follow the counter
     delay chain */
  current = EE_counter_RAM[c].first;

  if ( current == ObjectID ) {
    /* the alarm is the first one in the delta queue */
    EE_counter_RAM[c].first = EE_oo_counter_object_RAM[ObjectID].next;
  } else {
    /* the alarm is not the first one in the delta queue */
    /* Find it */
    do {
      previous = current;
      current = EE_oo_counter_object_RAM[current].next;
    } while ( current != ObjectID );
    /* remove it from the queue */
    EE_oo_counter_object_RAM[previous].next =
      EE_oo_counter_object_RAM[ObjectID].next;
  }
  /* Adjust delta value */
  if ( EE_oo_counter_object_RAM[ObjectID].next != INVALID_COUNTER_OBJECT ) {
    EE_oo_counter_object_RAM[EE_oo_counter_object_RAM[ObjectID].next].delta +=
      EE_oo_counter_object_RAM[ObjectID].delta;
  }

  EE_oo_counter_object_RAM[ObjectID].used = 0U;
}
#endif /* EE_COUNTER_OBJECTS_ROM_SIZE */
#endif /* EE_COUNTER_OBJECTS_ROM_SIZE > 0 */
#endif /* !__OO_NO_ALARMS__ || EE_AS_SCHEDULETABLES__ */

/*************************************************************************
 HAL extensions
 *************************************************************************/

 /* these are the functions that have been inserted to support tha OO layer
    under the EE HAL.
    
    - all the functions for interrupt handling (13.3)
    - EE_hal_begin_nested_primitive
      EE_hal_end_nested_primitive 
      (for primitives that can be called both into a task and into an ISR2
    - EE_hal_terminate_task(EE_TID t)
    - EE_hal_terminate_savestk(EE_TID t)
      EE_oo_preemption_point() execute task preemption if is needed
    - EE_oo_shutdown() if not redefined it does for(;;);
    - All the alarm constants listed in 13.6.4
 */


/***************************************************************************
 * Internal data structures and functions
 ***************************************************************************/

/*
    Call Hooks utilities
 */

#ifdef __OO_HAS_PRETASKHOOK__
__INLINE__ void __ALWAYS_INLINE__ EE_oo_call_PreTaskHook(void)
{
  EE_as_set_execution_context( PreTaskHook_Context );
  PreTaskHook();
  /* Set the execution context on TASK */
  EE_as_set_execution_context( TASK_Context );
}

#else /* __OO_HAS_PRETASKHOOK__ */
__INLINE__ void __ALWAYS_INLINE__ EE_oo_call_PreTaskHook(void)
{
  /* Set the execution context on TASK */
  EE_as_set_execution_context( TASK_Context );
}
#endif /* __OO_HAS_PRETASKHOOK__ */

#if defined(__OO_HAS_POSTTASKHOOK__)
__INLINE__ void __ALWAYS_INLINE__ EE_oo_call_PostTaskHook(void)
{
  EE_as_set_execution_context( PostTaskHook_Context );
  PostTaskHook();
}
#else /* __OO_HAS_POSTTASKHOOK__ */
#define EE_oo_call_PostTaskHook()   ((void)0)
#endif /* __OO_HAS_POSTTASKHOOK__ */

/* Used to check if we are in disable interrupt error. It return
   'signed register type' because usually it is native integer type. */
__INLINE__ EE_SREG __ALWAYS_INLINE__ EE_oo_check_disableint_error(void)
{
  return ( EE_oo_IRQ_disable_count != 0U );
}

#if defined(__OO_BCC2__) || defined(__OO_ECC2__)
/* A lookup table to speedup ready queue handling */
extern const EE_INT8 EE_rq_lookup[256];
/* Lookup functions */
#if defined(__OO_ECC2__)
__INLINE__ EE_INT8 __ALWAYS_INLINE__ EE_rq_get_first_not_empty_queue( void )
{
  /* Lookup at bits 15-9 */
  EE_INT8 x = EE_rq_lookup[(EE_rq_bitmask & 0xFF00U) >> 8];
  if (x == (EE_INT8)-1) {
    x = EE_rq_lookup[EE_rq_bitmask];
  } else {
    x += (EE_INT8)8;
  }
  return x;
}
#else /* __OO_ECC2__ */
__INLINE__ EE_INT8 __ALWAYS_INLINE__ EE_rq_get_first_not_empty_queue( void )
{
  return EE_rq_lookup[EE_rq_bitmask];
}
#endif /* __OO_ECC2__ */
#endif /* __OO_BCC2__ || __OO_ECC2__ */

/* Internal Queue management functions */

/* return the first ready task without extracting it */
#ifndef __PRIVATE_RQ_QUERYFIRST__
#if defined(__OO_BCC1__) || defined(__OO_ECC1__)
__INLINE__ EE_TID __ALWAYS_INLINE__ EE_rq_queryfirst(void)
{ return EE_rq_first; }
#endif

#if defined(__OO_BCC2__) || defined(__OO_ECC2__)
EE_TID EE_rq_queryfirst(void);
#endif
#endif

/* __INLINE__ EE_TID __ALWAYS_INLINE__ EE_stk_queryfirst(void) in intfunc.h */

/* Extract the RUNNING task from the stack, and return the new head */
#ifndef __PRIVATE_STK_GETFIRST__
__INLINE__ EE_TID __ALWAYS_INLINE__ EE_stk_getfirst(void)
{
  return (EE_stkfirst = EE_th_next[EE_stkfirst]);
}
#endif /* __PRIVATE_STK_GETFIRST__ */

/* insert a task into the stack data structures */
#ifndef __PRIVATE_STK_INSERTFIRST__
__INLINE__ void __ALWAYS_INLINE__ EE_stk_insertfirst(EE_TID t)
{
    EE_th_next[t] = EE_stkfirst;
    EE_stkfirst = t;
}
#endif

/* insert a task into the ready queue */
#ifndef __PRIVATE_RQ_INSERT__
void EE_rq_insert(EE_TID t);
#endif

/* put the first ready task into the stack */
#ifndef __PRIVATE_RQ2STK_EXCHANGE__
EE_TID EE_rq2stk_exchange(void);
#endif

#ifndef __OO_NO_RESOURCES__
/*
    Method to release all resources locked by a Thread,
    used to fulfill AS requirement OS070
 */
#if ((!defined(__OO_EXTENDED_STATUS__)) && \
    (!defined(__PRIVATE_RELEASEALLRESOURCE__))) && \
    (!defined(__OO_ISR2_RESOURCES__))
__INLINE__ void __ALWAYS_INLINE__ EE_oo_release_all_resources(EE_TID tid)
{
  /* release the internal resource. a EE_TYPEPRIO is a bit mask with only one
     bit set to one.
     If i subtract one to this I obtain a bit mask with all
     ones before the starting one and all zeros after 
     (exactly what I want to release resources!)
  */
  EE_sys_ceiling &= (EE_th_dispatch_prio[tid] - 1U);
}
#else
EE_UREG EE_oo_release_all_resources(EE_TID tid);
#endif /* !__OO_EXTENDED_STATUS__ && !__OO_ISR2_RESOURCES__ */
#else /* __OO_NO_RESOURCES__ */
#define EE_oo_release_all_resources(tid)    ((void)0)
#endif /* __OO_NO_RESOURCES__ */

#ifndef __PRIVATE_THREANTERMINATED__
/* this the function that will be called if a Task doesn't end calling
   TerminateTask */

extern void EE_thread_not_terminated( void );
#endif /* __PRIVATE_THREANTERMINATED__ */

/* This call terminates a thread instance. It must be called as the
   LAST function call BEFORE the `}' that ends a thread. If the
   primitive is not inserted at the end of */
#ifndef __PRIVATE_THREAD_END_INSTANCE__
void EE_thread_end_instance(void);
#endif

#if defined(__OO_ISR2_RESOURCES__) || defined(EE_AS_USER_SPINLOCKS__)
/* Index used to give ISR2 Temporary TID value and to access at
   EE_isr2_nesting_level array */
extern EE_UREG EE_isr2_index;

/* Assign a fake TID to an ISR2 to eventually handle resources clean-up */
EE_TID  EE_oo_assign_TID_to_ISR2(void);

/* Get the last assigned fake ISR2 TID */
__INLINE__ EE_TID __ALWAYS_INLINE__ EE_oo_get_ISR2_TID(void) {
  return (EE_isr2_index != EE_UREG_MINUS1)?
    (EE_MAX_TASK + (EE_TID)EE_isr2_index): EE_NIL;
}
#endif /* __OO_ISR2_RESOURCES__ || EE_AS_USER_SPINLOCKS__ */

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
/*
    Reset Active Events  THREAD utility method.

    When an extended task is transferred from suspended state
    into ready state all its events have to be cleared cleared
*/
__INLINE__ void __ALWAYS_INLINE__ EE_oo_reset_th_event_active(TaskType TaskID)
{
    EE_th_event_active[TaskID] = 0U;
}

/* 
  This method actually do a CONTEXT SWITCH, with the highest priority TASK
*/
__INLINE__ void __ALWAYS_INLINE__ EE_oo_run_next_task(void)
{
    register TaskType tmp;
    /* swap from ready queue to stack queue */
    tmp = EE_rq2stk_exchange();
    if (EE_th_waswaiting[tmp]) {
      /* if the task was waiting switch the context to restart it */
      EE_th_waswaiting[tmp] = 0U;
      EE_hal_stkchange(tmp);
    } else {
      /* the next task have to be started */
      EE_hal_ready2stacked(tmp);
    }
}
#else
#define EE_oo_reset_th_event_active(TaskID)    ((void) 0)

/* 
  This method actually do a CONTEXT SWITCH, with the highest priority TASK
*/
__INLINE__ void __ALWAYS_INLINE__ EE_oo_run_next_task(void)
{
    EE_hal_ready2stacked(EE_rq2stk_exchange());
}
#endif /* defined(__OO_ECC1__) || defined(__OO_ECC2__) */


#if defined(__OO_BCC2__) || defined(__OO_ECC2__)
  /*
    Set THREAD ready utility method.
    If the task is BCC2/ECC2 it can be that it is ready or 
    running. in that case we have to check and queue it anyway
  */
__INLINE__ void __ALWAYS_INLINE__ EE_oo_set_th_status_ready(TaskType TaskID)
{
    if (EE_th_status[TaskID] == SUSPENDED) {
        EE_th_status[TaskID] = READY;
        EE_oo_reset_th_event_active(TaskID);
    }
}
#else
  /*
    Set THREAD ready utility method
    If the task is BCC1/ECC1 it can be here only because
    it had rnact=1 before the call, and so it is in suspended state
  */
__INLINE__ void __ALWAYS_INLINE__ EE_oo_set_th_status_ready(TaskType TaskID)
{
    EE_th_status[TaskID] = READY;
    EE_oo_reset_th_event_active(TaskID);
}
#endif /* defined(__OO_BCC2__) || defined(__OO_ECC2__) */

__INLINE__ void __ALWAYS_INLINE__ EE_oo_task_in_ready_queue( TaskType tid )
{
  /* Decrement the residual number activation */
  --EE_th_rnact[tid];
  /* Set the status READY handling multiple activations */
  EE_oo_set_th_status_ready(tid);
  /* insert the task in the ready queue */
  EE_rq_insert(tid);
}

/* Execute a preemption */
#ifndef __PRIVATE_PREEMPTION_POINTS__
/*
  Moved preemption check implementation into an outside method because the
  behaviour is common to EE_ActivateTask, EE_oo_ForceSchedule,
  EE_oo_IncrementCounter, EE_oo_ReleaseResource, EE_oo_PostSem
*/
__INLINE__ void __ALWAYS_INLINE__ EE_oo_preemption_point(void)
{
  register EE_TID current, rq;

  /* check if there is a preemption */
  current = EE_stk_queryfirst();
  rq      = EE_rq_queryfirst();

#if defined(EE_AS_OSAPPLICATIONS__) && defined(__EE_MEMORY_PROTECTION__)
#if defined(EE_SYSCALL_NR) && defined(EE_MAX_SYS_SERVICEID) &&\
  (EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID)
  /* Reaction to timing protection can be defined to terminate the
     OSApplication. If a task is inside CallTrustedFunction() and task
     rescheduling takes place within the same OSApplication,
     the newly running higher priority task may cause timing protection and
     terminate the OSApplication, thus indirectly aborting the trusted
     function. To avoid this, the scheduling of other Tasks which belong to
     the same OS-Application as the caller needs to be restricted, as well as
     the availability of interrupts of the same OS-Application. */
  /* [SWS_Os_00563]: The OperatingSystem shall not schedule any other Tasks
      which belong to the same OS-Application as the non-trusted caller of the
      service. Also interrupts of Category 2 which belong to the same
      OS-Application shall be disabled during the execution of the service. */
  if (  (EE_as_active_app != EE_th_app[rq + 1]) ||
        (EE_as_Application_RAM[EE_as_active_app].
          TrustedFunctionCallsCounter == 0U) ||
        (EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE)
     )
#endif /* EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID */
#endif /* EE_AS_OSAPPLICATIONS__ && __EE_MEMORY_PROTECTION__ */
  {
    if ( rq != EE_NIL ) {
      /* We check if the system ceiling is greater or not the first task
         in the ready queue */
      if ( EE_sys_ceiling < EE_th_ready_prio[rq] ) {

        if ( current != EE_NIL ) {
          EE_oo_call_PostTaskHook();
          /* We have to put the task in the READY status */
          EE_th_status[current] = READY;
          /* !!! BUT NOT IN THE READY QUEUE !!!
             The task remains into the Stacked Queue! */
        }

        /* Get the new internal resource */
        EE_sys_ceiling |= EE_th_dispatch_prio[rq];
        /* Put the task in running state */
        EE_th_status[rq] = RUNNING;

        EE_ORTI_set_th_eq_dispatch_prio(rq);

        /* "Press TP start for the first time" for this new activation/release of
            the TASK */
        EE_as_tp_active_start_on_TASK_stacking(rq);

        /* Execute context SWITCH, this method return when we have a switch
           back on the previous TASK contest. */
        EE_oo_run_next_task();

        /* Call PreTaskHook in the first TASK context, after have checked that
           the current TASK is not main. */
        if ( current != EE_NIL ) {
          EE_oo_call_PreTaskHook();
        } else {
          /* We are going to get back in idle cycle */
          EE_as_set_execution_context( Idle_Context );
          EE_as_tp_active_start_idle();
        }
      }
    }
  }
}

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)

/* Prepare current Task to Block if Extended Task is configured */
__INLINE__ void __ALWAYS_INLINE__ EE_oo_prepare_to_block(void) {
  /* Get the current TASK */
  register EE_TID current = EE_stk_queryfirst();

  /* The task must go into the WAITING state */
  EE_th_status[current] = WAITING;

  /* [SWS_Os_00473]: The Operating System module shall reset a task’s
      OsTaskExecutionBudget on a transition to the SUSPENDED or WAITING states.
      (SRS_Os_11008) */
  EE_as_tp_stop_budget(EE_as_tp_active.active_tp_RAM_ref, EE_EXECUTION_BUDGET);

  /* Call the Post Task Hook before change stk data structure */
  EE_oo_call_PostTaskHook();

  /* Reset the thread priority bit in the system_ceiling */
  EE_sys_ceiling &= ~EE_th_dispatch_prio[current];
  /* The ready priority is not touched, it is not the same as Schedule! */

  /* Reset ORTI priority */
  EE_ORTI_set_th_priority(current, 0U);

  /* Since the task blocks, it has to be woken up by another EE_hal_stkchange */
  EE_th_waswaiting[current] = 1U;

  /* Extract the TASK from the stacked queue */
  (void)EE_stk_getfirst();
}

/* Reschedule on task blocking if Extended Task is configured */
__INLINE__ void __ALWAYS_INLINE__ EE_oo_reschedule_on_block(void)
{
  register EE_TID next;

  next = EE_rq_queryfirst();
  if ( (next == EE_NIL) || (EE_sys_ceiling >= EE_th_ready_prio[next]) ) {
    /* we have to schedule an interrupted thread that is on the top
     * of its stack; the state is already STACKED! */
    next = EE_stk_queryfirst();
    if ( next != EE_NIL ) {
      EE_th_status[next] = RUNNING;
    } else {
      /* We are switching back to the Idle loop */
      EE_as_set_execution_context( Idle_Context );
      EE_as_tp_active_start_idle();
    }

    /* Enable the TASK Timing Protection Set */
    EE_as_tp_active_set_from_TASK(next);

    /* CONTEXT SWITCH to a previous stacked Task */
    EE_hal_stkchange(next);
  } else { 
    /* We have to schedule a ready thread that is not yet on the stack.
       This means that the TASK set in excution for the first time or that it
       was waiting. */
    EE_th_status[next] = RUNNING;
    EE_sys_ceiling |= EE_th_dispatch_prio[next];

    EE_ORTI_set_th_eq_dispatch_prio(next);

    /* "Press TP start for the first time" for this new release of the
        TASK */
    EE_as_tp_active_start_on_TASK_stacking(next);

    /* Execute context SWITCH, this method return when we have a switch
       back on the previous TASK contest. */
    EE_oo_run_next_task();
  }
  /* We do not have to set the thread priority bit in the system_ceiling,
     it will be set by the primitives that put the task in the RUNNING state */

  /* I don't need to check if the TASK we are returning to is the IDLE Task
     (current == EE_NIL). Waiting is not allowed in IDLE Task. */
  EE_oo_call_PreTaskHook();
}
#endif /* __OO_ECC1__ || __OO_ECC2__ */
#endif /* __PRIVATE_PREEMPTION_POINTS__ */

/** Internal part of ShutdownOS Service */
void EE_oo_ShutdownOS_internal(StatusType Error);

#endif /* __INCLUDE_OO_INTERNAL_H__ */

