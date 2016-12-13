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
 * CVS: $Id: ee_common.h,v 1.7 2006/05/03 05:59:55 pj Exp $
 */

#ifndef __INCLUDE_OO_COMMON_H__
#define __INCLUDE_OO_COMMON_H__

/*************************************************************************
                            Kernel Constants
 *************************************************************************/

/* invalid TID */
#define EE_NIL          ((EE_TID)-1)

/* maximum value for EE_UREG (used in EE_th_resource_last) */
#define EE_UREG_MINUS1  ((EE_UREG)-1)

/*
The Kernel constants like NIL and other thread statuses are defined
in the following sections:

- invalid see 13.2.4
- thread statuses see 13.2.4
*/

/* Moreover, the user must specify (into types.h) in a fashion like
   #define identifier unique_number
   the following identifiers:
   - The resource identifiers declared with the macro
     DeclareResource (13.4.2.1)
   - The task identifiers declared with the macro TASK(TaskName) (13.2.5)
   - The resource (RES_SCHEDULER) MUST have the maximum ceiling possible
     for Tasks

   For the Extended status and ORTI support:
   - EE_MAX_TASK maximum number of tasks (used in TaskActivate
   - EE_MAX_RESOURCE maximum number of resources
   - EE_MAX_ALARM maximum number of alarms

   For TASK/ALARM Autostart support:
   - EE_MAX_APPMODE maximum number of Application modes (typically
     >0, because there is always the mode OSDEFAULTAPPMODE.
*/


/*************************************************************************
 Kernel Types
 *************************************************************************/

/* priority type */
#ifndef EE_TYPEPRIO
#define EE_TYPEPRIO EE_UREG
#endif

/* status type */
/* the status type, that usually was an used configurable type, is set
   by the OO Standard (Bindings specification, paragraph 3.2) to be
   unsigned char. */
#ifndef EE_TYPESTATUS
#define EE_TYPESTATUS EE_UREG
#endif

/* pending activation type */
#ifndef EE_TYPENACT
#define EE_TYPENACT EE_UREG
#endif

/* boolean type */
#ifndef EE_TYPEBOOL
#define EE_TYPEBOOL EE_UREG
#endif

/* boolean true define */
#ifndef EE_TRUE
#define EE_TRUE ((EE_TYPEBOOL)1)
#endif

/* boolean false define */
#ifndef EE_FALSE
#define EE_FALSE ((EE_TYPEBOOL)0)
#endif

/* event mask type */
#ifndef EE_TYPEEVENTMASK
#define EE_TYPEEVENTMASK EE_UREG
#endif

/* pair type (signed!) */
#ifndef EE_TYPEPAIR
#define EE_TYPEPAIR EE_SREG
#endif

/* application mode type */
#ifndef EE_TYPEAPPMODE
#define EE_TYPEAPPMODE EE_UREG
#endif

/* service id type */
#ifndef EE_TYPEOSSERVICEID
#define EE_TYPEOSSERVICEID EE_UINT8
#endif

/* resource id type */
#ifndef EE_TYPERESOURCE
#define EE_TYPERESOURCE EE_UREG
#endif

/* counter id type */
#ifndef EE_TYPECOUNTER
#define EE_TYPECOUNTER EE_UREG
#endif

/* counter object id type */
#ifndef EE_TYPECOUNTEROBJECT
#define EE_TYPECOUNTEROBJECT  EE_UREG
#endif

/* alarm id type */
#ifndef EE_TYPEALARM
#define EE_TYPEALARM EE_TYPECOUNTEROBJECT
#endif

/* action id type */
#ifndef EE_TYPEACTION
#define EE_TYPEACTION EE_UREG
#endif

/* counter tick type */
#ifndef EE_TYPETICK
#define EE_TYPETICK EE_UREG
#endif

#ifdef EE_AS_OSAPPLICATIONS__
/** @typedef This data type identifies the OS-Application. */
typedef EE_UTID                ApplicationType;
#define INVALID_OSAPPLICATION ((ApplicationType)-1)
#define KERNEL_OSAPPLICATION  ((ApplicationType)0U)
#endif /* EE_AS_OSAPPLICATIONS__ */

#if defined(__OO_BCC2__) || defined(__OO_ECC2__)
/* Maximum number of pending activations */
extern const EE_TYPENACT   EE_th_rnact_max[EE_MAX_TASK];
#endif /* __OO_BCC2__ || __OO_ECC2__ */

/* priorities (NB: they are bit fields!!!) */
extern const EE_TYPEPRIO EE_th_ready_prio[EE_MAX_TASK];
extern const EE_TYPEPRIO EE_th_dispatch_prio[EE_MAX_TASK];

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_data */
#define OS_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/*************************************************************************
 Kernel Variables defined by the application
 *************************************************************************/

/* thread status, all initialized to SUSPENDED */
extern EE_TYPESTATUS EE_th_status[EE_MAX_TASK];

/* next: is used for:
   - the stacked queue
   - WaitEvent (?)
   - the ready queue (BCC1, ECC1)
   all initialized with EE_NIL
*/
extern EE_TID EE_th_next[EE_MAX_TASK];

/*
 * remaining nact: init= maximum pending activations of a Task
 * =1 for BCC1 and ECC1, >= 0 for BCC2 and ECC2
 *
 * all initialized with 1 (ECC2, BCC2: or with a value >1)
 */
extern EE_TYPENACT   EE_th_rnact[EE_MAX_TASK];

#ifndef __OO_NO_CHAINTASK__
/* The next task to be activated after a ChainTask. initvalue=all EE_NIL */
extern EE_TID EE_th_terminate_nextask[EE_MAX_TASK];
#endif

/* The first stacked task (initvalue = EE_NIL) */
extern EE_TID       EE_stkfirst;

#if defined(__OO_BCC1__) || defined(__OO_ECC1__)
/* First task in the ready queue (initvalue = EE_NIL) */
extern EE_TID EE_rq_first;
#endif /*  __OO_BCC1__ || __OO_ECC1__ */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_bss */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* System ceiling (initvalue = 0) */
extern EE_TYPEPRIO   EE_sys_ceiling;

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#if defined(__OO_BCC2__) || defined(__OO_ECC2__)

/*
 * ready queue implementation:
 * - 16 priorities
 * - we use a queue for each priority. Head and tail are stored in an array
 * - each queue contains pairs (Task ID, next)
 * - the number of pairs is the sum of all the possible activations
 *   of all the tasks (in that way, an activation will never fail due
 *   to the lack of a pair)
 * - to know which queue have to be used, a bit mask is used to do a fast
 *   lookup (the 8 bit lookup table is defined into ee_lookup.c)
 */

/* bit mask with 16 or 8 priority levels (Initvalue = 0) */
#if defined(__OO_BCC2__)
typedef EE_UINT8 EE_TYPE_RQ_MASK;
#else
typedef EE_UINT16 EE_TYPE_RQ_MASK;
#endif

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_data */
#define OS_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* The following data structure gives the link between a task and its
   priority queue.  The values of this data structure are the same of
   EE_ready_prio, except that they are not stored as bitfields

   Initvalue: each TID = x such that th_ready_prio[TID]= 1<<x
*/
extern EE_TYPEPRIO EE_rq_link[EE_MAX_TASK];

/* The priority queues  (initvalue: all -1;
   number of elements: 8(BCC2) or 16(ECC2) ) */
extern EE_TYPEPAIR EE_rq_queues_head[EE_RQ_QUEUES_HEAD_SIZE];
extern EE_TYPEPAIR EE_rq_queues_tail[EE_RQ_QUEUES_TAIL_SIZE];

