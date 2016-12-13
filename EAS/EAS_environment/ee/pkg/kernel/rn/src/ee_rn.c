/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2011  Evidence Srl
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
 * Author: 2001, 2002 Paolo Gai
 *         2001 Bernardo  Dal Seno
 */

/* for ActivateTask() */
#include "ee.h"

#include "ee_internal.h"

#ifdef __RN__

/* this INTERNAL function is called to execute a notification procedure */
#ifndef __PRIVATE_RN_EXECUTE__
static void EE_rn_execute(EE_TYPERN rn, EE_TYPERN_SWITCH sw)
{
#if defined(__RN_COUNTER__) || defined(__RN_TASK__) || defined(__RN_FUNC__)
  register EE_UREG pend;
#endif

#ifdef __RN_COUNTER__
  if (EE_rn_type[rn][sw] & EE_RN_COUNTER) {
    for (pend = EE_rn_pending[rn][sw];
	 pend;
	 pend--) {

#if (defined(__OO_BCC1__) || defined(__OO_BCC2__) || defined(__OO_ECC1__) || defined(__OO_ECC2__) ) && !defined(__OO_NO_ALARMS__)
      EE_oo_IncrementCounterImplementation(EE_rn_counter[rn]);
#endif
#if defined(__ALARMS__)
      EE_counter_tick(EE_rn_counter[rn]);
#endif

    }
    EE_rn_pending[rn][sw] = 0;

    EE_rn_type[rn][sw] &= ~EE_RN_COUNTER;
  }
#endif



  /* Task before Events; in this way we can activate an extended task
     and then set events for it! */
#ifdef __RN_TASK__
  if (EE_rn_type[rn][sw] & EE_RN_TASK) {
    for (pend = EE_rn_pending[rn][sw];
	 pend;
	 pend--) {
#if defined(__FP__) || defined(__EDF__)
      EE_fp_ActivateTask(EE_rn_task[rn]);
#endif
#if defined(__OO_BCC1__) || defined(__OO_BCC2__) || defined(__OO_ECC1__) || defined(__OO_ECC2__)
      /* Any error is ignored and not reported back to the originating core */
      (void)EE_oo_ActivateTask(EE_rn_task[rn]);
#endif
#if defined(__FRSH__)
      EE_frsh_thread_activate(EE_rn_task[rn]);
#endif
    }
    EE_rn_pending[rn][sw] = 0U;

    EE_rn_type[rn][sw] &= ~EE_RN_TASK;
  }
#endif



#ifdef __RN_EVENT__
  if (EE_rn_type[rn][sw] & EE_RN_EVENT) {
    if (EE_rn_event[rn][sw]) {
      /* Any error is ignored and not reported back to the originating core */
      (void)EE_oo_SetEvent(EE_rn_task[rn], EE_rn_event[rn][sw]);
      /* we have to reset the event mask! */
      EE_rn_event[rn][sw] = 0U;
    }

    EE_rn_type[rn][sw] &= ~EE_RN_EVENT;
  }
#endif



#ifdef __RN_FUNC__
  if (EE_rn_type[rn][sw] & EE_RN_FUNC) {
    for (pend = EE_rn_pending[rn][sw];
	 pend;
	 pend--) {
      ((void (*)(void))(EE_rn_func[rn]))();
    }
    EE_rn_pending[rn][sw] = 0;

    EE_rn_type[rn][sw] &= ~EE_RN_FUNC;
  }
#endif



#ifdef __RN_BIND__
  if (EE_rn_type[rn][sw] & EE_RN_BIND) {
    EE_frsh_BindTask(EE_rn_vres[rn][sw], EE_rn_task[rn]);
    EE_rn_type[rn][sw] &= ~EE_RN_BIND;
  }
#endif



#ifdef __RN_UNBIND__
  if (EE_rn_type[rn][sw] & EE_RN_UNBIND) {
    EE_frsh_UnbindTask(EE_rn_task[rn]);
    EE_rn_type[rn][sw] &= ~EE_RN_UNBIND;
  }
#endif

}
#endif

/* this function can be used into an interrupt handler to handle
   pending notifications. It will execute all the pending
   notifications. Execution order may not respect the original
   notification order. */
#ifndef __PRIVATE_RN_HANDLER__
void EE_rn_handler(void)
{
  /* Probably (I hope ;-) an interprocessor interrupt has been raised.
     That also means that (probably) there are pending notifications
     that have to be notified. 
     
     Please note that we must disable interrupts in a way that the
     spin lock exits as soon as possible.
  */

  register EE_TYPERN current;
  register EE_TYPERN_SWITCH sw;
  register EE_FREG flag;
  int redo = 0;

  do {

      sw = EE_rn_switch[EE_CURRENTCPU] & EE_RN_SWITCH_COPY;

      flag = EE_hal_begin_nested_primitive();
      
      /* Spin Lock acquisition */
      EE_hal_spin_in(EE_rn_spin[EE_CURRENTCPU]);
      
      /* switch pending requests and set the inside irq flag */
      EE_rn_switch[EE_CURRENTCPU] = 
	(EE_rn_switch[EE_CURRENTCPU] ^ EE_RN_SWITCH_COPY) |
	EE_RN_SWITCH_INSIDEIRQ;

      /* Spin Lock release */
      EE_hal_spin_out(EE_rn_spin[EE_CURRENTCPU]);
      
      EE_hal_end_nested_primitive(flag);
      
      
      /* Note: from now on, we can assume that all the data structures
	 indexed by sw can be accessed without need of interrupt
	 disabling or spin locks. The only requirements that must be
	 fulfilled is that this function cannot be interrupted again by
	 itself (i.e., once an interprocessor interrupt is raised, it
	 cannot be raised again until this function has completed. */
      
      /* executing requests */
      for (current = EE_rn_first[EE_CURRENTCPU][sw];
	   current != (EE_TYPERN)-1;
	   current = EE_rn_next[current][sw]) {
	
	EE_rn_execute(current,sw);
      }
      
      EE_rn_first[EE_CURRENTCPU][sw] = -1;


      /* now we have to guarantee that we can exit from the
	 Interprocessor interrupt. That is, the other processor should
	 not have queued any other request. to do that, we have to
	 reaquire again the spin lock to check the appropriate
	 conditions. Moreover, we have to do that with interrupt
	 disabled, and we have to be sure that we will exit the
	 handler with INTERRUPT DISABLED. */
      
      flag = EE_hal_begin_nested_primitive();
      EE_hal_spin_in(EE_rn_spin[EE_CURRENTCPU]);

      /* if the other processor has queued another request */
      if (EE_rn_switch[EE_CURRENTCPU] & EE_RN_SWITCH_NEWRN) {
	/* reset the newrn flag */
	EE_rn_switch[EE_CURRENTCPU] &= ~EE_RN_SWITCH_NEWRN;
	/* redo the dispatching of the remote notifications */
	redo = 1;
      }
      else {
	/* we can exit the interrupt! */
        redo = 0;
	/* set that we are no more inside the interrupt */
	EE_rn_switch[EE_CURRENTCPU] &= ~EE_RN_SWITCH_INSIDEIRQ;

	EE_hal_IRQ_interprocessor_served((EE_UINT8)EE_CURRENTCPU);
      }

      /* Spin Lock release */
      EE_hal_spin_out(EE_rn_spin[EE_CURRENTCPU]);

      /* note: we end the handler with interrupt disabled! */
      if (redo) {
	EE_hal_end_nested_primitive(flag);
      }
  } while (redo);
}
#endif /* __PRIVATE_RN_HANDLER__ */

#endif /* __RN__ */
