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
 * Author: 2008 Paolo Tiberi & Francesco Focacci
 * CVS: $Id: ee_dlcheck.c,v 1.7 2008/07/18 09:53:55 tiberipa Exp $
 */

#include "ee_internal.h"

// PJ: we should initialize this function inside the frsh_init
// could we put it into the recharging queue?

// option1 --> create a task that calls this
// option2 --> create an alarm with an alarm callback

#ifndef __PRIVATE_IRQ_DLCHECK__
/* periodic check of deadline MUST be done with a rate
   at least equal to 1/4 of timer capacity*/
void EE_frsh_IRQ_dlcheck(void)
{
  register EE_TYPECONTRACT c;
  register EE_FREG flag;
  register EE_TIME tmp_time;
  
  flag = EE_hal_begin_nested_primitive();

  tmp_time = EE_hal_gettime();

  for (c=0; c<EE_MAX_CONTRACT; c++) {
    if (EE_vres[c].status != EE_VRES_FREEZED &&
	(EE_STIME)(tmp_time - EE_vres[c].absdline) > 0) {
      EE_vres[c].status = EE_VRES_FREEZED;
    }
  }

  // set this as a periodic interrupt
  EE_frsh_set_dline_timer(EE_TIMER_MAXFUTUREVALUE>>2);


  EE_hal_end_nested_primitive(flag);
}
#endif
