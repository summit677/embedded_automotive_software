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
 * CVS: $Id: ee_cap.c,v 1.6 2008/07/18 09:53:55 tiberipa Exp $
 */

#include "ee_internal.h"


/* tasks are put into the recharging queue with the following status:
   - the budget is always greater then the minimum capacity

   - the period is:

     - if the budget was >0 when the task was interrupted, it is the
       former theadline of the task when it was interrupted

     - if the budget was <0 then the deadline is postponed and the
       budget incremented until it is greater than 0

   - a problem may occur if the budget is -really- <0 due to shared
     resource usage. in that case the task deadline can drift in the
     future way over the lifetime of the timer. This is a limitation
     we do not address here, because we assume taht the task
     parameters and the resource usages are compatible, in other words
     the critical sections must be short in order the system to work
     fine.
*/

/* This function recharges a task budget up to a point that it can be
   inserted into the recharging queue safely. */
#ifndef __PRIVATE_RECHARGEBUDGET__
void EE_frsh_rechargebudget(EE_TID t) {
  EE_TYPECONTRACT c = EE_th[t].vres;
  /* it can not happen that the VRES is UNBOUND because we are
     recharging the budget, which means that the task was
     executing (not possible if the vres is unbound) */

  for(;;){
    // recharge the task.
    if (EE_vres[c].budget_avail > 0) {
      // the budget is positive, and we recharge to the maximum and we exit.
      EE_vres[c].budget_avail = EE_ct[c].budget;
      break;
    } else {
      // the budget is negative. we sum it and we check if the new budget is enough
      EE_vres[c].budget_avail += EE_ct[c].budget;
      if ( EE_vres[c].budget_avail > EE_TIMER_MINCAPACITY )
	break;
    }

    // we have to do another round! we update the period and we put the task back
    EE_vres[c].absdline += EE_ct[c].period;
  }
}
#endif



/* this function is called every time a thread is activated 
 * to update his budget available and to check if it should go
 * recharging
 *
 * it is similar to checkslice but it activates a task which for sure
 * has lockedcounter=0
 */
#ifndef __PRIVATE_UPDATECAPACITY__
ActionType EE_frsh_updatecapacity(EE_TID t, EE_TIME tmp_time)
{
  EE_TYPECONTRACT c = EE_th[t].vres;

  /* check if the VRES is UNBOUND. in this case, return nothing. the
     capacity is not updated because there is no vres bound to the
     task. */
  if (c == EE_VRES_NIL) {
    return EE_UC_NoVres;
  }

  if (EE_vres[c].status == EE_VRES_RECHARGING) {
    /* the VRES of a task just activated can be recharging in the following scenario:
       1 a task X ends its budget and its vres V goes to recharging
       2 an unbind is called on task X. the vres V remains floating and recharging
       3 a SUSPENDED task Y is binded to the vres V (which is in recharging)
       4 task Y is activated

       If the vres is recharging, we have nothing to do here.
    */
    return EE_UC_InsertedRCGQueue;
  }

  /* if the thread is not active or the current deadline is in the past */
  if (EE_vres[c].status == EE_VRES_FREEZED || (EE_STIME)(EE_vres[c].absdline - tmp_time) < 0) {
    /* task deadline is in the past, reset it to the default */
    EE_vres[c].absdline = tmp_time + EE_ct[c].period;

    /* In this case, we decided to put it equal to the budget because
       since the deadline is in the past or the task is not active it
       is not worth to check and use a += instead of = . also, using
       += does not guarantees that after the sum the capacity is
       greater than 0, and so we are not sure we can put the task into
       the ready queue! */
    EE_vres[c].budget_avail = EE_ct[c].budget;

    EE_vres[c].status = EE_VRES_ACTIVE;
    return EE_UC_InsertRDQueue;
  } 
  else{
    // task deadline is in the future 
    if (EE_vres[c].budget_avail < EE_TIMER_MINCAPACITY) {
      /* goes in recharging only if the remaining capacity is less than the minimum capacity */
      EE_frsh_rechargebudget(t);
      EE_vres[c].status = EE_VRES_RECHARGING;
      EE_rcg_insert(c);
      
      /* update the recharging IRQ if the activated task becomes the first */
      if (EE_rcg_queryfirst() == c)
        EE_frsh_set_recharging_timer(EE_vres[c].absdline - tmp_time);

      return EE_UC_InsertedRCGQueue;
    }
    /* This is the test of CBS, saying that if the bandwith of the
       task considered as the remaining capacity divided buy the time
       left to the deadline is greater than the nominal bandwidth of
       the task we have to reassign the deadlines to avoid that the
       task executes too much. This situation typically happens when
       the task stays idle for a while, or if the task has been
       blocked for a while. */

    if((EE_STIME)(EE_vres[c].absdline - EE_vres[c].budget_avail*EE_ct[c].inv_proc_util -
                   tmp_time) <= 0 ) {
      EE_vres[c].budget_avail = EE_ct[c].budget;
      EE_vres[c].absdline = tmp_time + EE_ct[c].period;
    }

    EE_vres[c].status = EE_VRES_ACTIVE;
    return EE_UC_InsertRDQueue;     
  }
}

