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
  *  @brief     AUTOSAR Schedule Table Types and Functions definitions
  *  @author    Errico Guidieri
  *  @date      2013
  */

#include "ee_internal.h"

/*  8.4.8 StartScheduleTableRel [OS347]
 *
 *  This service starts the processing of a schedule table at "Offset" relative
 *  to the "Now" value on the underlying counter.
 *
 *  Sync/Async: Synchronous
 *  Reentrancy: Reentrant
 *
 *  param ScheduleTableID Schedule table to be started.
 *
 *  param Offset Number of ticks on the counter before the the schedule table
 *     processing is started
 *
 *  return StatusType -
 *          E_OK: No Error
 *          E_OS_ID: (only in EXTENDED status): ScheduleTableID not valid.
 *          E_OS_VALUE: (only in EXTENDED status): Offset is greater than
 *            (OsCounterMaxAllowedValue - InitialOffset) or is equal to 0.
 *          E_OS_STATE: Schedule table was already started.
 */

StatusType EE_as_StartScheduleTableRel( ScheduleTableType ScheduleTableID,
  TickType Offset )
{
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)
  /* Counter Object ID that represent ScheduleTableID */
  register const CounterObjectType  obj_id = EE_MAX_ALARM + ScheduleTableID;
#endif /* EE_MAX_SCHEDULETABLE > 0 */
  /* Return error value */
  register StatusType                           ev;
  /* Primitive kernel locking  */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_STARTSCHEDULETABLEREL);

  EE_as_monitoring_the_stack();

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of  the service. (BSW11009, BSW11013) */
  /* StartScheduleTableRel is callable by Task, ISR2 */
  if ( EE_as_execution_context > ISR2_Context )
  {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */
#ifdef EE_AS_RPC__
  if ( EE_AS_ID_REMOTE(ScheduleTableID) ) {
    EE_os_param const
      unmarked_sched_table_id = { EE_AS_UNMARK_REMOTE_ID(ScheduleTableID) },
      as_offset = { Offset };
    /* forward the request to another CPU in synchronous way */
    ev = EE_as_rpc(OSServiceId_StartScheduleTableRel, unmarked_sched_table_id,
      as_offset, EE_OS_INVALID_PARAM);
  } else {
#endif /* EE_AS_RPC__ */

/* If local schedule tables are not defined cut everything else */
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)
  /* [OS275] If the schedule table <ScheduleTableID> in a call of
      StartScheduleTableRel() is not valid, StartScheduleTableRel()
      shall return E_OS_ID. */
#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
    if ( ScheduleTableID >= EE_MAX_SCHEDULETABLE ) {
      ev = E_OS_ID;
    } else if ( EE_SCHED_TABLE_ACCESS_ERR(ScheduleTableID, EE_as_active_app) ) {
      ev = E_OS_ACCESS;
    } else
#elif defined(__OO_EXTENDED_STATUS__)
    if ( ScheduleTableID >= EE_MAX_SCHEDULETABLE ) {
      ev = E_OS_ID;
    } else
#endif /* EE_FULL_SERVICE_PROTECTION || __OO_EXTENDED_STATUS__ */
#ifdef __OO_EXTENDED_STATUS__
    /* [OS452] If the schedule table <ScheduleTableID> in a call of
        StartScheduleTableRel() is implicitly synchronized
       (OsScheduleTblSyncStrategy = IMPLICIT), StartScheduleTableRel()
       shall return E_OS_ID. */
    /* [OS332] If <Offset> in a call of StartScheduleTableRel() is zero
       StartScheduleTableRel() shall return E_OS_VALUE. */
    /* [OS276] If the offset <Offset>) is greater than OsCounterMaxAllowedValue
       of the underlying counter minus the Initial Offset, StartScheduleTableRel()
       shall return E_OS_VALUE. */
    /* [OS277] If the schedule table <ScheduleTableID> in a call of
        StartScheduleTableRel() is not in the state SCHEDULETABLE_STOPPED,
        StartScheduleTableRel() shall return E_OS_STATE. */
    if( EE_as_Schedule_Table_ROM[ScheduleTableID].sync_strategy ==
      EE_SCHEDTABLE_SYNC_IMPLICIT )
    {
      ev = E_OS_ID;
    } else if ( (Offset == 0) || (Offset >
      EE_counter_ROM[EE_oo_counter_object_ROM[obj_id].c].maxallowedvalue) )
    {
      ev = E_OS_VALUE;
    } else if ( EE_as_Schedule_Table_RAM[ScheduleTableID].status !=
      SCHEDULETABLE_STOPPED )
    {
      ev = E_OS_STATE;
    } else
#endif /* __OO_EXTENDED_STATUS__ */
    {
      /* [OS278] If the input parameters of StartScheduleTableRel()
          are valid and the state of schedule table <ScheduleTableID> is
          SCHEDULETABLE_STOPPED, then StartScheduleTableRel() shall start the
          processing of a schedule table <ScheduleTableID>.
          The Initial Expiry Point shall be processed after
          <Offset> + Initial Offset ticks have elapsed on the underlying counter.
          The state of <ScheduleTableID> is set to SCHEDULETABLE_RUNNING before
          the service returns to the caller. */
      EE_as_Schedule_Table_RAM[ScheduleTableID].status  = SCHEDULETABLE_RUNNING;
      EE_as_Schedule_Table_RAM[ScheduleTableID].position    =
        EE_as_Schedule_Table_ROM[ScheduleTableID].expiry_point_first;
      EE_as_Schedule_Table_RAM[ScheduleTableID].deviation   = 0U;
      EE_as_Schedule_Table_RAM[ScheduleTableID].next_table  =
        INVALID_SCHEDULETABLE;

      /* Insert the alarm corresponding to this Schedule Table in alarm list */
      EE_oo_handle_rel_counter_object_insertion(obj_id, Offset +
        EE_as_Expiry_Point_ROM[EE_as_Schedule_Table_ROM[ScheduleTableID].
          expiry_point_first].offset, 0U);

      ev = E_OK;
    }
#else /* EE_MAX_SCHEDULETABLE > 0U */
    {
      ev = E_OS_ID;
    }
#endif /* EE_MAX_SCHEDULETABLE > 0U */
#ifdef EE_AS_RPC__
  }