/* The pairs that are enqueued into the priority queues */
/* initvalue: something like {1,2,3,4,5,...,-1}.
   the number of elements is equal to the sum of the elements of
   EE_th_rnact */
extern EE_TYPEPAIR EE_rq_pairs_next[EE_RQ_PAIRS_NEXT_SIZE];
/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_bss */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* init value=0 (no init value); the number of elements is equal to the
   sum of the elements of EE_th_rnact */
extern EE_TID      EE_rq_pairs_tid[EE_RQ_PAIRS_TID_SIZE];

/* RQ priority Mask */
extern EE_TYPE_RQ_MASK EE_rq_bitmask;
/* a list of unused pairs */
extern EE_TYPEPAIR EE_rq_free; /* pointer to a free pair; initvalue=0 */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* __OO_BCC2__ || __OO_ECC2__ */

/* Event handling */

/* Note: To save memory space, Extended tasks should have the smallest
   number into the task data structures. In that way, the following
   data structures can be sized to exactly the number of the extended
   tasks */
#if defined(__OO_ECC1__) || defined(__OO_ECC2__)

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_data */
#define OS_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* thread events already active; these events are set using the
   SetEvent primitive. initvalue = 0 */
extern EE_TYPEEVENTMASK EE_th_event_active[EE_MAX_TASK];

/* thread wait mask. this is the event mask the task is waiting using
   WaitEvent. A task IS waiting only if the value in this array IS != 0.
   initvalue = 0 */
extern EE_TYPEEVENTMASK EE_th_event_waitmask[EE_MAX_TASK];

/* this structure contains a flag that is 1 if a thread has been
   suspended using EE_hal_stkchange. In that case, the task have to
   be wakened again using the same function. initvalue = 0
*/
extern EE_TYPEBOOL EE_th_waswaiting[EE_MAX_TASK];

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* just a flag: this flag is 1 if the task is an extended task, 0 otherwise */
/* Please note that this flag is defined both in STANDARD and in
   EXTENDED status. The need for this flag in standard status is
   because task activations of an extended task clears its pending
   event mask. */
extern const EE_TYPEBOOL EE_th_is_extended[EE_MAX_TASK];

#endif /* __OO_ECC1__ || __OO_ECC2__ */

/* Resources data structures */

#ifndef __OO_NO_RESOURCES__

/* Resource ceiling */
extern const EE_TYPEPRIO   EE_resource_ceiling[EE_MAX_RESOURCE];

#ifdef __OO_ISR2_RESOURCES__
/* ISR2 priority tied to an resource */
extern const EE_TYPEISR2PRIO  EE_resource_isr2_priority[EE_MAX_RESOURCE];
#endif /* __OO_ISR2_RESOURCES__ */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_bss */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */


/* Note: There is no constraint on the index that is assigned
   RES_SCHEDULER!!! */
/* Old resource ceiling */
extern EE_TYPEPRIO   EE_resource_oldceiling[EE_MAX_RESOURCE];

#ifdef __OO_ISR2_RESOURCES__
/* New data structures to handle resource sharing with isr2 and isr2 hardware
   priority ceiling. */
/* Old ISR2 priority */
extern EE_TYPEISR2PRIO  EE_isr2_oldpriority[EE_MAX_RESOURCE];
#endif /* __OO_ISR2_RESOURCES__ */

#if defined(__OO_EXTENDED_STATUS__) || defined(__OO_ORTI_RES_ISLOCKED__)
/* Only in extended status or when using ORTI with resources; for each
   resource, a flag is allocated to see if the resource is locked or
   not.  Note that this information cannot be easily knew from the
   previous two data structures. initvalue=0
*/
extern EE_TYPEBOOL EE_resource_locked[EE_MAX_RESOURCE];
#endif /*__OO_EXTENDED_STATUS__ || __OO_ORTI_RES_ISLOCKED__ */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_data */
#define OS_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#if defined(__OO_EXTENDED_STATUS__) || defined(__OO_ISR2_RESOURCES__)
/* Only in extended status; for each task, we allocate a data
   structure that keeps track of the order in which every task has
   allocated a resource. This is needed to return a meaningful
   E_OS_NOFUNC error in the ReleaseResource call. */

/* This is the last resource that the task has locked. This array
   contains one entry for each task.  Initvalue= all -1. at runtime,
   it points to the first item in the EE_resource_stack data structure */
#ifndef EE_MAX_ISR2_WITH_RESOURCES
extern EE_UREG EE_th_resource_last[EE_MAX_TASK];
#else /* !EE_MAX_ISR2_WITH_RESOURCES */
extern EE_UREG EE_th_resource_last[EE_MAX_TASK + EE_MAX_ISR2_WITH_RESOURCES];
#endif /* !EE_MAX_ISR2_WITH_RESOURCES */
/* this array is used to store a list of resources locked by a
   task. there is one entry for each resource, initvalue = -1. the
   list of resources locked by a task is ended by -1. */
extern EE_UREG EE_resource_stack[EE_MAX_RESOURCE];
#endif /* __OO_EXTENDED_STATUS__ || __OO_ISR2_RESOURCES__ */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* __OO_NO_RESOURCES__ */

#if defined(__OO_ISR2_RESOURCES__) || defined(EE_AS_USER_SPINLOCKS__)

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_data */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#if defined (EE_MAX_ISR2_WITH_RESOURCES)
#if (EE_MAX_ISR2_WITH_RESOURCES > 0)
/* Array to hold corresponding isr2 nesting levels */
extern EE_UREG                EE_isr2_nesting_level[EE_MAX_ISR2_WITH_RESOURCES];
#endif /* EE_MAX_ISR2_WITH_RESOURCES > 0 */
#elif defined (EE_MAX_ISR2)
#if (EE_MAX_ISR2 > 0)
extern EE_UREG                EE_isr2_nesting_level[EE_MAX_ISR2];
#endif /* EE_MAX_ISR2 > 0 */
#endif

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_data */
#define OS_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
#endif /* __OO_ISR2_RESOURCES__ || EE_AS_USER_SPINLOCKS__ */

/* This is the stub that have to be put into the EE_th_body array */
extern void EE_oo_thread_stub(void);

/***************************************************************************
 * 13.1 Common data types
 ***************************************************************************/

/* see top of the file */

/***************************************************************************
 * 13.2 Task management
 ***************************************************************************/

/* 13.2.1 Data types                                                       */
/* ----------------------------------------------------------------------- */

/* This data type identifies a task. */
typedef EE_TID TaskType;

/* This data type points to a variable of TaskType. */
typedef EE_TID *TaskRefType;

/* This data type identifies the state of a task. */
typedef EE_TYPESTATUS TaskStateType;

/* This data type points to a variable of the data type TaskStateType. */
typedef EE_TYPESTATUS *TaskStateRefType;

/***************************************************************************
 * ISR2 management
 ***************************************************************************/
/** @typedef This data type identifies an interrupt service routine (ISR). */
typedef EE_UTID ISRType;
#define INVALID_ISR ((ISRType)-1)

/* 13.2.4 Constants                                                        */
/* ----------------------------------------------------------------------- */

/* Constant of data type TaskStateType for task state running. */
#define RUNNING   ((EE_TYPESTATUS)0U)

/* Constant of data type TaskStateType for task state waiting. */
#define WAITING   ((EE_TYPESTATUS)1U)

/* Constant of data type TaskStateType for task state ready. */
#define READY     ((EE_TYPESTATUS)2U)