#endif



/* 
 * input status: EE_exec is EE_NIL, ready or stacked queue may be full or empty
 * result:
 * EE_exec is set to the next task to schedule.
 * the status is untouched
 * if EE_exec is different from EE_NIL, then the task has been removed from its queue
 *
 * Note: WE CANNOT CHANGE THE STATUS TO EXEC HERE. The status brings
 * also the flag wasstacked. if we set it, then nothing will work
 * after this. A solution could be to put wasstacked in a separate
 * status variable, but I do not want to do that for now.
 */
#ifndef __PRIVATE_SELECTEXEC__
void EE_frsh_select_exec(void)
{
  register EE_TID tmp_rq;
  register EE_TID tmp_stk;

  tmp_rq = EE_rq_queryfirst();
  tmp_stk = EE_stk_queryfirst();
  
  if (tmp_rq == EE_NIL) {
    /* either the first on the stacked queue or nil */
    EE_exec = tmp_stk;
    if (tmp_stk != EE_NIL) { 
      EE_stk_getfirst();
    }
  } else {
    /* the ready queue is not empty */
    if (tmp_stk == EE_NIL ||
	((EE_STIME)(EE_vres[EE_th[tmp_stk].vres].absdline - EE_vres[EE_th[tmp_rq].vres].absdline) > 0
	 && EE_sys_ceiling < EE_th[tmp_rq].prlevel)) {
      EE_exec = tmp_rq;
      EE_rq_getfirst();
    } else {
      /* note that if the previous exec task ahs some locked
	 resources, then it was put into the stacked queue */
      EE_exec = tmp_stk;
      if (tmp_stk != EE_NIL) { 
	EE_stk_getfirst();
      }
    }
  }
}
#endif


/* 
 * If the tmp_exec is different from EE_exec, change context to it.
 * We suppose that if the task has been put to exec, then it has a valid VRES
 */
#ifndef __PRIVATE_RUNEXEC__
void EE_frsh_run_exec(EE_TID tmp_exec)
{
  register int wasstacked;

  if (EE_exec == EE_NIL) {
    /* switch to the background thread if needed */
    if (tmp_exec != EE_exec) {
      EE_hal_stkchange(EE_exec);
    }
  } else {
    /* there is a task to schedule */
    wasstacked = EE_th[EE_exec].status & EE_TASK_WASSTACKED;
    EE_th[EE_exec].status = EE_TASK_EXEC;  
  
    /* if different from the current running task implement the preemption */
    if (tmp_exec != EE_exec) {
      /* reprogram the capacity timer for the new task */
      EE_frsh_set_budget_timer(EE_vres[EE_th[EE_exec].vres].budget_avail);
      
      if (wasstacked)
	EE_hal_stkchange(EE_exec);
      else
	EE_hal_ready2stacked(EE_exec);
    }
  }
}
#endif




/* 
   check_slice

   first considers the exec task, and accounts the elapsed time.

   updates the last timer read

   put the exec task in the right queue:
   - stacked if the task has locked resources, independently from the budget
   - ready if the task has still budget
   - recharging if it has no more budget

   puts exec to -1
*/