#endif /* EE_AS_RPC__ */

  if ( ev != E_OK ) {
#ifdef __OO_HAS_ERRORHOOK__
    EE_OS_PARAM(os_sched_table_id);
    EE_OS_PARAM(os_offset);
    EE_OS_PARAM_VALUE(os_sched_table_id,ScheduleTableID);
    EE_OS_PARAM_VALUE(os_offset,Offset);
#endif /* __OO_HAS_ERRORHOOK__ */
    EE_os_notify_error(OSServiceId_StartScheduleTableRel, os_sched_table_id,
      os_offset, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_STARTSCHEDULETABLEREL);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

/*  8.4.9 StartScheduleTableAbs [SWS_Os_00358]
 *
 *  This service starts the processing of a schedule table at an absolute
 *  value "Start" on the underlying counter.
 *
 *  Sync/Async: Synchronous
 *  Reentrancy: Reentrant
 *
 *  param ScheduleTableID: Schedule table to be started
 *  param Start: Absolute counter tick value at which the schedule table is
 *    started
 *
 *  return StatusType
 *    E_OK: No Error
 *    E_OS_ID (only in EXTENDED status): ScheduleTableID not valid.
 *    E_OS_VALUE (only in EXTENDED status): "Start" is greater than
 *      OsCounterMaxAllowedValue
 *    E_OS_STATE: Schedule table was already started Description:
 */

StatusType EE_as_StartScheduleTableAbs( ScheduleTableType ScheduleTableID,
  TickType Start )
{
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)
  /* Counter Object ID that represent ScheduleTableID */
  register const CounterObjectType  obj_id = EE_MAX_ALARM + ScheduleTableID;
#endif /* EE_MAX_SCHEDULETABLE > 0 */
  /* Return error value */
  register StatusType                           ev;
  /* Primitive kernel locking  */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_STARTSCHEDTABABS);

  EE_as_monitoring_the_stack();

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of  the service. (BSW11009, BSW11013) */
  /* StartScheduleTableAbs is callable by Task, ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */
#ifdef EE_AS_RPC__
  if ( EE_AS_ID_REMOTE(ScheduleTableID) ) {
    EE_os_param const
      unmarked_sched_table_id = { EE_AS_UNMARK_REMOTE_ID(ScheduleTableID) },
      as_start = { Start };
    /* forward the request to another CPU in synchronous way */
    ev = EE_as_rpc(OSServiceId_StartScheduleTableAbs, unmarked_sched_table_id,
      as_start, EE_OS_INVALID_PARAM);
  } else {
#endif /* EE_AS_RPC__ */

/* If local schedule tables are not defined cut everything else */
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)

  /* [SWS_Os_00348] If the schedule table <ScheduleTableID> in a call of
      StartScheduleTableAbs() is not valid, StartScheduleTableAbs()
      shall return E_OS_ID. */
#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
    if ( ScheduleTableID >= EE_MAX_SCHEDULETABLE ) {
      ev = E_OS_ID;
    } else if ( EE_SCHED_TABLE_ACCESS_ERR(ScheduleTableID, EE_as_active_app) ) {
      ev = E_OS_ACCESS;
    } else
#elif defined(__OO_EXTENDED_STATUS__)
    if ( ScheduleTableID >= EE_MAX_SCHEDULETABLE ) {
      ev = E_OS_ID;
    } else