/* Constant of data type TaskStateType for task state suspended. */
#define SUSPENDED ((EE_TYPESTATUS)3U)


/***************************************************************************
 * 13.4 Resource management
 ***************************************************************************/

/* 13.4.1 Data types                                                       */
/* ----------------------------------------------------------------------- */

#ifndef __OO_NO_RESOURCES__
typedef EE_TYPERESOURCE ResourceType;
#endif

/***************************************************************************
 * 13.5 Event control
 ***************************************************************************/

/* 13.5.1 Data types                                                       */
/* ----------------------------------------------------------------------- */

/* Data type of the event mask. */
typedef EE_TYPEEVENTMASK EventMaskType;

/* Reference to an event mask. */
typedef EE_TYPEEVENTMASK *EventMaskRefType;

/***************************************************************************
 * 13.6 Counters & Alarms
 ***************************************************************************/
/** @typedef This data type represents count values in ticks. */
typedef EE_TYPETICK TickType;

/** @typedef This data type points to the data type TickType. */
typedef TickType *TickRefType;

#ifdef EE_CPU_CLOCK
/** Utility Macro that convert an amount of ms in number of ticks at the CPU
    clock frequency **/
#define EE_MILLI_TO_CPU_TICKS(X_MS) EE_MILLI_TO_TICKS(X_MS, EE_CPU_CLOCK)

/** Utility Macro that convert an amount of us in number of ticks of a given
    frequency **/
#define EE_MICRO_TO_CPU_TICKS(X_US) EE_MICRO_TO_TICKS(X_US, EE_CPU_CLOCK)
#endif /* EE_CPU_CLOCK */

/* Alarms Handling */
#if (!defined(__OO_NO_ALARMS__)) || defined(EE_AS_SCHEDULETABLES__)

/* Used to handle some corner cases with TickType */
#define EE_TYPETICK_HALF_VALUE ((((EE_TYPETICK)-1) >> 1) + 1U)

/* 13.6.1 Data types                                                       */
/* ----------------------------------------------------------------------- */
/* A structure for storage of counter characteristics. */
typedef struct {
  TickType maxallowedvalue; /* Maximum possible allowed count value in
                               ticks */
  TickType ticksperbase;    /* Number of hardware ticks required to reach a
                               counter-specific (significant) unit. */
#ifdef __OO_EXTENDED_STATUS__
  TickType mincycle; /* Smallest allowed value for the cycle-parameter
                        of SetRelAlarm/SetAbsAlarm) (only for systems
                        with extended status) */
#endif /* __OO_EXTENDED_STATUS__ */
} AlarmBaseType;

/** @typedef This data type points to the data type AlarmBaseType. */
typedef AlarmBaseType *AlarmBaseRefType;

/** @typedef This data type represent a counter object ID. */
typedef EE_TYPECOUNTEROBJECT  CounterObjectType;

/** Define for an invalid counter object ID */
#define INVALID_COUNTER_OBJECT  ((CounterObjectType)-1)

/** @typedef This data type represents an action ID. */
typedef EE_TYPEACTION ActionType;

/** @typedef This data type represents an alarm ID. */
typedef EE_TYPEALARM AlarmType;

/** Define for an invalid alarm */
#define INVALID_ALARM ((AlarmType)-1)

/** @typedef This is an additional data type that represents a counter ID. */
typedef EE_TYPECOUNTER CounterType;

/** Define for an invalid counter */
#define INVALID_COUNTER ((CounterType)-1)

/** Initialization Value: depends on configuration */
typedef struct {
  TickType        maxallowedvalue;
  TickType        ticksperbase;
  TickType        mincycle;
#ifdef EE_AS_OSAPPLICATIONS__
  /** The ID of the application to which this counter belong to. */
  ApplicationType ApplID;
#endif /* EE_AS_OSAPPLICATIONS__ */
} EE_oo_counter_ROM_type;

/** Initialization Value: {0, -1} */
typedef struct {
  TickType          value;  /**< current value of the counter */
  CounterObjectType first;  /**< first object queued on the counter */
} EE_oo_counter_RAM_type;

/** Add supplementary support info for hardware counters */
#ifdef EE_MAX_COUNTER_HW
typedef struct {
  TickType microsecondspertick;
} EE_oo_counter_hw_ROM_type;
#endif /* EE_MAX_COUNTER_HW */

/** These are the different kind of counter objects */
typedef enum {
  EE_ALARM,
  EE_SCHEDULETABLE
} EE_oo_counter_object_kind_type;

/** Typedef to have a more harmonized type name with other OSEK type names */
typedef EE_oo_counter_object_kind_type CounterObjectKindType;

/** These data structures represent a generic counter object. Object of this
    kind will build a counter's expire list */
typedef struct {
  /** The counter this counter is linked to */
  CounterType           c;
  /** ID specialized Id (In Alarm or Schedule Tables List) */
  CounterObjectType     spec_id;
  /** Which kind of Counter Object is this */
  CounterObjectKindType kind;
} EE_oo_counter_object_ROM_type;

/** These data structures represent a generic counter object. Object of this
    kind will build a counter's expire list */
typedef struct {
  EE_TYPEBOOL       used;   /**< a flag that is 1 when the alarm is armed */
  TickType          cycle;  /**< cycle for periodic alarms */
  TickType          delta;  /**< delta expiration time (into a queue!) */
  CounterObjectType next;  /**< next counter object in the delta queue */
} EE_oo_counter_object_RAM_type;

/** These are the different kind of counter object actions */
typedef enum {
  EE_ACTION_TASK      = 0U,
  EE_ACTION_EVENT     = 1U,
  EE_ACTION_COUNTER   = 2U,
  EE_ACTION_CALLBACK  = 3U
} EE_oo_action_kind_type;

/** Typedef to have a more harmonized type name with other OSEK type names */
typedef EE_oo_action_kind_type ActionKindType;

/** This data structure represent an counter object action */
typedef struct {
  ActionKindType    action_kind;
  TaskType          TaskID;
#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
  EventMaskType     Mask;
#endif /* __OO_ECC1__ || __OO_ECC2__ */
  EE_VOID_CALLBACK  f;
  /* Counter to increment when the alarm expire.*/
  CounterType       inccount;
} EE_oo_action_ROM_type;

typedef struct {
  /** The ID of the action that have to be executed when this alarm expire */
  ActionType        action_id;
#ifdef EE_AS_OSAPPLICATIONS__
  /** The ID of the application to which this alarm belong to. */
  ApplicationType   ApplID;
#endif /* EE_AS_OSAPPLICATIONS__ */
} EE_oo_alarm_ROM_type;

/* This array contains, for each counter, the characteristics of the counter.
   The initialization value is implementation dependent */
#if defined (EE_COUNTER_ROM_SIZE)
#if EE_COUNTER_ROM_SIZE > 0
extern const EE_oo_counter_ROM_type EE_counter_ROM[EE_COUNTER_ROM_SIZE];
#endif
#endif

#ifdef EE_MAX_COUNTER_HW
/* Add supplementary support info for HARDWARE counters.
   This array contains, for each HARDWARE counter, the supplementary
   characteristics of the counter.The initialization value is implementation
   dependent */
extern const EE_oo_counter_hw_ROM_type
  EE_counter_hw_ROM[EE_COUNTER_HW_ROM_SIZE];
#endif /* EE_MAX_COUNTER_HW */

/* this part is the fixed part of a counter object.
   Initvalue= depends on how the alarm or the schedule table represented by this
   have been configured */
