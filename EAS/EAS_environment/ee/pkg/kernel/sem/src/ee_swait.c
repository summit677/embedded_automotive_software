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
 * Author: 2002 Paolo Gai
 * CVS: $Id: ee_swait.c,v 1.6 2007/06/14 10:27:12 pj Exp $
 */

#include "ee_internal.h"

#ifndef __PRIVATE_SEM_WAIT__
void EE_sem_WaitSem(SemRefType s)
{
  EE_TID current;

  register EE_FREG np_flags;
  np_flags = EE_hal_begin_nested_primitive();

  if (s->count)
    s->count--;
  else {
    /* The running task blocks: 
       - it must be removed from the stacked queue
       - and then it must be inserted into the blocked queue */

    /* get the running task */	
    current = EE_stk_queryfirst();

    /* extract the task from the stk data structure */
    EE_stk_getfirst();

    /* The task state switch from STACKED TO BLOCKED */
#if defined(__MULTI__) || defined(__WITH_STATUS__)
    EE_th_status[current] = EE_BLOCKED;
#endif

    /* reset the thread priority bit in the system_ceiling */
    EE_sys_ceiling &= ~EE_th_dispatch_prio[current];

    if (s->first != EE_NIL)
      /* the semaphore queue is not empty */
      EE_th_next[s->last] = current;
    else
      /* the semaphore queue is empty */
      s->first = current;

    s->last = current;
    EE_th_next[current] = EE_NIL;

#if defined(__FP__)
    /* check if there is to schedule a ready thread or pop a preempted
     * thread 
     */
    if (EE_rq_queryfirst() == EE_NIL ||
	EE_sys_ceiling >= EE_th_ready_prio[EE_rq_queryfirst()])
#else
    /*
     * check if there is to schedule a ready thread or pop a preempted thread
     * th_absdline[stk_queryfirst()] <= th_absdline[rq_queryfirst()] 
     *
     * see also src/srpt/thendin.c
     *
     * note that this test work also for the main task!
     */
    if (EE_rq_queryfirst() == EE_NIL || 
        (EE_stk_queryfirst() != EE_NIL && 
          (
            (EE_STIME)(EE_th_absdline[EE_stk_queryfirst()] - 
		       EE_th_absdline[EE_rq_queryfirst()]) <= 0
            || EE_sys_ceiling >= EE_th_ready_prio[EE_rq_queryfirst()]
          ) 
        )
       )
#endif
    {
      /* we have to schedule an interrupted thread that is on the top 
       * of its stack; the state is already STACKED! */
      EE_hal_stkchange(EE_stk_queryfirst());
    }
    else { 
      /* we have to schedule a ready thread that may be not yet stacked */
#if defined(__MULTI__)
      register int flag;
      
      flag = EE_th_status[EE_rq_queryfirst()] & EE_WASSTACKED;
#endif
      
#if defined(__MULTI__) || defined(__WITH_STATUS__)
      EE_th_status[EE_rq_queryfirst()] = EE_STACKED;
#endif

      EE_sys_ceiling |= EE_th_dispatch_prio[EE_rq_queryfirst()];

#if defined(__MULTI__)
      if (flag)
	EE_hal_stkchange(EE_rq2stk_exchange());
      else
	EE_hal_ready2stacked(EE_rq2stk_exchange());
#else
      EE_hal_ready2stacked(EE_rq2stk_exchange());
#endif
    }
  }
  
  EE_hal_end_nested_primitive(np_flags);
}
#endif
