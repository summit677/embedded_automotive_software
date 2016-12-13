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
 * CVS: $Id: ee_swait.c,v 1.3 2008/07/18 09:53:55 tiberipa Exp $
 */



#include "ee_internal.h"
#include "frsh_core_types.h"
#include "frsh_error.h"

/* This file contains all what is needed to handle the timeout operation */

#ifndef __PRIVATE_TIMEOUT_INSERT__
void EE_frsh_timeout_insert(EE_TID t)
{
  EE_TYPEABSDLINE prio;
  EE_TID p,q;

  p = EE_NIL;
  q = EE_frsh_timeout_first;
  prio = EE_frsh_timeout[t].timeout;

  while ((q != EE_NIL) && 
	 (EE_STIME)(prio - EE_frsh_timeout[q].timeout) >= 0
  	) {
    p = q;
    q = EE_frsh_timeout[q].next;
  }

  if (p != EE_NIL)
    EE_frsh_timeout[p].next = t;
  else
    EE_frsh_timeout_first = t;

  EE_frsh_timeout[t].next = q;
}

#endif


#ifndef __PRIVATE_TIMEOUT_EXTRACT__
void EE_frsh_timeout_extract(EE_TID t, EE_TIME tmp_time)
{
  EE_TID p,q;

  p = EE_NIL;
  q = EE_frsh_timeout_first;

  while ( (q != EE_NIL) && (q != t)) {
    p = q;
    q = EE_frsh_timeout[q].next;
  }
  
  if (q == EE_NIL) {
    /* the thread is not there ??? */
  } else {
    /* q == t !!! */
    if ( p == EE_NIL ) {
      // remove the first item in the timeout queue
      EE_frsh_timeout_first = EE_frsh_timeout[EE_frsh_timeout_first].next;

      // reprogram the timer
      if (EE_frsh_timeout_first == EE_NIL) {
	EE_frsh_stop_synchobj_timeout_timer();
      } else {
	EE_frsh_set_synchobj_timeout_timer(EE_frsh_timeout[EE_frsh_timeout_first].timeout - tmp_time);
      }
    } else {
      // remove an item in the middle of the timeout queue
      EE_frsh_timeout[p].next = EE_frsh_timeout[q].next;
    }
  }
}

#endif



/* removes a task from the synchobj queue */
#ifndef __PRIVATE_SYNCHOBJ_EXTRACT__
void EE_frsh_synchobj_extract(EE_TID thread)
{
  EE_TID p;
  EE_TID t;
  frsh_synchobj_handle_t synch_handle = EE_frsh_timeout[thread].synchobj;
  
  p = EE_NIL;
  t = synch_handle->first;
  
  while ( (t != EE_NIL) && (t != thread)) {
    p = t;
    t = EE_th[t].next;
  }
  
  if (t == EE_NIL) {
    /* the thread is not there ??? */
  } else {
    /* t == thread !!! */
    if ( p == EE_NIL ) {
      // remove the first item in the synchronization object queue
      synch_handle->first = EE_th[synch_handle->first].next;
    } else {
      // remove an item in the middle of the synchobject queue
      EE_th[p].next = EE_th[t].next;
    }
  }
}
#endif





/*
 * This routine is called when a primitiev with timeout is called
 * The result is that one or more tasks are put back into the ready queue with the imeout set.
 */
#ifndef __PRIVATE_IRQ_SYNCHOBJ_TIMEOUT__
void EE_frsh_IRQ_synchobj_timeout(void)
{
  register EE_TIME tmp_time;
  register EE_FREG flag;
  register EE_TID t;
  
  flag = EE_hal_begin_nested_primitive();

  /* read the current time */
  tmp_time = EE_hal_gettime();

  /* Check for expired timeouts */
  t = EE_frsh_timeout_first;
  while (t != EE_NIL) {
    if ((EE_STIME)(EE_frsh_timeout[t].timeout - tmp_time) <= EE_TIMER_MINCAPACITY) {
      /* remove the task from the timeout queue */
      EE_frsh_timeout_first = EE_frsh_timeout[t].next;

      /* it could be queued because of the frsh_timed_wait */
      if (EE_frsh_timeout[t].synchobj) {
	/* remove the task from the synchobj queue */
	EE_frsh_synchobj_extract(t);
	
	/* reset the synchobj value, to say that the task is no more waiting on a synchobject, useful for the signal */
	EE_frsh_timeout[t].synchobj = 0;
      }

      /* set the timeout flag */
      EE_frsh_timeout[t].flag = 1;

      /* wakeup the task pointed by EE_frsh_timeout_first */
      if (EE_frsh_updatecapacity(t, tmp_time) == EE_UC_InsertRDQueue){
	/* In this case, the budhet has been updated and the task is ready to be executed */
	EE_rq_insert(t);
      }

      EE_th[t].status = EE_TASK_READY | EE_TASK_WASSTACKED;
    } else {
      /* the tasks are ordered by deadline. if one fails, the others are for sure in the future */
      break;
    }

    t = EE_frsh_timeout_first;
  }

  /* t points to the top of the recharging queue */

  // Program the recharging timer
  if (t == EE_NIL) {
      EE_frsh_stop_synchobj_timeout_timer();
  } else {
      EE_frsh_set_synchobj_timeout_timer(EE_frsh_timeout[t].timeout - tmp_time);
  }

  EE_hal_end_nested_primitive(flag);
}

#endif


