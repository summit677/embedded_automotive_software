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
 * Author: 2008 Paolo Tiberi & Francesco Focacci
 * CVS: $Id: ee_common.h,v 1.6 2008/07/18 09:53:55 tiberipa Exp $
 */

#ifndef __INCLUDE_FRSH_COMMON_H__
#define __INCLUDE_FRSH_COMMON_H__


/*************************************************************************
 Note
 *************************************************************************

Some details about the FRSH implementation:

FRSH uses a stacked queue.

The stacked queue contains all the tasks that has been executed for
some time, and that have locked a resource

Note that if a task has been executed it must have been that the task
preemption level was greater than the system ceiling, and that their
absolute deadline was earlier than the task on the top of the stacked
queue.

This also means that the stacked queue is by construction ordered by
increasing deadlines.

When a task is preempted and it has a locked resource, then the task
ends up in the stacked queue, otherwise it ends up into the ready
queue. 

In fact, when activating a task the ActivateTask does a preemption
Check between the running task and the first task in the ready queue,
including the preeemption level.

When a task terminates, we suppose that NO RESOURCES are locked. If
the task has a nact>0 the task is inserted in the ready queue (there
are no resources locked), and the next running task is chosen between
the ready and the stacked queue. The task on the ready queue wins if
it has a deadline earlier than the task on the stacked queue and if it
has a preemption level greater than the system ceiling (the system
ceiling subsumes in a single value all the ceilings of the locked
resources by the tasks in the stacked queue)

If a task to execute is chosen from the stacked queue, then the task
is removed from the stacked queue and it is put into exec.
*/

/*
Please note the following difference between (FP, EDF) and FRSH.  FRSH
does a distinction between exec and stkfirst. the rationale is that
exec may not influence the system ceiling (for example, the running
task did not lock any resource). The stacked queue maintains the
meaning of containing all the tasks that modified the system ceiling
(that is, that locked a resource).

There is a need to have a separate stacked queue because if the top 
ready task fails the preemption test at the end of a task, then the 
running task must become the highest priority task (the one with the
earliest deadline) among those who modified the system ceiling. 

Finally note that since FP and EDF implements preemption thresholds, 
all the tasks at the end make a modification to the system ceiling, 
and then the exec task can coincide without problems to the first 
stacked task.
*/

/*
Notes on thread and VRES status and parameters
----------------------------------------------
The kernel makes a distinction between the tasks and VRES

Tasks are the executing entities. All queues in the system queue the
tasks as executing entities.

Tasks have allocated a VRES. the VRES is the container which controls
when a task can be executed. the parameters of a VRES are:
- period
- budget
- absolute deadline
- active
- status

Please note that the VRES status and the task status are distinct
- the VRES status maps the capability of a VRES to execute
- the task status maps the status of the task

Example: a task may block on a semaphore for an indefinite time. while
the status becomes BLOCKED, the VRES status is initially Active, then
after a while it will become Inactive, and after a long time it will
be set to freezed.
*/

/*
Notes on the freezed state
--------------------------
The FRSH Kernel is using a timer representation embedded in a timer
value. All times in the system are relative to the value of the 
current timer.

When a task stop executing for some time, its deadline is no more 
updated, and it will tend to go far in the past. Since the time 
reference is relative to the current timer value, it may happen 
after some time (half the lifetime of the timer), that this deadline 
in the past becomes a deadline "in the future". At that point, all 
the deadline update strategies will fail.

To avoid that, the system have to periodically "take a look" at 
all the task deadlines to see if they go too much in the past 
(if a deadline goes in the past it measn the task has not executed 
for a while). If that happens, then the function 
EE_frsh_deadlinecheck will set these VRES to "freezed", and then
when the task wuill be activated again a fresh deadline will be 
assigned (done into EE_frsh_updatecapacity.
-- end note */



/*************************************************************************
 Kernel Constants
 *************************************************************************/

/* invalid TID */
#define EE_NIL            ((EE_TID)-1)

/* invalid TID, used in the VRES task field, to mean that the VRES has
   been binded BUT the binding has been deferred. Note that only VRES
   which are inactive/freezed may have this value */
#define EE_TID_DEFERRED   ((EE_TID)EE_MAX_TASK)

/* invalid VRES, used for tasks without vres binded to them*/
#define EE_VRES_NIL       ((EE_TYPECONTRACT)-1)

/* placeholder to say that the deferred VRES attached to a task has
 * been unbound when a task is safely unbound, its vres is
 * EE_VRES_NIL. We need this value when deferring the bind at the end
 * of a critical section. This enable to distinguish when there is
 * nothing to do (EE_VRES_NIL), the VRES have to be unbound
 * (EE_VRES_UNBOUND), or if there is a new VRES (one of the valid
 * values)
 */
#define EE_VRES_UNBOUND   ((EE_TYPECONTRACT)EE_MAX_CONTRACT)


/* VRES statuses */

#define EE_VRES_FREEZED    0
#define EE_VRES_INACTIVE   1
#define EE_VRES_ACTIVE     2
#define EE_VRES_RECHARGING 4


/* TASK statuses */

#define EE_TASK_SUSPENDED  0
#define EE_TASK_READY      1
#define EE_TASK_STACKED    2
#define EE_TASK_BLOCKED    4
#define EE_TASK_EXEC       8

/* flag used to know if a task has some space allocated on its stack */
/* WHEN CHANGING THIS WE WILL HAVE TO CHANGE THE ORTI FILE! */
#define EE_TASK_WASSTACKED 128

/*************************************************************************
 Kernel Types
 *************************************************************************/

/* priority type, used for system ceiling and preemption level */
#ifndef EE_TYPEPRIO
#define EE_TYPEPRIO EE_UREG
#endif

