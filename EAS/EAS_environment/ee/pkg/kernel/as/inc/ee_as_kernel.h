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

 /** @file      ee_as_kernel.h
  *  @brief     Types and macros for Autosar layer
  *  @author    Errico Guidieri
  *  @date      2012
  */

#ifndef INCLUDE_EE_KERNEL_AS_KERNEL__
#define INCLUDE_EE_KERNEL_AS_KERNEL__

#define OSId_AS_Sevices_Begin                   OSId_OO_Services_End

#define OSServiceId_GetNumberOfActivatedCores   OSId_AS_Sevices_Begin
#define OSServiceId_GetCoreID                   (OSId_AS_Sevices_Begin + 1U)
#define OSServiceId_StartCore                   (OSId_AS_Sevices_Begin + 2U)
#define OSServiceId_StartNonAutosarCore         (OSId_AS_Sevices_Begin + 3U)
#define OSServiceId_GetSpinlock                 (OSId_AS_Sevices_Begin + 4U)
#define OSServiceId_ReleaseSpinlock             (OSId_AS_Sevices_Begin + 5U)
#define OSServiceId_TryToGetSpinlock            (OSId_AS_Sevices_Begin + 6U)
#define OSServiceId_ShutdownAllCores            (OSId_AS_Sevices_Begin + 7U)
/*                  Inter OSApplication Communication (IOC)                   */
#define OSServiceId_IOCService                  (OSId_AS_Sevices_Begin + 8U)
/*                          OS-Applications Services                          */
#define OSServiceId_GetApplicationState         (OSId_AS_Sevices_Begin + 9U)
#define OSServiceId_TerminateApplication        (OSId_AS_Sevices_Begin + 10U)
#define OSServiceId_AllowAccess                 (OSId_AS_Sevices_Begin + 11U)
/*                             Trusted Function                               */
#define OSServiceId_CallTrustedFunction         (OSId_AS_Sevices_Begin + 12U)
/*                              Schedule Table                                */
#define OSServiceId_StartScheduleTableRel       (OSId_AS_Sevices_Begin + 13U)
#define OSServiceId_StartScheduleTableAbs       (OSId_AS_Sevices_Begin + 14U)
#define OSServiceId_StopScheduleTable           (OSId_AS_Sevices_Begin + 15U)
#define OSServiceId_NextScheduleTable           (OSId_AS_Sevices_Begin + 16U)
#define OSServiceId_GetScheduleTableStatus      (OSId_AS_Sevices_Begin + 17U)
#define OSServiceId_SyncScheduleTable           (OSId_AS_Sevices_Begin + 18U)
/*                       Service Protection Check API                         */
#define OSServiceId_CheckObjectOwnership        (OSId_AS_Sevices_Begin + 19U)
#define OSServiceId_CheckObjectAccess           (OSId_AS_Sevices_Begin + 20U)
/*                        Memory Protection Check API                         */
#define OSServiceId_CheckTaskMemoryAccess       (OSId_AS_Sevices_Begin + 21U)
#define OSServiceId_CheckISRMemoryAccess        (OSId_AS_Sevices_Begin + 22U)
/*                        GetApplicationID & GetISRID
          (put here because forgotten and RT-Druid support already ORTI)      */
#define OSServiceId_GetApplicationID            (OSId_AS_Sevices_Begin + 22U)
#define OSServiceId_GetISRID                    (OSId_AS_Sevices_Begin + 23U)

/*******************************************************************************
 *                            Standard Defines
 ******************************************************************************/

/* 8.1 Constants */
/* Application Mode do not Care */
#define DONOTCARE ((AppModeType)-1)

/*******************************************************************************
 *                              ORTI support
 ******************************************************************************/

/*  The last OO service called by the application. SERVICETRACE IDs
    are even numbers. The LSBit is used as a flag and it is set to 1
    when the servce is entered, to 0 at exit.
    (Needed here, not in intenal, for services inline implementation). */
#ifdef __OO_ORTI_SERVICETRACE__

#define EE_SERVICETRACE_AS_BEGIN                    EE_SERVICETRACE_OO_LAST

