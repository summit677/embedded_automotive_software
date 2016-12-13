/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2012  Evidence Srl
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

 /** @file      ee_as_internal.h
  *  @brief     Internals for Autosar layer
  *  @author    Errico Guidieri
  *  @date      2012
  */

#ifndef INCLUDE_EE_KERNEL_AS_INTERNAL__
#define INCLUDE_EE_KERNEL_AS_INTERNAL__

#include "kernel/as/inc/ee_as_kernel.h"

#ifdef EE_AS_USER_SPINLOCKS__
/*******************************************************************************
 *                        Spinlock Internal Clean-Up
 ******************************************************************************/
/* Internal Clean-up function */
SpinlockIdType EE_as_release_all_spinlocks( EE_TID tid );

#else  /* EE_AS_USER_SPINLOCKS__ */
#define EE_as_release_all_spinlocks(tid) ((void)0U)
#endif /* EE_AS_USER_SPINLOCKS__ */

/*******************************************************************************
 *                        Schedule Tables Utilities
 ******************************************************************************/
#ifdef EE_AS_SCHEDULETABLES__
/* Utilities inline functions to check ticks and deviation values */
__INLINE__ TickType __ALWAYS_INLINE__ EE_as_tick_min( TickType t1, TickType t2 )
{
  if ( t1 <= t2 ) {
    return t1;
  } else {
    return t2;
  }
}

__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_as_abs( TickType t )
{
  if ( t < EE_TYPETICK_HALF_VALUE ) {
    return t;
  } else {
    return -t;
  }
}

#endif /* EE_AS_SCHEDULETABLES__ */

#ifdef EE_AS_RPC__
/*******************************************************************************
 *                   Synchronous Remote Procedure Calls
 ******************************************************************************/
/** @brief Macro used to check if an id is a remote id */
#define EE_AS_ID_REMOTE(id)         ((((EE_UINT32)(id)) & \
    (EE_UINT32)EE_REMOTE_TID) != 0U)
/** @brief Macro used to unmark a remote id */
#define EE_AS_UNMARK_REMOTE_ID(id)  (((EE_UINT32)(id)) & (~EE_REMOTE_TID))

/** @brief define that identify an invalid ServiceId */
#define INVALID_SERVICE_ID      ((OSServiceIdType)-1)
#define INVALID_ERROR           ((StatusType)-1)

/** @brief RPC Handler to be called inside IIRQ handler */
extern void  EE_as_rpc_handler( void );

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following code belong to ERIKA API section ee_api_text */
#define API_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef __EE_MEMORY_PROTECTION__
/** @brief The following implement a synchronous RPC kernel primitive from
    "user space" (so it's a syscall). */
extern StatusType EE_as_rpc_from_us( OSServiceIdType ServiceId,
  EE_os_param param1, EE_os_param param2, EE_os_param param3 );
#else /* __EE_MEMORY_PROTECTION__ */
__INLINE__ StatusType __ALWAYS_INLINE__ EE_as_rpc_from_us( OSServiceIdType
  ServiceId, EE_os_param param1, EE_os_param param2, EE_os_param param3 )
{
  StatusType ev;
  register EE_FREG const flag = EE_hal_begin_nested_primitive();
  ev = EE_as_rpc(ServiceId, param1, param2, param3);
  EE_hal_end_nested_primitive(flag);
  return ev;
}
#endif /* __EE_MEMORY_PROTECTION__ */
#ifdef EE_SUPPORT_MEMMAP_H
#define API_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* EE_AS_RPC__ */

#ifdef EE_AS_OSAPPLICATIONS__
/*******************************************************************************
 *                        OSApplication Utilities
 ******************************************************************************/

/** @brief Used to terminate current OS-Application. */
void EE_as_terminate_current_app_task( void );

/** @brief Used to terminate current OS-Application. */
#if defined(EE_MAX_ISR2) && (EE_MAX_ISR2 > 0)
__INLINE__ void __ALWAYS_INLINE__ EE_as_terminate_current_app( void )
{
  if ( EE_hal_get_IRQ_nesting_level() == 0U ) {
    /* We are in a TASK */
    EE_as_terminate_current_app_task();
  } else {
    /* We are in the an ISR2 */
    (void)EE_as_TerminateISR2();
  }
}
#else  /* EE_MAX_ISR2 > 0 */
__INLINE__ void __ALWAYS_INLINE__ EE_as_terminate_current_app( void )
{
  /* There are no ISR2: We are in a TASK */
  EE_as_terminate_current_app_task();
}
#endif /* EE_MAX_ISR2 > 0 */

