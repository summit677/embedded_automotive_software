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

 /** @file      ee_as_schedule_tables.h
  *  @brief     AUTOSAR Schedule Table Types and Functions declaration
  *  @author    Errico Guidieri
  *  @date      2013
  */

#ifndef INCLUDE_EE_KERNEL_AS_SCHEDULE_TABLES__
#define INCLUDE_EE_KERNEL_AS_SCHEDULE_TABLES__

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
StatusType EE_as_StartScheduleTableRel( ScheduleTableType ScheduleTableID,
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

StatusType EE_as_StartScheduleTableAbs( ScheduleTableType ScheduleTableID,
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
StatusType EE_as_StopScheduleTable( ScheduleTableType ScheduleTableID );

/** 8.4.15 GetScheduleTableStatus [SWS_Os_00227]
 *
 *  @brief This service queries the state of a schedule table (also with respect
 *    to synchronization).
 *
 *  Sync/Async: Synchronous
 *  Reentrancy: Reentrant
 *
 *  @param ScheduleTableID (in): Schedule table for which status is requested.
 *  @param ScheduleStatus (out): Reference to ScheduleTableStatusType.
 *
 *  @return StatusType:
 *    E_OK: No Error
 *    E_OS_ID (only in EXTENDED status): Invalid ScheduleTableID
 *
 * (SRS_Os_11002)
 */
StatusType EE_as_GetScheduleTableStatus( ScheduleTableType ScheduleTableID,
  ScheduleTableStatusRefType ScheduleStatus );

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

StatusType EE_as_NextScheduleTable( ScheduleTableType ScheduleTableID_From,
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

StatusType EE_as_SyncScheduleTable( ScheduleTableType ScheduleTableID,
  TickType Value );

/* The following primitives are available only on local objects */
#endif /* EE_MAX_SCHEDULETABLE > 0 */

/*******************************************************************************
                Schedule Table Kernel API names remapping
*******************************************************************************/

#ifndef GetScheduleTableStatus
#define GetScheduleTableStatus  EE_as_GetScheduleTableStatus
#endif /* !GetScheduleTableStatus */

/* Direct mapping only without memory proetction */
#ifndef __EE_MEMORY_PROTECTION__

#ifndef StartScheduleTableRel
#define StartScheduleTableRel   EE_as_StartScheduleTableRel
#endif /* !StartScheduleTableRel */

#ifndef StartScheduleTableAbs
#define StartScheduleTableAbs   EE_as_StartScheduleTableAbs
#endif /* !StartScheduleTableAbs */

#ifndef StopScheduleTable
#define StopScheduleTable       EE_as_StopScheduleTable
#endif /* !StopScheduleTable */

#ifndef NextScheduleTable
#define NextScheduleTable       EE_as_NextScheduleTable
#endif /* !NextScheduleTable */

#ifndef SyncScheduleTable
#define SyncScheduleTable       EE_as_SyncScheduleTable
#endif /* !SyncScheduleTable */

#endif /* __EE_MEMORY_PROTECTION__ */

#endif /* INCLUDE_EE_KERNEL_AS_SCHEDULE_TABLES__ */
