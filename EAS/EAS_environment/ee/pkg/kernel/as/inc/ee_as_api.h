/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2013  Evidence Srl
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

/**  @file      ee_as_api.h
  *  @brief     Autosar API declaration
  *  @author    Errico Guidieri
  *  @date      2013
  */

#ifndef INCLUDE_EE_KERNEL_AS_API_H__
#define INCLUDE_EE_KERNEL_AS_API_H__

#ifdef EE_SUPPORT_MEMMAP_H
/* Put the following code in ee_kernel_text */
#define OS_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* This declaration is needed by eecfg.c for syscalls table definitions.
   Usually it is declared as internal kernel funtion, unfortunatly eecfg.c
   include only public layer (ee.h) so I have to put this somewere in a
   public included file */
extern void EE_thread_not_terminated( void );

/* Stop ee_kernel_text section */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef EE_SUPPORT_MEMMAP_H
/* The following code belong to ERIKA API section ee_api_text */
#define API_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/***************************************************************************
 * 13.3 Interrupt handling 
 ***************************************************************************/

/* 13.3.2.1: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_ENABLEALLINTERRUPTS__
void EnableAllInterrupts( void );
#endif /* ! __PRIVATE_ENABLEALLINTERRUPTS__ */

/* 13.3.2.2: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_DISABLEALLINTERRUPTS__
void DisableAllInterrupts( void );
#endif /* ! __PRIVATE_DISABLEALLINTERRUPTS__ */

/* 13.3.2.3: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_RESUMEALLINTERRUPTS__
void ResumeAllInterrupts( void );
#endif /* ! __PRIVATE_RESUMEALLINTERRUPTS__ */

/* 13.3.2.4: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_SUSPENDALLINTERRUPTS__
void SuspendAllInterrupts( void );
#endif /* ! __PRIVATE_SUSPENDALLINTERRUPTS__ */

/* 13.3.2.5: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_RESUMEOSINTERRUPTS__
void ResumeOSInterrupts( void );
#endif /* ! __PRIVATE_RESUMEOSINTERRUPTS__ */

/* 13.3.2.6: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_SUSPENDOSINTERRUPTS__
void SuspendOSInterrupts( void );
#endif /* ! __PRIVATE_SUSPENDOSINTERRUPTS__ */

/***************************************************************************
 * 13.2 Task management 
 ***************************************************************************/

/* 13.2.3 System services                                                  */
/* ----------------------------------------------------------------------- */

/* 13.2.3.1: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_ACTIVATETASK__
StatusType ActivateTask(TaskType TaskID);
#endif /* __PRIVATE_ACTIVATETASK__ */

/* 13.2.3.2: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_TERMINATETASK__
StatusType TerminateTask(void);
#endif /* __PRIVATE_TERMINATETASK__ */

/* 13.2.3.3: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_CHAINTASK__
StatusType ChainTask(TaskType TaskID);
#endif /* __PRIVATE_CHAINTASK__ */

/* 13.2.3.4: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_SCHEDULE__
StatusType Schedule(void);
#endif /* __PRIVATE_SCHEDULE__ */

/* Needed for counters (see later) */
#ifndef __PRIVATE_FORCESCHEDULE__
StatusType ForceSchedule(void);
#endif /* __PRIVATE_FORCESCHEDULE__ */

/***************************************************************************
 * 13.4 Resource management 
 ***************************************************************************/

#ifndef __OO_NO_RESOURCES__

/* 13.4.3 System services                                                  */
/* ----------------------------------------------------------------------- */

/* 13.4.3.1: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_GETRESOURCE__
StatusType GetResource(ResourceType ResID);
#endif /* __PRIVATE_GETRESOURCE__ */

/* 13.4.3.2: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_RELEASERESOURCE__
StatusType ReleaseResource(ResourceType ResID);
#endif /* __PRIVATE_RELEASERESOURCE__ */

#endif /* !__OO_NO_RESOURCES__ */

/***************************************************************************
 * 13.5 Event control 
 ***************************************************************************/

/* 13.5.3 System services                                                  */
/* ----------------------------------------------------------------------- */

/* see also internal.h */
/* 13.5.3.1: ECC1, ECC2 */
#if defined(__OO_ECC1__) || defined(__OO_ECC2__)

#ifndef __PRIVATE_SETEVENT__
StatusType SetEvent(TaskType TaskID, EventMaskType Mask);
#endif /* __PRIVATE_SETEVENT__ */

/* 13.5.3.2: ECC1, ECC2 */
#ifndef __PRIVATE_CLEAREVENT__
StatusType ClearEvent(EventMaskType Mask);
#endif /* __PRIVATE_CLEAREVENT__ */

