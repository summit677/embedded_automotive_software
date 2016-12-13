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
 * CVS: $Id: ee_altick.c,v 1.5 2006/06/08 20:40:42 pj Exp $
 */

#include "ee_internal.h"

/* If local alarm or schedule tables are not defined: cut everything */
#if (defined(EE_MAX_ALARM) && (EE_MAX_ALARM > 0U)) || \
  (defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0U))
#define EE_KEEP_ALARM_QUEUE_CODE
#endif /* (EE_MAX_ALARM > 0) || (EE_MAX_SCHEDULETABLE > 0U) */

#if defined(RTDRUID_CONFIGURATOR_NUMBER) \
 && (RTDRUID_CONFIGURATOR_NUMBER >= RTDRUID_CONFNUM_NO_ORTI_VARS)

/* ORTI variables */
#if defined(__OO_ORTI_ALARMTIME__) && defined (EE_KEEP_ALARM_QUEUE_CODE)
static EE_TYPETICK EE_ORTI_alarmtime[EE_MAX_ALARM
#ifdef EE_AS_SCHEDULETABLES__
  + EE_MAX_SCHEDULETABLE
#endif /* EE_AS_SCHEDULETABLES__ */
  ];
#endif /* __OO_ORTI_ALARMTIME__ && EE_KEEP_ALARM_QUEUE_CODE */

#endif /* RTDRUID_CONFIGURATOR_NUMBER */

#ifdef EE_AS_SCHEDULETABLES__
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)
#define EE_KEEP_ALARM_QUEUE_CODE
#endif /* EE_MAX_SCHEDULETABLE > 0 */
#endif /* EE_AS_SCHEDULETABLES__ */

#ifdef EE_KEEP_ALARM_QUEUE_CODE
/* Increment equal to 0 means next tick. Has been used this convention to
   utilize all values from zero to counter.maxallowedvalue range */
void EE_oo_counter_object_insert( CounterObjectType ObjectID,
  TickType increment )
{
  register CounterObjectType  current, previous;
  register CounterType        c = EE_oo_counter_object_ROM[ObjectID].c;

#ifdef __OO_ORTI_ALARMTIME__
  EE_ORTI_alarmtime[ObjectID] = increment + 1U + EE_counter_RAM[c].value;
#endif /* __OO_ORTI_ALARMTIME__ */

  current = EE_counter_RAM[c].first;

  if ( current == INVALID_COUNTER_OBJECT ) {
    /* The alarm becomes the first into the delta queue, because the queue was
       empty */
    EE_counter_RAM[c].first = ObjectID;
  } else if ( EE_oo_counter_object_RAM[current].delta > increment ) {
    /* The alarm becomes the first into the delta queue, because increment is
       less than previous delta */
    EE_counter_RAM[c].first = ObjectID;
    EE_oo_counter_object_RAM[current].delta -= increment;
  } else {
    /* The alarm is not the first into the delta queue */

    /* Follow the delta chain until I reach the right place */
    do {
      increment -= EE_oo_counter_object_RAM[current].delta;
      previous = current;
      current = EE_oo_counter_object_RAM[current].next;
    } while( (current != INVALID_COUNTER_OBJECT) &&
             (EE_oo_counter_object_RAM[current].delta <= increment) );

    /* Insert the alarm between previous and current */
    if ( current != INVALID_COUNTER_OBJECT ) {
      EE_oo_counter_object_RAM[current].delta -= increment;
    }
    EE_oo_counter_object_RAM[previous].next = ObjectID;
  }

  EE_oo_counter_object_RAM[ObjectID].delta = increment;
  EE_oo_counter_object_RAM[ObjectID].next = current;
}
#endif /* EE_KEEP_ALARM_QUEUE_CODE */

/* If counters are not defined cut everything */
#if defined(EE_MAX_COUNTER) && (EE_MAX_COUNTER > 0U)

