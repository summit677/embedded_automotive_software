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

#include "ee_internal.h"

/* this file contains a set of utility functions used by bind and unbind */


/*
 * This function detaches any VRES attached to a thread
 * returns:
 * - 1 if the VRES has been detached successfully
 * - 0 if the VRES has NOT been detached successfully - the VRES has been BOOKED for detachment
 *
 * Returns 1 if the VRES has been detached, 0 if the VRES detachment has been postponed.
 */
#ifndef __PRIVATE_BIND_DETACH_VRES__
int EE_frsh_bind_detach_thread(EE_TID thread)
{
  register EE_TYPESTATUS status;
  register EE_TYPECONTRACT vres;

  vres = EE_th[thread].vres;

  /* check if a task has already been detached */
  if (vres == EE_VRES_NIL) {
    return 1;
  }

  status = EE_th[thread].status;

  if (status & EE_TASK_READY) {
    /* the states FREEZED, INACTIVE, NOT_BOUND are not possible for the VRES since the task is active.
     */

    /* detach the thread and the VRES */
    EE_th[thread].vres = EE_VRES_NIL;
    EE_vres[vres].task = EE_NIL;

    if (EE_vres[vres].status == EE_VRES_ACTIVE) {
      /* EE_VRES_ACTIVE 
	 We need to remove the task from the ready queue and put the VRES inactive
      */
      /* Put the VRES to INACTIVE */
      EE_vres[vres].status = EE_VRES_INACTIVE;

      /* remove the task from the ready queue */
      EE_rq_extract(thread);
    }

    // else {
      /* EE_VRES_RECHARGING - The vres is recharging
	 we can detach it. at the recharging time the detached vres will be put INACTIVE
       */

      /* the vres status remains recharging */

      /* we do not have to remove the thread from the ready queue
	 because it is not there (its vres is in the recharging
	 queue) */
    
      /* nothing has to be done. commenting out the else. */
    //}

  } else if (status & EE_TASK_STACKED) {
    /* EE_TASK_STACKED - defer the unbind at the end of the critical section 
     * The current VRES must stay attached to the thread,
     * it will be detached later on. 
     */

    return 0;
  } else {
    //else if (status & (EE_TASK_SUSPENDED | EE_TASK_BLOCKED)) {
    /* detach the thread and the VRES */
    EE_th[thread].vres = EE_VRES_NIL;
    EE_vres[vres].task = EE_NIL;
  }


  /* if we return 1, the task is not on the ready queue */

  return 1;
}
#endif