/* 13.5.3.4: ECC1, ECC2 */
#ifndef __PRIVATE_WAITEVENT__
StatusType WaitEvent(EventMaskType Mask);
#endif /* __PRIVATE_WAITEVENT__ */

#endif /* __OO_ECC1__ || __OO_ECC2__ */

/***************************************************************************
 * 13.6 Alarms 
 ***************************************************************************/

#ifndef __OO_NO_ALARMS__

/* Implementation defined functions */

/* Counter initialization
   ----------------------

   Counter initialization is done at initialization time OUTSIDE
   StartOS.  Since software counters are stored in RAM, their
   initialization values are typically set to 0. That also conforms to
   note 10 page 43 of the specification version 2.2.1 that says:

   "Counters are - if possible - set to zero by the system
   initialization before alarms are autostarted. Exception: calendar
   timers, etc. For autostarted alarms, all values are relative
   values.

*/

/*
   AS 4.0 OS SWS 8.4.16
   AS OS requirement OS399:

   This function notifies a tick to a counter. That is, the counter is
   incremented by 1.

   The function will also implement the notification of expired alarms
   (calling an alarm callback, setting an event, or activating a
   task). 

   And DO RESCHEDULING.
   see also internal.h
*/
#if (!defined(__PRIVATE_INCREMENTCOUNTER__)) && (EE_MAX_COUNTER > 0U)
StatusType IncrementCounter(CounterType CounterID);
#endif  /* __PRIVATE_INCREMENTCOUNTER__ */

#ifndef __PRIVATE_GETALARMBASE__
StatusType GetAlarmBase(AlarmType AlarmID, AlarmBaseRefType Info);
#endif /* __PRIVATE_GETALARMBASE__ */

/* 13.6.3.2 BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_GETALARM_
StatusType GetAlarm(AlarmType AlarmID, TickRefType Tick);
#endif /* __PRIVATE_GETALARM_ */

/* 13.6.3.3 BCC1, BCC2, ECC1, ECC2; Events only ECC1, ECC2 */
#ifndef __PRIVATE_SETRELALARM__
StatusType SetRelAlarm(AlarmType AlarmID, TickType increment, TickType cycle);
#endif /* __PRIVATE_SETRELALARM__ */

/* 13.6.3.4 BCC1, BCC2, ECC1, ECC2; Events only ECC1, ECC2 */
#ifndef __PRIVATE_SETABSALARM__
StatusType SetAbsAlarm(AlarmType AlarmID, TickType start, TickType cycle);
#endif /* __PRIVATE_SETABSALARM__ */

/* 13.6.3.5 BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_CANCELALARM__
StatusType CancelAlarm(AlarmType AlarmID);
#endif

#endif /* !__OO_NO_ALARMS__ */

/***************************************************************************
 * 13.7 Operating system execution control 
 ***************************************************************************/

/***************************************************************************
 * 13.7 Operating system execution control 
 ***************************************************************************/

/* 13.7.2.3: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_SHUTDOWNOS__
void ShutdownOS(StatusType Error);
#endif /* !__PRIVATE_SHUTDOWNOS__ */

#ifdef EE_AS_SCHEDULETABLES__
/*******************************************************************************
 *                             Schedule Tables API
 ******************************************************************************/
/** 8.4.8 StartScheduleTableRel [OS347]
 *
 *  @brief This service starts the processing of a schedule table at "Offset"
 *    relative to the "Now" value on the underlying counter.
 *
 *  Sync/Async: Synchronous
 *  Reentrancy: Reentrant
 *
 *  @param ScheduleTableID Schedule table to be started.
 *
 *  @param Offset Number of ticks on the counter before the the schedule table
 *     processing is started
 *
 *  @return StatusType -
 *          E_OK: No Error
 *          E_OS_ID: (only in EXTENDED status): ScheduleTableID not valid.
 *          E_OS_VALUE: (only in EXTENDED status): Offset is greater than
 *            (OsCounterMaxAllowedValue - InitialOffset) or is equal to 0.
 *          E_OS_STATE: Schedule table was already started.
 */
StatusType StartScheduleTableRel( ScheduleTableType ScheduleTableID,
  TickType Offset );

/** 8.4.9 StartScheduleTableAbs [SWS_Os_00358]
 *
 * @brief This service starts the processing of a schedule table at an absolute
 *  value "Start" on the underlying counter.
 *
 *  Sync/Async: Synchronous
 *  Reentrancy: Reentrant
 *
 *  @param ScheduleTableID: Schedule table to be started
 *  @param Start: Absolute counter tick value at which the schedule table is
 *    started
 *
 *  @return StatusType
 *    E_OK: No Error
 *    E_OS_ID (only in EXTENDED status): ScheduleTableID not valid.
 *    E_OS_VALUE (only in EXTENDED status): "Start" is greater than
 *      OsCounterMaxAllowedValue
 *    E_OS_STATE: Schedule table was already started Description:
 */
