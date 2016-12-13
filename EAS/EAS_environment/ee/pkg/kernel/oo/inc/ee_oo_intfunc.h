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
 * Author: 2002-2004 Paolo Gai
 * CVS: $Id: ee_intfunc.h,v 1.2 2006/04/08 21:15:23 pj Exp $
 */

#ifndef __INCLUDE_OO_INTFUNC_H__
#define __INCLUDE_OO_INTFUNC_H__

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA OS section: ee_kernel_bss */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* This file contains internal functions and data structures needed to
   inline some kernel functions. */
extern EE_UREG EE_oo_IRQ_disable_count;

/***************************************************************************
 * The simbol EE_OLD_HAL marks architecture that do not implement new
 * HAL APIs (MUST be defined in the header ee_cpu.h of these architectures)
 ***************************************************************************/
#ifndef EE_OLD_HAL
extern EE_FREG EE_oo_IRQ_suspend_status;
#endif /* EE_OLD_HAL */

extern EE_TYPEAPPMODE EE_ApplicationMode;

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/***************************************************************************
 * 13.7 Operating system execution control 
 ***************************************************************************/

/* 13.7.2 System services                                                  */
/* ----------------------------------------------------------------------- */

/* 13.7.2.1: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_GETACTIVEAPPLICATIONMODE__
/* by default there is only 6the default application mode defined!... */
__INLINE__ AppModeType __ALWAYS_INLINE__ EE_oo_GetActiveApplicationMode( void )
{
  /* EG: XXX Add Service Protection? (I think is just overkilling, can we
     just count this as features extension?) */
  /* Both assignment to enable smart debuggers to notice the entry and
     exit from GetActiveApplicationMode.
     Note that the variable is volatile, so both the writings succeeds */
  EE_ORTI_set_service_in(EE_SERVICETRACE_GETACTIVEAPPLICATIONMODE);
  EE_ORTI_set_service_out(EE_SERVICETRACE_GETACTIVEAPPLICATIONMODE);

  return EE_ApplicationMode; 
}
#endif /* __PRIVATE_GETACTIVEAPPLICATIONMODE__ */

#if defined(__MSRP__) && (!defined(__OO_NO_RESOURCES__))
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_oo_isGlobal(ResourceType ResID)
{
  register EE_UREG isGlobal, ureg_tmp1, ureg_tmp2;
  EE_SREG sreg_tmp;
/* mask off the MSB, that indicates whether this is a global or a
     local resource */

  /*
   * This is the compact expression
   * isGlobal = ((ResID & EE_GLOBAL_MUTEX) != (ResourceType)0U);
   *
   * The following is the extended version introduced to
   * meet MISRA requirements
   */
  ureg_tmp1 = (ResID & EE_GLOBAL_MUTEX);
  ureg_tmp2 = (ResourceType)0U;
  sreg_tmp = (EE_SREG)(ureg_tmp1 != ureg_tmp2);
  isGlobal = (EE_UREG)sreg_tmp;

  return isGlobal;
}
#endif /* __MSRP__ && !__OO_NO_RESOURCES__ */

#ifdef __OO_HAS_ERRORHOOK__

#ifdef EE_SERVICE_PROTECTION__

__INLINE__ void __ALWAYS_INLINE__  EE_oo_call_ErrorHook( StatusType Error )
{
#ifdef EE_AS_OSAPPLICATIONS__
  /* Call status application hook with right privileges */
  extern void EE_hal_call_app_status_hook(StatusType Error, EE_STATUSHOOKTYPE
    status_hook, ApplicationType app);

  register  ApplicationType i;
#endif /* EE_AS_OSAPPLICATIONS__ */

  /* Handle Error Hooks execution context */
  EE_TYPECONTEXT prev_context = EE_as_execution_context;

  /* Set the context execution at ErrorHook Context */
  EE_as_set_execution_context(ErrorHook_Context);

  ErrorHook( Error );

#ifdef EE_AS_OSAPPLICATIONS__
  /* [SWS_Os_00246] When an error occurs AND an application-specific error
      hook is configured for the faulty OS-Application <App>, the Operating
      System module shall call that application-specific error hook
      ErrorHook_<App> after the system specific error hook is called
      (if configured). (SRS_Os_11013) */

  /* ApplID 0 is always reserved by the Kernel */
  for ( i = 1U; i < EE_MAX_APP; ++i ) {
    register EE_STATUSHOOKTYPE error_hook =
      EE_as_Application_errorhook[i];

    if ( error_hook != 0U ) {
      /* [SWS_Os_00085] The Operating System module shall execute an
          application-specific error hook with the access rights of the
          associated OS-Application. */
      EE_hal_call_app_status_hook(Error, error_hook, i);
    }
  }
#endif /* EE_AS_OSAPPLICATIONS__ */

  /* Set back the previous context */
  EE_as_set_execution_context( prev_context );
}
#else /* EE_SERVICE_PROTECTION__ */
__INLINE__ void __ALWAYS_INLINE__  EE_oo_call_ErrorHook( StatusType Error )
{
  /* Just Call the error Hook */
  ErrorHook( Error );
}
#endif /* EE_SERVICE_PROTECTION__ */

