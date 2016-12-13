/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2009  Evidence Srl
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
 * CVS: $Id: ee_spost.c,v 1.2 2008/07/16 09:46:12 francesco Exp $
 */

#include "ee_internal.h"
#include "frsh_core_types.h"
#include "frsh_error.h"

/*
  This function sends a notification event to the synchronization
  object specified as parameter. If there is at least one vres waiting
  on the synchronization object, it is awaken. If more than one vres
  are waiting, just one of them is awaken. However, which one is
  awaken is implementation dependent. If no vres is waiting on the
  synchronization object, the notification event is queued.

  Parameters:
  synch_handle the handle of the synchronization object to notify.

  Returns:
  0 if no error
  FRSH_ERR_BAD_ARGUMENT : if synch_handle is 0
*/


void EE_frsh_timeout_extract(EE_TID t, EE_TIME tmp_time);


#ifndef __PRIVATE_FRSH_SYNCOBJ_SIGNAL__
int EE_frsh_synchobj_signal(const frsh_synchobj_handle_t synch_handle)
{
  register EE_FREG flag;
  register EE_TIME tmp_time;
  register EE_TID tmp_exec;
  register EE_TID tmp;

  if (synch_handle == 0) {
    return FRSH_ERR_BAD_ARGUMENT;
  }

  flag = EE_hal_begin_nested_primitive();

  tmp_time = EE_hal_gettime();

  /* save the current running task into a temporary variable */
  tmp_exec = EE_exec;
  /* --- */
  
  /* check_slice: checks the elapsed time on the exec task, putting it into the right
     queue (recharging or ready). at the end EE_exec is EE_NIL */
  EE_frsh_check_slice(tmp_time);
  /* --- */
  
  /* implement the signal behavior */

  if (synch_handle->first != EE_NIL) {
    // wake up a blocked thread
    tmp = synch_handle->first;

    synch_handle->first = EE_th[tmp].next;

    if (synch_handle->first == EE_NIL)
      synch_handle->last = EE_NIL;

    if (EE_frsh_updatecapacity(tmp, tmp_time) == EE_UC_InsertRDQueue){
      /* In this case, the budhet has been updated and the task is ready to be executed */
      EE_rq_insert(tmp);
    }
      
    EE_th[tmp].status = EE_TASK_READY | EE_TASK_WASSTACKED;

    // remove the task from the timeout queue
    if (EE_frsh_timeout[tmp].synchobj) {
      // reset to say that the task is no more waiting with timeout
      EE_frsh_timeout[tmp].synchobj = 0;

      // extract the task from the timeout queue, and eventually reprogram the timer
      EE_frsh_timeout_extract(tmp, tmp_time);
    }
  }
  else
    synch_handle->count++;

  /* --- */


  /* check_recharging: if ready and stacked queue are empty pulls from the recharging queue */
  EE_frsh_check_recharging(tmp_time);
  /* --- */
  
  /* at this point, exec is for sure EE_NIL (it is set by check_slice) */
  /* select the first task from the ready or stacked queue */
  /* the function set the EE_exec value, removing the task from the queue
     the status is untouched */
  
  EE_frsh_select_exec();
  /* --- */
  
  EE_frsh_run_exec(tmp_exec);

  EE_hal_end_nested_primitive(flag);

  return 0;
}
#endif
