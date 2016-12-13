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
 * Author: 2009 Paolo Gai
 * CVS: $Id: ee_cap.c,v 1.6 2008/07/18 09:53:55 tiberipa Exp $
 */

#include <string.h>
#include "ee_internal.h"
#include "frsh_configuration_parameters.h"
#include "frsh_core_types.h"
#include "frsh_error.h"

/* These functions have been inserted for compatibility with the FRSH API */

/*
frsh_contract_get_basic_params()

This operation obtains from the specified contract object its budget,
period, and workload, and copies them to the places pointed to by the
corresponding output parameters.

Input Parameters:
- contract the pointer to the contract object

Output Parameters:
- budget_min pointer to preallocated space
- period_max pointer to preallocated space
- workload pointer to preallocated space
- contract_type pointer to preallocated space

Returns:
0 if no error
FRSH_ERR_BAD_ARGUMENT : if one of the contract or pointers is NULL.

*/
#ifndef __PRIVATE_FRSH_CONTRACT_GET_BASIC_PARAMS__
int EE_frsh_contract_get_basic_params (const frsh_contract_t *contract,
				       frsh_rel_time_t *budget_min,
				       frsh_rel_time_t *period_max,
				       frsh_workload_t *workload,
				       frsh_contract_type_t *contract_type)
{
  if (!contract || !budget_min || !period_max || !workload || !contract_type) {
    return FRSH_ERR_BAD_ARGUMENT;
  }

  *budget_min = contract->budget;
  *period_max = contract->period;
  *workload = FRSH_WT_INDETERMINATE;
  *contract_type = FRSH_CT_REGULAR;

  return FRSH_NO_ERROR;
}
#endif



/*
  frsh_contract_get_timing_reqs()

  The operation obtains the corresponding input parameters from the
  specified contract object. If d_equals_t is true, the deadline will
  be set to FRSH_NULL_DEADLINE.

  Returns:
  - 0 if no error
  - FRSH_ERR_BAD_ARGUMENT : if contract is NULL
*/
#ifndef __PRIVATE_FRSH_CONTRACT_GET_TIMING_REQS__
int EE_frsh_contract_get_timing_reqs(const frsh_contract_t *contract, 
				     int *d_equals_t,
				     frsh_rel_time_t *deadline, 
				     frsh_signal_t *budget_overrun_signal, 
				     frsh_signal_info_t *budget_overrun_siginfo, 
				     frsh_signal_t *deadline_miss_signal, 
				     frsh_signal_info_t *deadline_miss_siginfo)
{
  int i;

  if (!contract) {
    return FRSH_ERR_BAD_ARGUMENT;
  }

  for (i=0; i<EE_MAX_CONTRACT; i++) {
    if (&EE_ct[i] == contract)
      break;
  }
  
  if (i == EE_MAX_CONTRACT) {
    return FRSH_ERR_BAD_ARGUMENT;
  }

  *d_equals_t = 1;
  *deadline = FRSH_NULL_DEADLINE;
  *budget_overrun_signal = 0;
  *budget_overrun_siginfo = 0;
  *deadline_miss_signal = 0;
  *deadline_miss_siginfo = 0;
     
  return FRSH_NO_ERROR;
}
#endif
