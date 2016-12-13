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
 * Edited for HR by Alessandro Biondi (2013)
 */

#include "ee_internal.h"
#include "../inc/hr_frescor.h"

/*
This operation stores the Id of the vres associated with the specified thread in the variable pointed to by
vres. It returns an error if the thread does not exist, it is not under the control of the scheduling framework,
or is not bound.
Returns:
0 if no error
HR_ERR_NOT_BOUND : if the given thread does not have a valid vres bound to it
HR_ERR_BAD_ARGUMENT : if the given thread does not exist or the vres argument is NULL
*/
#ifndef __PRIVATE_GETVRESID__
int EE_hr_thread_get_vres_id(const hr_thread_id_t thread, hr_vres_id_t *vres_id)
{
  register EE_FREG flag;

  /* consistency check on the parameters. these checks does not require interrupt disabling */
  if (thread<0 || thread >= EE_MAX_TASK || vres_id == NULL)
    return HR_ERR_BAD_ARGUMENT;

  flag = EE_hal_begin_nested_primitive();

  if (EE_th[thread].vres == EE_VRES_NIL) {
    EE_hal_end_nested_primitive(flag);
    return HR_ERR_NOT_BOUND;
  }

  *vres_id = EE_th[thread].vres;

  EE_hal_end_nested_primitive(flag);

  return 0;
}
#endif