#define EE_SERVICETRACE_GETNUMBEROFACTIVATEDCORES   EE_SERVICETRACE_AS_BEGIN
#define EE_SERVICETRACE_GETCOREID             (EE_SERVICETRACE_AS_BEGIN + 2U)
#define EE_SERVICETRACE_STARTCORE             (EE_SERVICETRACE_AS_BEGIN + 4U)
#define EE_SERVICETRACE_STARTNONAUTOSARCORE   (EE_SERVICETRACE_AS_BEGIN + 6U)
#define EE_SERVICETRACE_GETSPINLOCK           (EE_SERVICETRACE_AS_BEGIN + 8U)
#define EE_SERVICETRACE_RELEASESPINLOCK       (EE_SERVICETRACE_AS_BEGIN + 10U)
#define EE_SERVICETRACE_TRYTOGETSPINLOCK      (EE_SERVICETRACE_AS_BEGIN + 12U)
#define EE_SERVICETRACE_SHUTDOWNALLCORES      (EE_SERVICETRACE_AS_BEGIN + 14U)
#define EE_SERVICETRACE_CHECKTASKMEMORYACCESS (EE_SERVICETRACE_AS_BEGIN + 16U)
#define EE_SERVICETRACE_CHECKISRMEMORYACCESS  (EE_SERVICETRACE_AS_BEGIN + 18U)
#define EE_SERVICETRACE_GETAPPLICATIONID      (EE_SERVICETRACE_AS_BEGIN + 20U)
#define EE_SERVICETRACE_GETISRID              (EE_SERVICETRACE_AS_BEGIN + 22U)
#define EE_SERVICETRACE_GETAPPLICATIONSTATE   (EE_SERVICETRACE_AS_BEGIN + 24U)
#define EE_SERVICETRACE_TERMINATEAPPLICATION  (EE_SERVICETRACE_AS_BEGIN + 26U)
#define EE_SERVICETRACE_ALLOWACCESS           (EE_SERVICETRACE_AS_BEGIN + 28U)
#define EE_SERVICETRACE_CHECKOBJECTACCESS     (EE_SERVICETRACE_AS_BEGIN + 30U)
#define EE_SERVICETRACE_CHECKOBJECTOWNERSHIP  (EE_SERVICETRACE_AS_BEGIN + 32U)
#define EE_SERVICETRACE_CALLTRUSTEDFUNCTION   (EE_SERVICETRACE_AS_BEGIN + 34U)
#define EE_SERVICETRACE_STARTSCHEDULETABLEREL (EE_SERVICETRACE_AS_BEGIN + 36U)
#define EE_SERVICETRACE_STARTSCHEDTABABS      (EE_SERVICETRACE_AS_BEGIN + 38U)
#define EE_SERVICETRACE_STOPSCHEDULETABLE     (EE_SERVICETRACE_AS_BEGIN + 40U)
#define EE_SERVICETRACE_NEXTSCHEDULETABLE     (EE_SERVICETRACE_AS_BEGIN + 42U)
#define EE_SERVICETRACE_GETSCHEDULETABLESTATUS  \
  (EE_SERVICETRACE_AS_BEGIN + 44U)
#define EE_SERVICETRACE_SYNCSCHEDULETABLE     (EE_SERVICETRACE_AS_BEGIN + 46U)
#endif /* __OO_ORTI_SERVICETRACE__ */

/*******************************************************************************
 *                            System services
 ******************************************************************************/
/* This inclusion is put here instead of inter layer because eecfg.c need to see
   declarations inside to configure the feature */
#include  "kernel/as/inc/ee_as_timing_prot.h"

#ifdef __MSRP__
/*******************************************************************************
 *               System Services Implemented Inline inclusion
 ******************************************************************************/
#include "kernel/as/inc/ee_as_inline.h"

/* Following types and services declaration have reason only in multicore
   environment */
#ifndef __PRIVATE_STARTOS__
void EE_as_StartCore( CoreIdType CoreID, StatusType *Status );
void EE_as_StartNonAutosarCore( CoreIdType CoreID, StatusType *Status );
#endif /* !__PRIVATE_STARTOS__ */


#if defined(EE_AS_USER_SPINLOCKS__)  && (!defined(EE_PRIVATE_SPINLOCK__))
StatusType EE_as_GetSpinlock( SpinlockIdType SpinlockId );
StatusType EE_as_ReleaseSpinlock( SpinlockIdType SpinlockId );
StatusType EE_as_TryToGetSpinlock( SpinlockIdType SpinlockId,
  TryToGetSpinlockType* Success );

#endif /* EE_AS_USER_SPINLOCKS__ && !EE_PRIVATE_SPINLOCK__ */

