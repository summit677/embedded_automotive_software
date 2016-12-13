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
 * Author:2003 Paolo Gai
 * CVS: $Id: ee_internal.h,v 1.3 2008/07/16 09:46:12 francesco Exp $
 */

#include "ee_common.h"
#include "ee_irq.h"

#ifndef __INCLUDE_FRSH_INTERNAL_H__
#define __INCLUDE_FRSH_INTERNAL_H__


/*************************************************************************
 Internal data structures
 *************************************************************************/

/* this array is defined in a separate file at the same level of
   eecfg.c because it stores the contract names in string format. If
   there are no functions called which needs this variable (this is
   usually the case) then the variable is not linked to the final
   executable.
*/

extern const char *EE_frsh_contract_label[EE_MAX_CONTRACT];


/*************************************************************************
 Internal Queue management functions
 *************************************************************************/


#ifndef __PRIVATE_RQ_QUERYFIRST__
/* return the first ready task without extracting it */
__INLINE__ EE_TID __ALWAYS_INLINE__ EE_rq_queryfirst(void)
{
  return EE_rqfirst;  
}
#endif

#ifndef __PRIVATE_STK_QUERYFIRST__
/* return the first stacked task (the running task) without extracting it */
__INLINE__ EE_TID __ALWAYS_INLINE__ EE_stk_queryfirst(void)
{
  return EE_stkfirst;  
}
#endif

#ifndef __PRIVATE_RQ_GETFIRST__
__INLINE__ void __ALWAYS_INLINE__ EE_rq_getfirst(void)
{
  EE_rqfirst = EE_th[EE_rqfirst].next;
}
#endif

#ifndef __PRIVATE_STK_GETFIRST__
/* extract the running task from the stack */
__INLINE__ void __ALWAYS_INLINE__ EE_stk_getfirst(void)
{
  EE_stkfirst = EE_th[EE_stkfirst].next;
}
#endif

#ifndef __PRIVATE_STK_INSERTFIRST__
/* insert a task into the stack  data structures */
__INLINE__ void __ALWAYS_INLINE__ EE_stk_insertfirst(EE_TID t)
{
  EE_th[t].next = EE_stkfirst;
  EE_stkfirst = t;
}
#endif

#ifndef __PRIVATE_RCG_QUERYFIRST__
/* return the first recharging task (the running task) without extracting it */
__INLINE__ EE_TYPECONTRACT __ALWAYS_INLINE__ EE_rcg_queryfirst(void)
{
  return EE_rcgfirst;  
}
#endif

#ifndef __PRIVATE_RCG_GETFIRST__
__INLINE__ void __ALWAYS_INLINE__ EE_rcg_getfirst(void)
{
  EE_rcgfirst = EE_vres[EE_rcgfirst].next;
}
#endif

#ifndef __PRIVATE_RQ_INSERT__
/* insert a task into the ready queue */
void EE_rq_insert(EE_TID t);
#endif

#ifndef __PRIVATE_RQ_EXTRACT__
/* extract a task from the ready queue. if not present, do nothing */
void EE_rq_extract(EE_TID t);
#endif

#ifndef __PRIVATE_RCG_INSERT__
/* insert a task into the recharging queue */
void EE_rcg_insert(EE_TYPECONTRACT c);
#endif


#ifndef __PRIVATE_RECHARGEBUDGET__
void EE_frsh_rechargebudget(EE_TID t);
#endif

#ifndef __PRIVATE_UPDATECAPACITY__
/* check the current value of a deadline and updates it following the
   IRIS rules */

typedef enum AT{EE_UC_InsertedRCGQueue, EE_UC_InsertRDQueue, EE_UC_NoVres} ActionType;

ActionType EE_frsh_updatecapacity(EE_TID t, EE_TIME tmp_time);
#endif

#ifndef __PRIVATE_SELECTEXEC__
void EE_frsh_select_exec(void);
#endif

#ifndef __PRIVATE_RUNEXEC__
void EE_frsh_run_exec(EE_TID tmp_exec);
#endif

