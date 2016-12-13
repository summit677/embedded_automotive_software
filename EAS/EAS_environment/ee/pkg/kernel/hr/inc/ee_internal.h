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

#include "ee_common.h"

#ifndef __INCLUDE_HR_INTERNAL_H__
#define __INCLUDE_HR_INTERNAL_H__


/*************************************************************************
 Internal data structures
 *************************************************************************/

/* this array is defined in a separate file at the same level of
   eecfg.c because it stores the contract names in string format. If
   there are no functions called which needs this variable (this is
   usually the case) then the variable is not linked to the final
   executable.
*/

extern const char *EE_hr_contract_label[EE_MAX_CONTRACT];


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

#ifndef __PRIVATE_RQ_QUERYFIRST_VRES__
/* return the first vres in ready queue without extracting it */
__INLINE__ EE_TID __ALWAYS_INLINE__ EE_rq_queryfirst_vres(void)
{
  return EE_rqfirst_vres;  
}
#endif

#ifndef __PRIVATE_STK_QUERYFIRST__
/* return the first stacked task (the running task) without extracting it */
__INLINE__ EE_TID __ALWAYS_INLINE__ EE_stk_queryfirst(void)
{
  return EE_stkfirst;  
}
#endif

#ifndef __PRIVATE_LOC_STK_QUERYFIRST__
/* return the first local stacked task of one vres without extracting it */
__INLINE__ EE_TID __ALWAYS_INLINE__ EE_loc_stk_queryfirst(EE_TYPECONTRACT vres)
{
  return EE_vres[vres].stkfirst;  
}
#endif

#ifndef __PRIVATE_RQ_GETFIRST_VRES__
__INLINE__ void __ALWAYS_INLINE__ EE_rq_getfirst_vres(void)
{
  EE_vres[EE_rqfirst_vres].status2 &= ~EE_VRES_INRQ; // inRQ=0
  EE_rqfirst_vres = EE_vres[EE_rqfirst_vres].next;
  if(EE_rqfirst_vres!=EE_NIL) {
    EE_vres[EE_rqfirst_vres].prev = EE_NIL;
	EE_rqfirst = EE_vres[EE_rqfirst_vres].task;
  }
  else EE_rqfirst = EE_NIL;

}
#endif