/**
 * @brief Supposed to be called by EE_as_TerminateISR2 to restore trackin
 *  OSApplication global variable and eventually terminate stacked.
 */
void EE_as_after_IRQ_interrupted_app ( ApplicationType interrupted_app );

/* @brief Function determines the currently running OS-Application.
 *         (a unique identifier has to be allocated to each application).
 */
/* ApplicationType EE_as_GetApplicationID_internal( void ); */

/** @brief his service determines if the OS-Applications, given by ApplID, is
 *    allowed to use the IDs of a Task, ISR, Resource, Counter, Alarm or
 *    Schedule Table in API calls. */
ObjectAccessType EE_as_CheckObjectAccess_internal( ApplicationType ApplID,
  ObjectTypeType ObjectType, EE_UTID ObjectID );

/** @brief his service determines the OS-Applications to which the
  ID of a Task, ISR, Resource, Counter, Alarm or Schedule Table belong to. */
ApplicationType EE_as_CheckObjectOwnership_internal( ObjectTypeType ObjectType,
  EE_os_param_id ObjectID );

/** @brief Internal part of TerminateApplication service */
void EE_as_TerminateApplication_internal( ApplicationType Application,
  RestartType RestartOption ); 

/* THESE HAL FUNCTIONS DECLARATION ARE PUT HERE BECAUSE SIGNATURE DEPENDS ON
   AS KERNEL TYPES */

/** @brief Call application hook with right privileges */
void EE_hal_call_app_hook( EE_HOOKTYPE hook, ApplicationType app );

/** @brief Call status application hook with right privileges */
void EE_hal_call_app_status_hook( StatusType Error, EE_STATUSHOOKTYPE
  status_hook, ApplicationType app );

/** @brief Utility macro used to transform a Application ID in a bit mask */
#define EE_APP_TO_MASK(app_id)  ((EE_TYPEACCESSMASK)1U << (app_id))

#ifdef EE_SERVICE_PROTECTION__
/*******************************************************************************
 *            OSApplication Service Protection Access Data Structures
 ******************************************************************************/
/* [OS056] If an OS-object identifier is the parameter of an Operating System
    module's system service, and no sufficient access rights have been assigned
    to this OS-object at configuration time
    (Parameter Os[...]AccessingApplication) to the calling Task/Category 2 ISR,
    the Operating System module's system service shall return E_OS_ACCESS.
    (BSW11001, BSW11010, BSW11013) */
/* [OS448]: The Operating System module shall prevent access of OS-Applications,
    trusted or non-trusted, to objects not belonging to this OS-Application,
    except access rights for such objects are explicitly granted by
    configuration.
*/
/* [OS504] The Operating System module shall deny access to Operating System
    objects from other OS-Applications to an OS-Application which is not in
    state APPLICATION_ACCESSIBLE. */
/* [OS509] If a service call is made on an Operating System object that is owned
    by another OS-Application without state APPLICATION_ACCESSIBLE,
    then the Operating System module shall return E_OS_ACCESS. */

/** @var Contains access rules for TASKs */
extern EE_TYPEACCESSMASK const EE_as_task_access_rules[/*EE_MAX_TASK*/];

/** @var Contains access rules for ISRs */
extern EE_TYPEACCESSMASK const EE_as_isr_access_rules[/*EE_MAX_ISR_ID*/];

#ifndef __OO_NO_RESOURCES__
/** @var Contains access rules for RESOURCESs */
extern EE_TYPEACCESSMASK const EE_as_resource_access_rules[/*EE_MAX_RESOURCE*/];
#endif /* !__OO_NO_RESOURCES__ */

#if (!defined(__OO_NO_ALARMS__)) || defined(EE_AS_SCHEDULETABLES__)
/** @var Contains access rules for COUNTERs */
extern EE_TYPEACCESSMASK const EE_as_counter_access_rules[/*EE_MAX_COUNTER*/];
#endif /* !__OO_NO_ALARMS__ || !EE_AS_SCHEDULETABLES__ */

#ifndef __OO_NO_ALARMS__
/** @var Contains access rules for ALARMs */
extern EE_TYPEACCESSMASK const EE_as_alarm_access_rules[/*EE_MAX_ALARM*/];
#endif /* !__OO_NO_ALARMS__ */

#ifdef EE_AS_SCHEDULETABLES__
/** @var Contains access rules for SCHEDULE TABLEs */
extern EE_TYPEACCESSMASK const 
  EE_as_scheduletable_access_rules[/*EE_MAX_SCHEDULETABLE*/];
#endif /* EE_AS_SCHEDULETABLES__ */