/* status type */
#ifndef EE_TYPESTATUS
#define EE_TYPESTATUS EE_UREG
#endif

/* relative deadline type */
#ifndef EE_TYPERELDLINE
#define EE_TYPERELDLINE EE_TIME
#endif

/* absolute deadline type */
#ifndef EE_TYPEABSDLINE
#define EE_TYPEABSDLINE EE_TIME
#endif

/* capacity type */
#ifndef EE_TYPEBUDGET
#define EE_TYPEBUDGET EE_STIME
#endif

/* pending activation type */
#ifndef EE_TYPENACT
#define EE_TYPENACT EE_UREG
#endif

/* mutex ID type */
#ifndef EE_TYPERESOURCE
#define EE_TYPERESOURCE EE_UREG
#endif

/* contract ID */
#ifndef EE_TYPECONTRACT
#define EE_TYPECONTRACT EE_SREG
#endif

/* vres ID */
#ifndef EE_TYPEVRES
#define EE_TYPEVRES EE_UREG
#endif

/* Contract */
typedef struct {
  EE_TYPEBUDGET   budget;
  EE_TYPERELDLINE period;
  EE_TYPERELDLINE inv_proc_util;
} EE_TYPECONTRACTSTRUCT;

/* VRES */
typedef struct {
  EE_TYPEBUDGET   budget_avail;  /* available budget (initvalue 0) */
  EE_TYPEBUDGET   usage;         /* overall incremental budget used by the vres (initvalue 0) */
  EE_TYPEABSDLINE absdline;      /* absolute deadline (initvalue 0) */
  EE_TYPESTATUS   status;        /* status (initvalue freezing that is 0) */
  EE_TID          task;          /* the task binded to the VRES */
  EE_TYPECONTRACT next;          /* next vres in the recharging queue (initvalue EE_VRES_NIL ) */
} EE_TYPEVRESSTRUCT;

/* TASK */
typedef struct {
  EE_TYPERELDLINE prlevel;       /* task preemption level */
  EE_TYPESTATUS   status;        /* task status (initvalue EE_TASK_SUSPENDED ) */
  EE_TID          next;          /* next task in the queue (initvalue EE_NIL ) */
  EE_TYPENACT     nact;          /* number of pending activations (initvalue 0) */
  EE_UREG         lockedcounter; /* number of locked resources (initvalue 0) */
  EE_TYPECONTRACT vres;          /* the vres linked to the task */
  EE_TYPECONTRACT vres_deferred; /* when != EE_VRES_NIL stores the deferred VRES after a bind */
  EE_UREG         timedout;      /* used with synchronization objects */
} EE_TYPETASKSTRUCT;

typedef EE_UREG bool;

/*************************************************************************
 Kernel Variables
 *************************************************************************/


/*************
 * CONTRACTS
 *************/

/* This structure contains the statically defined contractd defined into the OIL File */
extern const EE_TYPECONTRACTSTRUCT EE_ct[];


/*************
 * VRES
 *************/

/* This structure contains all the VRES data. (initvalue 0) */
extern EE_TYPEVRESSTRUCT EE_vres[EE_MAX_CONTRACT];


/*************
 * TASKS
 *************/

/* task data */
extern EE_TYPETASKSTRUCT EE_th[EE_MAX_TASK];


/*************
 * GLOBAL
 *************/

/* a temporary value that stores the timer read that was done before
   executing a task. That value is useful for capacity accounting 
   (initvalue: none)
*/
extern EE_TIME EE_last_time;

/* system ceiling (initvalue 0) */
extern EE_TYPEPRIO EE_sys_ceiling;


/* The first task into the ready queue -> ready means that the task
   has been activated without never running or that it has been
   preempted by another task, in both cases without currently locking
   any resource 
   (initvalue: EE_NIL)
*/
extern EE_TID EE_rqfirst;

/* The first stacked task -> stacked means a preempted task that owns a mutex 
   (initvalue: EE_NIL)
*/
extern EE_TID EE_stkfirst;

/* The first vres in the recharging queue
*/
extern EE_TYPECONTRACT EE_rcgfirst;


/* The running task 
   (initvalue: EE_NIL)
*/
extern EE_TID EE_exec;


/*************
 * MUTEXES
 *************/

extern const EE_TYPEPRIO EE_resource_ceiling[EE_MAX_RESOURCE];

extern EE_TYPEPRIO       EE_resource_oldceiling[EE_MAX_RESOURCE];


/*************
 * SYNCHOBJs
 *************/

/* The synchobj descriptor */
typedef struct {
  int count;
  EE_TID    first;
  EE_TID    last;
} SynchObjType;

typedef SynchObjType *SynchObjRefType;
typedef SynchObjRefType frsh_synchobj_handle_t;

#define SYNCHOBJINIT { 0, EE_NIL, EE_NIL }

#ifdef __FRSH_SYNCHOBJ__
/* Timeouts for synchronization objects */
typedef struct {
  int flag;                        /* timeout flag */
  EE_TYPEABSDLINE timeout;         /* absolute timeout */
  EE_TID next;                     /* used to queue the timeout structure */
  frsh_synchobj_handle_t synchobj; /* the synchobj the task is waiting on, 
				      != 0 if the task is queued on a synchobj
				      == 0 if I'm not waiting on a wait with timeout. 
				      reset to 0 when the timeout fires or by the signal */
} EE_TYPETIMEOUTSTRUCT;

// initvalue 0
extern EE_TYPETIMEOUTSTRUCT EE_frsh_timeout[];

// initvalue: EE_NIL
extern EE_TID EE_frsh_timeout_first;
#endif

#endif

