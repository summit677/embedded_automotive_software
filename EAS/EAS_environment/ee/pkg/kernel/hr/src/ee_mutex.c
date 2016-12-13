/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2013  Evidence Srl
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
 * Author: 2013 Alessandro Biondi
 *
 */

#include "ee_internal.h"

#ifndef __PRIVATE_GETRESOURCE__
void EE_hr_GetResource(EE_TYPERESOURCE m)
{
  register EE_TYPERESOURCE tmp;
  register EE_FREG flag;
  register EE_TID tmp_exec;
  register EE_TIME tmp_time,delta,t_r;
  register EE_TYPECONTRACT c;

  
  flag = EE_hal_begin_nested_primitive();
  
  c = EE_th[EE_exec].vres;
  
  /* remove the resource type flag */
  tmp = m & ~EE_MUTEX_MASK;
  
  if (!(m & EE_SYSTEM_MUTEX)) // vres local resource
  {
	EE_local_resource_oldceiling[tmp] = EE_vres[c].sys_ceiling;
    EE_vres[c].sys_ceiling |= EE_local_resource_ceiling[tmp];
	EE_th[EE_exec].lockedcounter++;
  }
  else { // System or global resource
	   
	  EE_th[EE_exec].sys_lockedcounter++;
	  
	  /* update vres local ceiling to maximum value in order to avoid local preemption */
	  EE_vres[c].old_sys_ceiling = EE_vres[c].sys_ceiling;
	  EE_vres[c].sys_ceiling = EE_MAX_PRIO;
	  
	  tmp_time = EE_hal_gettime();
	  
	  /* account the capacity to the task that is currently executing */
      delta = tmp_time - EE_last_time;
      EE_vres[c].budget_avail -= delta;

      /* account the overall usage */
      EE_vres[c].usage += delta;
	  
	  /* update the last timer read */
	  EE_last_time = tmp_time;
	  
	  /* A full budget recharge is performed if the available budget is not sufficient to complete the critical section  */
	  /* This is an implementation of the BROE protocol */
	  if(EE_th[EE_exec].wc_rht!=EE_NULL_WCRHT 
	     && EE_th[EE_exec].wc_rht > EE_vres[c].budget_avail) 
	  {
	
		t_r = EE_vres[c].absdline - (EE_TIME)(EE_hr_budget_times_inv_proc_util(c));
		
		if(t_r-tmp_time>0) {
		
			/* To not violate the maximum service delay of the reservation, the server is blocked until t_r */
			EE_vres[c].budget_avail = EE_ct[c].budget;
			EE_vres[c].absdline 	= t_r;
			EE_vres[c].status 		= EE_VRES_RECHARGING;
			
			
			/* Extract vres from RQ */ 
			EE_rq_extract_vres(c); 
			/* Insert vres in recharging queue */
			EE_rcg_insert(c);

			/* update the recharging IRQ if the activated task becomes the first */
			if(EE_rcg_queryfirst() == c)
				EE_hr_set_recharging_timer(EE_vres[c].absdline - tmp_time);
		}
		else 
		{			
			/* Total budget recharge */
			EE_vres[c].budget_avail = EE_ct[c].budget;
				
			/* Deadline shift */
			EE_vres[c].absdline = t_r + EE_ct[c].period;
				
			/* Reorder ready queue*/
			EE_rq_extract_vres(c);
			EE_rq_insert_vres(c);
			
		}
		
		/* Schedule */
		
		/* Since the global system ceiling will be incremented when the task grabs the lock,
		   we have to insert EE_exec in the local stacked queue */
		EE_loc_stk_insertfirst(EE_exec,c);
		EE_th[EE_exec].status = EE_TASK_STACKED | EE_TASK_WASSTACKED;
		/* --- */
		
		tmp_exec = EE_exec;
		EE_exec = EE_NIL;

		EE_hr_check_recharging(tmp_time);
		EE_hr_select_exec();

		EE_hr_run_exec(tmp_exec);
    
	  }
	  
	  /* Here we (should) have sufficient budget to execute the critical section */
	  
	  /* update system ceiling and old system ceiling */ 
	  EE_sys_resource_oldceiling[tmp] = EE_sys_ceiling;
	  EE_sys_ceiling |= EE_sys_resource_ceiling[tmp];
	  
	  
	  #ifdef __MSRP__	   
	  /* if this is a global resource, lock the others CPUs */
	  if (m & EE_GLOBAL_MUTEX) EE_hal_spin_in(tmp);
	  #endif
	  
	  
  } /* End if for *non* local resources */

 	EE_hal_end_nested_primitive(flag);
}
#endif /* __PRIVATE_MUTEX_LOCK__ */


//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

#ifndef __PRIVATE_RELEASERESOURCE__
void EE_hr_ReleaseResource(EE_TYPERESOURCE m)
{
  register EE_FREG flag;
  register EE_TID tmp_exec;
  register EE_TIME tmp_time;

  register EE_TYPERESOURCE tmp;

  flag = EE_hal_begin_nested_primitive();
  
#ifdef __MSRP__
  /* if this is a global resource, unlock the others CPUs */
  if (m & EE_GLOBAL_MUTEX) EE_hal_spin_out(tmp);
#endif  
  
  tmp = m & ~EE_MUTEX_MASK;  
  
  
  if (!(m &  EE_SYSTEM_MUTEX)) // vres local resource
  {
    EE_vres[EE_th[EE_exec].vres].sys_ceiling = EE_local_resource_oldceiling[tmp];
	EE_th[EE_exec].lockedcounter--;
  }
  else { // System or global resource
    
	  /* restore vres local ceiling in order to re-enable local preemption */
	  EE_vres[EE_th[EE_exec].vres].sys_ceiling = EE_vres[EE_th[EE_exec].vres].old_sys_ceiling;
	  
	  /* restore system ceiling*/
	  EE_sys_ceiling = EE_sys_resource_oldceiling[tmp];
	  
      EE_th[EE_exec].sys_lockedcounter--;
  }
    

  /* this part is very similar to ActivateTask */

  tmp_time = EE_hal_gettime();

  /* save the current running task into a temporary variable */
  tmp_exec = EE_exec;
  /* --- */

  /* check_slice: checks the elapsed time on the exec task, putting it into the right
     queue (recharging or ready). at the end EE_exec is EE_NIL */
  EE_hr_check_slice(tmp_time);
  /* --- */
  
  /* check_recharging: if ready and stacked queue are empty pulls from the recharging queue */
  EE_hr_check_recharging(tmp_time);
  /* --- */
  
  /* at this point, exec is for sure EE_NIL (it is set by check_slice) */
  /* select the first task from the ready or stacked queue */
  /* the function set the EE_exec value, removing the task from the queue
     the status is untouched */
  
  EE_hr_select_exec();
  /* --- */

  EE_hr_run_exec(tmp_exec);
  
  EE_hal_end_nested_primitive(flag);
}

#endif /* __PRIVATE_MUTEX_UNLOCK__ */