#ifdef EE_AS_USER_SPINLOCKS__
/** @var Contains access rules for SCHEDULE TABLEs */
extern EE_TYPEACCESSMASK const 
  EE_as_spinlock_access_rules[/*EE_MAX_SPINLOCK_USER*/];
#endif /* EE_AS_USER_SPINLOCKS__ */


/* OSApplication Objects belog to active Macros */
#define EE_OSAPP_TASK_ACCESS(TaskID) (EE_as_Application_RAM[\
  EE_th_app[(TaskID + 1U)]].ApplState == APPLICATION_ACCESSIBLE)

#define EE_OSAPP_ISR_ACCESS(ISRID) (EE_as_Application_RAM[\
  EE_as_ISR_ROM[ISRID].ApplID].ApplState == APPLICATION_ACCESSIBLE)

#define EE_OSAPP_COUNTER_ACCESS(CounterID) (EE_as_Application_RAM[\
  EE_counter_ROM[CounterID].ApplID].ApplState == APPLICATION_ACCESSIBLE)

#define EE_OSAPP_ALARM_ACCESS(AlarmID) (EE_as_Application_RAM[\
  EE_alarm_ROM[AlarmID].ApplID].ApplState == APPLICATION_ACCESSIBLE)

#define EE_OSAPP_SCHED_TABLE_ACCESS(SchedTableID) (EE_as_Application_RAM[\
  EE_as_Schedule_Table_ROM[SchedTableID].ApplID].ApplState == \
  APPLICATION_ACCESSIBLE)

/* Access Macros */
#define EE_TASK_ACCESS(TaskID, ApplID) (((EE_as_task_access_rules[TaskID] & \
  EE_APP_TO_MASK(ApplID)) && EE_OSAPP_TASK_ACCESS(TaskID)))

#define EE_ISR_ACCESS(ISRID, ApplID) (((EE_as_isr_access_rules[ISRID] & \
  EE_APP_TO_MASK(ApplID)) && EE_OSAPP_ISR_ACCESS(ISRID)))

#define EE_COUNTER_ACCESS(CounterID, ApplID) \
  (((EE_as_counter_access_rules[CounterID] & EE_APP_TO_MASK(ApplID)) && \
    EE_OSAPP_COUNTER_ACCESS(CounterID)))

#define EE_ALARM_ACCESS(AlarmID, ApplID) (((EE_as_alarm_access_rules[\
  AlarmID] & EE_APP_TO_MASK(ApplID)) && EE_OSAPP_ALARM_ACCESS(AlarmID)))

#define EE_SCHED_TABLE_ACCESS(SchedTableID, ApplID) \
  (((EE_as_scheduletable_access_rules[SchedTableID] & EE_APP_TO_MASK(ApplID)) \
   && EE_OSAPP_SCHED_TABLE_ACCESS(SchedTableID)))

#define EE_RESOURCE_ACCESS(ResourceID, ApplID) \
  ((EE_as_resource_access_rules[ResourceID] & EE_APP_TO_MASK(ApplID)))

#define EE_SPINLOCK_ACCESS(SpinlockID, ApplID) \
  ((EE_as_spinlock_access_rules[SpinlockID] & EE_APP_TO_MASK(ApplID)))

/* Error Macros */
#define EE_TASK_ACCESS_ERR(TaskID, ApplID) (!EE_TASK_ACCESS(TaskID, ApplID))

#define EE_ISR_ACCESS_ERR(ISRID, ApplID) (!EE_ISR_ACCESS(ISRID, ApplID))

#define EE_COUNTER_ACCESS_ERR(CounterID, ApplID) (!EE_COUNTER_ACCESS(CounterID,\
  ApplID))

#define EE_ALARM_ACCESS_ERR(AlarmID, ApplID) (!EE_ALARM_ACCESS(AlarmID, \
  ApplID))

#define EE_SCHED_TABLE_ACCESS_ERR(SchedTableID, ApplID) \
  (!EE_SCHED_TABLE_ACCESS(SchedTableID, ApplID))

#define EE_RESOURCE_ACCESS_ERR(ResourceID, ApplID) \
  (!EE_RESOURCE_ACCESS(ResourceID, ApplID))

#define EE_SPINLOCK_ACCESS_ERR(SpinlockID, ApplID) \
  (!EE_SPINLOCK_ACCESS(SpinlockID, ApplID))

#endif /* EE_SERVICE_PROTECTION__ */
#endif /* EE_AS_OSAPPLICATIONS__ */

#ifdef EE_AS_HAS_PROTECTIONHOOK__
/*******************************************************************************
 *                     ProtectionHook Internal Support
 ******************************************************************************/
