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
 * Author: 2013 Alessandro Biondi
 * Based on ee_api.h in FRSH, by Paolo Gai (2009)
 */


#ifndef __INCLUDE_KERNEL_HR_EE_API__
#define __INCLUDE_KERNEL_HR_EE_API__

#if defined(__HR__)

#ifndef Schedule
#define Schedule		EE_hr_Schedule
#endif

#ifndef ActivateTask
#define ActivateTask            EE_hr_thread_activate
#endif

#ifndef GetResource
#define GetResource             EE_hr_GetResource
#endif

#ifndef ReleaseResource
#define ReleaseResource 	EE_hr_ReleaseResource
#endif

#ifndef EE_sys_gettime
#ifdef __TIME_SUPPORT__
#define EE_sys_gettime		EE_hr_sys_gettime
#endif
#endif

/* HR specific API */

#ifndef hr_init
#define hr_init 	        EE_hr_init
#endif

#ifndef hr_strerror
#define hr_strerror 	        EE_hr_strerror
#endif

/* HR specific API inherited from FRSH */

#ifndef hr_contract_get_basic_params
#define hr_contract_get_basic_params EE_hr_contract_get_basic_params
#endif

#ifndef hr_contract_get_timing_reqs
#define hr_contract_get_timing_reqs EE_hr_contract_get_timing_reqs
#endif


#ifndef hr_thread_get_vres_id
#define hr_thread_get_vres_id EE_hr_thread_get_vres_id
#endif

#ifndef hr_vres_get_contract
#define hr_vres_get_contract EE_hr_vres_get_contract
#endif


#ifndef hr_config_is_admission_test_enabled
#define hr_config_is_admission_test_enabled EE_hr_config_is_admission_test_enabled
#endif

#ifndef hr_vres_get_remaining_budget
#define hr_vres_get_remaining_budget EE_hr_vres_get_remaining_budget
#endif

#ifndef hr_vres_get_usage
#define hr_vres_get_usage EE_hr_vres_get_usage
#endif

#ifndef hr_vres_get_budget_and_period
#define hr_vres_get_budget_and_period EE_hr_vres_get_budget_and_period
#endif


#endif
#endif
