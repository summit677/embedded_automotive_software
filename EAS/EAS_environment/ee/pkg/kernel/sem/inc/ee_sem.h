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
 * CVS: $Id: ee_sem.h,v 1.6 2006/12/31 10:24:00 pj Exp $
 */

#ifndef __INCLUDE_KERNEL_SEM_SEM_H__
#define __INCLUDE_KERNEL_SEM_SEM_H__

/* Semaphores 
   ----------

   These file declares the Erika enterprise Semaphore API
   A semaphore is contained in a data structure called EE_SEM.
   That structure can be initialized statically (recommended), or
   dynamically using the macro sem_init.

   These functions can ONLY be used with a multistack HAL or similar,
   because these semaphore primitives are BLOCKING primitives.
*/

/* The semaphore descriptor */
typedef struct {
  int count;
  EE_TID    first;
  EE_TID    last;
} SemType;

typedef SemType *SemRefType;

#define STATICSEM(value) { (value), EE_NIL, EE_NIL }

#ifndef __PRIVATE_SEM_INIT__
#define EE_sem_InitSem(s,value) \
  (s).count = (value),	  \
  (s).first = EE_NIL,  \
  (s).last = EE_NIL
#endif

#ifndef __PRIVATE_SEM_WAIT__
void EE_sem_WaitSem(SemRefType s);
#endif

#ifndef __PRIVATE_SEM_TRYWAIT__
/* returns 1 if the counter is decremented, 0 if not */
int EE_sem_TryWaitSem(SemRefType s);
#endif

#ifndef __PRIVATE_SEM_POST__
void EE_sem_PostSem(SemRefType s);
#endif

#ifndef __PRIVATE_SEM_GETVALUE__
int EE_sem_GetValueSem(SemRefType s);
#endif

#endif