#ifdef EE_AS_OSAPPLICATIONS__
/**
 * @brief This function wraps the call to the protection hook.
 * Also, it does what is required to do according to what the ProtectionHook
 * returns.
 * @param error_app the OS-Application that caused the error
 * @param error     the error to send to the ProtectionHook function
 */
void EE_as_handle_protection_error( ApplicationType error_app,
  StatusType error );
#else /* EE_AS_OSAPPLICATIONS__ */
/**
 * @brief This function wraps the call to the protection hook.
 * Also, it does what is required to do according to what the ProtectionHook
 * returns.
 * @param error     the error to send to the ProtectionHook function
 */
void EE_as_handle_protection_error( StatusType error );
#endif /* EE_AS_OSAPPLICATIONS__ */
#endif /* EE_AS_HAS_PROTECTIONHOOK__ */

#ifdef __EE_MEMORY_PROTECTION__
/*******************************************************************************
 *                   Memory Protection Internal Support
 ******************************************************************************/
/** Syscall table */
extern EE_FADDR const EE_syscall_table[/*EE_SYSCALL_NR*/];

/** @typedef for TRUSTED Function pointers */
typedef StatusType (*EE_TRUSTEDFUNCTYPE)(TrustedFunctionIndexType,
  TrustedFunctionParameterRefType);

/* THIS HAL FUNCTIONS DECLARATION ARE PUT HERE BECAUSE SIGNATURE DEPENDS ON
   AS KERNEL TYPES */

/**
 * Return the access permission for the given memory area.  Defined in the CPU
 * layer. */
AccessType EE_hal_get_app_mem_access(ApplicationType app,
  MemoryStartAddressType beg, MemorySizeType size);

#if defined(EE_SYSCALL_NR) && defined(EE_MAX_SYS_SERVICEID) &&\
  (EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID)
__INLINE__ EE_TYPEBOOL __ALWAYS_INLINE__
  EE_as_active_app_is_inside_trusted_function_call ( void )
{
  return EE_as_Application_RAM[EE_as_active_app].
    TrustedFunctionCallsCounter != 0U;
}
#else /* EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID */
#define EE_as_active_app_is_inside_trusted_function_call()  EE_FALSE
#endif /* EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID */
#else /* __EE_MEMORY_PROTECTION__ */
#define EE_as_active_app_is_inside_trusted_function_call()  EE_FALSE
#endif /* __EE_MEMORY_PROTECTION__ */

/*******************************************************************************
 *                     Stack Monitoring Internal Support
 ******************************************************************************/
#ifdef EE_STACK_MONITORING__

#ifdef EE_AS_OSAPPLICATIONS__
/* Functions used to check and handle Stack Overflow, with short cut to
   pass current application */
void EE_as_check_and_handle_stack_overflow( ApplicationType appid,
  EE_UREG tos );
#else /* EE_AS_OSAPPLICATIONS__ */
/* Functions used to check and handle Stack Overflow. Have to be to be
   implemented in each porting that support stack monitoring */
void EE_as_check_and_handle_stack_overflow( EE_UREG tos );
#endif  /* EE_AS_OSAPPLICATIONS__ */

/* Used Internally in Kernel primitives */
void EE_as_monitoring_the_stack( void );

#else /* EE_STACK_MONITORING__ */
#ifdef EE_AS_OSAPPLICATIONS__
#define EE_as_check_and_handle_stack_overflow( appid, tos )   ((void)0)
#else /* EE_AS_OSAPPLICATIONS__ */
#define EE_as_check_and_handle_stack_overflow( tos )          ((void)0)
#endif /* EE_AS_OSAPPLICATIONS__ */
#define EE_as_monitoring_the_stack()                          ((void)0)
#endif /* EE_STACK_MONITORING__ */

/* Used to select witch system ERROR handling function call */
#ifdef EE_AS_HAS_PROTECTIONHOOK__
#ifdef EE_AS_OSAPPLICATIONS__
#define EE_as_call_protection_error(app, error)  \
  EE_as_handle_protection_error(app, error)
#else /* EE_AS_OSAPPLICATIONS__ */
#define EE_as_call_protection_error(app, error)  \
  EE_as_handle_protection_error(error)
#endif /* EE_AS_OSAPPLICATIONS__ */
#else /* EE_AS_HAS_PROTECTIONHOOK__ */
#define EE_as_call_protection_error(app, error)  \
  EE_oo_ShutdownOS_internal(error);
#endif /* EE_AS_HAS_PROTECTIONHOOK__ */

#endif /* INCLUDE_EE_KERNEL_AS_INTERNAL__ */
