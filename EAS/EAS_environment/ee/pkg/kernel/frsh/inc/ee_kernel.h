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
 * Author: 2003 Paolo Gai
 * CVS: $Id: ee_kernel.h,v 1.4 2008/07/16 09:46:12 francesco Exp $
 */

#include "kernel/frsh/inc/ee_common.h"

#ifndef __INCLUDE_FRSH_KERN_H__
#define __INCLUDE_FRSH_KERN_H__

/*************************************************************************
 Public Kernel Types and Constants
 *************************************************************************/

#define INVALID_TASK EE_NIL

typedef EE_TID TaskType;
typedef EE_TYPERESOURCE ResourceType;
typedef EE_TYPERELDLINE TimeRelType;
typedef EE_TYPEABSDLINE TimeAbsType;


/*************************************************************************
 Kernel Primitives
 *************************************************************************/

/* This macros are used to define a task */
#define DeclareTask(t) void Func##t(void)
#define TASK(t) void Func##t(void)

/*************************************************************************
 System functions
 *************************************************************************/

#ifndef __PRIVATE_SYS_GETTIME__
#ifdef __TIME_SUPPORT__
EE_TIME EE_frsh_sys_gettime(void);
#endif
#endif


/*************************************************************************
 Primitives
 *************************************************************************/




#ifndef __PRIVATE_THREAD_ACTIVATE__
void EE_frsh_thread_activate(EE_TID t);
#endif

#ifndef __PRIVATE_SCHEDULE__
void EE_frsh_Schedule(void);
#endif

#ifndef __PRIVATE_GETRESOURCE__
void EE_frsh_GetResource(EE_TYPERESOURCE m);
#endif

#ifndef __PRIVATE_RELEASERESOURCE__
void EE_frsh_ReleaseResource(EE_TYPERESOURCE m);
#endif



/* FRESCOR API Implementation */

#include "frsh_configuration_parameters.h"
#include "frsh_core_types.h"
#include "frsh_error.h"


/* Basic services */

#ifndef __PRIVATE_FRSH_INIT__
int EE_frsh_init(void);
#endif

#ifndef __PRIVATE_FRSH_STRERROR__
int EE_frsh_strerror(int error, char *message, size_t size);
#endif




/* Contract Creation and initialization */

#ifndef __PRIVATE_FRSH_CONTRACT_GET_BASIC_PARAMS__
int EE_frsh_contract_get_basic_params (const frsh_contract_t *contract,
				       frsh_rel_time_t *budget_min,
				       frsh_rel_time_t *period_max,
				       frsh_workload_t *workload,
				       frsh_contract_type_t *contract_type);
#endif

#ifndef __PRIVATE_FRSH_CONTRACT_GET_RESOURCE_AND_LABEL__
int EE_frsh_contract_get_resource_and_label(const frsh_contract_t *contract,
					    frsh_resource_type_t *resource_type,
					    frsh_resource_id_t *resource_id, 
					    char *contract_label);
#endif

#ifndef __PRIVATE_FRSH_CONTRACT_GET_TIMING_REQS__
int EE_frsh_contract_get_timing_reqs(const frsh_contract_t *contract, 
				     int *d_equals_t,
				     frsh_rel_time_t *deadline, 
				     frsh_signal_t *budget_overrun_signal, 
				     frsh_signal_info_t *budget_overrun_siginfo, 
				     frsh_signal_t *deadline_miss_signal, 
				     frsh_signal_info_t *deadline_miss_siginfo);
#endif






/* Negotiate Contract Functions */

#ifndef __PRIVATE_BINDTASK__
int EE_frsh_BindTask(const frsh_vres_id_t vres, const frsh_thread_id_t thread);
#endif

#ifndef __PRIVATE_UNBINDTASK__
int EE_frsh_UnbindTask(const frsh_thread_id_t thread);
#endif

#ifndef __PRIVATE_GETVRESID__
int EE_frsh_thread_get_vres_id(const frsh_thread_id_t thread, frsh_vres_id_t *vres_id);
#endif

#ifndef __PRIVATE_GETCONTRACT__
int EE_frsh_vres_get_contract (const frsh_vres_id_t vres, frsh_contract_t *contract);
#endif



#ifndef __PRIVATE_FRSH_SYNCOBJ_SIGNAL__
int EE_frsh_synchobj_signal(const frsh_synchobj_handle_t synch_handle);
#endif