#endif /* EE_FULL_SERVICE_PROTECTION || __OO_EXTENDED_STATUS__ */
#ifdef __OO_EXTENDED_STATUS__
    /* [SWS_Os_00349] If the <Start> in a call of StartScheduleTableAbs()
        is greater than the OsCounterMaxAllowedValue of the underlying counter,
        StartScheduleTableAbs() shall return E_OS_VALUE. */
    /* [SWS_Os_00350] If the schedule table <ScheduleTableID> in a call of
        StartScheduleTableAbs() is not in the state SCHEDULETABLE_STOPPED,
        StartScheduleTableAbs() shall return E_OS_STATE. */
    if ( Start > EE_counter_ROM[EE_oo_counter_object_ROM[obj_id].c].
      maxallowedvalue )
    {
      ev = E_OS_VALUE;
    } else if ( EE_as_Schedule_Table_RAM[ScheduleTableID].
      status != SCHEDULETABLE_STOPPED )
    {
      ev = E_OS_STATE;
    } else
#endif /* __OO_EXTENDED_STATUS__ */
    {
      /* [SWS_Os_00351] If the input parameters of StartScheduleTableAbs() are
          valid and <ScheduleTableID> is in the state SCHEDULETABLE_STOPPED,
          StartScheduleTableAbs() shall start the processing of schedule table
          <ScheduleTableID> when the underlying counter next equals <Start> and
          shall set the state of <ScheduleTableID> to
          - SCHEDULETABLE_RUNNING (for a non-synchronized / Explicitly
            synchronized schedule table) OR
          - SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS (for implicitly synchronized
            schedule table)
          before returning to the user. (The Initial Expiry Point will be
          processed when the underlying counter next equals
          <Start>+Initial Offset). */
      EE_as_Schedule_Table_RAM[ScheduleTableID].status  =
        (EE_as_Schedule_Table_ROM[ScheduleTableID].
          sync_strategy == EE_SCHEDTABLE_SYNC_IMPLICIT) ?
            SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS: SCHEDULETABLE_RUNNING;

      EE_as_Schedule_Table_RAM[ScheduleTableID].position    =
        EE_as_Schedule_Table_ROM[ScheduleTableID].expiry_point_first;
      EE_as_Schedule_Table_RAM[ScheduleTableID].deviation   = 0U;
      EE_as_Schedule_Table_RAM[ScheduleTableID].next_table  =
        INVALID_SCHEDULETABLE;

      /* Insert the alarm corresponding to this Schedule Table in alarm list */
      EE_oo_handle_abs_counter_object_insertion(obj_id, Start +
        EE_as_Expiry_Point_ROM[EE_as_Schedule_Table_ROM[ScheduleTableID].
          expiry_point_first].offset, 0U);

      ev = E_OK;
    }
#else /* EE_MAX_SCHEDULETABLE > 0U */
    {
      ev = E_OS_ID;
    }
#endif /* EE_MAX_SCHEDULETABLE > 0U */
#ifdef EE_AS_RPC__
  }
#endif /* EE_AS_RPC__ */

  if ( ev != E_OK ) {
#ifdef __OO_HAS_ERRORHOOK__
    EE_OS_PARAM(os_sched_table_id);
    EE_OS_PARAM(os_start);
    EE_OS_PARAM_VALUE(os_sched_table_id,ScheduleTableID);
    EE_OS_PARAM_VALUE(os_start,Start);
#endif /* __OO_HAS_ERRORHOOK__ */
    EE_os_notify_error(OSServiceId_StartScheduleTableAbs, os_sched_table_id,
      os_start, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_STARTSCHEDTABABS);
  EE_OS_EXIT_CRITICAL_SECTION();  

  return ev;
}

/*  8.4.10 StopScheduleTable [SWS_Os_00006]
 *
 *  This service cancels the processing of a schedule table immediately at any
 *  point while the schedule table is running.
 *
 *  Sync/Async: Synchronous
 *  Reentrancy: Reentrant
 *
 *  param ScheduleTableID: Schedule table to be stopped
 *
 *  return StatusType
 *    E_OK: No Error
 *    E_OS_ID (only in EXTENDED status): ScheduleTableID not valid.
 *    E_OS_NOFUNC: Schedule table was already stopped.
 */

StatusType EE_as_StopScheduleTable( ScheduleTableType ScheduleTableID )
{
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)
  /* Counter Object ID that represent ScheduleTableID */
  register const CounterObjectType  obj_id = EE_MAX_ALARM + ScheduleTableID;
  /* To Stop Eventual Next Table */
  register ScheduleTableType                    next_table;
#endif /* EE_MAX_SCHEDULETABLE > 0 */
  /* Return error value */
  register StatusType                           ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_STOPSCHEDULETABLE);

  EE_as_monitoring_the_stack();

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of  the service. (BSW11009, BSW11013) */
  /* StopScheduleTable is callable by Task, ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

#ifdef EE_AS_RPC__
  if ( EE_AS_ID_REMOTE(ScheduleTableID) ) {
    EE_os_param const
      unmarked_sched_table_id = { EE_AS_UNMARK_REMOTE_ID(ScheduleTableID) };

    /* forward the request to another CPU in synchronous way */
    ev = EE_as_rpc(OSServiceId_StopScheduleTable, unmarked_sched_table_id,
      EE_OS_INVALID_PARAM, EE_OS_INVALID_PARAM);
  } else {
#endif /* EE_AS_RPC__ */

/* If local schedule tables are not defined cut everything else */
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)

  /* [SWS_Os_00279] If the schedule table identifier <ScheduleTableID> in a
      call of StopScheduleTable() is not valid, StopScheduleTable()
      shall return E_OS_ID. */