#ifndef __PRIVATE_CHECKSLICE__
void EE_frsh_check_slice(EE_TIME tmp_time)
{
  register EE_TIME delta;
  
  if (EE_exec != EE_NIL) {
    EE_TYPECONTRACT c = EE_th[EE_exec].vres;
    /* it can never happen that the vres is UNBOUND because the task
       cannot execute without a VRES */

    /* account the capacity to the task that is currently executing */
    delta = tmp_time - EE_last_time;
    EE_vres[c].budget_avail -= delta;

    /* account the overall usage */
    EE_vres[c].usage += delta;

    if (EE_th[EE_exec].lockedcounter) {
      /* The task is holding a resource, put it into the stacked queue 
	 regardless of the budget it has.
       */
      EE_stk_insertfirst(EE_exec);
      EE_th[EE_exec].status = EE_TASK_STACKED | EE_TASK_WASSTACKED;
      // VRES status unchanged
    } else if ( (EE_STIME)EE_vres[c].budget_avail < EE_TIMER_MINCAPACITY) {
      /* if the budget is exhausted then insert in the recharging queue */
      EE_frsh_rechargebudget(EE_exec);
      EE_vres[c].status = EE_VRES_RECHARGING;
      EE_th[EE_exec].status = EE_TASK_READY | EE_TASK_WASSTACKED;
      EE_rcg_insert(c);

      /* update the recharging IRQ if the activated task becomes the first */
      if(EE_rcg_queryfirst() == c)
        EE_frsh_set_recharging_timer(EE_vres[c].absdline - tmp_time);

    } else {
      EE_rq_insert(EE_exec);
      
      EE_th[EE_exec].status = EE_TASK_READY | EE_TASK_WASSTACKED;
      // VRES status unchanged
    }
    
    /* EE_exec has been inserted into a queue */
    EE_exec = EE_NIL;
  }

  /* update the last timer read */
  EE_last_time = tmp_time;
}
#endif



/* 
   end_slice

   This is similar to check-slice, but it is different because it is
   executed at the end of a task.

   first considers the exec task, and accounts the elapsed time.

   updates the last timer read

   put the exec task in the right queue:
   if nact >1 and budget>0
     the task is put back into the ready queue
   if nact =0 and budget >0
     the task is suspended waiting for the next activation
   if nact >1 and budget <0
     the ending task has consumed all its budget.
     I put the task in recharging
   if nact =0 and budget <0
     I cannot put this task in the recharging queue because it is no more active
     but it will go in the recharging queue when it will be activated again
     so in this case I do nothing

   puts exec to -1
*/

#ifndef __PRIVATE_ENDSLICE__
void EE_frsh_end_slice(EE_TIME tmp_time)
{
  register int time_check;
  register int nact_check;
  register EE_TYPECONTRACT c;
  register EE_TYPEBUDGET delta;

  c = EE_th[EE_exec].vres;
  /* it can never happen that the vres is UNBOUND because the task
     cannot execute without a VRES */

  /* account the capacity to the task that is currently executing */
  delta = tmp_time - EE_last_time;
  EE_vres[c].budget_avail -= delta;

  /* account the overall usage */
  EE_vres[c].usage += delta;

  time_check = (EE_STIME)EE_vres[c].budget_avail < EE_TIMER_MINCAPACITY;
  nact_check = EE_th[EE_exec].nact;
  
  if (time_check) {
    // no budget left
    if (nact_check) {
      // nact > 1
      // the ending task has consumed all its budget.
      // It has still pending activations, I put the task in recharging
      EE_frsh_rechargebudget(EE_exec);
      EE_vres[c].status = EE_VRES_RECHARGING;
      EE_th[EE_exec].status = EE_TASK_READY;
      EE_rcg_insert(c);

      /* update the recharging IRQ if the activated task becomes the first */
      if(EE_rcg_queryfirst() == c)
        EE_frsh_set_recharging_timer(EE_vres[c].absdline - tmp_time);

      // WASSTACKED is not set, because the task just ended
    } else {
      // nact = 0
      // do nothing. 
      // the task is currently in any queue
      // the task will be inserted in the recharging queue when activated again
      EE_vres[c].status = EE_VRES_INACTIVE;
      EE_th[EE_exec].status = EE_TASK_SUSPENDED;
    }
  } else {
    // there is still budget
    if (nact_check) {
      // nact > 1
      // the task is put back into the ready queue, no need to update the budget
      EE_rq_insert(EE_exec);
      // VRES status unchanged
      EE_th[EE_exec].status = EE_TASK_READY;
      // WASSTACKED is not set, because the task just ended
    } else {
      // nact = 0 - tested
      // the task is suspended waiting for the next activation. 
      // the budget is left as it was
      EE_vres[c].status = EE_VRES_INACTIVE;
      EE_th[EE_exec].status = EE_TASK_SUSPENDED;
    }
  }
  
  /* EE_exec has been inserted into a queue or is terminated */
  EE_exec = EE_NIL;
  
  /* update the last timer read */
  EE_last_time = tmp_time;
}
#endif