#ifndef __PRIVATE_RQ_GETFIRST__
__INLINE__ void __ALWAYS_INLINE__ EE_rq_getfirst(void)
{
  EE_rqfirst = EE_th[EE_rqfirst].next;
  EE_vres[EE_rqfirst_vres].task = EE_rqfirst;
  //if(EE_rqfirst==EE_NIL && EE_vres[EE_rqfirst_vres].stkfirst==EE_NIL) EE_rq_getfirst_vres();
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

#ifndef __PRIVATE_LOC_STK_GETFIRST__
/* extract the running task from the local stack */
__INLINE__ void __ALWAYS_INLINE__ EE_loc_stk_getfirst(EE_TYPECONTRACT vres)
{
  EE_vres[vres].stkfirst = EE_th[EE_vres[vres].stkfirst].next;
}
#endif

#ifndef __PRIVATE_LOC_STK_INSERTFIRST__
/* insert a task into the local stack data structures */
__INLINE__ void __ALWAYS_INLINE__ EE_loc_stk_insertfirst(EE_TID t,EE_TYPECONTRACT vres)
{
  EE_th[t].next = EE_vres[vres].stkfirst;
  EE_vres[vres].stkfirst = t;
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

/* ------------------------------------- */

#ifndef __PRIVATE_RQ_INSERT__
/* insert a task into the ready queue */
void EE_rq_insert(EE_TID t);
#endif

#ifndef __PRIVATE_RQ_INSERT_VRES__
/* insert a vres into the ready queue */
void EE_rq_insert_vres(EE_TYPECONTRACT v);
#endif

#ifndef __PRIVATE_INSERT_IN_VRES__
/* insert a task into its vres's ready queue */
void EE_insert_in_vres(EE_TID t);
#endif

#ifndef __PRIVATE_RQ_EXTRACT__
/* extract a task from the ready queue. if not present, do nothing */
void EE_rq_extract(EE_TID t);
#endif

#ifndef __PRIVATE_RQ_EXTRACT_VRES__
/* extract a vres from the ready queue. if not present, do nothing */
void EE_rq_extract_vres(EE_TYPECONTRACT v);
#endif

#ifndef __PRIVATE_RCG_INSERT__
/* insert a task into the recharging queue */
void EE_rcg_insert(EE_TYPECONTRACT c);
#endif


#ifndef __PRIVATE_RECHARGEBUDGET__
void EE_hr_rechargebudget(EE_TYPECONTRACT c);
#endif

#ifndef __PRIVATE_UPDATECAPACITY__
/* check the current value of a deadline and updates it following the
   IRIS rules */

typedef enum AT{EE_UC_InsertedRCGQueue, EE_UC_InsertRDQueue, EE_UC_NoVres} ActionType;

ActionType EE_hr_updatecapacity(EE_TID t, EE_TIME tmp_time);
#endif

#ifndef __PRIVATE_LOCAL_SELECTEXEC__
void EE_hr_local_select_exec(void);
#endif

#ifndef __PRIVATE_SELECTEXEC__
void EE_hr_select_exec(void);
#endif

#ifndef __PRIVATE_RUNEXEC__
void EE_hr_run_exec(EE_TID tmp_exec);
#endif

#ifndef __PRIVATE_CHECKSLICE__
void EE_hr_check_slice(EE_TIME tmp_time);
#endif

#ifndef __PRIVATE_ENDSLICE__
void EE_hr_end_slice(EE_TIME tmp_time);
#endif

#ifndef __PRIVATE_PROCESSRECHARGING__
int EE_hr_process_recharging(EE_TYPECONTRACT c);
#endif

#ifndef __PRIVATE_CHECKRECHARGING__
void EE_hr_check_recharging(EE_TIME tmp_time);
#endif

#ifndef __PRIVATE_IRQ_RECHARGING__
void EE_hr_IRQ_recharging(void);
#endif

#ifndef __PRIVATE_IRQ_BUDGET__
void EE_hr_IRQ_budget(void);
#endif

#ifndef __PRIVATE_IRQ_DLCHECK__
void EE_hr_IRQ_dlcheck(void);
#endif

#ifdef __HR_SINGLEIRQ__
void EE_hr_timer_reprogram(void);
void EE_hr_timer_set(int timer, EE_STIME t);
void EE_hr_timer_stop(int timer);
void EE_hr_IRQ_timer_multiplexer(void);
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
 Primitives that have to be called into an IRQ
 *************************************************************************/

#ifndef __PRIVATE_IRQ_END_INSTANCE__
/* This primitive shall be atomic.
   This primitive shall be inserted as the last function in an IRQ handler.
   If the HAL allow IRQ nesting the end_instance should work as follows:
   - it must implement the preemption test only if it is the last IRQ on the stack
   - if there are other interrupts on the stack the IRQ end_instance should do nothing
*/
void EE_IRQ_end_instance(void);
#endif




/*************************************************************************
 Timers
 *************************************************************************/

#ifdef __HR_SINGLEIRQ__
// only the budget timer is available

// these two functions are used to set and stop the budget timer,
// which is the only available when SINGLEIRQ is specified
// the timer parameter is used to understand which interrupt source is called
// and it is statically defined in ee_hr_timers.c

// TODO: we could probably do some more work to make this piece of code more
// general, exporting it as a service independent from HR

void EE_hr_timer_set(int timer, EE_STIME t);
void EE_hr_timer_stop(int timer);


__INLINE__ void __ALWAYS_INLINE__ EE_hr_set_budget_timer(EE_STIME t) 
{
  EE_hr_timer_set(0, t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_hr_stop_budget_timer(void)
{
  EE_hr_timer_stop(0);
}

__INLINE__ void __ALWAYS_INLINE__ EE_hr_set_recharging_timer(EE_STIME t)
{
  EE_hr_timer_set(1, t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_hr_stop_recharging_timer(void)
{
  EE_hr_timer_stop(1);
}

__INLINE__ void __ALWAYS_INLINE__ EE_hr_set_dline_timer(EE_STIME t)
{
  EE_hr_timer_set(2, t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_hr_stop_dline_timer(void)
{
  EE_hr_timer_stop(2);
}


#else

// Three different hardware timers available
__INLINE__ void __ALWAYS_INLINE__ EE_hr_set_budget_timer(EE_STIME t) 
{
  EE_hal_set_budget_timer(t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_hr_stop_budget_timer(void)
{
  EE_hal_stop_budget_timer();
}

__INLINE__ void __ALWAYS_INLINE__ EE_hr_set_recharging_timer(EE_STIME t)
{
  EE_hal_set_recharging_timer(t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_hr_stop_recharging_timer(void)
{
  EE_hal_stop_recharging_timer();
}

__INLINE__ void __ALWAYS_INLINE__ EE_hr_set_dline_timer(EE_STIME t)
{
  EE_hal_set_dline_timer(t);
}

__INLINE__ void __ALWAYS_INLINE__ EE_hr_stop_dline_timer(void)
{
  EE_hal_stop_dline_timer();
}

#endif

/*************************************************************************
 Bandwidth operations
 *************************************************************************/
/*
*
*	This function computes the multiplication budget_avail*(1/alpha), where
*   alpha is the bandwidth of a reservation. (1/alpha) is  a floating
*	point value and is stored in the contract struct as follows:
*   	inv_proc_util = ceil((1/alpha)*(2^25))
*		EE_ct[c].inv_proc_util_top = (inv_proc_util>>16) 
*		EE_ct[c].inv_proc_util_bot = (inv_proc_util&0x0000FFFF)
*
*	In order to avoid constraints on the maximum budget for a reservation 
*	server, the quantity budget_avail*(1/alpha) is calculated using a 
*	result on 64 bits.
*/
 
__INLINE__ EE_TIME __ALWAYS_INLINE__ EE_hr_budget_times_inv_proc_util(EE_TYPECONTRACT c)
{
	register EE_UINT32 budget_top = (EE_vres[c].budget_avail>>16);
	register EE_UINT32 budget_bot = (EE_vres[c].budget_avail&0x0000FFFF);
	register EE_UINT32 res_M, res_H, res_L;
	
	res_M = (budget_top*EE_ct[c].inv_proc_util_bot + EE_ct[c].inv_proc_util_top*budget_bot);
	res_H = budget_top*EE_ct[c].inv_proc_util_top + (res_M>>16);
	res_L = ((res_M<<16)+ budget_bot*EE_ct[c].inv_proc_util_bot);

	res_H = res_H << 7; 
	res_L = res_L >> 25;
	res_H |= res_L;
	
	return res_H;
}

#endif