#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
    if ( ScheduleTableID >= EE_MAX_SCHEDULETABLE ) {
      ev = E_OS_ID;
    } else if ( EE_SCHED_TABLE_ACCESS_ERR(ScheduleTableID, EE_as_active_app) ) {
      ev = E_OS_ACCESS;
    } else
#elif defined(__OO_EXTENDED_STATUS__)
    if ( ScheduleTableID >= EE_MAX_SCHEDULETABLE ) {
      ev = E_OS_ID;
    } else
#endif /* EE_FULL_SERVICE_PROTECTION || __OO_EXTENDED_STATUS__ */
#ifdef __OO_EXTENDED_STATUS__
    /* [SWS_Os_00280] If the schedule table with identifier <ScheduleTableID> is
        in state SCHEDULETABLE_STOPPED when calling StopScheduleTable(),
        StopScheduleTable() shall return E_OS_NOFUNC. */
    if ( EE_as_Schedule_Table_RAM[ScheduleTableID].
      status == SCHEDULETABLE_STOPPED )
    {
      ev = E_OS_NOFUNC;
    } else
#endif /* __OO_EXTENDED_STATUS__ */
    {
      /* [SWS_Os_00281] If the input parameters of StopScheduleTable() are valid,
          StopScheduleTable()shall set the state of <ScheduleTableID> to
          SCHEDULETABLE_STOPPED and (stop the schedule table <ScheduleTableID> from
          processing any further expiry points and) shall return E_OK. */
      next_table = EE_as_Schedule_Table_RAM[ScheduleTableID].next_table;
      if ( next_table != INVALID_SCHEDULETABLE ) {
        /* Stop the next_table */
        EE_as_Schedule_Table_RAM[next_table].status = SCHEDULETABLE_STOPPED;
      }

      EE_as_Schedule_Table_RAM[ScheduleTableID].status = SCHEDULETABLE_STOPPED;

      /* Cancel the alarm related with the schedule table */
      EE_oo_handle_counter_object_cancellation( obj_id );

      ev = E_OK;
    }
#else /* EE_MAX_SCHEDULETABLE > 0U */
    {
      ev = E_OS_ID;
    }
#endif /* EE_MAX_SCHEDULETABLE > 0U */
#ifdef EE_AS_RPC__
  }