#ifndef __PRIVATE_CHECKSLICE__
void EE_frsh_check_slice(EE_TIME tmp_time);
#endif

#ifndef __PRIVATE_ENDSLICE__
void EE_frsh_end_slice(EE_TIME tmp_time);
#endif

#ifndef __PRIVATE_PROCESSRECHARGING__
int EE_frsh_process_recharging(EE_TYPECONTRACT c);
#endif

#ifndef __PRIVATE_CHECKRECHARGING__
void EE_frsh_check_recharging(EE_TIME tmp_time);
#endif

#ifndef __PRIVATE_IRQ_RECHARGING__
void EE_frsh_IRQ_recharging(void);
#endif

#ifndef __PRIVATE_IRQ_BUDGET__
void EE_frsh_IRQ_budget(void);
#endif

#ifndef __PRIVATE_IRQ_DLCHECK__
void EE_frsh_IRQ_dlcheck(void);
#endif

#ifndef __PRIVATE_IRQ_SYNCHOBJ_TIMEOUT__
void EE_frsh_IRQ_synchobj_timeout(void);
#endif

#ifdef __FRSH_SINGLEIRQ__
void EE_frsh_timer_reprogram(void);
void EE_frsh_timer_set(int timer, EE_STIME t);
void EE_frsh_timer_stop(int timer);
void EE_frsh_IRQ_timer_multiplexer(void);
#endif

#ifndef __PRIVATE_BIND_DETACH_VRES__
int EE_frsh_bind_detach_thread(EE_TID thread);
#endif

/*************************************************************************
 Primitives
 *************************************************************************/

#ifndef __PRIVATE_THREAD_END_INSTANCE__
/* This call terminates a thread instance. It must be called as the
   LAST function call BEFORE the `}' that ends a thread. If the
   primitive is not inserted at the end of */
void EE_thread_end_instance(void);
#endif

/*************************************************************************
 Timers
 *************************************************************************/

#ifdef __FRSH_SINGLEIRQ__
// only the budget timer is available

// these two functions are used to set and stop the budget timer,
// which is the only available when SINGLEIRQ is specified
// the timer parameter is used to understand which interrupt source is called
// and it is statically defined in ee_frsh_timers.c

// TODO: we could probably do some more work to make this piece of code more
// general, exporting it as a service independent from FRSH

void EE_frsh_timer_set(int timer, EE_STIME t);
void EE_frsh_timer_stop(int timer);




__INLINE__ void __ALWAYS_INLINE__ EE_frsh_set_budget_timer(EE_STIME t) 
{
  EE_frsh_timer_set(0, t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_stop_budget_timer(void)
{
  EE_frsh_timer_stop(0);
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_set_recharging_timer(EE_STIME t)
{
  EE_frsh_timer_set(1, t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_stop_recharging_timer(void)
{
  EE_frsh_timer_stop(1);
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_set_synchobj_timeout_timer(EE_STIME t)
{
  EE_frsh_timer_set(2, t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_stop_synchobj_timeout_timer(void)
{
  EE_frsh_timer_stop(2);
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_set_dline_timer(EE_STIME t)
{
  EE_frsh_timer_set(3, t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_stop_dline_timer(void)
{
  EE_frsh_timer_stop(3);
}


#else

// four different hardware timers available
__INLINE__ void __ALWAYS_INLINE__ EE_frsh_set_budget_timer(EE_STIME t) 
{
  EE_hal_set_budget_timer(t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_stop_budget_timer(void)
{
  EE_hal_stop_budget_timer();
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_set_recharging_timer(EE_STIME t)
{
  EE_hal_set_recharging_timer(t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_stop_recharging_timer(void)
{
  EE_hal_stop_recharging_timer();
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_set_synchobj_timeout_timer(EE_STIME t)
{
  EE_hal_set_synchobj_timeout_timer(t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_stop_synchobj_timeout_timer(void)
{
  EE_hal_stop_synchobj_timeout_timer();
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_set_dline_timer(EE_STIME t)
{
  EE_hal_set_dline_timer(t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_frsh_stop_dline_timer(void)
{
  EE_hal_stop_dline_timer();
}

#endif


#endif
