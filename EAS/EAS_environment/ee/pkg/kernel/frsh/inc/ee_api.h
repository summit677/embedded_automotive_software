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
 * Author: Paolo Gai
 * CVS: $Id: ee_api.h,v 1.4 2008/07/16 09:46:12 francesco Exp $
 */


#ifndef __INCLUDE_KERNEL_FRSH_EE_API__
#define __INCLUDE_KERNEL_FRSH_EE_API__

#ifdef __FRSH__

#ifndef Schedule
#define Schedule		EE_frsh_Schedule
#endif

#ifndef ActivateTask
#define ActivateTask            EE_frsh_thread_activate
#endif

#ifndef GetResource
#define GetResource             EE_frsh_GetResource
#endif

#ifndef ReleaseResource
#define ReleaseResource 	EE_frsh_ReleaseResource
#endif

#ifndef EE_sys_gettime
#ifdef __TIME_SUPPORT__
#define EE_sys_gettime		EE_frsh_sys_gettime
#endif
#endif

#ifndef GetTime
#ifdef __TIME_SUPPORT__
#define GetTime		        EE_frsh_sys_gettime
#endif
#endif


/* FRSH specific API */

#ifndef frsh_init
#define frsh_init 	        EE_frsh_init
#endif

#ifndef frsh_strerror
#define frsh_strerror 	        EE_frsh_strerror
#endif

#ifndef frsh_contract_get_basic_params
#define frsh_contract_get_basic_params EE_frsh_contract_get_basic_params
#endif

#ifndef frsh_contract_get_timing_reqs
#define frsh_contract_get_timing_reqs EE_frsh_contract_get_timing_reqs
#endif

#ifndef frsh_thread_bind
#define frsh_thread_bind EE_frsh_BindTask
#endif

#ifndef frsh_thread_unbind
#define frsh_thread_unbind EE_frsh_UnbindTask
#endif

#ifndef frsh_thread_get_vres_id
#define frsh_thread_get_vres_id EE_frsh_thread_get_vres_id
#endif

#ifndef frsh_vres_get_contract
#define frsh_vres_get_contract EE_frsh_vres_get_contract
#endif

#ifndef frsh_synchobj_signal
#define frsh_synchobj_signal EE_frsh_synchobj_signal
#endif

#ifndef frsh_synchobj_wait
#define frsh_synchobj_wait EE_frsh_synchobj_wait
#endif

#ifndef frsh_synchobj_wait_with_timeout
#define frsh_synchobj_wait_with_timeout EE_frsh_synchobj_wait_with_timeout
#endif

#ifndef frsh_timed_wait
#define frsh_timed_wait EE_frsh_timed_wait
#endif

#ifndef frsh_config_is_admission_test_enabled
#define frsh_config_is_admission_test_enabled EE_frsh_config_is_admission_test_enabled
#endif

#ifndef frsh_vres_get_remaining_budget
#define frsh_vres_get_remaining_budget EE_frsh_vres_get_remaining_budget
#endif

#ifndef frsh_vres_get_usage
#define frsh_vres_get_usage EE_frsh_vres_get_usage
#endif

#ifndef frsh_vres_get_budget_and_period
#define frsh_vres_get_budget_and_period EE_frsh_vres_get_budget_and_period
#endif


#endif
#endif