#endif /* EE_AS_RPC__ */

  if ( ev != E_OK ) {
 #ifdef __OO_HAS_ERRORHOOK__
    EE_OS_PARAM(os_sched_table_id);
    EE_OS_PARAM_VALUE(os_sched_table_id,ScheduleTableID);
 #endif /* __OO_HAS_ERRORHOOK__ */
    EE_os_notify_error(OSServiceId_StopScheduleTable, os_sched_table_id,
      EE_OS_INVALID_PARAM, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_STOPSCHEDULETABLE);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

/*  8.4.15 GetScheduleTableStatus [SWS_Os_00227]
 *
 *  This service queries the state of a schedule table (also with respect to
 *  synchronization).
 *
 *  Sync/Async: Synchronous

 *  Reentrancy: Reentrant
 *
 *  param ScheduleTableID (in): Schedule table for which status is requested.
 *  param ScheduleStatus (out): Reference to ScheduleTableStatusType.
 *
 *  return StatusType:
 *    E_OK: No Error
 *    E_OS_ID (only in EXTENDED status): Invalid ScheduleTableID
 *
 * (SRS_Os_11002)
 */

StatusType EE_as_GetScheduleTableStatus( ScheduleTableType ScheduleTableID,
  ScheduleTableStatusRefType ScheduleStatus )
{
  /* Return error value */
  register StatusType ev;

  EE_ORTI_set_service_in(EE_SERVICETRACE_GETSCHEDULETABLESTATUS);

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of  the service. (BSW11009, BSW11013) */
  /* GetScheduleTableStatus is callable only by Task and ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

  /* OS566: The Operating System API shall check in extended mode all pointer
      argument for NULL pointer and return OS_E_PARAMETER_POINTER
      if such argument is NULL.
      +
      MISRA dictate NULL check for pointers always. */
  if ( ScheduleStatus == NULL ) {
    ev = E_OS_PARAM_POINTER;
  } else
#if defined(__EE_MEMORY_PROTECTION__) && defined(EE_SERVICE_PROTECTION__)
  /* [SWS_Os_00051] If an invalid address (address is not writable by this
      OS-Application) is passed as an out-parameter to an Operating System
      service, the Operating System module shall return the status code
      E_OS_ILLEGAL_ADDRESS. (SRS_Os_11009, SRS_Os_11013) */
  if ( !OSMEMORY_IS_WRITEABLE(EE_hal_get_app_mem_access(EE_as_active_app,
    ScheduleStatus, sizeof(*ScheduleStatus))) )
  {
    ev = E_OS_ILLEGAL_ADDRESS;
  } else
#endif /* __EE_MEMORY_PROTECTION__ && EE_SERVICE_PROTECTION__ */
#ifdef EE_AS_RPC__
  if ( EE_AS_ID_REMOTE(ScheduleTableID) ) {
    EE_os_param as_schedule_status;
    EE_os_param const
      unmarked_sched_table_id = { EE_AS_UNMARK_REMOTE_ID(ScheduleTableID) };
    as_schedule_status.schedule_table_status_ref =ScheduleStatus;

    /* forward the request to another CPU in synchronous way */
    ev = EE_as_rpc_from_us(OSServiceId_GetScheduleTableStatus,
      unmarked_sched_table_id, as_schedule_status, EE_OS_INVALID_PARAM);
  } else {
#endif /* EE_AS_RPC__ */
/* If local schedule tables are not defined cut everything else */
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)

    /* [SWS_Os_00293] If the identifier <ScheduleTableID> in a call of
        GetScheduleTableStatus() is NOT valid, GetScheduleTableStatus() shall
        return E_OS_ID. */
#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
    if ( ScheduleTableID >= EE_MAX_SCHEDULETABLE ) {
      ev = E_OS_ID;
    } else if ( EE_SCHED_TABLE_ACCESS_ERR(ScheduleTableID, EE_as_active_app) ) {
      ev = E_OS_ACCESS;
    } else
#elif defined(__OO_EXTENDED_STATUS__)
    if ( ScheduleTableID >= EE_MAX_SCHEDULETABLE ) {
      ev = E_OS_ID;
    } else
#endif /* EE_FULL_SERVICE_PROTECTION || __OO_EXTENDED_STATUS__ */
    {
      /* [SWS_Os_00289] If the schedule table <ScheduleTableID> in a call of
          GetScheduleTableStatus() is NOT started, GetScheduleTableStatus()
          shall pass back SCHEDULETABLE_STOPPED via the reference parameter
          <ScheduleStatus> AND shall return E_OK. */
      /* [SWS_Os_00353] If the schedule table <ScheduleTableID> in a call of
          GetScheduleTableStatus() was used in a NextScheduleTable() call AND
          waits for the end of the current schedule table,
          GetScheduleTableStatus() shall return SCHEDULETABLE_NEXT via the
          reference parameter <ScheduleStatus> AND shall return E_OK. */
      /* [SWS_Os_00354] If the schedule table <ScheduleTableID> in a call of
          GetScheduleTableStatus() is configured with explicit synchronization
          AND <ScheduleTableID> was started with StartScheduleTableSynchron()
          AND no synchronization count was provided to the Operating System,
          GetScheduleTableStatus() shall return SCHEDULETABLE_WAITING via the
          reference parameter <ScheduleStatus> AND shall return E_OK. */
      /* [SWS_Os_00290] If the schedule table <ScheduleTableID> in a call of
          GetScheduleTableStatus() is started AND synchronous,
          GetScheduleTableStatus() shall pass back
          SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS via the reference parameter
          <ScheduleStatus> AND shall return E_OK. */
      /* [SWS_Os_00291] If the schedule table <ScheduleTableID> in a call of
          GetScheduleTableStatus() is started AND NOT synchronous
          (deviation is not within the precision interval OR the schedule table
          has been set asynchronous), GetScheduleTableStatus() shall pass back
          SCHEDULETABLE_RUNNING via the reference parameter ScheduleStatus AND
          shall return E_OK. */
      /* XXX: ScheduleTableStatusType is a EE_TYPESTATUS that is a EE_UREG so a
          read it SHOULD Atomic (Check this in other Architectures other than
          TriCore) */
      *ScheduleStatus = EE_as_Schedule_Table_RAM[ScheduleTableID].status &
        (~SCHEDULETABLE_ASYNC);

      ev = E_OK;
    }
#else /* EE_MAX_SCHEDULETABLE > 0U */
    {
      ev = E_OS_ID;
    }
#endif /* EE_MAX_SCHEDULETABLE > 0U */
#ifdef EE_AS_RPC__
  }
#endif /* EE_AS_RPC__ */
  if ( ev != E_OK ) {
    EE_OS_ERROR_PARAMETERS();
    EE_OS_ERROR_PARAMETERS_PARAM1_VALUE(ScheduleTableID);
    EE_OS_ERROR_PARAMETERS_PARAM2_REF(schedule_table_status_ref,ScheduleStatus);

    EE_os_notify_error_from_us(OSServiceId_GetScheduleTableStatus,
      &error_parameters, ev);
    EE_ORTI_set_service_out(EE_SERVICETRACE_GETSCHEDULETABLESTATUS);
  } else {
    EE_ORTI_set_service_out(EE_SERVICETRACE_GETSCHEDULETABLESTATUS);
  }

  return ev;
}

/* The following primitives are available only on local objects */
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)