/* frsh_process_recharging
  
   this function is used to process a recharging VRES when the
   recharging time arrive or when the recharging time is reduiced due
   to a time-warp.
   
   The deadline and budget of the VRES is the final one and does not
   need any change. The budget is greater than the minimum.

   We have to process the task and understand if it has to be inserted
   in any queue

   the function returns 1 when the recharge processed is related to a
   VRES without a task binded to it. this is important in this situation:
   - a task is unbinded, and its VRES is put in the recharging queue
   - a time-warp happens, and the first VRES in the recharging queue is the VRES without tasks binded to them. In that case, we have to take another VRES from the queue, because we want to have a good "delta" value!
*/
#ifndef __PRIVATE_PROCESSRECHARGING__
int EE_frsh_process_recharging(EE_TYPECONTRACT c)
{
  register EE_TID t;
  register EE_TYPESTATUS status;

  /* consider the task linked to the VRES */
  t = EE_vres[c].task;
  
  if (t != EE_NIL && t != EE_TID_DEFERRED) {
    /* there is a task linked to the VRES.
       the task status can be:
       READY|WASSTACKED
         typical case
	 --> VRES set to active, task in the ready queue
       READY (task ended without capacity and nact > 1)
         same as ready
       STACKED
         --> VRES set to active, the task is already in the stacked queue so we do nothing
       SUSPENDED
         may happen after a bind to the task of a vres in recharging
	 --> VRES set to inactive
       BLOCKED
         may happen after a bind to the task of a vres in recharging
	 same as SUSPENDED
    */
    
    status = EE_th[t].status;
    if (status & EE_TASK_READY) {
      /* EE_TASK_READY, independently of the WASSTACKED */
      EE_rq_insert(t);
      EE_vres[c].status = EE_VRES_ACTIVE;
    } else if (status & EE_TASK_STACKED) {
      /* EE_TASK_STACKED */
      EE_vres[c].status = EE_VRES_ACTIVE;
    } else {
      /* EE_TASK_SUSPENDED or EE_TASK_BLOCKED */
      EE_vres[c].status = EE_VRES_INACTIVE;
    }
    return 0;
  } else {
    /* EE_NIL --> this is a VRES without bindings to a task. may happen after an unbind
     * EE_TID_DEFERRED --> may happen if a task has been binded to the
     *                     VRES BUT the binding was not yet done because the task is STACKED
     */
    EE_vres[c].status = EE_VRES_INACTIVE;
    return 1;
  }
}
#endif