#ifndef __OO_ERRORHOOK_NOMACROS__
/* 
    Inlines functions to fill parameters of the system service which called
    ErrorHook
*/

__INLINE__ void __ALWAYS_INLINE__ EE_oo_set_error_serviceid(OSServiceIdType
  ServiceID)
{
  (*EE_oo_get_errorhook_service_id()) = ServiceID;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_ActivateTask(TaskType
  TaskID)
{
  (*EE_oo_get_errorhook_service_id()) = OSServiceId_ActivateTask;
  EE_oo_get_errorhook_data()->param1.value_param = (EE_UREG)TaskID;
}

#ifndef __OO_NO_CHAINTASK__
__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_ChainTask(TaskType
  TaskID)
{
  (*EE_oo_get_errorhook_service_id())= OSServiceId_ChainTask;
  EE_oo_get_errorhook_data()->param1.value_param = (EE_UREG)TaskID;
}
#else /* __OO_NO_CHAINTASK__ */
#define EE_oo_fill_error_data_ChainTask(TaskID)         ((void)0)
#endif /* __OO_NO_CHAINTASK__ */

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_GetTaskID(TaskRefType
  TaskID)
{
  (*EE_oo_get_errorhook_service_id()) = OSServiceId_GetTaskID;
  EE_oo_get_errorhook_data()->param1.task_ref = TaskID;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_GetTaskState(TaskType
  TaskID, TaskStateRefType State)
{
  register EE_oo_ErrorHook_parameters * const
    p_errorhook_parameter = EE_oo_get_errorhook_data();
  (*EE_oo_get_errorhook_service_id()) = OSServiceId_GetTaskState;
  p_errorhook_parameter->param1.value_param    = (EE_UREG)TaskID;
  p_errorhook_parameter->param2.task_state_ref = State;
}

#ifndef __OO_NO_RESOURCES__
__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_GetResource(
  ResourceType ResID)
{
  (*EE_oo_get_errorhook_service_id()) = OSServiceId_GetResource;
  EE_oo_get_errorhook_data()->param1.value_param = ResID;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_ReleaseResource(
  ResourceType ResID)
{
  (*EE_oo_get_errorhook_service_id()) = OSServiceId_ReleaseResource;
  EE_oo_get_errorhook_data()->param1.value_param = ResID;
}
#else /* __OO_NO_RESOURCES__ */
#define EE_oo_fill_error_data_GetResource(ResID)        ((void)0)
#define EE_oo_fill_error_data_ReleaseResource(ResID)    ((void)0)
#endif /* __OO_NO_RESOURCES__ */

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_SetEvent(TaskType
  TaskID, EventMaskType Mask)
{
  register EE_oo_ErrorHook_parameters * const
    p_errorhook_parameter = EE_oo_get_errorhook_data();

  (*EE_oo_get_errorhook_service_id()) = OSServiceId_SetEvent;
  p_errorhook_parameter->param1.value_param = (EE_UREG)TaskID;
  p_errorhook_parameter->param2.value_param = Mask;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_ClearEvent(
  EventMaskType Mask)
{
  (*EE_oo_get_errorhook_service_id()) = OSServiceId_ClearEvent;
  EE_oo_get_errorhook_data()->param1.value_param = Mask;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_GetEvent(TaskType
    TaskID, EventMaskRefType Event)
{
  register EE_oo_ErrorHook_parameters * const
    p_errorhook_parameter = EE_oo_get_errorhook_data();

  (*EE_oo_get_errorhook_service_id()) = OSServiceId_GetEvent;
  p_errorhook_parameter->param1.value_param = (EE_UREG)TaskID;
  p_errorhook_parameter->param2.event_ref   = Event;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_WaitEvent(EventMaskType
  Mask)
{
  (*EE_oo_get_errorhook_service_id()) = OSServiceId_WaitEvent;
  EE_oo_get_errorhook_data()->param1.value_param = Mask;
}
#else /* defined(__OO_ECC1__) || defined(__OO_ECC2__) */
#define EE_oo_fill_error_data_SetEvent(TaskID, Mask)    ((void)0)
#define EE_oo_fill_error_data_ClearEvent(Mask)          ((void)0)
#define EE_oo_fill_error_data_GetEvent(TaskID, Event)   ((void)0)
#define EE_oo_fill_error_data_WaitEvent(Mask)           ((void)0)
#endif /* defined(__OO_ECC1__) || defined(__OO_ECC2__) */

#ifndef __OO_NO_ALARMS__
__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_GetAlarmBase(AlarmType
  AlarmID, AlarmBaseRefType Info)
{
  register EE_oo_ErrorHook_parameters * const
    p_errorhook_parameter = EE_oo_get_errorhook_data();
  
  (*EE_oo_get_errorhook_service_id()) = OSServiceId_GetAlarmBase;
  p_errorhook_parameter->param1.value_param = AlarmID;
  p_errorhook_parameter->param2.alarm_base_ref = Info;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_GetAlarm(AlarmType
  AlarmID, TickRefType Tick)
{
  register EE_oo_ErrorHook_parameters * const
    p_errorhook_parameter = EE_oo_get_errorhook_data();

  (*EE_oo_get_errorhook_service_id()) = OSServiceId_GetAlarm;
  p_errorhook_parameter->param1.value_param = AlarmID;
  p_errorhook_parameter->param2.tick_ref = Tick;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_SetRelAlarm(AlarmType
  AlarmID, TickType increment, TickType cycle)
{
  register EE_oo_ErrorHook_parameters * const
    p_errorhook_parameter = EE_oo_get_errorhook_data();

  (*EE_oo_get_errorhook_service_id()) = OSServiceId_SetRelAlarm;
  p_errorhook_parameter->param1.value_param = AlarmID;
  p_errorhook_parameter->param2.value_param = increment;
  p_errorhook_parameter->param3.value_param = cycle;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_SetAbsAlarm(AlarmType
  AlarmID, TickType start, TickType cycle)
{
  register EE_oo_ErrorHook_parameters * const
    p_errorhook_parameter = EE_oo_get_errorhook_data();

  (*EE_oo_get_errorhook_service_id()) = OSServiceId_SetAbsAlarm;
  p_errorhook_parameter->param1.value_param = AlarmID;
  p_errorhook_parameter->param2.value_param = start;
  p_errorhook_parameter->param3.value_param = cycle;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_CancelAlarm(AlarmType
  AlarmID)
{
  (*EE_oo_get_errorhook_service_id()) = OSServiceId_CancelAlarm;
  EE_oo_get_errorhook_data()->param1.value_param = AlarmID;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_IncrementCounter(
  CounterType CounterID)
{
  register EE_oo_ErrorHook_parameters * const
    p_errorhook_parameter = EE_oo_get_errorhook_data();

  (*EE_oo_get_errorhook_service_id()) = OSServiceId_IncrementCounter;
  p_errorhook_parameter->param1.value_param = CounterID;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_GetCounterValue(
  CounterType CounterID, TickRefType Value)
{
  register EE_oo_ErrorHook_parameters * const
    p_errorhook_parameter = EE_oo_get_errorhook_data();

  (*EE_oo_get_errorhook_service_id()) = OSServiceId_GetCounterValue;
  p_errorhook_parameter->param1.value_param = CounterID;
  p_errorhook_parameter->param2.tick_ref = Value;
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_GetElapsedValue(
  CounterType CounterID, TickRefType Value, TickRefType ElapsedValue)
{
  register EE_oo_ErrorHook_parameters * const
    p_errorhook_parameter = EE_oo_get_errorhook_data();

    (*EE_oo_get_errorhook_service_id()) = OSServiceId_GetElapsedValue;
  p_errorhook_parameter->param1.value_param = CounterID;
  p_errorhook_parameter->param2.tick_ref = Value;
  p_errorhook_parameter->param3.tick_ref = ElapsedValue;
}

#else /* __OO_NO_ALARMS__ */
#define EE_oo_fill_error_data_GetAlarmBase(AlarmID, Info)           ((void)0)
#define EE_oo_fill_error_data_GetAlarm(AlarmID, Tick)               ((void)0)
#define EE_oo_fill_error_data_SetRelAlarm(AlarmID, increment, cycle) \
    ((void)0)
#define EE_oo_fill_error_data_SetAbsAlarm(AlarmID, start, cycle)    ((void)0)
#define EE_oo_fill_error_data_CancelAlarm(AlarmID)                  ((void)0)
#define EE_oo_fill_error_data_IncrementCounter(CounterID)           ((void)0)
#define EE_oo_fill_error_data_GetCounterValue(CounterID, Value)     ((void)0)
#define EE_oo_fill_error_data_GetElapsedValue(CounterID, Value, ElapsedValue) \
    ((void)0)
#endif /* __OO_NO_ALARMS__ */

__INLINE__ void __ALWAYS_INLINE__ EE_oo_fill_error_data_StartOS(AppModeType
  Mode)
{
  (*EE_oo_get_errorhook_service_id()) = OSServiceId_StartOS;
  EE_oo_get_errorhook_data()->param1.value_param = Mode;
}

#else /* __OO_ERRORHOOK_NOMACROS__ */
#define EE_oo_set_error_serviceid(ServiceID)                        ((void)0)
#define EE_oo_fill_error_data_ActivateTask(TaskID)                  ((void)0)
#define EE_oo_fill_error_data_ChainTask(TaskID)                     ((void)0)
#define EE_oo_fill_error_data_GetTaskID(TaskID)                     ((void)0)
#define EE_oo_fill_error_data_GetTaskState(TaskID, State)           ((void)0)
#define EE_oo_fill_error_data_GetResource(ResID)                    ((void)0)
#define EE_oo_fill_error_data_ReleaseResource(ResID)                ((void)0)
#define EE_oo_fill_error_data_SetEvent(TaskID, Mask)                ((void)0)
#define EE_oo_fill_error_data_ClearEvent(Mask)                      ((void)0)
#define EE_oo_fill_error_data_GetEvent(TaskID, Event)               ((void)0)
#define EE_oo_fill_error_data_WaitEvent(Mask)                       ((void)0)
#define EE_oo_fill_error_data_GetAlarmBase(AlarmID, Info)           ((void)0)
#define EE_oo_fill_error_data_GetAlarm(AlarmID, Tick)               ((void)0)
#define EE_oo_fill_error_data_SetRelAlarm(AlarmID, increment, cycle) \
    ((void)0)
#define EE_oo_fill_error_data_SetAbsAlarm(AlarmID, start, cycle)    ((void)0)
#define EE_oo_fill_error_data_CancelAlarm(AlarmID)                  ((void)0)
#define EE_oo_fill_error_data_IncrementCounter(AlarmID, TaskID, ActionKind) \
    ((void)0)
#define EE_oo_fill_error_data_with_mask_IncrementCounter(AlarmID, TaskID, \
    Mask, ActionKind) ((void)0)
#define EE_oo_fill_error_data_GetCounterValue(CounterID, Value)     ((void)0)
#define EE_oo_fill_error_data_GetElapsedValue(CounterID, Value, ElapsedValue) \
    ((void)0)
#define EE_oo_fill_error_data_StartOS(Mode)                         ((void)0)
#define EE_oo_fill_error_data_WaitSem(Sem)                          ((void)0)
#define EE_oo_fill_error_data_PostSem(Sem)                          ((void)0)
#endif /* __OO_ERRORHOOK_NOMACROS__ */

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_service(OSServiceIdType 
    ServiceID, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) 
    {
      EE_oo_set_error_serviceid(ServiceID);
      EE_ErrorHook_nested_flag = 1U;
      EE_oo_call_ErrorHook(Error);
      EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_ActivateTask(TaskType
    TaskID, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_ActivateTask(TaskID);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

#ifndef __OO_NO_CHAINTASK__
__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_ChainTask(TaskType TaskID,
    StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_ChainTask(TaskID);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

#else /* __OO_NO_CHAINTASK__ */
#define EE_oo_notify_error_ChainTask(TaskID, Error)   ((void)0);
#endif /* __OO_NO_CHAINTASK__ */

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_GetTaskID(TaskRefType
    TaskID, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_GetTaskID(TaskID);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_GetTaskState(TaskType
    TaskID, TaskStateRefType State, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_GetTaskState(TaskID, State);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

#ifndef __OO_NO_RESOURCES__
__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_GetResource(
    ResourceType ResID, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
      EE_oo_fill_error_data_GetResource(ResID);
      EE_ErrorHook_nested_flag = 1U;
      EE_oo_call_ErrorHook(Error);
      EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_ReleaseResource(
    ResourceType ResID, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
      EE_oo_fill_error_data_ReleaseResource(ResID);
      EE_ErrorHook_nested_flag = 1U;
      EE_oo_call_ErrorHook(Error);
      EE_ErrorHook_nested_flag = 0U;
    }
}
#else /* __OO_NO_RESOURCES__ */
#define EE_oo_notify_error_GetResource(ResID, Error)      ((void)0)
#define EE_oo_notify_error_ReleaseResource(ResID, Error)  ((void)0)
#endif /* __OO_NO_RESOURCES__ */

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_SetEvent(TaskType
    TaskID, EventMaskType Mask, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_SetEvent(TaskID, Mask);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_ClearEvent(EventMaskType
    Mask, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_ClearEvent(Mask);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_GetEvent(TaskType TaskID,
    EventMaskRefType Event, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
      EE_oo_fill_error_data_GetEvent(TaskID, Event);
      EE_ErrorHook_nested_flag = 1U;
      EE_oo_call_ErrorHook(Error);
      EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_WaitEvent(EventMaskType
    Mask, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
      EE_oo_fill_error_data_WaitEvent(Mask);
      EE_ErrorHook_nested_flag = 1U;
      EE_oo_call_ErrorHook(Error);
      EE_ErrorHook_nested_flag = 0U;
    }
}
#else /* #if defined(__OO_ECC1__) || defined(__OO_ECC2__) */
#define EE_oo_notify_error_SetEvent(TaskID, Mask, Error)        ((void)0)
#define EE_oo_notify_error_ClearEvent(Mask, Error)              ((void)0)
#define EE_oo_notify_error_GetEvent(TaskID, Event, Error)       ((void)0)
#define EE_oo_notify_error_WaitEvent(Mask, Error)               ((void)0)
#endif /* #if defined(__OO_ECC1__) || defined(__OO_ECC2__) */

#ifndef __OO_NO_ALARMS__
__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_GetAlarmBase(AlarmType
    AlarmID, AlarmBaseRefType Info, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_GetAlarmBase(AlarmID, Info);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_GetAlarm(AlarmType AlarmID,
    TickRefType Tick, StatusType Error)
{ 
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_GetAlarm(AlarmID, Tick);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_SetRelAlarm(AlarmType
    AlarmID, TickType increment, TickType cycle, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_SetRelAlarm(AlarmID, increment, cycle);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_SetAbsAlarm(AlarmType
    AlarmID, TickType start, TickType cycle, StatusType Error )
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_SetAbsAlarm(AlarmID, start, cycle);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_CancelAlarm(AlarmType 
    AlarmID, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_CancelAlarm(AlarmID);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_IncrementCounter(
    CounterType CounterID, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_IncrementCounter(CounterID);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_GetCounterValue(
    CounterType CounterID, TickRefType Value, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_GetCounterValue(CounterID, Value);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_GetElapsedValue(CounterType
    CounterID, TickRefType Value, TickRefType ElapsedValue, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
        EE_oo_fill_error_data_GetElapsedValue(CounterID, Value, ElapsedValue);
        EE_ErrorHook_nested_flag = 1U;
        EE_oo_call_ErrorHook(Error);
        EE_ErrorHook_nested_flag = 0U;
    }
}

#else /* __OO_NO_ALARMS__ */
#define EE_oo_notify_error_GetAlarmBase(AlarmID, Tick, Error)       ((void)0)
#define EE_oo_notify_error_GetAlarm(AlarmID, Tick, Error)           ((void)0)
#define EE_oo_notify_error_SetRelAlarm(AlarmID, increment, cycle, Error) \
    ((void) 0)
#define EE_oo_notify_error_SetAbsAlarm(AlarmID, start, cycle, Error) \
    ((void)0)
#define EE_oo_notify_error_CancelAlarm(AlarmID, Error)              ((void)0)
#define EE_oo_notify_error_IncrementCounter(CounterID, Error)       ((void)0)
#define EE_oo_notify_error_GetCounterValue(CounterID, Value, Error) ((void)0)
#define EE_oo_notify_error_GetElapsedValue(CounterID, Value, ElapsedValue, \
    Error)    ((void) 0)
#endif /* __OO_NO_ALARMS__ */

__INLINE__ void __ALWAYS_INLINE__ EE_oo_notify_error_StartOS(AppModeType
    Mode, StatusType Error)
{
    if (EE_ErrorHook_nested_flag == 0U) {
      EE_oo_fill_error_data_StartOS(Mode);
      EE_ErrorHook_nested_flag = 1U;
      EE_oo_call_ErrorHook(Error);
      EE_ErrorHook_nested_flag = 0U;
    }
}

#else /* __OO_HAS_ERRORHOOK__ */
#define EE_oo_notify_error_service(ServiceID, Error)            ((void)0)
#define EE_oo_notify_error_ActivateTask(TaskID, Error)          ((void)0)
#define EE_oo_notify_error_ChainTask(TaskID, Error)             ((void)0)
#define EE_oo_notify_error_TerminateTask                        ((void)0)
#define EE_oo_notify_error_GetTaskID(TaskID, Error)             ((void)0)
#define EE_oo_notify_error_GetTaskState(TaskID, State, Error)   ((void)0)
#define EE_oo_notify_error_GetResource(ResID, Error)            ((void)0)
#define EE_oo_notify_error_ReleaseResource(ResID, Error)        ((void)0)
#define EE_oo_notify_error_SetEvent(TaskID, Mask, Error)        ((void)0)
#define EE_oo_notify_error_ClearEvent(Mask, Error)              ((void)0)
#define EE_oo_notify_error_GetEvent(TaskID, Event, Error)       ((void)0)
#define EE_oo_notify_error_WaitEvent(Mask, Error)               ((void)0)
#define EE_oo_notify_error_GetAlarmBase(AlarmID, Tick, Error)   ((void)0)
#define EE_oo_notify_error_GetAlarm(AlarmID, Tick, Error)       ((void)0)
#define EE_oo_notify_error_SetRelAlarm(AlarmID, increment, cycle, Error) \
    ((void) 0)
#define EE_oo_notify_error_SetAbsAlarm(AlarmID, start, cycle, Error) \
    ((void)0)
#define EE_oo_notify_error_CancelAlarm(AlarmID, Error)          ((void)0)
#define EE_oo_notify_error_IncrementCounter(CounterID, Error)   ((void)0)
#define EE_oo_notify_error_GetCounterValue(CounterID, Value, Error) \
    ((void)0)
#define EE_oo_notify_error_GetElapsedValue(CounterID, Value, ElapsedValue, \
    Error)    ((void) 0)
#define EE_oo_notify_error_StartOS(Mode, Error)                 ((void)0)
#define EE_oo_notify_error_WaitSem(Sem, Error)                  ((void)0)
#define EE_oo_notify_error_PostSem(Sem, Error)                  ((void)0)
#endif /* __OO_HAS_ERRORHOOK__ */

/* XXX: Here because have to be seen in eecfg.c to generate syscall array */
#if defined(__EE_MEMORY_PROTECTION__) && defined(__OO_HAS_ERRORHOOK__)
void EE_oo_notify_error_from_us_internal( OSServiceIdType ServiceID,
  const EE_oo_ErrorHook_parameters * const error_parameters_ref,
  StatusType Error );
#endif /* __INCLUDE_OO_INTFUNC_H__ && __OO_HAS_ERRORHOOK__*/

#endif /* __INCLUDE_OO_INTFUNC_H__ */