#if defined (EE_COUNTER_OBJECTS_ROM_SIZE)
#if (EE_COUNTER_OBJECTS_ROM_SIZE > 0)
extern const EE_oo_counter_object_ROM_type
  EE_oo_counter_object_ROM[EE_COUNTER_OBJECTS_ROM_SIZE];
#endif
#endif

/* this part represent the list of the configured actions.
   Initvalue= depends on how the alarms and the schedule tables have been
   have been configured */
#if defined (EE_ACTION_ROM_SIZE)
#if (EE_ACTION_ROM_SIZE > 0)
extern const EE_oo_action_ROM_type EE_oo_action_ROM[EE_ACTION_ROM_SIZE];
#endif
#endif

/* this is the fixed part of the configuration of an alarm
   Initvalue= depends on how the alarm have been configured */
#if defined (EE_ALARM_ROM_SIZE)
#if (EE_ALARM_ROM_SIZE > 0)
extern const EE_oo_alarm_ROM_type   EE_alarm_ROM[EE_ALARM_ROM_SIZE];
#endif
#endif

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_data */
#define OS_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* This is the RAM part of a counter.
   Initialization value = an array of {0,-1} elements */
extern EE_oo_counter_RAM_type       EE_counter_RAM[EE_MAX_COUNTER];

/* this part is the variable part of a counter object.
   Initvalue: all zeros. Note that setting the next value to 0 and
   not -1 does not give problems because used=0; the next field will
   be set by counter_object_insert when inserting the alarm in the queue */
#if defined (EE_COUNTER_OBJECTS_ROM_SIZE)
#if (EE_COUNTER_OBJECTS_ROM_SIZE > 0)
extern EE_oo_counter_object_RAM_type
  EE_oo_counter_object_RAM[EE_COUNTER_OBJECTS_ROM_SIZE];
#endif
#endif

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* !__OO_NO_ALARMS__ || EE_AS_SCHEDULETABLES__ */

/***************************************************************************
 * 13.7 Operating system execution control
 ***************************************************************************/

/* 13.7.1 Data types                                                       */
/* ----------------------------------------------------------------------- */

/* This data type represents the application mode. */
typedef EE_TYPEAPPMODE AppModeType;

/***************************************************************************
 * 13.1 Common data types
 ***************************************************************************/

/* 3.2 (Bindings). StatusType is an unsigned char */
#ifndef STATUSTYPEDEFINED
#define STATUSTYPEDEFINED
typedef unsigned char StatusType;
#define E_OK                        ((StatusType)0U)
#endif

#define E_OS_ACCESS                 ((StatusType)1U)
#define E_OS_CALLEVEL               ((StatusType)2U)
#define E_OS_ID                     ((StatusType)3U)
#define E_OS_LIMIT                  ((StatusType)4U)
#define E_OS_NOFUNC                 ((StatusType)5U)
#define E_OS_RESOURCE               ((StatusType)6U)
#define E_OS_STATE                  ((StatusType)7U)
#define E_OS_VALUE                  ((StatusType)8U)
#define E_OS_SERVICEID              ((StatusType)9U)
#define E_OS_ILLEGAL_ADDRESS        ((StatusType)10U)
#define E_OS_MISSINGEND             ((StatusType)11U)
#define E_OS_DISABLEDINT            ((StatusType)12U)
#define E_OS_STACKFAULT             ((StatusType)13U)
#define E_OS_PARAM_POINTER          ((StatusType)14U)
#define E_OS_PROTECTION_MEMORY      ((StatusType)15U)
#define E_OS_PROTECTION_TIME        ((StatusType)16U)
#define E_OS_PROTECTION_ARRIVAL     ((StatusType)17U)
#define E_OS_PROTECTION_LOCKED      ((StatusType)18U)
#define E_OS_PROTECTION_EXCEPTION   ((StatusType)19U)
/* Spinlocks errors */
#define E_OS_SPINLOCK               ((StatusType)20U)
#define E_OS_INTERFERENCE_DEADLOCK  ((StatusType)21U)
#define E_OS_NESTING_DEADLOCK       ((StatusType)22U)
/* RPC errors */
#define E_OS_CORE                   ((StatusType)23U)

/* Implementation specific errors: they must start with E_OS_SYS_ */
/* Error during StartOS */
#define E_OS_SYS_INIT               ((StatusType)24U)

/***************************************************************************
 * ORTI support
 ***************************************************************************/

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_api_bss */
#define API_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef __OO_ORTI_LASTERROR__

/* a lasterror value that can be used with ORTI */
extern StatusType EE_ORTI_lasterror;
/* Macro to set lasterror value that can be used with ORTI */
#define EE_ORTI_set_lasterror(ERR) (EE_ORTI_lasterror = (ERR))
#else /* __OO_ORTI_LASTERROR__ */

#define EE_ORTI_set_lasterror(ERR) ((void)0)
#endif  /* __OO_ORTI_LASTERROR__ */

#ifdef __OO_ORTI_SERVICETRACE__
/* the last OO service called by the application.  SERVICETRACE IDs
   are even numbers. The LSBit is used as a flag and it is set to 1
   when the servce is entered, to 0 at exit.
*/

#define EE_SERVICETRACE_ACTIVATETASK              2U
#define EE_SERVICETRACE_TERMINATETASK             4U
#define EE_SERVICETRACE_CHAINTASK                 6U
#define EE_SERVICETRACE_SCHEDULE                  8U
#define EE_SERVICETRACE_GETTASKID                 10U
#define EE_SERVICETRACE_GETTASKSTATE              12U
#define EE_SERVICETRACE_DISABLEALLINTERRUPTS      14U
#define EE_SERVICETRACE_ENABLEALLINTERRUPTS       16U
#define EE_SERVICETRACE_SUSPENDALLINTERRUPTS      18U
#define EE_SERVICETRACE_RESUMEALLINTERRUPTS       20U
#define EE_SERVICETRACE_SUSPENDOSINTERRUPTS       22U
#define EE_SERVICETRACE_RESUMEOSINTERRUPTS        24U
#define EE_SERVICETRACE_GETRESOURCE               26U
#define EE_SERVICETRACE_RELEASERESOURCE           28U
#define EE_SERVICETRACE_SETEVENT                  30U
#define EE_SERVICETRACE_CLEAREVENT                32U
#define EE_SERVICETRACE_GETEVENT                  34U
#define EE_SERVICETRACE_WAITEVENT                 36U
#define EE_SERVICETRACE_GETALARMBASE              38U
#define EE_SERVICETRACE_GETALARM                  40U
#define EE_SERVICETRACE_SETRELALARM               42U
#define EE_SERVICETRACE_SETABSALARM               44U
#define EE_SERVICETRACE_CANCELALARM               46U
#define EE_SERVICETRACE_GETACTIVEAPPLICATIONMODE  48U
#define EE_SERVICETRACE_STARTOS                   50U
#define EE_SERVICETRACE_SHUTDOWNOS                52U
#define EE_SERVICETRACE_FORCESCHEDULE             54U
#define EE_SERVICETRACE_INCREMENTCOUNTER          56U
#define EE_SERVICETRACE_GETCOUNTERVALUE           58U
#define EE_SERVICETRACE_GETELAPSEDVALUE           60U

#ifdef __OO_SEM__
#define EE_SERVICETRACE_INITSEM                   62U
#define EE_SERVICETRACE_WAITSEM                   64U
#define EE_SERVICETRACE_TRYWAITSEM                66U
#define EE_SERVICETRACE_POSTSEM                   68U
#define EE_SERVICETRACE_GETVALUESEM               70U
#define EE_SERVICETRACE_OO_LAST                   72U
#else /* __OO_SEM__ */
#define EE_SERVICETRACE_OO_LAST                   62U
#endif /* __OO_SEM__ */