/* 8.4.11 NextScheduleTable [SWS_Os_00191]
 *
 *  This service switches the processing from one schedule table to another
 *  schedule table.
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
  ScheduleTableType ScheduleTableID_To )
{
  /* Return error value */
  register StatusType                           ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_NEXTSCHEDULETABLE);

  EE_as_monitoring_the_stack();

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of  the service. (BSW11009, BSW11013) */
  /* NextScheduleTable is callable by Task, ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

  /* [SWS_Os_00282] If the input parameter <ScheduleTableID_From> or
      <ScheduleTableID_To> in a call of NextScheduleTable() is not valid,
      NextScheduleTable() shall return E_OS_ID. */
  /* [SWS_Os_00330] If in a call of NextScheduleTable() schedule table
      <ScheduleTableID_To> is driven by different counter than schedule table
      <ScheduleTableID_From> then NextScheduleTable() shall return an error
      E_OS_ID. */
  /* [SWS_Os_00279] If the schedule table identifier <ScheduleTableID> in a
      call of StopScheduleTable() is not valid, StopScheduleTable()
      shall return E_OS_ID. */
#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
    if ( (ScheduleTableID_From >= EE_MAX_SCHEDULETABLE) ||
        (ScheduleTableID_To >= EE_MAX_SCHEDULETABLE) )
    {
      ev = E_OS_ID;
    } else if (
      EE_SCHED_TABLE_ACCESS_ERR(ScheduleTableID_From, EE_as_active_app) ||
      EE_SCHED_TABLE_ACCESS_ERR(ScheduleTableID_To, EE_as_active_app) )
    {
      ev = E_OS_ACCESS;
    } else
#elif defined(__OO_EXTENDED_STATUS__)
    if ( (ScheduleTableID_From >= EE_MAX_SCHEDULETABLE) ||
        (ScheduleTableID_To >= EE_MAX_SCHEDULETABLE)) {
      ev = E_OS_ID;
    } else
#endif /* EE_FULL_SERVICE_PROTECTION || __OO_EXTENDED_STATUS__ */

#ifdef __OO_EXTENDED_STATUS__
  /* [SWS_Os_00484] If OsScheduleTblSyncStrategy of <ScheduleTableID_To> in a
      call of NextScheduleTable() is not equal to the OsScheduleTblSyncStrategy
      of <ScheduleTableID_From> then NextScheduleTable() shall return E_OS_ID.
   */
  /* [SWS_Os_00283] If the schedule table <ScheduleTableID_From> in a call
      of NextScheduleTable() is in state SCHEDULETABLE_STOPPED OR in state
      SCHEDULETABLE_NEXT, NextScheduleTable() shall leave the state of
      <ScheduleTable_From> and <ScheduleTable_To> unchanged and return
      E_OS_NOFUNC. */
  /* [SWS_Os_00309] If the schedule table <ScheduleTableID_To> in a call of
      NextScheduleTable() is not in state SCHEDULETABLE_STOPPED,
      NextScheduleTable() shall leave the state of <ScheduleTable_From> and
      <ScheduleTable_To> unchanged and return E_OS_STATE. */
  /*   XXX: !!! Contradiction with SWS_Os_00324 && SWS_Os_00453 !!!  */
  if ( (EE_oo_counter_object_ROM[EE_MAX_ALARM + ScheduleTableID_From].c !=
          EE_oo_counter_object_ROM[EE_MAX_ALARM + ScheduleTableID_To].c) ||
       (EE_as_Schedule_Table_ROM[ScheduleTableID_From].sync_strategy != 
          EE_as_Schedule_Table_ROM[ScheduleTableID_To].sync_strategy) )
  {
    ev = E_OS_ID;
  } else if ( (EE_as_Schedule_Table_RAM[ScheduleTableID_From].
      status == SCHEDULETABLE_STOPPED) ||
    (EE_as_Schedule_Table_RAM[ScheduleTableID_From].
      status == SCHEDULETABLE_NEXT) )
  {
    ev = E_OS_NOFUNC;
  } else if ( EE_as_Schedule_Table_RAM[ScheduleTableID_To].
    status != SCHEDULETABLE_STOPPED )
  {
    ev = E_OS_STATE;
  } else
