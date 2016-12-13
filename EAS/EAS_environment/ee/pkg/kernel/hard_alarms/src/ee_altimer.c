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
 * CVS: $Id: ee_altimer.c,v 1.6 2007/06/01 09:00:21 pj Exp $
 */

#include "ee.h"
#include "hard_alarms/hard_alarms.h"

#ifndef EE_N_HARD_ALARMS
#define EE_N_HARD_ALARMS 255
#endif

#if (0 < EE_N_HARD_ALARMS)
void EE_altimer0 (void)
{
  EE_alarm_timer(0);
}
#endif

#if (1 < EE_N_HARD_ALARMS)
void EE_altimer1 (void)
{
  EE_alarm_timer(1);
}
#endif

#if (2 < EE_N_HARD_ALARMS)
void EE_altimer2 (void)
{
  EE_alarm_timer(2);
}
#endif

#ifndef __PRIVATE_TIMERALARM__

void EE_alarm_timer(EE_TYPECOUNTER c)
{
  register EE_TYPEALARM current;
  register EE_TID t;
  register EE_FREG flag;
  register EE_SREG res;
  register EE_UREG inf;
  int rn_return_val;
  
  flag = EE_hal_begin_nested_primitive();
  
  current = EE_counter_RAM[c];
  inf = EE_hal_read_timer (EE_alarm_ROM[current].c);
  
  res = EE_alarm_RAM[current].value - inf;
        
  while (res < 0)
  {
    switch (EE_alarm_ROM[current].action) 
    {
      case  EE_ALARM_ACTION_TASK:
      /* activate the task; NOTE: no preemption at all... 
       * This code was directly copied from ActivateTask 
       */
      t = EE_alarm_ROM[current].TaskID;
    
      /* this code is similar to the first part of thread_activate */
#ifdef __RN_TASK__
        if (EE_IS_TID_REMOTE(t))
        { /* forward the request to another CPU whether the thread do
           * not become to the current CPU 
           */
	  EE_TYPERN_PARAM par;
	  par.pending = 1;
          rn_return_val = EE_rn_send((EE_SREG)EE_MARK_REMOTE_TID(t),
EE_RN_TASK, par );
        } 
        else 
        {
#endif
        if (EE_th_nact[t] == 0) 
          { /* IDLE
             * no preemption --> the thread goes into the ready
             * queue The preemption test will be done into
             * sys_scheduler()
             */ 
#ifdef __EDF__
            // compute the deadline 
            EE_th_absdline[t] = EE_hal_gettime()+EE_th_reldline[t];
#endif

#if defined(__MULTI__) || defined(__WITH_STATUS__)
            EE_th_status[t] = EE_READY;
#endif
            EE_rq_insert(t);
          }
          EE_th_nact[t]++;
#ifdef __RN_TASK__
        }
#endif
    
      break;

      case EE_ALARM_ACTION_CALLBACK:
        (EE_alarm_ROM[current].f)();
        break;
    };
      
    /* remove the current entry */
    EE_counter_RAM[c] = EE_alarm_RAM[current].next;
      
    /* Update the queue head */
    if (EE_alarm_RAM[current].next != -1)
      EE_alarm_RAM[EE_alarm_RAM[current].next].value += 
            EE_alarm_RAM[current].value;
    
    /* the alarm is cyclic? */
    if (EE_alarm_RAM[current].cycle) 
    { /* enqueue it again 
       * note: this can modify EE_counter_RAM[c].first!!! see (*)
       */
      EE_alarm_insert(current,
            EE_alarm_RAM[current].value + 
            EE_alarm_RAM[current].cycle);
    }
    
    if ((current = EE_counter_RAM[c]) == -1)
    {
      EE_hal_stop_timer(c);
      EE_hal_end_nested_primitive(flag);
      return;
    }
      
    inf = EE_hal_read_timer (EE_alarm_ROM[current].c);
    
    res = EE_alarm_RAM[current].value - inf;
  }
 
  EE_hal_set_timer(EE_alarm_ROM[current].c, EE_alarm_RAM[current].value);
 
  EE_hal_end_nested_primitive(flag);
}
#endif