#if (!defined(__PRIVATE_SHUTDOWNOS__)) && defined(EE_AS_RPC__)
void EE_as_ShutdownAllCores( StatusType Error );
#endif /* !__PRIVATE_SHUTDOWNOS__ && EE_AS_RPC__ */

#endif /* __MSRP__ */

/*******************************************************************************
 *                          Errors Handling API
 ******************************************************************************/

/* [OS439]: The Operating System module shall provide the OSEK error macros
    (OSError...()) to all configured error hooks AND there shall be two
    (like in OIL) global configuration parameters to switch these macros on
    or off. */
#if defined(__OO_HAS_ERRORHOOK__) && (!defined(__OO_ERRORHOOK_NOMACROS__))
#ifdef EE_AS_USER_SPINLOCKS__
__INLINE__ SpinlockIdType __ALWAYS_INLINE__
  OSError_GetSpinlock_SpinlockId( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ SpinlockIdType __ALWAYS_INLINE__
  OSError_ReleaseSpinlock_SpinlockId( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ SpinlockIdType __ALWAYS_INLINE__
  OSError_TryToGetSpinlock_SpinlockId( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ TryToGetSpinlockType * __ALWAYS_INLINE__
  OSError_TryToGetSpinlock_Success( void )
{
  return EE_oo_get_errorhook_data()->param2.try_to_get_spinlock_ref;
}
#endif /* EE_AS_USER_SPINLOCKS__ */
#ifdef EE_AS_SCHEDULETABLES__
__INLINE__ ScheduleTableType __ALWAYS_INLINE__
  OSError_GetScheduleTableStatus_ScheduleTableID( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ ScheduleTableStatusRefType __ALWAYS_INLINE__
  OSError_GetScheduleTableStatus_ScheduleStatus( void )
{
  return EE_oo_get_errorhook_data()->param2.schedule_table_status_ref;
}

__INLINE__ ScheduleTableType __ALWAYS_INLINE__
  OSError_StartScheduleTableRel_ScheduleTableID( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ TickType __ALWAYS_INLINE__
  OSError_StartScheduleTableRel_Offset( void )
{
  return EE_oo_get_errorhook_data()->param2.value_param;
}

__INLINE__ ScheduleTableType __ALWAYS_INLINE__
  OSError_StartScheduleTableAbs_ScheduleTableID( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ TickType __ALWAYS_INLINE__
  OSError_StartScheduleTableAbs_Start( void )
{
  return EE_oo_get_errorhook_data()->param2.value_param;
}

__INLINE__ ScheduleTableType __ALWAYS_INLINE__
  OSError_StopScheduleTable_ScheduleTableID( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ ScheduleTableType __ALWAYS_INLINE__
  OSError_NextScheduleTable_ScheduleTableID_From( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ ScheduleTableType __ALWAYS_INLINE__
  OSError_NextScheduleTable_ScheduleTableID_To( void )
{
  return EE_oo_get_errorhook_data()->param2.value_param;
}

__INLINE__ ScheduleTableType __ALWAYS_INLINE__
  OSError_SyncScheduleTable_ScheduleTableID( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}
 
__INLINE__ TickType __ALWAYS_INLINE__
  OSError_SyncScheduleTable_Value( void )
{
  return EE_oo_get_errorhook_data()->param2.value_param;
}
/* TODO: Add Macro to read error informations of last Scheduletable API */

#endif  /* EE_AS_SCHEDULETABLES__ */
#ifdef EE_AS_OSAPPLICATIONS__

__INLINE__ ApplicationType __ALWAYS_INLINE__
  OSError_TerminateApplication_Application( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ RestartType __ALWAYS_INLINE__
  OSError_TerminateApplication_RestartOption( void )
{
  return EE_oo_get_errorhook_data()->param2.value_param;
}

__INLINE__ ApplicationType __ALWAYS_INLINE__
  OSError_GetApplicationState_Application( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ ApplicationStateRefType __ALWAYS_INLINE__
  OSError_GetApplicationState_Value( void )
{
  return EE_oo_get_errorhook_data()->param2.application_state_ref;
}

__INLINE__ ApplicationType __ALWAYS_INLINE__
  OSError_CheckObjectAccess_ApplID( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ ObjectTypeType __ALWAYS_INLINE__
  OSError_CheckObjectAccess_ObjectType( void )
{
  return EE_oo_get_errorhook_data()->param2.value_param;
}

__INLINE__ EE_os_param_id __ALWAYS_INLINE__
  OSError_CheckObjectAccess_ObjectID( void )
{
  return EE_oo_get_errorhook_data()->param3.value_param;
}

#ifdef EE_SERVICE_PROTECTION__

__INLINE__ ObjectTypeType __ALWAYS_INLINE__
  OSError_CheckObjectOwnership_ObjectType( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ EE_os_param_id __ALWAYS_INLINE__
  OSError_CheckObjectOwnership_ObjectID( void )
{
  return EE_oo_get_errorhook_data()->param2.value_param;
}
#endif /* EE_SERVICE_PROTECTION__ */

#ifdef  __EE_MEMORY_PROTECTION__
__INLINE__ TaskType __ALWAYS_INLINE__
  OSError_CheckTaskMemoryAccess_TaskID( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ MemoryStartAddressType __ALWAYS_INLINE__
  OSError_CheckTaskMemoryAccess_Address( void )
{
  return EE_oo_get_errorhook_data()->param2.memory_address;
}

__INLINE__ MemorySizeType __ALWAYS_INLINE__
  OSError_CheckTaskMemoryAccess_Size( void )
{
  return EE_oo_get_errorhook_data()->param3.value_param;
}

__INLINE__ ISRType __ALWAYS_INLINE__
  OSError_CheckISRMemoryAccess_ISRID( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ MemoryStartAddressType __ALWAYS_INLINE__
  OSError_CheckISRMemoryAccess_Address( void )
{
  return EE_oo_get_errorhook_data()->param2.memory_address;
}

__INLINE__ MemorySizeType __ALWAYS_INLINE__
  OSError_CheckISRMemoryAccess_Size( void )
{
  return EE_oo_get_errorhook_data()->param3.value_param;
}

#if defined(EE_SYSCALL_NR) && defined(EE_MAX_SYS_SERVICEID) &&\
  (EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID)

__INLINE__ TrustedFunctionIndexType __ALWAYS_INLINE__
  OSError_CallTrustedFunction_FunctionIndex( void )
{
  return EE_oo_get_errorhook_data()->param1.value_param;
}

__INLINE__ TrustedFunctionParameterRefType __ALWAYS_INLINE__
  OSError_CallTrustedFunction_FunctionParams( void )
{
  return EE_oo_get_errorhook_data()->param2.trusted_function_parameter_ref;
}

#endif /* EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID */
#endif /* __EE_MEMORY_PROTECTION__ */
#endif /* EE_AS_OSAPPLICATIONS__ */
#endif /* __OO_HAS_ERRORHOOK__ && !__OO_ERRORHOOK_NOMACROS__ */

#ifdef EE_AS_RPC__
/*******************************************************************************
 *                   Synchronous Remote Procedure Calls
 ******************************************************************************/

/** @brief type that hold params and return value for a RPC */
typedef struct
{
  CoreIdType      serving_core;
  OSServiceIdType remote_procedure;
  EE_os_param     param1;
  EE_os_param     param2;
  EE_os_param     param3;
  StatusType      error;
} EE_TYPEASRPC;

typedef struct {
  CoreIdType      core_id;
  EE_os_param_id  param_id;
#ifdef  EE_SERVICE_PROTECTION__
  EE_UREG         core0_index;
#endif /*  EE_SERVICE_PROTECTION__ */
} EE_TYPEASREMOTEID;

typedef EE_TYPEASREMOTEID const * EE_TYPEASREMOTEIDCONSTREF;

/* The following are moved in interface because used by inline generated code
   for IOC */

/** @brief Map the core id with his corresponding spinlock */
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
/** @brief Map the core id with his corresponding spinlock */
extern EE_TYPESPIN const EE_SHARED_CDATA EE_as_core_spinlocks[EE_MAX_CPU];
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
/** @brief Flag that a core is serving a RPCs */
extern EE_BIT volatile EE_SHARED_IDATA EE_as_rpc_serving[EE_MAX_CPU];
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef __EE_MEMORY_PROTECTION__
/** @typedef   Autosar RPC with memory protection temporary return parameters */
typedef union EE_as_rpc_outparam_type {
  TaskStateType                task_state;
#ifndef __OO_NO_ALARMS__
  AlarmBaseType                alarm_base;
  TickType                     tick;
#endif /* !__OO_NO_ALARMS__ */
#ifdef EE_AS_SCHEDULETABLES__
  ScheduleTableStatusType      schedule_table_status;
#endif /* EE_AS_SCHEDULETABLES__ */
#ifdef EE_AS_OSAPPLICATIONS__
  ApplicationStateType         application_state;
#endif /* EE_AS_OSAPPLICATIONS__ */
} EE_as_rpc_outparam;

#endif /* __EE_MEMORY_PROTECTION__ */

/** @brief The following implement a synchronous RPC kernel primitive. */
/* This is put here because have to be seen in eecfg.c in case of
   __EE_MEMORY_PROTECTION__ */
extern StatusType EE_as_rpc( OSServiceIdType ServiceId, EE_os_param param1,
  EE_os_param param2, EE_os_param param3 );

#endif /* EE_AS_RPC__ */

#ifdef EE_AS_OSAPPLICATIONS__
/*******************************************************************************
 *                          OSApplication API
 *            (Types & Hooks are declared in ee_oo_common.h)
 ******************************************************************************/

/*  Autosar Kernel Functions Declarations
 *  8.4 Function definitions
 */

/** @brief This service determines the currently running OS-Application.
 *         (a unique identifier has to be allocated to each application).
 */
ApplicationType EE_as_GetApplicationID( void );

/** @brief This service returns the identifier of the currently executing ISR.
 */
ISRType         EE_as_GetISRID( void );

/**
 *  @brief  This service terminates the OS-Application to which the calling
 *    Task/Category 2 ISR/application specific error hook belongs.
 *  Sync/Async: Synchronous.
 *  Reentrancy: Reentrant.
 *  @param Application The identifier of the OS-Application to be terminated.
 *    If the caller belongs to <Application> the call results in a self
 *    termination.
 *  @param RestartOption Either RESTART for doing a restart of the
 *    OS-Application or NO_RESTART if OS-Application shall not be restarted.
 *  @return E_OK: No errors
 *          E_OS_ID: <Application> was not valid (only in EXTENDED status)
 *          E_OS_VALUE: <RestartOption> was neither RESTART nor NO_RESTART
 *            (only in EXTENDED status)
 *          E_OS_ACCESS: The caller does not have the right to terminate
 *             <Application> (only in EXTENDED status)
 *          E_OS_STATE: The state of <Application> does not allow
 *             terminating <Application>
 */
StatusType  EE_as_TerminateApplication( ApplicationType Application,
  RestartType RestartOption );

/**
 *  @brief This service sets the own state of an OS-Application from
 *    APPLICATION_RESTARTING to APPLICATION_ACCESSIBLE. (Supposed to be called
 *    by OS-Application restarting TASK).
 *  Sync/Async: Synchronous.
 *  Reentrancy: Reentrant.
 *  @return E_OK: No errors
 *          E_OS_STATE: The OS-Application of the caller is in the wrong state
 */
StatusType  EE_as_AllowAccess( void );

/**
 *  @brief This service returns the current state of an OS-Application.
 *  Sync/Async: Synchronous.
 *  Reentrancy: Reentrant.
 *  @param Application: The OS-Application from which the state is requested.
 *  @param Value: The current state of the application.
 *  @return E_OK: No errors
 *          E_OS_ID: <Application> is not valid (only in EXTENDED status)
 */
StatusType  EE_as_GetApplicationState( ApplicationType Application,
  ApplicationStateRefType Value );

/**
 *  @brief This service determines if the OS-Applications, given by ApplID, is
 *    allowed to use the IDs of a Task, ISR, Resource, Counter, Alarm or
 *    Schedule Table in API calls.
 *  Sync/Async: Synchronous
 *  Reentrancy: Reentrant
 *  @param ApplID: OS-Application identifier
 *  @param ObjectType: Type of the following parameter
 *  @param ObjectID: The object to be examined
 *  @return ObjectAccessType ACCESS if the ApplID has access to the object
 *    NO_ACCESS otherwise
 */
ObjectAccessType EE_as_CheckObjectAccess( ApplicationType ApplID,
  ObjectTypeType ObjectType, EE_os_param_id ObjectID );

#ifdef EE_SERVICE_PROTECTION__
/**
 *  @brief This service determines to which OS-Application a given Task, ISR,
 *    Counter, Alarm or Schedule Table belongs.
 *  Sync/Async: Synchronous
 *  Reentrancy: Reentrant
 *  @param ObjectType: Type of the following parameter
 *  @param ObjectID: The object to be examined
 *  @return ApplicationType: <OS-Application> the OS-Application to which the
 *    object ObjectType belongs or INVALID_OSAPPLICATION if the object does not
 *    exists
 */
ApplicationType EE_as_CheckObjectOwnership( ObjectTypeType ObjectType,
  EE_os_param_id ObjectID );
#endif /* EE_SERVICE_PROTECTION__ */

/** @brief Called at the end of an ISR2 for termination */
StatusType EE_as_TerminateISR2(void);

/* The following functions are always called directly, never by syscall */
#define GetApplicationID      EE_as_GetApplicationID
#define GetISRID              EE_as_GetISRID
#define GetApplicationState   EE_as_GetApplicationState
#define CheckObjectOwnership  EE_as_CheckObjectOwnership
#define CheckObjectAccess     EE_as_CheckObjectAccess

#ifdef  __EE_MEMORY_PROTECTION__
/*******************************************************************************
 *                        Memory Protection Support
 ******************************************************************************/

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define API_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/*  Autosar Kernel Functions Declarations
 *  8.4 Function definitions
 */

/**
 *  @brief  This service checks if a memory region is write/read/execute
 *          accessible for a TASK and also returns information if the memory
 *          region is part of the stack space.
 *  Sync/Async: Synchronous.
 *  Reentrancy: Reentrant.
 *
 *  @param ISRID Index: ISR reference.
 *  @param Address: Start of memory area
 *  @param Size: Size of memory area
 *  @return AccessType: Value which contains the access rights to the memory
 *    area
 */
AccessType EE_as_CheckTaskMemoryAccess( TaskType TaskID,
  MemoryStartAddressType Address, MemorySizeType Size );

/**
 *  @brief  This service checks if a memory region is write/read/execute
 *          accessible for an ISR and also returns information if the memory
 *          region is part of the stack space.
 *  Sync/Async: Synchronous.
 *  Reentrancy: Reentrant.
 *
 *  @param ISRID Index: ISR reference.
 *  @param Address: Start of memory area
 *  @param Size: Size of memory area
 *  @return AccessType: Value which contains the access rights to the memory
 *    area
 */
AccessType EE_as_CheckISRMemoryAccess( ISRType ISRID, MemoryStartAddressType
  Address, MemorySizeType Size );

/* The following functions are always called directly never by a syscall */
#define CheckTaskMemoryAccess EE_as_CheckTaskMemoryAccess
#define CheckISRMemoryAccess EE_as_CheckISRMemoryAccess

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define API_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#if defined(EE_SYSCALL_NR) && defined(EE_MAX_SYS_SERVICEID) &&\
  (EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID)
/**
 *  @brief  A (trusted or non-trusted) OS-Application uses this service to call
 *          a trusted function
 *  Sync/Async: Depends on called function. If called function is synchronous
 *          then service is synchronous. May cause rescheduling.
 *  Reentrancy: Reentrant.
 *  @param FunctionIndex Index of the function to be called.
 *  @param FunctionParams Pointer to the parameters for the function - specified
 *    by the FunctionIndex - to be called. If no parameters are provided,
 *    a NULL pointer has to be passed.
 *  @return StatusType E_OK: No Error E_OS_SERVICEID: No function defined for
 *    this index
 */
StatusType EE_as_CallTrustedFunction( TrustedFunctionIndexType FunctionIndex,
  TrustedFunctionParameterRefType FunctionParams );
#endif /* EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID */

/**
 * The APIs below should modify the state of interrupts, but they cannot
 * manipulate the state register directly when a syscall is used. So they take
 * the value of the state register before the syscall and return the value to be
 * set after the syscall. The syscall handler code takes care of updating the
 * state register. */

/* DisableAllInterrupts() body */
EE_FREG EE_as_DisableAllInterrupts(EE_FREG prev);
/* EnableAllInterrupts() body */
EE_FREG EE_as_EnableAllInterrupts(EE_FREG prev);
/* SuspendAllInterrupts() body */
EE_FREG EE_as_SuspendAllInterrupts(EE_FREG prev);
/* ResumeAllInterrupts() body */
EE_FREG EE_as_ResumeAllInterrupts(EE_FREG prev);
/* SuspendOSInterrupts() body */
EE_FREG EE_as_SuspendOSInterrupts(EE_FREG prev);
/* ResumeOSInterrupts() body */
EE_FREG EE_as_ResumeOSInterrupts(EE_FREG prev);

#else  /* __EE_MEMORY_PROTECTION__ */
/* TerminateApplication & AllowAccess services remapping on body functions
  (is an part API in the ERIKA's meaning of that) */
#define TerminateApplication  EE_as_TerminateApplication
#define AllowAccess           EE_as_AllowAccess
#endif /* __EE_MEMORY_PROTECTION__ */

#endif /* EE_AS_OSAPPLICATIONS__ */

#ifdef EE_AS_HAS_PROTECTIONHOOK__
/*******************************************************************************
 *                        Protection Hook Support
 ******************************************************************************/
/**
 * @typedef ProtectionReturnType
 *
 * Defines what to do after returning from ProtectionHook.
 *
 * See paragraph 8.3.16 of AUTOSAR OS SWS 5.0.0
 */
typedef enum
{
  PRO_IGNORE,                 /**< do nothing */
  PRO_TERMINATETASKISR,       /**< terminate the faulty task or ISR2 */
  PRO_TERMINATEAPPL,          /**< terminate the faulty application */
  PRO_TERMINATEAPPL_RESTART,  /**< restart the faulty application */
  PRO_SHUTDOWN                /**< shutdown the OS */
} ProtectionReturnType;

/**
 * User protection hook callback function.
 *
 * @param FatalError the kind of error
 *
 * @return what to do after this error
 *
 * see paragraph 8.7.1 of AUTOSAR OS SWS 5.0.0
 */
extern ProtectionReturnType ProtectionHook( StatusType FatalError );
#endif /* EE_AS_HAS_PROTECTIONHOOK__ */

/*******************************************************************************
 *            Multicore Startup synchronization data structures
 ******************************************************************************/
#ifdef __MSRP__

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
/* The following contains cores application modes */
extern AppModeType volatile EE_SHARED_UDATA
  EE_as_os_application_mode[EE_MAX_CPU];

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
/* Mask for Autosar cores started */
extern EE_UREG EE_SHARED_IDATA volatile EE_as_core_mask;
/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* __MSRP__ */

#ifdef EE_AS_USER_SPINLOCKS__
/*******************************************************************************
 *                      Spinlocks data Structures
 ******************************************************************************/
/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#if defined (EE_MAX_SPINLOCK_USER) && (EE_MAX_SPINLOCK_USER > 0)
/** @brief Hold which core is locking the spinlock */
extern CoreIdType volatile EE_SHARED_IDATA EE_as_spinlocks_locker_core[EE_MAX_SPINLOCK_USER];

/** @brief Spinlock Stack */
extern SpinlockIdType volatile EE_SHARED_IDATA EE_as_spinlocks_stack[EE_MAX_SPINLOCK_USER];
#endif /* EE_MAX_SPINLOCK_USER && EE_MAX_SPINLOCK_USER */

#if defined (EE_MAX_CPU) && (EE_MAX_CPU > 0)
/** @brief Spinlock Stack head */
extern SpinlockIdType volatile EE_SHARED_IDATA EE_as_spinlocks_last[EE_MAX_CPU];
#endif /* EE_MAX_CPU && EE_MAX_CPU */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
#endif /* EE_AS_USER_SPINLOCKS__ */

#if defined (EE_MAX_TASK) && defined (EE_MAX_ISR2)
#define EE_TASK_PLUS_ISR2 (EE_MAX_TASK + EE_MAX_ISR2)

#ifdef EE_SUPPORT_MEMMAP_H
#define OS_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
/** @brief Hold which task is locking the spinlock */
extern TaskType EE_as_spinlocks_locker_task_or_isr2[EE_TASK_PLUS_ISR2];
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* EE_MAX_TASK && EE_MAX_ISR2 */

#ifdef EE_AS_RPC__
/*******************************************************************************
 *            Synchronous Remote Procedure Calls Data Structures
 ******************************************************************************/

#if defined (EE_AS_RPC_SERVICES_TABLE_SIZE) && \
  (EE_AS_RPC_SERVICES_TABLE_SIZE > 0)

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** @brief used to map global ids on local ids */
extern EE_TYPEASREMOTEIDCONSTREF const EE_SHARED_CDATA
  EE_as_rpc_services_table[EE_AS_RPC_SERVICES_TABLE_SIZE];

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* EE_AS_RPC_SERVICES_TABLE_SIZE > 0 */

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** @brief Mask used in shutdown all cores procedure for synchronization */
extern EE_UREG volatile EE_SHARED_IDATA EE_as_shutdown_mask;

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#if defined (EE_AS_RPC_TASKS_SIZE) && (EE_AS_RPC_TASKS_SIZE > 0)
extern EE_TYPEASREMOTEID const EE_SHARED_CDATA
  EE_as_rpc_tasks[EE_AS_RPC_TASKS_SIZE];
#endif /* EE_AS_RPC_TASKS_SIZE > 0 */

#if defined (EE_AS_RPC_ALARMS_SIZE) && (EE_AS_RPC_ALARMS_SIZE  > 0)
extern EE_TYPEASREMOTEID const EE_SHARED_CDATA
  EE_as_rpc_alarms[EE_AS_RPC_ALARMS_SIZE];
#endif /* EE_AS_RPC_ALARMS_SIZE > 0 */

#if defined (EE_AS_RPC_COUNTERS_SIZE) && (EE_AS_RPC_COUNTERS_SIZE  > 0)
extern EE_TYPEASREMOTEID const EE_SHARED_CDATA
  EE_as_rpc_counters[EE_AS_RPC_COUNTERS_SIZE];
#endif /* EE_AS_RPC_COUNTERS_SIZE > 0 */

#if defined (EE_AS_RPC_SCHEDTABS_SIZE) && (EE_AS_RPC_SCHEDTABS_SIZE > 0)
extern EE_TYPEASREMOTEID const EE_SHARED_CDATA
  EE_as_rpc_schedTabs[EE_AS_RPC_SCHEDTABS_SIZE];
#endif /* EE_AS_RPC_SCHEDTABS_SIZE > 0 */

#if defined (EE_AS_RPC_OSAPPLS_SIZE) && (EE_AS_RPC_OSAPPLS_SIZE > 0)
extern EE_TYPEASREMOTEID const EE_SHARED_CDATA
  EE_as_rpc_osAppls[EE_AS_RPC_OSAPPLS_SIZE];
#endif /* EE_AS_RPC_OSAPPLS_SIZE > 0 */

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef __EE_MEMORY_PROTECTION__

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** @brief array used to crossing memory protection between cores when call a
    service with param2 as some kind of reference */
extern EE_as_rpc_outparam EE_SHARED_UDATA EE_as_rpc_out_param2[EE_MAX_CPU];
/** @brief array used to crossing memory protection between cores when call a
    service with param3 as some kind of reference */
extern EE_as_rpc_outparam EE_SHARED_UDATA EE_as_rpc_out_param3[EE_MAX_CPU];

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* __EE_MEMORY_PROTECTION__ */

#if defined (EE_MAX_CPU) && (EE_MAX_CPU > 0)
/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
/** @brief Data structures to pass RPC parameters */
extern EE_TYPEASRPC volatile EE_SHARED_IDATA EE_as_rpc_RAM[EE_MAX_CPU];

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
#endif /* EE_MAX_CPU > 0 */

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** @brief Flag used to signal that ShutdownAllCores have been called */
extern EE_BIT volatile EE_SHARED_UDATA EE_as_shutdown_all_cores_flag;

/** @brief Used to pass ShutdownAllCores error parameter to other cores */
extern StatusType volatile EE_SHARED_UDATA EE_as_shutdown_all_cores_error;

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* EE_AS_RPC__ */

#ifdef EE_AS_IOC__
/*******************************************************************************
 *          Inter OSApplication Communication (IOC) Data Structures
 ******************************************************************************/

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** @brief Flag used to signal a IOC request */
extern EE_BIT EE_SHARED_UDATA EE_as_rpc_IOC[ EE_MAX_CPU ];

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* EE_AS_IOC__ */

#if (defined(EE_AS_OSAPPLICATIONS__)  && defined(EE_SERVICE_PROTECTION__)) &&\
  defined(EE_AS_RPC__)
/*******************************************************************************
 *     OSApplication Service Protection Access Shared Data Structures
 ******************************************************************************/
/* Extra info used to implement service protection in Multi-core environment */
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

extern EE_TYPEACCESSMASK const EE_SHARED_CDATA EE_as_rpc_remote_access_rules[
  EE_AS_RPC_REMOTE_ACCESS_RULES_SIZE];

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
#endif /* EE_AS_OSAPPLICATIONS__ && EE_SERVICE_PROTECTION__ && EE_AS_RPC__ */

#endif /* INCLUDE_EE_KERNEL_AS_KERNEL__ */