extern volatile EE_UINT8 EE_ORTI_servicetrace;

__INLINE__ void EE_ORTI_set_service(EE_UINT8 srv)
{
  EE_ORTI_servicetrace = srv;
#ifdef __OO_ORTI_USE_OTM__
  EE_ORTI_send_otm_servicetrace(srv);
#endif /* __OO_ORTI_USE_OTM__ */
}

#else /* __OO_ORTI_SERVICETRACE__ */

#define EE_ORTI_set_service(srv) ((void)0)

#endif /* else __OO_ORTI_SERVICETRACE__ */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_api_bss */
#define API_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* Mapping for ORTI service In and Out */
#define EE_ORTI_set_service_in(id) EE_ORTI_set_service((id) + 1U)
#define EE_ORTI_set_service_out(id) EE_ORTI_set_service(id)

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_bss */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef __OO_ORTI_PRIORITY__
/* This flag enables the visualization of the current task priority in ORTI
   debuggers. */

/* this variable contains the current task priority and that is the priority
   that is read by the ORTI debugger
   Initvalue: the ready priority set for the task
*/
extern EE_TYPEPRIO EE_ORTI_th_priority[EE_MAX_TASK];

/* this variable contains the current task priority saved when a task
   locked a resource. It works because a resource can be
   locked only by one task at a time.
   InitValue: all 0
*/
#if defined (EE_MAX_RESOURCE) && (EE_MAX_RESOURCE > 0U)
extern EE_TYPEPRIO EE_ORTI_resource_oldpriority[EE_MAX_RESOURCE];
#endif /* EE_MAX_RESOURCE && EE_MAX_RESOURCE > 0U */

#define EE_ORTI_set_th_eq_dispatch_prio(tmp) \
  (EE_ORTI_th_priority[(tmp)] = EE_th_dispatch_prio[(tmp)])

__INLINE__ void __ALWAYS_INLINE__ EE_ORTI_set_th_priority(EE_TID index,
    EE_TYPEPRIO prio)
{
    EE_ORTI_th_priority[index] = prio;
}
#else
#define EE_ORTI_set_th_eq_dispatch_prio(tmp)    ((void)0)
#define EE_ORTI_set_th_priority(index, prio)    ((void)0)
#endif /*__OO_ORTI_PRIORITY__ */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/***************************************************************************
 * Semaphores
 ***************************************************************************/

/* Semaphores are not part of the OSEK API, please refer to the EE
   reference manual */
/* Constants                                                               */
/* ----------------------------------------------------------------------- */

/* the maximum unsigned integer */
#define EE_MAX_SEM_COUNTER ((EE_UREG)-1)

/* Data types                                                              */
/* ----------------------------------------------------------------------- */
#ifdef __OO_SEM__

struct EE_TYPESEM {
  EE_UREG   count;
#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
  EE_TID    first;
  EE_TID    last;
#endif /* __OO_ECC1__ || __OO_ECC2__ */
};

/* Data type of the event mask. */
typedef struct EE_TYPESEM  SemType;

/* Reference to an event mask. */
typedef SemType *SemRefType;

#endif /* __OO_SEM__ */

/***************************************************************************
 * Autostart Features inside StartOS()
 ***************************************************************************/

/* Note: Autostart uses the symbol EE_MAX_APPMODE that represents
   the number of different application modes in the system. */

#ifdef __OO_AUTOSTART_TASK__

/* This is the data structure used to store the autostart data for tasks.

   - n contains the number of tasks that have to be automatically
     activated at startup for a given application mode.

   - task contains the list of TIDs that have to be activated for a
     given application mode.
*/

struct EE_oo_autostart_task_type {
  EE_UREG n;
  const EE_TID *task;
};

/* For each valid APPMODE (that ranges from 0 to EE_MAX_APPMODE-1) there must
   be an item in this array with the tasks that are activated at startup. */
extern const struct EE_oo_autostart_task_type
  EE_oo_autostart_task_data[EE_MAX_APPMODE];

#endif

#ifdef __OO_AUTOSTART_ALARM__

/* This is the data structure used to store the autostart data for alarms.

   - n contains the number of alarms that have to be automatically
     set at startup for a given application mode.

   - alarm contains the list of Alarm IDs that have to be activated for a
     given application mode.
*/

struct EE_oo_autostart_alarm_type {
  EE_UREG n;
  const EE_TYPEALARM *alarm;
};

/* For each valid APPMODE (that ranges from 0 to EE_MAX_APPMODE-1) there must
   be an item in this array with the tasks that are activated at startup. */
extern const struct EE_oo_autostart_alarm_type
  EE_oo_autostart_alarm_data[EE_MAX_APPMODE];

/* For each Alarm that is activated there should be an item in these
   arrays that contains the cycle and increment values that have to be
   used when activating a given alarm ID. Note that cycle/increment
   are unique for each alarm activation time.
   The size of these two arrays is MAXALARM.
*/
extern const EE_TYPETICK EE_oo_autostart_alarm_increment[EE_MAX_ALARM];
extern const EE_TYPETICK EE_oo_autostart_alarm_cycle[EE_MAX_ALARM];
#endif

#ifdef __MSRP__
/*******************************************************************************
 *               Autosar Multicore support Type Declarations
 ******************************************************************************/
/*  The following type declarations are added here and not inside AS kernel
    because i need "TryToGetSpinlockType *" to completely define EE_os_param
    type (used inside Error parameters structure).
    The problem is that theoretically there is not in C such a thing as
    "standard pointer dimension" but each pointer COULD HAVE his own dimension
    (it never happens... Actually the 99% of architectures has pointer and
    value registers with same dimension). */

typedef EE_TYPECOREID CoreIdType;

#ifdef EE_AS_USER_SPINLOCKS__
typedef EE_TYPESPIN   SpinlockIdType;
#define INVALID_SPINLOCK ((SpinlockIdType)-1)

typedef enum {
  TRYTOGETSPINLOCK_NOSUCCESS = 0x0,
  TRYTOGETSPINLOCK_SUCCESS
} TryToGetSpinlockType;
#endif /*  EE_AS_USER_SPINLOCKS__ */
#endif /* __MSRP__ */

/*[OS088] If an OS-Application makes a service call from the wrong context AND
  is currently not inside a Category 1 ISR the Operating System module shall
  not perform the requested action (the service call shall have no effect),
  and return E_OS_CALLEVEL (see [12], section 13.1) or the “invalid value” of
  the service. (BSW11009, BSW11013) */
/** @typedef OS-Application contexts enum used to implement Autosar OS O0S88
    requirement */
typedef enum {
  Kernel_Context,
  TASK_Context,
  ISR2_Context,
  ErrorHook_Context,
  ProtectionHook_Context,
  PreTaskHook_Context,
  PostTaskHook_Context,
  StartupHook_Context,
  ShutdownHook_Context,
  AlarmCallback_Context,
  Idle_Context
} EE_TYPECONTEXT;

#ifdef EE_SERVICE_PROTECTION__
/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_data */
#define OS_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** @var used by the kernel to flag in witch context is executing */
extern EE_TYPECONTEXT EE_as_execution_context;

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

__INLINE__ EE_TYPECONTEXT __ALWAYS_INLINE__ EE_as_get_execution_context( void )
{
  return EE_as_execution_context;
}

