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

/*
 * Author: 2013 Alessandro Biondi
 *
 */

#ifndef __INCLUDE_HR_COMMON_H__
#define __INCLUDE_HR_COMMON_H__

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


/* Additional VRES statuses */

#define EE_VRES_INRQ       1
#define EE_VRES_ALTCEIL    2


/* TASK statuses */

#define EE_TASK_SUSPENDED  0
#define EE_TASK_READY      1
#define EE_TASK_STACKED    2
#define EE_TASK_BLOCKED    4
#define EE_TASK_EXEC       8

/* SCHED ALGO types */

#define EE_SCHED_EDF       0
#define EE_SCHED_FP        1


/* flag used to know if a task has some space allocated on its stack */
/* WHEN CHANGING THIS WE WILL HAVE TO CHANGE THE ORTI FILE! */
#define EE_TASK_WASSTACKED 128

#ifndef EE_SYSTEM_MUTEX
/* the 2nd MSB of MUTEX type indicates if a resource is a system resource (1) 
   local to a processor or a resource local to a vres (0) */
#define EE_SYSTEM_MUTEX 0x40000000 
#endif

#ifndef EE_MUTEX_MASK
/* mask useful for remove the flag that indicates the type of resource (global, system, local) */
#define EE_MUTEX_MASK 0xC0000000
#endif

#ifndef EE_MAX_PRIO
#define EE_MAX_PRIO 0xFF
#endif

#ifndef EE_NULL_WCRHT
#define EE_NULL_WCRHT 0
#endif

#ifndef EE_ZERO_BUDGET
#define EE_ZERO_BUDGET 0
#endif


/*************************************************************************
 Kernel Types
 *************************************************************************/

/* priority type, used for system ceiling and preemption level */
#ifndef EE_TYPEPRIO
#define EE_TYPEPRIO EE_UREG
#endif

/* scheduler algo type */
#ifndef EE_TYPESCHED
#define EE_TYPESCHED EE_UREG
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

/* system resource local ceiling ID */
#ifndef EE_TYPESYSRESLOCCEIL
#define EE_TYPESYSRESLOCCEIL EE_UREG
#endif

/* Contract */
typedef struct {
  EE_TYPEBUDGET   budget;
  EE_TYPERELDLINE period;
  EE_UINT16 inv_proc_util_top;
  EE_UINT16 inv_proc_util_bot;
  EE_TYPESCHED    sched_algo;
} EE_TYPECONTRACTSTRUCT;

/* VRES */
typedef struct {
  EE_TYPEBUDGET   budget_avail;    /* available budget (initvalue 0) */
  EE_TYPEBUDGET   usage;           /* overall incremental budget used by the vres (initvalue 0) */
  EE_TYPEABSDLINE absdline;        /* absolute deadline (initvalue 0) */
  EE_TYPESTATUS   status;          /* status (initvalue freezing that is 0) */
  EE_TYPESTATUS   status2;         /* additional non-exclusive status information */
  EE_TID          task;            /* head of VRES's ready queue */
  EE_TYPECONTRACT prev;            /* previous vres in the ready queue (initvalue EE_VRES_NIL ) */
  EE_TYPECONTRACT next;            /* next vres in the recharging or ready queue (initvalue EE_VRES_NIL ) */
  EE_UREG         act_tasks;       /* number of activated tasks (initvalue 0) */ 
  EE_TID          stkfirst;        /* head of local stacked queue */
  EE_TYPEPRIO     sys_ceiling;     /* local system ceiling */
  EE_TYPEPRIO     old_sys_ceiling; /* old local system ceiling */
  EE_TYPERELDLINE prlevel;         /* server preemption level */
} EE_TYPEVRESSTRUCT;

/* TASK */
typedef struct {
  EE_TYPERELDLINE prlevel;           /* task preemption level */
  EE_TYPERELDLINE reldline;          /* task relative deadline (EE_SCHED_EDF) or priority (EE_SCHED_FP)*/
  EE_TYPEABSDLINE absdline;          /* absolute deadline (initvalue 0)*/
  EE_TYPESTATUS   status;            /* task status (initvalue EE_TASK_SUSPENDED ) */
  EE_TID          next;              /* next task in the queue (initvalue EE_NIL ) */
  EE_TYPENACT     nact;              /* number of pending activations (initvalue 0) */
  EE_UREG         lockedcounter;     /* number of locked resources (initvalue 0) */
  EE_UREG         sys_lockedcounter; /* number of locked system resources (initvalue 0) */
  EE_TYPECONTRACT vres;              /* the vres linked to the task */
  EE_TYPEBUDGET	  wc_rht;			 /* maximum resource holding time for critical sections in the task */
} EE_TYPETASKSTRUCT;

typedef EE_UREG bool;

/*************************************************************************
 Kernel Variables
 *************************************************************************/


/*************
 * CONTRACTS
 *************/

/* This structure contains the statically defined contract defined into the OIL File */
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

/* The first vres into the ready queue 
   (initvalue: EE_NIL)
*/
extern EE_TYPECONTRACT EE_rqfirst_vres;

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

extern const EE_TYPEPRIO EE_local_resource_ceiling[EE_MAX_LOCAL_RESOURCE];

extern EE_TYPEPRIO EE_local_resource_oldceiling[EE_MAX_LOCAL_RESOURCE];

extern const EE_TYPEPRIO EE_sys_resource_ceiling[EE_MAX_SYS_RESOURCE];

extern EE_TYPEPRIO EE_sys_resource_oldceiling[EE_MAX_SYS_RESOURCE];

#endif

