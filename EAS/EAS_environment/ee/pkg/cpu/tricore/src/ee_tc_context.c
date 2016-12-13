/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2012  Evidence Srl
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

 /**
    @file ee_tc_context.c
    @brief  Context switch function for multistack on TriCore
    Implementation of EE_std_change_context() as described in
    pkg/cpu/common/inc/ee_context.h
    @author Errico Guidieri
    @date 2012
  */
/* Include the whole internal because I need EE_NIL from Kernel + In case of
   Memory protection i need some types and data structures declarations */
#include "ee_internal.h"
/* Change protection set code in case of memory protection */
#include "cpu/tricore/inc/ee_tc_mem_prot_internal.h"

#ifdef __MULTI__

#ifdef EE_AS_KERNEL_STACKS__

/* Saves necessary context information to resume a thread later */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_task_save( EE_UTID utid, EE_UREG tos )
{
  EE_tc_tasks_RA[utid].ra         = EE_tc_get_RA();
  EE_tc_tasks_RA[utid].kernel_sp  = EE_tc_system_tos[tos].ram_tos;
  EE_tc_system_tos[tos].ram_tos   = EE_tc_get_SP();
}

/* Restores previously saved context information of a thread */
__INLINE__ void __ALWAYS_INLINE__ EE_TC_CHANGE_STACK_POINTER
  EE_tc_task_restore( EE_UTID utid, EE_UREG tos )
{
  EE_ADDR const ra  = EE_tc_tasks_RA[utid].ra;
  EE_ADDR const sp  = EE_tc_tasks_RA[utid].ram_tos;

  EE_tc_system_tos[tos].ram_tos = EE_tc_tasks_RA[utid].kernel_sp;

  EE_tc_set_RA(ra);
  EE_tc_get_SP(sp);
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_stack_save( EE_UREG tos )
{
  EE_tc_system_tos[tos].pcxi_tos  = EE_tc_get_pcxi();
}

__INLINE__ void __ALWAYS_INLINE__
  EE_tc_stack_restore( EE_UREG tos )
{
  struct EE_TC_TOS  * const p_tos = &EE_tc_system_tos[tos];
  EE_UREG             const pcxi  = p_tos->pcxi_tos;

  /* Save a dummy context if it's the first time that a Task goes in
     execution in this stack (At start-up or after an OS-Application restart)
  */
  if ( pcxi == 0U ) {
    EE_tc_svlcx();
    p_tos->pcxi_bos = EE_tc_get_pcxi();
  } else {
    /* Otherwise simply restore the stack PCXI head */
    EE_tc_set_pcxi(pcxi);
  }
}

#else /* EE_AS_KERNEL_STACKS__ */
/* Saves necessary context information to resume a thread later */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_task_save( EE_UTID utid, EE_UREG tos )
{
  EE_tc_tasks_RA[utid] = EE_tc_get_RA();
}

/* Restores previously saved context information of a thread */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_task_restore( EE_UTID utid,
  EE_UREG tos )
{
  EE_ADDR const ra    = EE_tc_tasks_RA[utid];
  EE_tc_set_RA(ra);
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_stack_save( EE_UREG tos )
{
  struct EE_TC_TOS  * const p_tos = &EE_tc_system_tos[tos];

  p_tos->ram_tos   = EE_tc_get_SP();
  p_tos->pcxi_tos  = EE_tc_get_pcxi();
}

#ifdef EE_AS_OSAPPLICATIONS__
__INLINE__ void __ALWAYS_INLINE__ EE_TC_CHANGE_STACK_POINTER
  EE_tc_stack_restore( EE_UREG tos )
{
  struct EE_TC_TOS  * const p_tos = &EE_tc_system_tos[tos];
  EE_ADDR             const sp    = p_tos->ram_tos;
  EE_UREG             const pcxi  = p_tos->pcxi_tos;

  EE_tc_set_SP(sp);

  /* Save a dummy context if it's the first time that a Task goes in
     execution in this stack (At start-up or after an OS-Application restart)
  */
  if ( pcxi == 0U ) {
    EE_tc_svlcx();
    p_tos->pcxi_bos = EE_tc_get_pcxi();
  } else {
    /* Otherwise simply restore the stack PCXI head */
    EE_tc_set_pcxi(pcxi);
  }
}
#else /* EE_AS_OSAPPLICATIONS__ */

__INLINE__ void __ALWAYS_INLINE__ EE_TC_CHANGE_STACK_POINTER
  EE_tc_stack_restore( EE_UREG tos )
{
  struct EE_TC_TOS  * const p_tos = &EE_tc_system_tos[tos];
  EE_ADDR             const sp    = p_tos->ram_tos;
  EE_UREG             const pcxi  = p_tos->pcxi_tos;

  EE_tc_set_SP(sp);
  /* This will work without checking the initialization value (pcxi_tos == 0U)
     of a EE_TC_TOS struct because we won't return from the first context
     of a stack */
  EE_tc_set_pcxi(pcxi);
}
#endif /* EE_AS_OSAPPLICATIONS__ */
#endif /* EE_AS_KERNEL_STACKS__ */

/*
   I needed to move here the TASK runner otherwise I could have race conditions
   that could lead in a wrong behavior in case of OS-Application Termination.
                                  IMPORTANT
   I would stress that these race conditions could have happened only if
   support for OS-Application would have been activated: so previous
   implementation were safe.
*/
static EE_TID EE_tc_run_task_code(EE_TID tid)
{
#ifdef __FP__
  /* Call directly the body */
  /* Enable Interrupts */
  EE_tc_enableIRQ();
  /* Task function call */
  EE_hal_thread_body[tid]();
  /* Disable Interrupts */
  EE_tc_disableIRQ();
#else /* __FP__ */
  /* Call the stub that call the body */
  EE_oo_thread_stub();
#endif /* __FP__ */
  /* Call the scheduler */
  EE_thread_end_instance();
  return EE_std_endcycle_next_tid;
}

/*  Pseudo code for EE_std_change_context in multistack environment:

    begin:
        tos_index = EE_std_thread_tos[tid+1];
        if is_not_the_current_stack(tos_index) {
            save_caller_saved_registers();
            switch_stacks(tos_index);
            restore_caller_saved_registers();
        }
        if (is_not_marked_stacked(tid)) {
            tid = EE_std_run_task_code(tid);
            goto begin;
        }

    Please notice that the "goto begin" is actually a recursive call to
    EE_std_change_context_multi(), but in this way there is no stack growing.

    For processors where the return address is saved in a register, that
    register must be saved in the stack too.

    switch_stacks() should also update EE_hal_active_tos.
*/

/*  I need to save a PCXI & RA per TASK so I need to track task in
    execution beyond stacks. Moreover in case of Memory Protection i need to
    access to interrupted TASK to get Active OS-Application*/
EE_UTID volatile EE_tc_active_utid;

void EE_TC_CHANGE_STACK_POINTER EE_std_change_context( EE_TID tid )
{
  register EE_UTID utid;
  register EE_UREG tos_new;
  register EE_UREG tos_from;

  do {
    utid        =   EE_tc_tid_as_index(tid);
    tos_new     =   EE_std_thread_tos[utid];
    tos_from    =   EE_tc_active_tos;

    /* Save context information per Task */
    EE_tc_task_save(EE_tc_active_utid, tos_from);

    /* Switch Stack */
    if ( tos_from != tos_new ) {
      /* Save context information per Stack */
      EE_tc_stack_save(tos_from);
      EE_tc_active_tos  = tos_new;
      EE_tc_stack_restore(tos_new);
    }

    /* Switch Task */
    EE_tc_active_utid = utid;

    if ( EE_std_tid_is_marked_stacked(tid) ) {
      /* Return to Preempted Task */
      EE_tc_task_restore(utid, tos_new);
#ifdef EE_AS_OSAPPLICATIONS__
      /* Restore the OSApplication (but wait to change the protection set: too
         early) */
      EE_as_active_app = EE_th_app[utid];
#endif /* EE_AS_OSAPPLICATIONS__ */
      break;
    } else {
      /* Run New Task */
      tid = EE_tc_run_task_code(tid);
    }
  } while ( 1U );

  /* The following is needed for the code generated by HIGHTEC GNU Compiler */
  EE_tc_isync();
}

#endif  /* __MULTI__ */