__INLINE__ void __ALWAYS_INLINE__ EE_as_set_execution_context ( EE_TYPECONTEXT
  ctx )
{
  EE_as_execution_context = ctx;
}
#else /* EE_SERVICE_PROTECTION__ */
#define EE_as_get_execution_context()     Kernel_Context
__INLINE__ void __ALWAYS_INLINE__ EE_as_set_execution_context ( EE_TYPECONTEXT
  ctx ) {
  /* Unusefull operation to meet Linters requirements */
  (void)ctx;
}
#endif /* EE_SERVICE_PROTECTION__ */

#ifdef EE_AS_OSAPPLICATIONS__
/*******************************************************************************
 *                    Autosar support Type Declarations
 ******************************************************************************/
 /* The following type declarations are added here and not inside AS kernel
    because i need "TrustedFunctionParameterRefType" and
    "ApplicationStateRefType" to completely define EE_os_param type
    (used inside Error parameters structure).
    The problem is that theoretically there is not in C such a thing as
    "standard pointer dimension" but each pointer COULD HAVE his own dimension
    (it never happens... Actually the 99% of architectures has pointer and
     value registers with same dimension). */

/** @typedef This data type identifies the state of an OS-Application. */
typedef enum {
  APPLICATION_ACCESSIBLE,
  APPLICATION_RESTARTING,
  APPLICATION_TERMINATED
} ApplicationStateType;

/** @typedef This data type points to location where a ApplicationStateType can
      be stored. */
typedef ApplicationStateType  *ApplicationStateRefType;

/** @typedef This data type identifies if an OS-Application has access to an object. */
typedef enum {
  NO_ACCESS = 0U,
  ACCESS
} ObjectAccessType;

/** @typedef This data type defines the use of a Restart Task after terminating
    an OS-Application. */
typedef enum {
  NO_RESTART = 0U,
  RESTART
} RestartType;

/** @typedef This type will hold the access mask for an "object" system ID */
typedef EE_UREG EE_TYPEACCESSMASK;

/** @typedef This data type identifies an object. */
typedef enum {
  OBJECT_TASK = 0U,
  OBJECT_ISR,
  OBJECT_ALARM,
  OBJECT_RESOURCE,
  OBJECT_COUNTER,
  OBJECT_SCHEDULETABLE,
  OBJECT_SPINLOCK
} ObjectTypeType;

/** @typedef Static ISR descriptor */
typedef struct {
  ApplicationType ApplID; /**< ID of the application the ISR belongs to */
#ifdef __OO_ORTI_RUNNINGISR2__
  EE_ORTI_runningisr2_type  handler; /**< Handler for ISR2 tracing */
#endif /* __OO_ORTI_RUNNINGISR2__ */
} EE_as_ISR_ROM_type;

/** @typedef This data type is a pointer which is able to point to any location
    in the MCU. */
typedef EE_ADDR MemoryStartAddressType;

/** @typedef Runtime ISR descriptor. */
typedef struct {
  /** TOS to be restored when the ISR is terminated. */
  MemoryStartAddressType  TerminationTOS;
  /** ISR ID that hold the position in ISR stack. */
  ISRType                 ISR_ID;
  /** Contain the interrupted OS Application */
  ApplicationType         Interrupted_App;
  /** Architecture dependent register value used to rewind ISR call stack
      in ISR termination. */
  EE_UREG                 ISR_Terminate_data;
} EE_as_ISR_RAM_type;

/** @typedef Static OS Application descriptor */
typedef struct {
  EE_APP_SEC_INFO_T sec_info; /**< Address and size of application sections */
  /** Mode bits, to distinguish between trusted and untrusted applications
      (encoding is platform dependent). NOTE: it could be critical for
      performance, as it is accessed on every mode switch. */
  EE_UREG           Mode;
  EE_UREG           ISRTOS; /**< TOS used by the application's ISRs. */
  TaskType          restart_task; /**< ID of the application restarting TASK */
} EE_as_Application_ROM_type;

/** @typedef Runtime OS Application descriptor */
typedef struct {
  ApplicationStateType    ApplState; /**< State of the application */
  /** Used to flag if the OS-Application is calling a TRUSTED Service */
  EE_UREG                 TrustedFunctionCallsCounter;
} EE_as_Application_RAM_type;

/** Utility to fill EE_as_Application_RAM_type struct */
#define EE_APP_RAM_INIT(stack) { ((MemoryStartAddressType)(stack)), \
  APPLICATION_ACCESSIBLE, 0U }

/** @brief Type that hold an hook function pointer (without parameters) */
typedef void (* EE_HOOKTYPE) ( void );

/** @brief Type that hold an hook function pointer
    (with a StatusType parameter) */
typedef void (* EE_STATUSHOOKTYPE) ( StatusType );

/** @var Static OS Application info. The first entry is reserved for Kernel
    OS-Application. */
extern const EE_as_Application_ROM_type EE_as_Application_ROM[/*EE_MAX_APP*/];
/** @var Mapping from tasks to applications */
extern const ApplicationType EE_th_app[/*EE_MAX_TASK*/];

/** @var Mapping from ISRs to applications */
extern const EE_as_ISR_ROM_type EE_as_ISR_ROM[/* No Macro */];

/** @brief OS Applications STARTUPHOOKs List */
extern EE_HOOKTYPE const EE_as_Application_startuphook[/*EE_MAX_APP*/];

/** @brief OS Applications SHUTDOWNHOOKs List */
extern EE_STATUSHOOKTYPE const EE_as_Application_shutdownhook[/*EE_MAX_APP*/];

/** @brief OS Applications ERRORHOOKs List */
extern EE_STATUSHOOKTYPE const EE_as_Application_errorhook[/*EE_MAX_APP*/];

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_bss */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** @var To flag which OS-Application is running */
extern ApplicationType EE_as_active_app;

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_data */
#define OS_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** @var Run-time info about applications. The first entry is reserved for
    Kernel OS-Application. */
extern EE_as_Application_RAM_type EE_as_Application_RAM[/*EE_MAX_APP*/];

/** @var LIFO list of running ISRs. The current record is given by
   (EE_hal_get_IRQ_nesting_level() - 1U), when an ISR is running. */
extern EE_as_ISR_RAM_type EE_as_ISR_stack[/*EE_MAX_NESTING_LEVEL*/];

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef  __EE_MEMORY_PROTECTION__
/*******************************************************************************
 *                        Memory Protection Support
 ******************************************************************************/

/*
 * 8.2 Macros
 */
#define OSMEMORY_IS_READABLE(acc)   (((acc) & EE_ACCESS_READ) != (AccessType)0)
#define OSMEMORY_IS_WRITEABLE(acc)  (((acc) & EE_ACCESS_WRITE) != (AccessType)0)
#define OSMEMORY_IS_EXECUTABLE(acc) (((acc) & EE_ACCESS_EXEC) != (AccessType)0)
#define OSMEMORY_IS_STACKSPACE(acc) (((acc) & EE_ACCESS_STACK) != (AccessType)0)

/*
 * 8.3 Type definitions
 */

/** @typedef This data type identifies a trusted function. */
typedef EE_UINT32 TrustedFunctionIndexType;
/** @typedef This data type points to a structure which holds the arguments
            for a call to a trusted function. */
typedef void *TrustedFunctionParameterRefType;

/** @typedef This type holds information how a specific memory region can be
             accessed. */
typedef EE_UREG AccessType;