static void EE_oo_handle_action_task(EE_oo_action_ROM_type const * const
  p_action)
{
  /* Error Value */
  register StatusType ev;
/* Activate the task; NOTE: no pre-emption at all...
   This code was directly copied from ActivateTask */
  register TaskType TaskID = 0;

  if ( p_action != NULL ) {
    TaskID = p_action->TaskID;
  }

#ifdef EE_AS_RPC__
  if ( EE_IS_TID_REMOTE(TaskID) ) {
    EE_os_param unmarked_tid;
    unmarked_tid.value_param = (EE_UREG)EE_UNMARK_REMOTE_TID(TaskID);
    /* forward the request to another CPU in synchronous way */
    ev = EE_as_rpc(OSServiceId_ActivateTask, unmarked_tid,
      EE_OS_INVALID_PARAM, EE_OS_INVALID_PARAM);
  } else {
#elif defined(__RN_TASK__)
  if ( EE_IS_TID_REMOTE(TaskID) ) {
    register EE_TYPERN_PARAM par;
    par.pending = 1U;
    /* forward the request to another CPU whether the thread do
       not become to the current CPU */
    (void)EE_rn_send(EE_UNMARK_REMOTE_TID(TaskID), EE_RN_TASK, par);
    ev = E_OK;
  } else {
#endif /* EE_AS_RPC__ || __RN_TASK__ */

#ifdef __OO_EXTENDED_STATUS__
    /* check if the task Id is valid */
    if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
      ev = E_OS_ID;
      /* XXX: This means a configuration Error it should never happens! */
    } else
#endif /* __OO_EXTENDED_STATUS__ */

    /* check for pending activations */
    if ( EE_th_rnact[TaskID] == 0U ) {
      ev = E_OS_LIMIT;
    } else {
      /* [SWS_Os_00469]: The Operating System module shall start an
          OsTaskTimeFrame when a task is activated successfully.
          (SRS_Os_11008) */
      /* [SWS_Os_00466]: If an attempt is made to activate a task before the
          end of an OsTaskTimeFrame then the Operating System module shall not
          perform the activation AND shall call the ProtectionHook()
          with E_OS_PROTECTION_ARRIVAL. */
      /* Check Interarrival Frame */
      if ( EE_as_tp_handle_interarrival(EE_AS_TP_ID_FROM_TASK(TaskID)) ) {
        /* insert the task in the ready queue */
        EE_oo_task_in_ready_queue(TaskID);
      }
      ev = E_OK;
    }
#if defined(EE_AS_RPC__) || defined(__RN_TASK__)
  }
#endif /* EE_AS_RPC__ || __RN_TASK__ */

  if ( ev != E_OK ) {
#ifdef __OO_HAS_ERRORHOOK__
    EE_OS_PARAM(os_task_id);
    EE_OS_PARAM(os_action_type);
    EE_OS_PARAM_VALUE(os_task_id,(EE_UREG)TaskID);
    EE_OS_PARAM_VALUE(os_action_type,(EE_UREG)EE_ACTION_TASK);
#endif /* __OO_HAS_ERRORHOOK__ */
    EE_os_notify_error(OSId_Action, os_task_id, EE_OS_INVALID_PARAM,
      os_action_type, ev);
  }
}

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
static void EE_oo_handle_action_event(EE_oo_action_ROM_type const * const
  p_action)
{
  /* Error Value */
  register StatusType     ev;
  /* Set an event for a task... NOTE: no pre-emption at all...
     This code was directly copied from SetEvent */
  register TaskType       TaskID  = 0;
  register EventMaskType  Mask    = 0U;

  if ( p_action != NULL ) {
    TaskID = p_action->TaskID;
    Mask = p_action->Mask;
  }

#ifdef EE_AS_RPC__
  if ( EE_IS_TID_REMOTE(TaskID) ) {
    EE_os_param unmarked_tid, as_mask;

    unmarked_tid.value_param = EE_UNMARK_REMOTE_TID(TaskID);
    as_mask.value_param      = Mask;

    /* forward the request to another CPU in synchronous way */
    ev = EE_as_rpc(OSServiceId_SetEvent, unmarked_tid,
      as_mask, EE_OS_INVALID_PARAM);
  } else {
#elif defined( __RN_EVENT__ )
  if ( EE_IS_TID_REMOTE(TaskID) ) {
    register EE_TYPERN_PARAM par;
    par.ev = Mask;
    /* forward the request to another CPU whether the thread do
       not become to the current CPU */
    (void)EE_rn_send((EE_SREG)EE_UNMARK_REMOTE_TID(TaskID), EE_RN_EVENT, par);
    ev = E_OK;
  } else {
#endif /* EE_AS_RPC__ || __RN_EVENT__ */

#ifdef __OO_EXTENDED_STATUS__    
    /* check if the task Id is valid */
    if ( (TaskID < 0) || (TaskID >= EE_MAX_TASK) ) {
      ev = E_OS_ID;
    } else if ( EE_th_is_extended[TaskID] == 0U ) {
      ev = E_OS_ACCESS;
    } else
    /* XXX: These means a configuration Error: it should never happens! */
#endif /* __OO_EXTENDED_STATUS__ */

    if ( EE_th_status[TaskID] == SUSPENDED ) {
      ev = E_OS_STATE;
    } else {
      /* [SWS_Os_00472] The Operating System module shall start an
          OsTaskTimeFrame when a task is released successfully.
          (SRS_Os_11008) */
      /* [SWS_Os_00467] If an attempt is made to release a task before the end
          of an OsTaskTimeFrame then the Operating System module shall not
          perform the release AND shall call the ProtectionHook() with
          E_OS_PROTECTION_ARRIVAL AND the EVENT SHALL BE SET. */
      /* Set the event mask only if the task is not suspended */
      EE_th_event_active[TaskID] |= Mask;
      /* Check Interarrival Frame */
      if ( EE_as_tp_handle_interarrival(EE_AS_TP_ID_FROM_TASK(TaskID)) )
      {
        /* Check if the task was waiting for an event we just set
         *
         * WARNING:
         * the test with status==WAITING is FUNDAMENTAL to avoid double
         * insertion of the task in the ready queue!!! Example, when I call
         * two times the same setevent... the first time the task must go in
         * the ready queue, the second time NOT!!!
         */
        if ( ((EE_th_event_waitmask[TaskID] & Mask) != 0U) &&
          (EE_th_status[TaskID] == WAITING) )
        {
          /* if yes, the task must go back into the READY state */
          EE_th_status[TaskID] = READY;
          /* insert the task in the ready queue */
          EE_rq_insert(TaskID);
        }
      }

      ev = E_OK;
    }
#if defined(EE_AS_RPC__) || defined(__RN_TASK__)
  }
#endif /* EE_AS_RPC__ || __RN_TASK__ */

  if ( ev != E_OK ) {
#ifdef __OO_HAS_ERRORHOOK__
    EE_OS_PARAM(os_task_id);
    EE_OS_PARAM(os_mask);
    EE_OS_PARAM(os_action_type);
    EE_OS_PARAM_VALUE(os_task_id,(EE_UREG)TaskID);
    EE_OS_PARAM_VALUE(os_mask,Mask);
    EE_OS_PARAM_VALUE(os_action_type,(EE_UREG)EE_ACTION_EVENT);
#endif /* __OO_HAS_ERRORHOOK__ */
    EE_os_notify_error(OSId_Action, os_task_id, os_mask,
      os_action_type, ev);
  }
}
#endif /* __OO_ECC1__ || __OO_ECC2__ */

#if defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__)
static void EE_oo_handle_action_callback ( const EE_oo_action_ROM_type *
  const p_action )
{
  /* Handle Alarm callback execution context */
  EE_TYPECONTEXT prev_context = EE_as_execution_context;

  EE_as_execution_context = AlarmCallback_Context;
  (p_action->f)();
  EE_as_execution_context = prev_context;
}
#else /* EE_AS_OSAPPLICATIONS__ && EE_SERVICE_PROTECTION__ */
static void EE_oo_handle_action_callback ( const EE_oo_action_ROM_type *
  const p_action ) {
  if ( p_action != NULL ) {
    (p_action->f)();
  }
}
#endif /* EE_AS_OSAPPLICATIONS__ && EE_SERVICE_PROTECTION__ */

static void EE_oo_handle_action(EE_oo_action_ROM_type const * const p_action)
{
  if ( p_action != NULL ) {

    switch ( p_action->action_kind ) {

      case  EE_ACTION_TASK:
        /* activate the task */
        EE_oo_handle_action_task(p_action);
        break;

#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
      case EE_ACTION_EVENT:
        /* set an event for a task */
        EE_oo_handle_action_event(p_action);
        break;
#endif /* defined(__OO_ECC1__) || defined(__OO_ECC2__) */

      case  EE_ACTION_CALLBACK:
        EE_oo_handle_action_callback(p_action);
        break;

      case EE_ACTION_COUNTER:
        /*
         * This "case" statement is not fully supported.
         * It has been temporary commented to prevent from MISRA
         * error dealing with recursive functions. If
         * it will be fully supported and recursive call is
         * unavoidable, thus consider a MISRA deviation.
         */
        /* Recursive Call
           TODO: HANDLE CYCLIC COUNTERS !!! */
        /*EE_oo_IncrementCounterImplementation(p_action->inccount);*/
        break;

      default:
        /* Invalid action: this should never happen, as `action' is
           initialized by RT-Druid */
        break;
    }
  }
}

#ifdef EE_AS_SCHEDULETABLES__
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)
static void  EE_as_handle_schedule_table( ScheduleTableType STId )
{
  /* Index to traverse expiry point actions */
  register EE_UREG                              i;
  /* Expiry point description */
  register EE_as_Expiry_Point_ROM_type const    *p_expiry_point;
  /* Get Schedule Table Configuration Structures */
  register EE_as_Schedule_Table_ROM_type const  *p_schedule_table_ROM = 
    &EE_as_Schedule_Table_ROM[STId];
  register EE_as_Schedule_Table_RAM_type        *p_schedule_table_RAM = 
    &EE_as_Schedule_Table_RAM[STId];
  /* Expiry point position */
  register EE_UREG expiry_position = p_schedule_table_RAM->position;

  /* This can only happen when a next schedule table is activated to stop the
     original schedule table after the final delay */
  if ( expiry_position == INVALID_SCHEDULETABLE_POSITION ) {
    p_schedule_table_RAM->status = SCHEDULETABLE_STOPPED;
    /* This is needed to stop the underlying alarm tied to the schedule table,
       otherwise the alarm handling cycle will be reschedule this alarm */
    EE_oo_counter_object_RAM[EE_MAX_ALARM + STId].cycle = 0;

    /* Get the next Schedule Table */
    STId = p_schedule_table_RAM->next_table;
    p_schedule_table_RAM->next_table = INVALID_SCHEDULETABLE;
    p_schedule_table_ROM = &EE_as_Schedule_Table_ROM[STId];
    p_schedule_table_RAM = &EE_as_Schedule_Table_RAM[STId];

    p_schedule_table_RAM->status      = SCHEDULETABLE_RUNNING;
    p_schedule_table_RAM->position    = p_schedule_table_ROM->
      expiry_point_first;
    p_schedule_table_RAM->next_table  = INVALID_SCHEDULETABLE;

    /* Schedule the alarm tied to the next Schedule Table */
    EE_oo_handle_rel_counter_object_insertion(EE_MAX_ALARM + STId,
      EE_as_Expiry_Point_ROM[p_schedule_table_ROM->expiry_point_first].offset,
      0U);
  } else {

    /* Get the Expiry point */
    p_expiry_point =  &EE_as_Expiry_Point_ROM[expiry_position];

    for ( i = p_expiry_point->actions_first; i <= p_expiry_point->actions_last;
        ++i )
    {
      /* Execute the action */
      EE_oo_handle_action( &EE_oo_action_ROM[i] );
    }

    /* Handle next expiry point insertion in alarm queue */
    /* if it is the last expiry point and if this is not a repeating
       schedule table, handle next schedule table or stop it */
    if ( (expiry_position == p_schedule_table_ROM->expiry_point_last) &&
         (p_schedule_table_ROM->repeated == 0) )
    {
      /* We reached the end of schedule table so we stop it */
      if ( p_schedule_table_RAM->next_table == INVALID_SCHEDULETABLE ) {
        /* [SWS_Os_00009] If the schedule table is single-shot, the Operating
           System module shall stop the processing of the schedule table
           Final Delay ticks after the Final Expiry Point is processed. */
        p_schedule_table_RAM->status     = SCHEDULETABLE_STOPPED;
        /* This is needed to stop the underlying alarm tied to the schedule
           table, otherwise the alarm handling cycle will be reschedule this
           alarm */
        EE_oo_counter_object_RAM[EE_MAX_ALARM + STId].cycle = 0;
      } else { 
        /* Schedule the final delay for original schedule table */
        p_schedule_table_RAM->position = INVALID_SCHEDULETABLE_POSITION;
        /* [OS427] If the schedule table is single-shot, the Operating System
            module shall allow a Final Delay between
            0 .. OsCounterMaxAllowedValue of the underlying counter. */
        /* This is an Hack to let alarm handling cycle reschedule the schedule
            table alarm with the right offset (increment) */
        EE_oo_counter_object_RAM[EE_MAX_ALARM + STId].cycle =
          p_schedule_table_ROM->duration - p_expiry_point->offset;
      }
    } else {
      if ( p_schedule_table_ROM->sync_strategy != EE_SCHEDTABLE_SYNC_NONE ) {
        /* *** TODO: HANDLE SYNCRONIZATION *** */
      }
      /* Schedule the next expiry point */
      ++expiry_position;
      p_schedule_table_RAM->position = expiry_position;
      /* This is an Hack to let alarm handling cycle reschedule the schedule
         table alarm with the right offset (increment) */
      EE_oo_counter_object_RAM[EE_MAX_ALARM + STId].cycle =
        EE_as_Expiry_Point_ROM[expiry_position].offset - p_expiry_point->offset;
    }
  }
}
#endif /* EE_MAX_SCHEDULETABLE > 0 */
#endif /* EE_AS_SCHEDULETABLES__ */