/* 
   check-recharging

   if ready and stacked queue are empty pulls from the recharging queue 

   it reprograms the recharging IRQ
*/
#ifndef __PRIVATE_CHECKRECHARGING__
void EE_frsh_check_recharging(EE_TIME tmp_time)
{
  register EE_TYPEBUDGET delta;
  register EE_TYPECONTRACT c;
  
  /* if there are not tasks in the ready and in the stacked queue,
   * check if there are task in recharging queue and if so, immediately
   * recharge one or more of them.
   */

  /* check if the ready and stacked queue are empty */
  if (EE_stk_queryfirst() != EE_NIL || EE_rq_queryfirst() != EE_NIL)
    return;


  /* 
     this first cycle is used to identify a VRES inside the recharging
     queue which is LINKED TO A TASK. that is, since we want ti
     implement a recharge which brings a task in recharging into the
     ready queue, it must haoppen taht we really find one.

     In reality the cycle is needed because it may be that the first n
     entries in the VRES queue are related to VRES without a task
     attached to them due to bind/unbind. We basically have to process
     these VRES until either the recharging queue is empty or we find
     a VRES with a task attached to it.
  */
  do {
    /* we take the first vres in the recharging queue.
       the tasks has been inserted into the queue with a reasonable budget
       (see EE_frsh_rechargebudget) */
    c = EE_rcg_queryfirst();
    
    /* exit if the recharging queue is empty */
    if (c == EE_VRES_NIL) {
      EE_frsh_stop_recharging_timer();
      return;
    }
    
    /* remove the vres from the recharging queue */
    EE_rcg_getfirst();
    
    /* delta is the amount of time we have to shift all the recharging deadlines */
    delta = EE_vres[c].absdline - tmp_time; 
    
    /* at this point t is the head of the recharging queue
       t has a budget which is > EE_TIMER_MINCAPACITY
       t has the deadline which was the one which it was inserted (or postponed) into the recharging queue
       c is no more inside the recharging queue
       t is not inserted in the ready queue
       delta is the shift to be applied to all the tasks remaining in the recharging queue
    */
    
    /* we update the deadline of the vres */
    EE_vres[c].absdline = tmp_time + EE_ct[c].period;
    
    /* we process the task pointed by the VRES */
  } while (EE_frsh_process_recharging(c));

  /* If we arrive here, it must be that we found a VRES with a task
     linked to it, and there are more VRES to process. */

  /* we have to shift all the recharging times by delta */
  c = EE_rcg_queryfirst();
  while (c != EE_VRES_NIL) {
    EE_vres[c].absdline -= delta;
    
    /* if the deadline is (almost) 0, it means that the task's recharge IRQ once put in the ready queue
       will fire in a few microseconds */
    if ((EE_STIME)(EE_vres[c].absdline - tmp_time) <= EE_TIMER_MINCAPACITY) {
      // we have to reassign deadline and budget
      // the check with the minimum capacity is to avoid the recharging interrupt to arrive too early
      
      // update the deadline. Note the deadline has been shifted by delta already
      // The budget has already been set when inserting into the recharging queue
      EE_vres[c].absdline += EE_ct[c].period;
        
      /* we process the task pointed by the VRES */
      EE_frsh_process_recharging(c);

      // remove from the top of the recharging queue
      EE_rcg_getfirst();
      c = EE_rcg_queryfirst();
      
    } else {
      // the recharging queue is ordered by deadline. if one fails,
      // all the rest will fail because they will come later.
      
      // go to the next vres, it has to be processed by the next while
      c = EE_vres[c].next;
      break;
    }
  }

  /* do the rest of the vres */
  while (c != EE_VRES_NIL) {
    EE_vres[c].absdline -= delta;
    c = EE_vres[c].next;
  }
  
  c = EE_rcg_queryfirst();

  // at the end of the update, we have to reprogram the recharging IRQ
  if (c != EE_VRES_NIL) {
      EE_frsh_set_recharging_timer(EE_vres[c].absdline - tmp_time);
  }
  else
    EE_frsh_stop_recharging_timer();
}
#endif

/* #ifdef __SEM_FRSH__ */
/*     // PJ: da rivedere quando farò i semafori */
/*     // note: both recharging times and semaphore timeouts timers are on the same queue */
/*     // PJ: potremmo usare un vettore per gli accodamenti nel recharging evitando gli if... */
/*     if (EE_th_status[EE_rcg_queryfirst()] & EE_BLOCKED) { */
/*       EE_frsh_set_recharging_timer(EE_th_timeouts[EE_rcg_queryfirst()] - tmp_time); */
/*     }  */
/*     else */
/* #endif */