/* AccessType is a mask made by ORing together a subset of these bits: */
#define EE_ACCESS_READ        ((AccessType)0x1U)
#define EE_ACCESS_WRITE       ((AccessType)0x2U)
#define EE_ACCESS_EXEC        ((AccessType)0x4U)
#define EE_ACCESS_STACK       ((AccessType)0x8U)

/** @typedef This data type holds the size (in bytes) of a memory region. */
typedef EE_UREG MemorySizeType;
#endif /* __EE_MEMORY_PROTECTION__ */

#endif /* EE_AS_OSAPPLICATIONS__ */

/*******************************************************************************
              7.3 Schedule Tables (AUTOSAR_SWS_OS R4.0 Rev 3)
 ******************************************************************************/
#ifdef EE_AS_SCHEDULETABLES__
/*                   IMPORTANT (Implementation Notes):
   In order to Implement Schedule Tables re-using the majority possible of
   alarms handling code, we will declare a "complete alarm" (this means with
   both RAM and ROM structures configured) for each schedule table appended to
   the "Users Alarms". And a "partial alarm" (this means with only the ROM
   structure configured) for each "Expiry Point Action", appended to
   "Schedule Tables Alarms".
 */

/** @typedef This data type identifies a schedule table. */
typedef EE_UTID ScheduleTableType;
#define INVALID_SCHEDULETABLE          ((ScheduleTableType)-1)

#ifdef EE_AS_AUTOSTART_SCHEDULETABLE__

typedef enum EE_as_kind_of_start_type {
  /** Declare that the schedule table have to be started as
      StartScheduleTableRel does */
  EE_ST_START_ABSOLUTE,
  /** Declare that the schedule table have to be started as
      StartScheduleTableAbs does */
  EE_ST_START_RELATIVE,
  /** Declare that the schedule table have to be started as
      StartScheduleTableSynchron does.XXX: Not implement yet */
  EE_ST_START_SYNCHRON
} EE_as_kind_of_start;

/** @typedef Type used to support schedule table autostart data */
typedef struct EE_as_schedule_table_autostart_data_type {
  /** ID of the schedule table to be started */
  ScheduleTableType   scheduletable_id;
  /** Type of start */
  EE_as_kind_of_start start_kind;
  /** Start primitive parameters his semantics is tied to the start kind:
      it could be an offset or the absolute counter value. In case of SYNCRON
      Start it is useless */
  TickType            start_value;
} EE_as_schedule_table_autostart_data;

/** @typedef Type used to support schedule table autostart */
typedef struct EE_as_schedule_table_autostart_type_ {
  /** Number of alarms that have to be automatically set at startup for a given
      application mode */
  EE_UREG                                     n;
  const EE_as_schedule_table_autostart_data * autostart_schedule_tables;
} EE_as_schedule_table_autostart_type;

/** For each valid APPMODE (that ranges from 0 to EE_MAX_APPMODE-1) there must
    be an item in this array with the tasks that are activated at startup. */
extern const EE_as_schedule_table_autostart_type
  EE_as_schedule_table_autostart[/*EE_MAX_APPMODE*/];

#endif /* EE_AS_AUTOSTART_SCHEDULETABLE__ */

/** @typedef This type describes the status of a schedule. The status can be
    one of the following: */
typedef EE_TYPESTATUS ScheduleTableStatusType;

/** The schedule table is not started. */
#define SCHEDULETABLE_STOPPED         0U
/** The schedule table will be started after the end of currently running
    schedule table (schedule table was used in NextScheduleTable() service). */
#define SCHEDULETABLE_NEXT            1U
/** The schedule table uses explicit synchronization, has been started and is
    waiting for the global time. */
#define SCHEDULETABLE_WAITING         2U
/** The schedule table is running, but is currently not synchronous to a
    global time source. */
#define SCHEDULETABLE_RUNNING         3U
/** Used as bit-mask, flag if the schedule table is synchronized */
#define SCHEDULETABLE_SYNCHRONOUS     0x04U
/** Used as bit-mask, flag if the schedule table shall be not synchronized */
#define SCHEDULETABLE_ASYNC           0x08U
/** The schedule table is running and is synchronous to a global time source
    (SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS) */
#define SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS SCHEDULETABLE_RUNNING +\
  SCHEDULETABLE_SYNCHRONOUS


/** @typedef This data type points to a variable of the data type
    ScheduleTableStatusType. */
typedef ScheduleTableStatusType *ScheduleTableStatusRefType;

/** @typedef Schedule Table Synchronization strategies symbols */
typedef enum {
  /** No support for synchronization. (default) */
  EE_SCHEDTABLE_SYNC_NONE,
  /** The counter driving the schedule table is the counter with which
      synchronization is required */
  EE_SCHEDTABLE_SYNC_IMPLICIT,
  /** The schedule table is driven by an OS counter but processing needs to be
      synchronized with a different counter which is not an OS counter object.
      Specification of Operating System */
  EE_SCHEDTABLE_SYNC_EXPLICIT
} EE_as_synch_strategy;

/** @typedef to have a more harmonized type name with other OSEK type names. */
typedef EE_as_synch_strategy SynchStrategyType;

/** @typedef Structures that hold the information related to an expiry point */
typedef struct {
  /** Expiry point offset in its own schedule table */
  TickType                offset;
  /** Index of the first expiry point's action */
  ActionType              actions_first;
  /** Index of the last expiry point's action */
  ActionType              actions_last;
  /** Maximum value that can be subtracted from the expiry offset */
  TickType                max_shorten;
  /** Maximum value that can be added to the expiry point offset */
  TickType                max_lengthen;
} EE_as_Expiry_Point_ROM_type;

/** @typedef to have a more harmonized type name with other OSEK type names. */
typedef EE_UREG ExpiryPointType;

/**
 * @typedef EE_as_Schedule_Table_ROM_type
 *
 * This is the data structure used to describe the constant part of a
 * schedule table. */
typedef struct {
  /** Index of the first schedule table's expiry point. */
  ExpiryPointType               expiry_point_first;
  /** Index of the last schedule table's expiry point. */
  ExpiryPointType               expiry_point_last;
  /** Schedule table synchronization strategy */
  SynchStrategyType             sync_strategy;
  /** The length of the schedule table in ticks */
  TickType                      duration;
  /** Minimum deviation from synchronization source to be synchronized */
  TickType                      precision;
  /** TRUE if the schedule table shall be repeated after the last expiry point,
      FALSE if the schedule table is single-shot */
  EE_TYPEBOOL                   repeated;
#ifdef EE_AS_OSAPPLICATIONS__
  /** The ID of the application to which this schedule table belong to. */
  ApplicationType               ApplID;
#endif /* EE_AS_OSAPPLICATIONS__ */
} EE_as_Schedule_Table_ROM_type;

#define INVALID_SCHEDULETABLE_PRECISION ((TickType)-1)

/**
 * @typedef EE_as_Schedule_Table_RAM_type
 *
 * Data structure to store variable informations about a schedule table.
 */
typedef struct {
  /** Schedule Table current status */
  ScheduleTableStatusType status;
  /** Expiry point to be processed in the schedule table */
  EE_UREG                 position;
  /** Deviation of the schedule table from synchronization */
  TickType                deviation;
  /** ID of next schedule table to be started */
  ScheduleTableType       next_table;
} EE_as_Schedule_Table_RAM_type;
#define INVALID_SCHEDULETABLE_POSITION ((EE_UREG)-1)

/** @var Array of constant descriptors of Expiry accessed by
    #ScheduleTableType values*/
extern const EE_as_Expiry_Point_ROM_type
  EE_as_Expiry_Point_ROM[/*EE_MAX_SCHEDULETABLE*/];