StatusType StartScheduleTableAbs( ScheduleTableType ScheduleTableID,
  TickType Start );

/** 8.4.10 StopScheduleTable [SWS_Os_00006]
 *
 *  @brief This service cancels the processing of a schedule table immediately
 *    at any point while the schedule table is running.
 *
 *  Sync/Async: Synchronous
 *  Reentrancy: Reentrant
 *
 *  @param ScheduleTableID: Schedule table to be stopped
 *
 *  @return StatusType
 *    E_OK: No Error
 *    E_OS_ID (only in EXTENDED status): ScheduleTableID not valid.
 *    E_OS_NOFUNC: Schedule table was already stopped.
 */
StatusType StopScheduleTable( ScheduleTableType ScheduleTableID );

/* The following primitives are available only on local objects */
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)

/** 8.4.11 NextScheduleTable [SWS_Os_00191]
 *
 *  @brief This service switches the processing from one schedule table to
 *    another schedule table.
 *
 *  Sync/Async: Synchronous
 *  Reentrancy: Reentrant
 *
 *  @param ScheduleTableID_From: Currently processed schedule table
 *  @param ScheduleTableID_To: Schedule table that provides its series of
 *    expiry points
 *
 * @return StatusType
 *    E_OK: No error
 *    E_OS_ID (only in EXTENDED status): ScheduleTableID_From or
 *      ScheduleTableID_To not valid.
 *    E_OS_NOFUNC: ScheduleTableID_From not started
 *    E_OS_STATE: ScheduleTableID_To is started or next
 *
 * (SRS_Os_00099)
 */
StatusType NextScheduleTable( ScheduleTableType ScheduleTableID_From,
  ScheduleTableType ScheduleTableID_To );

/** 8.4.13 SyncScheduleTable [SWS_Os_00199]
 *
 *  @brief This service provides the schedule table with a synchronization
 *    count and start synchronization.
 *
 *  Sync/Async: Synchronous
 *  Reentrancy: Reentrant
 *
 *  @param ScheduleTableID (in): Schedule table to be synchronized
 *  @param Value (in): The current value of the synchronization counter
 *
 *  @return StatusType:
 *    E_OK: No errors
 *    E_OS_ID (only in EXTENDED status): The ScheduleTableID was not valid or
 *      schedule table can not be synchronized (OsScheduleTblSyncStrategy not
 *      set or OsScheduleTblSyncStrategy = IMPLICIT)
 *    E_OS_VALUE (only in EXETENDED status): The <Value> is out of range.
 *    E_OS_STATE: The state of schedule table <ScheduleTableID> is equal to
 *      SCHEDULETABLE_STOPPED
 * (SRS_Os_11002)
 */
StatusType SyncScheduleTable( ScheduleTableType ScheduleTableID,
  TickType Value );

/* The following primitives are available only on local objects */
#endif /* EE_MAX_SCHEDULETABLE > 0 */
#endif /* EE_AS_SCHEDULETABLES__ */

#ifdef EE_AS_OSAPPLICATIONS__
/*******************************************************************************
 *                          OSApplication API
 ******************************************************************************/

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
StatusType  TerminateApplication( ApplicationType Application,
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
StatusType  AllowAccess( void );

/*******************************************************************************
 *                        Memory Protection Support
 ******************************************************************************/
#ifdef __EE_MEMORY_PROTECTION__
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
StatusType CallTrustedFunction( TrustedFunctionIndexType FunctionIndex,
  TrustedFunctionParameterRefType FunctionParams );
#endif /* EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID */

#endif /* __EE_MEMORY_PROTECTION__ */ 

#endif /* EE_AS_OSAPPLICATIONS__ */

#if defined(EE_AS_USER_SPINLOCKS__)  && (!defined(EE_PRIVATE_SPINLOCK__))
StatusType GetSpinlock( SpinlockIdType SpinlockId );
StatusType ReleaseSpinlock( SpinlockIdType SpinlockId );
StatusType TryToGetSpinlock( SpinlockIdType SpinlockId,
  TryToGetSpinlockType* Success );

#endif /* EE_AS_USER_SPINLOCKS__ && !EE_PRIVATE_SPINLOCK__ */

#if (!defined(__PRIVATE_SHUTDOWNOS__)) && defined(EE_AS_RPC__)
void ShutdownAllCores( StatusType Error );
#endif /* !__PRIVATE_SHUTDOWNOS__ && EE_AS_RPC__ */

/* Stop ee_api_text section */
#ifdef EE_SUPPORT_MEMMAP_H
#define API_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* INCLUDE_EE_KERNEL_AS_API_H__ */