void EE_oo_IncrementCounterImplementation(CounterType CounterID)
{
  /* to_fire: Is the head of the splitted queue that have to be served at this
              counter tick. */
  register CounterObjectType to_fire = EE_counter_RAM[CounterID].first;

  /* Increment the counter value or reset it when overcome maxallowedvalue.
     I need this behaviour for AS services GetCounterValue and GetElapsedValue
   */
  EE_counter_RAM[CounterID].value += 1U;
  if (EE_counter_RAM[CounterID].value >
      EE_counter_ROM[CounterID].maxallowedvalue)
  {
    EE_counter_RAM[CounterID].value = 0U;
  }

  /* I split Alarm queue in two: The queue that SHALL be handled at this tick
     and the remainder.
     current & previous are double indexes used to split the alarm queue.
   */
  /* If the alarm queue is empty I have to do nothing */
  if ( to_fire != INVALID_COUNTER_OBJECT ) {
    /* If the head of alarm queue has not delta equal to zero I have only
       to decrement the first alarm delta    */
    if (EE_oo_counter_object_RAM[to_fire].delta == 0U) {
      /* current point to the first alarm at the beginning */
      register CounterObjectType current = to_fire;
      /* previous: Is a temporary copy of the index used as utility */
      register CounterObjectType previous;

      do {
        /* Now I will use previous to hold the previous checked alarm */
        previous = current;
        current = EE_oo_counter_object_RAM[current].next;
      } while ( (current != INVALID_COUNTER_OBJECT) &&
          (EE_oo_counter_object_RAM[current].delta == 0U) );

      /* I set the end of handled queue */
      EE_oo_counter_object_RAM[previous].next = INVALID_COUNTER_OBJECT;

      /* I set the head of the alarm queue to the current value (maybe -1) */
      EE_counter_RAM[CounterID].first = current;

      /* If not empty I decrement the first alarm delta in queue */
      if ( current != INVALID_COUNTER_OBJECT ) {
        --EE_oo_counter_object_RAM[current].delta;
      }

      /* Handle the alarm queue active at this tick */
      do {
        /* Select which handler call */
        switch ( EE_oo_counter_object_ROM[to_fire].kind ) {
#if defined(EE_MAX_ALARM) && (EE_MAX_ALARM > 0U)
          case EE_ALARM:
            EE_oo_handle_action(&EE_oo_action_ROM[EE_alarm_ROM[
              EE_oo_counter_object_ROM[to_fire].spec_id].action_id]);
              break;
#endif /* EE_MAX_ALARM > 0 */
#ifdef EE_AS_SCHEDULETABLES__
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)
          case EE_SCHEDULETABLE:
              EE_as_handle_schedule_table(EE_oo_counter_object_ROM[to_fire].
                spec_id);
              break;
#endif /* EE_MAX_SCHEDULETABLE > 0 */
#endif /* EE_AS_SCHEDULETABLES__ */
          default:
            /* Invalid counter object kind: this should never happen, as
               `counter object` is initialized by RT-Druid */
             break;
        }

        /* Save the actual alarm in previous and get the next to be executed */
        previous = to_fire;
        to_fire = EE_oo_counter_object_RAM[to_fire].next;

        /* The previous alarm is cyclic? */
        if (EE_oo_counter_object_RAM[previous].cycle > 0U) {
          /* Enqueue it again */
          EE_oo_counter_object_insert(previous,
            (EE_oo_counter_object_RAM[previous].cycle - 1U));
        } else {
          /* Counter Object no more used! */
          EE_oo_counter_object_RAM[previous].used = 0U;
        }
      } while ( to_fire != INVALID_COUNTER_OBJECT );
    } else {
      /* I do not handle any alarm but I have to decrement the first delta */
      --EE_oo_counter_object_RAM[to_fire].delta;
    }
  }
}

