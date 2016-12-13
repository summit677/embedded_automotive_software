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
 * Edited for HR by Alessandro Biondi (2013)
 */

#include "kernel/hr/inc/ee_common.h"

#ifndef __INCLUDE_HR_KERN_H__
#define __INCLUDE_HR_KERN_H__

/* This macros are used to define a task */
#define DeclareTask(t) void Func##t(void)
#define TASK(t) void Func##t(void)

/*************************************************************************
 System functions
 *************************************************************************/

#ifndef __PRIVATE_SYS_GETTIME__
#ifdef __TIME_SUPPORT__
EE_TIME EE_hr_sys_gettime(void);
#endif
#endif


/*************************************************************************
 Primitives
 *************************************************************************/




#ifndef __PRIVATE_THREAD_ACTIVATE__
void EE_hr_thread_activate(EE_TID t);
#endif

#ifndef __PRIVATE_SCHEDULE__
void EE_hr_Schedule(void);
#endif

#ifndef __PRIVATE_GETRESOURCE__
void EE_hr_GetResource(EE_TYPERESOURCE m);
#endif

#ifndef __PRIVATE_RELEASERESOURCE__
void EE_hr_ReleaseResource(EE_TYPERESOURCE m);
#endif



/* FRESCOR API Implementation - inherited from FRSH */

#include "hr_frescor.h"

/* Basic services */

#ifndef __PRIVATE_HR_INIT__
int EE_hr_init(void);
#endif

#ifndef __PRIVATE_HR_STRERROR__
int EE_hr_strerror(int error, char *message, size_t size);
#endif




/* Contract Creation and initialization */

#ifndef __PRIVATE_HR_CONTRACT_GET_BASIC_PARAMS__
int EE_hr_contract_get_basic_params (const hr_contract_t *contract,
				       hr_rel_time_t *budget_min,
				       hr_rel_time_t *period_max,
				       hr_workload_t *workload,
				       hr_contract_type_t *contract_type);
#endif

#ifndef __PRIVATE_HR_CONTRACT_GET_RESOURCE_AND_LABEL__
int EE_hr_contract_get_resource_and_label(const hr_contract_t *contract,
					    hr_resource_type_t *resource_type,
					    hr_resource_id_t *resource_id, 
					    char *contract_label);
#endif

#ifndef __PRIVATE_HR_CONTRACT_GET_TIMING_REQS__
int EE_hr_contract_get_timing_reqs(const hr_contract_t *contract, 
				     int *d_equals_t,
				     hr_rel_time_t *deadline, 
				     hr_signal_t *budget_overrun_signal, 
				     hr_signal_info_t *budget_overrun_siginfo, 
				     hr_signal_t *deadline_miss_signal, 
				     hr_signal_info_t *deadline_miss_siginfo);
#endif






/* Negotiate Contract Functions */


#ifndef __PRIVATE_GETVRESID__
int EE_hr_thread_get_vres_id(const hr_thread_id_t thread, hr_vres_id_t *vres_id);
#endif

#ifndef __PRIVATE_GETCONTRACT__
int EE_hr_vres_get_contract (const hr_vres_id_t vres, hr_contract_t *contract);
#endif


/*
  Returns true if the system is configured with the on-line admission test enabled, or false otherwise. This
  situation can only be changed at compile time.
*/
#ifdef __PRIVATE_HR_ISADMISSIONTESTENABLED__
__INLINE__ bool __ALWAYS_INLINE__ EE_hr_config_is_admission_test_enabled(void)
{
  return 0;
}
#endif

/*
  This function stores in the variable pointed to by budget the
  remaining execution-time budget associated with the specified vres.

  Returns:

  0 if successful
  HR_ERR_BAD_ARGUMENT : if the value of the vres argument is not in range or budget is NULL
*/
#ifdef __PRIVATE_HR_GETREMAININGBUDGET__
int EE_hr_vres_get_remaining_budget (const hr_vres_id_t vres, hr_rel_time_t *budget)
{  
  register EE_FREG flag;
  
  if (vres<0 || vres >= EE_MAX_CONTRACT || !budget)
    return HR_ERR_BAD_ARGUMENT;

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
  HR_ERR_BAD_ARGUMENT : if the value of the vres argument is not in range or spent is
  NULL
*/
#ifdef __PRIVATE_HR_GETUSAGE__
int EE_hr_vres_get_usage (const hr_vres_id_t vres, hr_rel_time_t *spent)
{
  register EE_FREG flag;
  
  if (vres<0 || vres >= EE_MAX_CONTRACT || !spent)
    return HR_ERR_BAD_ARGUMENT;

  flag = EE_hal_begin_nested_primitive();
  *spent = EE_vres[vres].usage;
  EE_hal_end_nested_primitive(flag);
  return 0;
}
#endif

/*
  hr_vres_get_budget_and_period()

  This function stores in the variables pointed to by budget and
  period, the execution-time budget and the period respectively
  associated with the specified vres. If any of these pointers is
  NULL, the corresponding information is not stored.

Returns:

0 if successful

HR_ERR_BAD_ARGUMENT : if the value of the vres argument is not in
range, or budget and period are both NULL
*/
#ifdef __PRIVATE_HR_GETBUDGETANDPERIOD__
int hr_vres_get_budget_and_period (const hr_vres_id_t vres, 
				     hr_rel_time_t *budget,
				     hr_rel_time_t *period)
{
  register EE_FREG flag;
  
  if (vres<0 || vres >= EE_MAX_CONTRACT || (!budget && !period))
    return HR_ERR_BAD_ARGUMENT;
  
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


