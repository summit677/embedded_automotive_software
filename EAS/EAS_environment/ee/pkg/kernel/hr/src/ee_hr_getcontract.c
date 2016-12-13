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
 * 		   2013 Alessandro Biondi
 */

#include "ee_internal.h"
#include "../inc/hr_frescor.h"

/*
This operation stores the contract parameters currently associated with the specified vres in the variable
pointed to by contract. It returns an error if the vres_id is not recognised.
Returns:
0 if no error
HR_ERR_BAD_ARGUMENT : if the contract argument is NULL or the value of the vres argument
is not in range
*/
#ifndef __PRIVATE_GETCONTRACT__
int EE_hr_vres_get_contract (const hr_vres_id_t vres, hr_contract_t *contract)
{
  /* consistency check on the parameters. these checks does not require interrupt disabling */
  if (vres<0 || vres >= EE_MAX_CONTRACT || contract == NULL)
    return HR_ERR_BAD_ARGUMENT;

  // no need for interrupt disabling. Contracts are consts...
  contract->budget = EE_ct[vres].budget;
  contract->period = EE_ct[vres].period;
  contract->inv_proc_util_top = EE_ct[vres].inv_proc_util_top;
  contract->inv_proc_util_bot = EE_ct[vres].inv_proc_util_bot;

  return 0;
}
#endif