/** @var Array of constant descriptors of schedule tables accessed by
    #ExpiryPointType values */
extern const EE_as_Schedule_Table_ROM_type
  EE_as_Schedule_Table_ROM[/*EE_MAX_SCHEDULETABLE*/];

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA API section: ee_kernel_data */
#define OS_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */


/** @var Array of mutable descriptors of schedule tables accessed by
    #ScheduleTableType values */
extern EE_as_Schedule_Table_RAM_type
  EE_as_Schedule_Table_RAM[/*EE_MAX_SCHEDULETABLE*/];

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* EE_AS_SCHEDULETABLES__ */

/***************************************************************************
 * 13.8 Hook routines
 ***************************************************************************/


/* 13.8.1 Data Types                                                       */
/* ----------------------------------------------------------------------- */

/** @typedef This data type represents the identification of system services. */
typedef EE_TYPEOSSERVICEID OSServiceIdType;

/** @typedef primitive object id parameter type */
typedef EE_UTID EE_os_param_id;
#define INVALID_OBJECTID ((EE_os_param_id)-1)

/** @typedef primitive parameter type */
typedef union EE_os_param_type {
  EE_UREG                         value_param;
  TaskRefType                     task_ref;
  TaskStateRefType                task_state_ref;
#ifndef __OO_NO_ALARMS__
  AlarmBaseRefType                alarm_base_ref;
  TickRefType                     tick_ref;
#endif /* !__OO_NO_ALARMS__ */
#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
  EventMaskRefType                event_ref;
#endif /* __OO_ECC1__ || __OO_ECC2__ */
#ifdef EE_AS_USER_SPINLOCKS__
  TryToGetSpinlockType *          try_to_get_spinlock_ref;
#endif /* EE_AS_USER_SPINLOCKS__ */
#ifdef EE_AS_SCHEDULETABLES__
  ScheduleTableStatusRefType      schedule_table_status_ref;
#endif /* EE_AS_SCHEDULETABLES__ */
#ifdef EE_AS_OSAPPLICATIONS__
#if defined(EE_SYSCALL_NR) && defined(EE_MAX_SYS_SERVICEID) &&\
  (EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID)
  TrustedFunctionParameterRefType trusted_function_parameter_ref;
#endif /* EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID */
  ApplicationStateRefType         application_state_ref;
#ifdef __EE_MEMORY_PROTECTION__
  MemoryStartAddressType          memory_address;
#endif /* __EE_MEMORY_PROTECTION__ */
#endif /* EE_AS_OSAPPLICATIONS__ */
#ifdef __OO_SEM__
  SemRefType                      sem_ref;
#endif /* __OO_SEM__ */
} EE_os_param;

/* 13.8.3 Constants                                                        */
/* ----------------------------------------------------------------------- */

/* To be used as place holder when an API doesn't have a parameter */
static const EE_os_param EE_os_invalid_param = { EE_UREG_MINUS1 };

#define EE_OS_INVALID_PARAM  (EE_os_invalid_param)

/* unique identifier of system service xx */
#define OSServiceId_ActivateTask     1U
#define OSServiceId_TerminateTask    2U
#define OSServiceId_ChainTask        3U
#define OSServiceId_Schedule         4U
#define OSServiceId_GetTaskID        5U
#define OSServiceId_GetTaskState     6U
/* DisableAllInterrupts, EnableAllInterrupts, SuspendAllInterrupts,
   ResumeAllInterrupts, SuspendOSInterrupts, ResumeOSInterrupts never
   return an error */
#define OSServiceId_GetResource       7U
#define OSServiceId_ReleaseResource   8U
#define OSServiceId_SetEvent          9U
#define OSServiceId_ClearEvent        10U
#define OSServiceId_GetEvent          11U
#define OSServiceId_WaitEvent         12U
#define OSServiceId_GetAlarmBase      13U
#define OSServiceId_GetAlarm          14U
#define OSServiceId_SetRelAlarm       15U
#define OSServiceId_SetAbsAlarm       16U
#define OSServiceId_CancelAlarm       17U
#define OSServiceId_IncrementCounter  18U
#define OSServiceId_GetCounterValue   19U
#define OSServiceId_GetElapsedValue   20U

/* GetActiveApplicationMode, ShutdownOS never return an error */
#define OSServiceId_StartOS           21U
#define OSServiceId_ForceSchedule     22U

/* Special value to flag an error happened in the Task body
   needed for AS requirement [OS069] */
#define OSId_TaskBody                 23U
/* Special value to flag an error happened in the ISR2 body
   needed for AS requirement [OS368] */
#define OSId_ISR2Body                 24U
/* Special value to flag an error happened in a Alarm or Schedule Table
   action */
#define OSId_Action                   25U
/* Special value to flag an error happened in a Kernel only callable service
   (like EE_oo_IncrementCounterHardware) */
#define OSId_Kernel                   26U

#ifdef __OO_SEM__
/* InitSem, TryWaitSem, GetValueSem never return an error */
#define OSServiceId_GetValueSem       27U
#define OSServiceId_WaitSem           28U
#define OSServiceId_PostSem           29U
#define OSServiceId_TryWaitSem        30U
/* Flag the first id free for new services */
#define OSId_OO_Services_End          30U
#else /* __OO_SEM__ */
/* Flag the first id free for new services */
#define OSId_OO_Services_End          27U
#endif /* __OO_SEM__ */

/* 13.8.4 Macros                                                           */
/* ----------------------------------------------------------------------- */

/* The following type MUST be visible always, because the "New Error Handling"
   implementation */
/** @var structure that hold the service that caused an Error parameter value */
typedef struct EE_oo_ErrorHook_parameters_type {
  EE_os_param param1;
  EE_os_param param2;
  EE_os_param param3;
} EE_oo_ErrorHook_parameters;

#if defined(__OO_HAS_ERRORHOOK__) && (!defined(__OO_ERRORHOOK_NOMACROS__))

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA OS section: ee_kernel_bss */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* Used to flag if we are already in a ErrorHook */
extern EE_TYPEBOOL                EE_ErrorHook_nested_flag;

#ifdef EE_SUPPORT_MEMMAP_H
/* Stop ERIKA API */
#define OS_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifndef __MSRP__
/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA OS section: ee_kernel_bss */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

extern OSServiceIdType            EE_oo_ErrorHook_ServiceID;
extern EE_oo_ErrorHook_parameters EE_oo_ErrorHook_data;

#define EE_oo_get_errorhook_service_id()  (&EE_oo_ErrorHook_ServiceID)
#define EE_oo_get_errorhook_data()        (&EE_oo_ErrorHook_data)

#ifdef EE_SUPPORT_MEMMAP_H
/* Stop ERIKA API */
#define OS_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#else /* !__MSRP__ */

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

extern OSServiceIdType  EE_SHARED_UDATA EE_oo_ErrorHook_ServiceID[EE_MAX_CPU];
extern EE_oo_ErrorHook_parameters EE_SHARED_UDATA
    EE_oo_ErrorHook_data[EE_MAX_CPU];
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#define EE_oo_get_errorhook_service_id()  \
  (&EE_oo_ErrorHook_ServiceID[ EE_hal_get_core_id() ])
#define EE_oo_get_errorhook_data()        \
  (&EE_oo_ErrorHook_data[ EE_hal_get_core_id() ])
#endif /* !__MSRP__ */

#endif /* __OO_ERRORHOOK_NOMACROS__ && !__OO_HAS_ERRORHOOK__ */

#endif /* __INCLUDE_OO_COMMON_H__ */