/* Flag from wich index software counters starts */
#ifdef EE_MAX_COUNTER_HW
#define EE_SOFT_COUNTERS_START EE_MAX_COUNTER_HW
#else
#define EE_SOFT_COUNTERS_START 0
#endif /* EE_MAX_COUNTER_HW */

/* Internal primitive */
StatusType EE_oo_IncrementCounterHardware(CounterType CounterID)
{
  /* Error Value */
  register StatusType ev;
  /* Start Critical Section */
  register EE_FREG const flag = EE_hal_begin_nested_primitive();
#ifdef EE_SERVICE_PROTECTION__
  /* This function can be call only inside the Kernel */
  if ( EE_as_execution_context != Kernel_Context ) {
    ev = E_OS_CALLEVEL;
  } else
#endif /* EE_SERVICE_PROTECTION__ */
#if (EE_SOFT_COUNTERS_START > 0)
  if ( CounterID >= (CounterType)EE_SOFT_COUNTERS_START ) {
    ev = E_OS_ID;
  } else
#endif /* (EE_SOFT_COUNTERS_START > 0) */
  {
    EE_oo_IncrementCounterImplementation(CounterID);
    ev = E_OK;
  }
/* This if statement is not always necessary */
#if defined(EE_SERVICE_PROTECTION__) || (EE_SOFT_COUNTERS_START > 0)
  if ( ev != E_OK ) {
#endif
    EE_ORTI_set_lasterror(ev);
    EE_oo_notify_error_service(OSId_Kernel, ev);
    /* XXX: This is in any case a Kernel Bug it should never happens */
#if defined(EE_SERVICE_PROTECTION__) || (EE_SOFT_COUNTERS_START > 0)
  }
#endif

  EE_hal_end_nested_primitive(flag);
  return ev;
}