#ifndef __PRIVATE_FRSH_SYNCOBJ_WAIT__
int EE_frsh_synchobj_wait (const frsh_synchobj_handle_t synch_handle,
			   frsh_rel_time_t *next_budget,
			   frsh_rel_time_t *next_period,
			   bool *was_deadline_missed, 
			   bool *was_budget_overran);
#endif

#ifndef __PRIVATE_FRSH_SYNCOBJ_WAIT_TIMEOUT__
int EE_frsh_synchobj_wait_with_timeout (const frsh_synchobj_handle_t synch_handle, 
					const frsh_abs_time_t *abs_timeout,
					bool *timed_out,
					frsh_rel_time_t *next_budget,
					frsh_rel_time_t *next_period,
					bool *was_deadline_missed,
					bool *was_budget_overran);
#endif

#ifndef __PRIVATE_FRSH_SYNCOBJ_TIMEDWAIT__
int EE_frsh_timed_wait (const frsh_abs_time_t *abs_time, 
			frsh_rel_time_t *next_budget, 
			frsh_rel_time_t *next_period, 
			bool *was_deadline_missed, 
			bool *was_budget_overran);
#endif


/*
  Returns true if the system is configured with the on-line admission test enabled, or false otherwise. This
  situation can only be changed at compile time.
*/
#ifdef __PRIVATE_FRSH_ISADMISSIONTESTENABLED__
__INLINE__ bool __ALWAYS_INLINE__ EE_frsh_config_is_admission_test_enabled(void)
{
  return 0;
}
#endif

/*
  This function stores in the variable pointed to by budget the
  remaining execution-time budget associated with the specified vres.

  Returns:

  0 if successful
  FRSH_ERR_BAD_ARGUMENT : if the value of the vres argument is not in range or budget is NULL
*/
#ifdef __PRIVATE_FRSH_GETREMAININGBUDGET__
__INLINE__ int__ALWAYS_INLINE__ EE_frsh_vres_get_remaining_budget (const frsh_vres_id_t vres, frsh_rel_time_t *budget)
{  
  register EE_FREG flag;
  
  if (vres<0 || vres >= EE_MAX_CONTRACT || !budget)
    return FRSH_ERR_BAD_ARGUMENT;

  flag = EE_hal_begin_nested_primitive();
  *budget = EE_vres[vres].avail_budget;
  EE_hal_end_nested_primitive(flag);
  return 0;
}
#endif

/*
  This function stores the current execution time spent by the threads
  bound to the specified vres in the variable pointed to by cpu_time.

  Returns:
  0 if successful
  FRSH_ERR_BAD_ARGUMENT : if the value of the vres argument is not in range or spent is
  NULL
*/
#ifdef __PRIVATE_FRSH_GETUSAGE__
__INLINE__ int __ALWAYS_INLINE__ EE_frsh_vres_get_usage (const frsh_vres_id_t vres, frsh_rel_time_t *spent)
{
  register EE_FREG flag;
  
  if (vres<0 || vres >= EE_MAX_CONTRACT || !spent)
    return FRSH_ERR_BAD_ARGUMENT;

  flag = EE_hal_begin_nested_primitive();
  *spent = EE_vres[vres].usage;
  EE_hal_end_nested_primitive(flag);
  return 0;
}
#endif

/*
  frsh_vres_get_budget_and_period()

  This function stores in the variables pointed to by budget and
  period, the execution-time budget and the period respectively
  associated with the specified vres. If any of these pointers is
  NULL, the corresponding information is not stored.

Returns:

0 if successful

FRSH_ERR_BAD_ARGUMENT : if the value of the vres argument is not in
range, or budget and period are both NULL
*/
#ifdef __PRIVATE_FRSH_GETBUDGETANDPERIOD__
__INLINE__ int __ALWAYS_INLINE__ frsh_vres_get_budget_and_period (const frsh_vres_id_t vres, 
				     frsh_rel_time_t *budget,
				     frsh_rel_time_t *period)
{
  register EE_FREG flag;
  
  if (vres<0 || vres >= EE_MAX_CONTRACT || (!budget && !period))
    return FRSH_ERR_BAD_ARGUMENT;
  
  flag = EE_hal_begin_nested_primitive();

  if (budget) {
    *budget = EE_ct[vres].budget;
  if (period) {
    *period = EE_ct[vres].period;

  EE_hal_end_nested_primitive(flag);
  return 0;
}
#endif

#endif


