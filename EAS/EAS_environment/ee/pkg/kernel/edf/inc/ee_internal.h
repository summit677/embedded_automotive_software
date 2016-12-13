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
 * Author: 2001,2002 Paolo Gai
 * CVS: $Id: ee_internal.h,v 1.3 2007/05/25 15:55:12 pj Exp $
 */

#include "kernel/edf/inc/ee_common.h"
#include "kernel/edf/inc/ee_irq.h"

#ifndef __INCLUDE_EDF_INTERNAL_H__
#define __INCLUDE_EDF_INTERNAL_H__

/*************************************************************************
 Kernel Constants
 *************************************************************************/

/* Task statuses:
   In general, a task status is needed for two reasons:
   - for debug purposes,
   - and, if used with a Multistack HAL, for storing a flag that let
   the kernel know if the task has some space allocated on its stack.
*/

#if defined(__MULTI__) || defined(__WITH_STATUS__)
//#define EE_READY      1 in ee_common.h
#define EE_STACKED    2

/* used by semaphores and blocking primitives in general */
#define EE_BLOCKED    4

#if defined(__MULTI__)
/* used to know if a task has some space allocated on its stack */
#define EE_WASSTACKED 8
#endif

#endif



/*************************************************************************
 System functions
 *************************************************************************/

#ifndef __PRIVATE_THREAD_END_INSTANCE__
void EE_thread_end_instance(void);
#endif


/*************************************************************************
 Internal Queue management functions
 *************************************************************************/


#ifndef __PRIVATE_RQ_QUERYFIRST__
/* return the first ready task without extracting it */
__INLINE__ EE_TID __ALWAYS_INLINE__ EE_rq_queryfirst(void)
{
  return EE_rqfirst;  
}
#endif

#ifndef __PRIVATE_STK_QUERYFIRST__
/* return the first stacked task (the running task) without extracting it */
__INLINE__ EE_TID __ALWAYS_INLINE__ EE_stk_queryfirst(void)
{
  return EE_stkfirst;  
}
#endif

#ifndef __PRIVATE_STK_GETFIRST__
/* extract the running task from the stack */
__INLINE__ void __ALWAYS_INLINE__ EE_stk_getfirst(void)
{
  EE_stkfirst = EE_th_next[EE_stkfirst];
}
#endif

#ifndef __PRIVATE_STK_INSERTFIRST__
/* insert a task into the stack  data structures */
__INLINE__ void __ALWAYS_INLINE__ EE_stk_insertfirst(EE_TID t)
{
  EE_th_next[t] = EE_stkfirst;
  EE_stkfirst = t;
}
#endif

#ifndef __PRIVATE_RQ_INSERT__
/* insert a task into the ready queue */
void EE_rq_insert(EE_TID t);
#endif

#ifndef __PRIVATE_RQ2STK_EXCHANGE__
/* put the first ready task into the stack */
EE_TID EE_rq2stk_exchange(void);
#endif

#endif


