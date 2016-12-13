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
 * Author: 2003 Bertozzi Francesco
 * CVS: $Id: ee_alget.c,v 1.1.1.1 2004/11/05 16:03:03 pj Exp $
 */

#include "ee.h"
#include "hard_alarms/hard_alarms.h"

#ifndef __PRIVATE_GETALARM__
void EE_alarm_get(EE_TYPEALARM AlarmID, EE_TYPETICK *Tick)
{
  register EE_TYPEALARM current;
  register EE_FREG flag;
  register EE_UREG timer_value;
  
  flag = EE_hal_begin_nested_primitive();

  timer_value = EE_hal_read_timer (EE_alarm_ROM[AlarmID].c);
  
  /* to compute the relative value in ticks, we have to follow the counter
     delay chain */
  current = EE_counter_RAM[EE_alarm_ROM[AlarmID].c];
  *Tick = EE_alarm_RAM[current].value - timer_value;
  while (current != AlarmID) 
  {
    current = EE_alarm_RAM[current].next;
    *Tick += EE_alarm_RAM[current].value;
  } 

  EE_hal_end_nested_primitive(flag);
}
#endif