#ifndef __PRIVATE_INCREMENTCOUNTER__
/* [OS399]: IncrementCounter */
StatusType EE_oo_IncrementCounter(CounterType CounterID)
{
  /* Error Value */
  register StatusType ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_INCREMENTCOUNTER);

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
  /* IncrementCounter is callable by Task and ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

#if ( ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )||\
defined (__OO_EXTENDED_STATUS__) )
#ifdef EE_AS_RPC__
  /* [OS589]: All functions that are not allowed to operate cross core shall
      return E_OS_CORE in extended status if called with parameters that
      require a cross core operation. (BSW4080013) */
  if ( EE_AS_ID_REMOTE(CounterID) )
  {
    ev = E_OS_CORE;
  } else
#endif /* EE_AS_RPC__ */
  /* [OS285]: If the input parameter CounterID in a call of IncrementCounter()
      is not valid OR the counter is a hardware counter, IncrementCounter()
      shall return E_OS_ID. */
  if (
#if (EE_SOFT_COUNTERS_START > 0)
    (CounterID < (CounterType)EE_SOFT_COUNTERS_START) ||
#endif /* (EE_SOFT_COUNTERS_START > 0) */
    (CounterID >= EE_MAX_COUNTER) )
  {
    ev = E_OS_ID;
  } else
#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
  if ( EE_COUNTER_ACCESS_ERR(CounterID, EE_as_active_app) ) {
    ev = E_OS_ACCESS;
  } else
#endif /* EE_AS_OSAPPLICATIONS__ || E_SERVICE_PROTECTION__ */
#endif /* EE_AS_OSAPPLICATIONS__ || E_SERVICE_PROTECTION__ ||
E_SERVICE_PROTECTION__ ||
__OO_EXTENDED_STATUS__ */
  {
    /* Call to function that actually increment the counter */
    EE_oo_IncrementCounterImplementation(CounterID);

    /* After all counter updates check if I'm not in a ISR2 and then
       execute rescheduling. */
    if ( EE_hal_get_IRQ_nesting_level() == 0U ) {
      EE_oo_preemption_point();
    }
    ev = E_OK;
  }

  if ( ev != E_OK ) {
    EE_ORTI_set_lasterror(ev);
    EE_oo_notify_error_IncrementCounter(CounterID, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_INCREMENTCOUNTER);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}
#endif /* __PRIVATE_INCREMENTCOUNTER__ */

#endif /* EE_MAX_COUNTER > 0 */