#endif /* __OO_EXTENDED_STATUS__ */
  {
    /* [SWS_Os_00284] If the input parameters of NextScheduleTable() are valid
        then NextScheduleTable() shall start the processing of schedule table
        <ScheduleTableID_To> <ScheduleTableID_From>. FinalDelay ticks after the
        Final Expiry Point on <ScheduleTableID_From> is processed and shall
        return E_OK. NextScheduleTable() shall process the Initial Expiry Point
        on <ScheduleTableID_To> at <ScheduleTableID_From>.
        Final Delay + <ScheduleTable_To>.Initial Offset ticks after the Final
        Expiry Point on <ScheduleTableID_From> is processed. */
    /* [SWS_Os_00324] If the input parameters of NextScheduleTable() are valid
        AND the <ScheduleTableID_From> already has a "next" schedule table then
        NextScheduleTable()shall replace the previous "next" schedule table with
        <ScheduleTableID_To> and shall change the old "next" schedule table state
        to SCHEDULETABLE_STOPPED. XXX: !!! Contradiction with SWS_Os_00309 !!! */
    /* [SWS_Os_00505] If OsScheduleTblSyncStrategy of the schedule tables
        <ScheduleTableID_From> and <ScheduleTableID_To> in a call of
        NextScheduleTable() is EXPLICIT and the Operating System module already
        synchronizes <ScheduleTableID_From>, NextScheduleTable() shall continue
        synchronization after the start of processing <ScheduleTableID_To>. */
    /* [SWS_Os_00453] If the <ScheduleTableID_From> in a call of
        NextScheduleTable() is stopped, NextScheduleTable() shall not start the
        "next" schedule table and change its state to SCHEDULETABLE_STOPPED.
         XXX: !!! Contradiction with SWS_Os_00309 !!! */
    EE_as_Schedule_Table_RAM[ScheduleTableID_From].
      next_table = ScheduleTableID_To;
    EE_as_Schedule_Table_RAM[ScheduleTableID_To].status = SCHEDULETABLE_NEXT;

    ev = E_OK;
  }

  if ( ev != E_OK ) {
#ifdef __OO_HAS_ERRORHOOK__
    EE_OS_PARAM(os_sched_table_id_from);
    EE_OS_PARAM(os_sched_table_id_to);
    EE_OS_PARAM_VALUE(os_sched_table_id_from,ScheduleTableID_From);
    EE_OS_PARAM_VALUE(os_sched_table_id_to,ScheduleTableID_To);
#endif /* __OO_HAS_ERRORHOOK__ */
    EE_os_notify_error(OSServiceId_NextScheduleTable, os_sched_table_id_from,
      os_sched_table_id_to, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_NEXTSCHEDULETABLE);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

StatusType EE_as_SyncScheduleTable( ScheduleTableType ScheduleTableID,
  TickType Value )
{
  /* Return error value */
  register StatusType                           ev;

  /* Schedule Table position locals */
  register EE_UREG                              i;
  register TickType                             st_pos;
  register TickType                             next_ep_delay;
  register TickType                             temp_deviation;

  /* Counter Object ID that represent ScheduleTableID */
  register const CounterObjectType  obj_id = EE_MAX_ALARM + ScheduleTableID;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_SYNCSCHEDULETABLE);

  EE_as_monitoring_the_stack();

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of  the service. (BSW11009, BSW11013) */
  /* SyncScheduleTable is callable only by Task and ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */


#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
    /* [SWS_Os_00279] If the schedule table identifier <ScheduleTableID> in a
        call of StopScheduleTable() is not valid, StopScheduleTable()
        shall return E_OS_ID. */
    if ( ScheduleTableID >= EE_MAX_SCHEDULETABLE ) {
      ev = E_OS_ID;
    } else if ( EE_SCHED_TABLE_ACCESS_ERR(ScheduleTableID, EE_as_active_app) ) {
      ev = E_OS_ACCESS;
    } else
#elif defined(__OO_EXTENDED_STATUS__)
    if ( ScheduleTableID >= EE_MAX_SCHEDULETABLE ) {
      ev = E_OS_ID;
    } else
#endif /* EE_FULL_SERVICE_PROTECTION || __OO_EXTENDED_STATUS__ */
#ifdef __OO_EXTENDED_STATUS__
  /* [SWS_Os_00454] If the <ScheduleTableID> in a call of SyncScheduleTable()
      is not valid OR schedule table can not be explicitly synchronized
      (OsScheduleTblSyncStrategy is not equal to EXPLICIT)
      SyncScheduleTable() shall return E_OS_ID. */
  /* [SWS_Os_00454] If the <ScheduleTableID> in a call of SyncScheduleTable()
      is not valid OR schedule table can not be explicitly synchronized
      (OsScheduleTblSyncStrategy is not equal to EXPLICIT)
      SyncScheduleTable() shall return E_OS_ID. */
  /* [SWS_Os_00455] If the <Value> in a call of SyncScheduleTable() is
      greater or equal than the OsScheduleTableDuration, SyncScheduleTable()
      shall return E_OS_VALUE. */
  /* [SWS_Os_00456] If the state of the schedule table <ScheduleTableID> in a
      call of SyncScheduleTable() is equal to SCHEDULETABLE_STOPPED or
      SCHEDULETABLE_NEXT SyncScheduleTable() shall return E_OS_STATE. */
  if ( EE_as_Schedule_Table_ROM[ScheduleTableID].sync_strategy !=
    EE_SCHEDTABLE_SYNC_EXPLICIT )
  {
    ev = E_OS_ID;
  } else if ( Value >= EE_as_Schedule_Table_ROM[ScheduleTableID].duration ) {
    ev = E_OS_VALUE;
  } else if ( (EE_as_Schedule_Table_RAM[ScheduleTableID].
      status == SCHEDULETABLE_STOPPED) ||
    (EE_as_Schedule_Table_RAM[ScheduleTableID].status == SCHEDULETABLE_NEXT) ) 
  {
    ev = E_OS_STATE;
  } else
#endif /* __OO_EXTENDED_STATUS__ */
  {
    /* Check if the ScheduleTable is Started */
    if ( (EE_as_Schedule_Table_RAM[ScheduleTableID].status &
            SCHEDULETABLE_RUNNING) == SCHEDULETABLE_RUNNING )
    {
      /* [SWS_Os_00457] If the parameters in a call of SyncScheduleTable() are
          valid, SyncScheduleTable() shall provide the Operating System module
          with the current synchronization count for the given schedule table.
          (It is used to synchronize the processing of the schedule table to the
            synchronization counter.) */
      for ( i = 0U, st_pos = 0; i <= EE_as_Schedule_Table_RAM[ScheduleTableID].
        position; ++i )
      {
        st_pos += EE_as_Expiry_Point_ROM[EE_as_Schedule_Table_ROM[
          ScheduleTableID].expiry_point_first + i].offset;
      }
      /* After have summed all the passed and next expiry points offsets,
         subtract the remaining tick before the alarm => alarm delta.
         N.B delta == 0 in alarm is next tick */
      next_ep_delay = EE_oo_counter_object_RAM[obj_id].delta;
      st_pos -= (next_ep_delay + 1U);

      /* Evaluate Schedule Table Deviation before synchronization */
      temp_deviation = st_pos - Value;

      /* [SWS_Os_00420]: IF the deviation is non-zero AND the next expiry point
          is adjustable AND the table is behind the sync counter
          (TableTicksAheadOfSyncCounter <= TableTicksBehindOfSyncCounter)
          THEN the OS shall set the next EP to expire
          delay - min(MaxShorten, Deviation) ticks from the current expiry. */
      /* [SWS_Os_00421]: IF the deviation is non-zero AND the next expiry point
          is adjustable AND the table is ahead of the sync counter
          (TableTicksAheadOfSyncCounter > TableTicksBehindOfSyncCounter)
          THEN the OS shall set the next EP to expire delay +
          min(MaxLengthen, Deviation) ticks from the current expiry. */
      /* Try to synchronize */
      if ( temp_deviation != 0U ) {
        if ( temp_deviation < EE_TYPETICK_HALF_VALUE ) {
          /* Schedule Table is in advance */
          TickType max_shorten = EE_as_Expiry_Point_ROM[
            EE_as_Schedule_Table_RAM[ScheduleTableID].position].max_shorten;
          TickType shortening = EE_as_tick_min(max_shorten, temp_deviation);
          /* Shorten ep delay */
          next_ep_delay -=  shortening;
          /* Adjust deviation */
          temp_deviation -= shortening; 
        } else {
          /* Schedule Table is in delay */
          TickType max_lengthen = EE_as_Expiry_Point_ROM[
            EE_as_Schedule_Table_RAM[ScheduleTableID].position].max_lengthen;
          TickType lengthening = EE_as_tick_min(max_lengthen, -temp_deviation);
          /* Lengthen ep delay */
          next_ep_delay += lengthening;
          /* Adjust deviation */
          temp_deviation -= lengthening;
        }

        /* Cancel the alarm related with the schedule table */
        EE_oo_handle_counter_object_cancellation(obj_id);
        /* Restart the alarm related with the schedule table */
        EE_oo_handle_rel_counter_object_insertion(obj_id, next_ep_delay, 0U);
      }

      /* if st abs(deviation) < st precision -> synchronized! */  
      if ( EE_as_abs(temp_deviation) <
        EE_as_Schedule_Table_ROM[ScheduleTableID].precision )
      {
        /* [SWS_Os_00418]: The Operating System module shall set the state of
            an explicitly synchronized schedule table to
            "running and synchronous" if the deviation is less than or equal to
            the configured OsScheduleTblExplicitPrecision threshold. */
        EE_as_Schedule_Table_RAM[ScheduleTableID].status =
          SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS;
      } else {
        /* Try to synchronize */
        /* [SWS_Os_00419]: The Operating System module shall set the state of
            an explicitly synchronized schedule table to "running" if the
            deviation is greater than the configured
            OsScheduleTblExplicitPrecision threshold. */
        EE_as_Schedule_Table_RAM[ScheduleTableID].status =
          SCHEDULETABLE_RUNNING;
      }
      EE_as_Schedule_Table_RAM[ScheduleTableID].deviation = temp_deviation;
    } else {
      /* Otherwise is waiting for synchronization before start
         (p_schedule_table_RAM->status == SCHEDULETABLE_WAITING) */
      /* TODO: handle Schedule Table Synchronous Start */
    }

    ev = E_OK;
  }

  if ( ev != E_OK ) {
#ifdef __OO_HAS_ERRORHOOK__
    EE_OS_PARAM(os_schedule_table_id);
    EE_OS_PARAM(os_value);
    EE_OS_PARAM_VALUE(os_schedule_table_id,ScheduleTableID);
    EE_OS_PARAM_VALUE(os_value,Value);
#endif /* __OO_HAS_ERRORHOOK__ */
    EE_os_notify_error(OSServiceId_SyncScheduleTable, os_schedule_table_id,
      os_value, EE_OS_INVALID_PARAM, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_SYNCSCHEDULETABLE);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

#endif /* EE_MAX_SCHEDULETABLE > 0 */

