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

 /** @file   ee_tc27x_internal.c
  *  @brief  CPU-dependent internal part of HAL implementation
  *  @author Errico Guidieri
  *  @date 2012
  */

/* Include ERIKA API */
#include "ee.h"
/* CPU Memory Protection & Stack Monitoring Internal declarations and inline
   definitions */
#include "cpu/tricore/inc/ee_tc_mem_prot_internal.h"

/* If MemMap support is enabled: use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* Put the following code in ee_kernel_text */
#define OS_START_SEC_CODE
/* Put the following variables in ee_kernel_bss */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef EE_STACK_MONITORING__
/* Used to handle Stack Monitoring */
#ifdef EE_AS_OSAPPLICATIONS__
void EE_TC_CHANGE_STACK_POINTER
  EE_as_check_and_handle_stack_overflow( ApplicationType appid,
    EE_UREG tos )
{
  if ( EE_tc_check_stack_overflow(tos) ) {
    /* The stack is corrupted I have to switch to a new one */
    EE_tc_set_SP((EE_ADDR)EE_tc_get_csfr(EE_CPU_REG_ISP));
    EE_as_call_protection_error( appid, E_OS_STACKFAULT );
  }
}

#else /* EE_AS_OSAPPLICATIONS__ */

void EE_TC_CHANGE_STACK_POINTER
  EE_as_check_and_handle_stack_overflow( EE_UREG tos )
{
  if ( EE_tc_check_stack_overflow(tos) ) {
    /* The stack is corrupted I have to switch to a new one */
    EE_tc_set_SP((EE_ADDR)EE_tc_get_csfr(EE_CPU_REG_ISP));
    EE_oo_ShutdownOS_internal( E_OS_STACKFAULT );
  }
}
#endif /* EE_AS_OSAPPLICATIONS__ */
#endif /* EE_STACK_MONITORING__ */

#ifdef EE_AS_OSAPPLICATIONS__
/******************************************************************************
                    OS-Applications & Memory Protection HALs
 ******************************************************************************/
/* I need Common Context Implementation */
#include "cpu/common/inc/ee_context.h"
/* Plus I need Common End-ISR Scheduler Implementation */
#include "cpu/common/inc/ee_irqstub.h"

/* If ISR2 Support is required */
#if defined(EE_MAX_ISR_ID) && (EE_MAX_ISR_ID > 0)

#ifdef __GNUC__
/* This disable the selected warning of unsused variables only for this code
   (available in gcc from version 4.6), the warning is suppressed between 
   until "#pragma GCC diagnostic pop" */
#pragma GCC diagnostic push
/* I declare some volatile registers located variables to "discourage" compiler
   to use that registers */
#pragma GCC diagnostic ignored "-Wunused-variable"
/* I don't want read or write on register variables. Just I don't want them be
   touched */
#pragma GCC diagnostic ignored "-Wvolatile-register-var"
/* d12 and d13 are inizialized by The Assembler Handler (Unfortunatley seem to
   not work... ) */
/* #pragma GCC diagnostic ignored "-Wuninitialized" */
#endif /* __GNUC__ */

#ifdef __OO_ORTI_RUNNINGISR2__
#define EE_ORTI_running_isr2_restore_preempted()  do {                    \
  /* Get next ISR2 Stack Data Structure */                                \
  isr_stack_ptr = &EE_as_ISR_stack[EE_hal_get_IRQ_nesting_level() - 1];   \
  EE_ORTI_set_runningisr2(EE_as_ISR_ROM[isr_stack_ptr->ISR_ID].handler);  \
} while ( 0 )
#else /* __OO_ORTI_RUNNINGISR2__ */
#define EE_ORTI_running_isr2_restore_preempted()  ((void)0)
#endif /* __OO_ORTI_RUNNINGISR2__ */

/*
 * EE_as_TerminateISR2(): Terminate the ISR2
 * Since this function does stack rewind for an ISR it must be an ISR handler
 * too.
 */
StatusType EE_TC_INTERRUPT_HANDER EE_TC_CHANGE_STACK_POINTER
  EE_as_TerminateISR2( void )
{
  /* TASK previous CSAs list tail */
  EE_CSA_LINK task_queue_tail;
  /* Pointer to TASK Tail CSA */
  EE_CSA * p_task_tail_csa;
  /* "Check Point" (The context we want set back). */
  EE_CSA_LINK cp;
  /* Free-list head */
  EE_CSA_LINK fcx;
  /* Previous CSA list head */
  EE_CSA_LINK pcx;

  /*                        !!! WARNING !!!
    Make a local copy of the original interrupted (TASK) information.
    We are going out to the first stacked ISR2: rescheduling will
    happen and EE_as_ISR_stack global data structure could be changed
    when we will come back in this context to restore original preempted
    TASK.
  */
  /*         !!! WARNING !!!  !!! WARNING !!!  !!! WARNING !!!
    Make the following declaration volatile is LETHAL. In That case GCC will
    try to save the value on the original stack.
    If I don't use volatile gcc just access data structures again: exactly
    what I'm trying to avoid.
    So I will use register allocated variables. These register MUST be Upper
    Context registers: those Saved and Restored by a function call.
    Checking GCC generated assembler, a12 and d12 are not used so
    I'll use them.
    N.B. a0, a1, a8, a9 cannot be used because they are Global Address
    registers. Moreover a10, a11 are respectively SP, RA Pointers.
  */
  /* Get ISR2 Stack Data Structures. The following Data Structures is used
     to save SP and OS-Application for an interrupted ISR2/TASK.
     It is accessed via EE_IRQ_nesting_level (not incremented). */
#ifdef __GNUC__
  register EE_as_ISR_RAM_type * isr_stack_ptr __asm("a12");
  register ApplicationType interrupted_appid  __asm("d12");
  register EE_UREG interrupted_tos            __asm("d13");
#elif defined(__DCC__)
  static EE_as_ISR_RAM_type * isr_stack_ptr   __asm("a12");
  static ApplicationType interrupted_appid    __asm("d12");
  static EE_UREG interrupted_tos              __asm("d13");
#endif /* __GNUC__ || __DCC__ */

  /* This will be a while to handle multiple interrupted ISRs stacked */
  while ( 1U ) {
    /* ISR2 instance clean-up as requested by AR, must be
       called  before EE_decrement_IRQ_nesting_level */
    EE_std_end_IRQ_post_stub();
    /* Decrement nesting level here */
    EE_decrement_IRQ_nesting_level();

    /* ISR2 Stack Data Structure */
    isr_stack_ptr     = &EE_as_ISR_stack[EE_IRQ_nesting_level];
    interrupted_appid = isr_stack_ptr->Interrupted_App;
    interrupted_tos   = EE_as_Application_ROM[interrupted_appid].ISRTOS;

    /* REWIND THE ISR CALL STACK */
    /* Flush shadow CSAs & any kind of write before access */
    EE_tc_dsync();
    /* Obtain free-list head */
    fcx = EE_tc_csa_get_fcx();
    /* Obtain previous CSA list head */
    pcx = EE_tc_csa_get_pcxi();
    /* Obtain the right context values to set back */
    task_queue_tail.reg = isr_stack_ptr->ISR_Terminate_data;
    p_task_tail_csa = EE_tc_csa_make_addr(task_queue_tail);

    /* The NULL check is just needed for MISRA compliance */
    if ( p_task_tail_csa != NULL ) {
      cp = p_task_tail_csa->next;
      /* Link tail of dead CSAs to free-list */
      EE_tc_csa_set_next(task_queue_tail, fcx);
      /* Dead CSAs become head of free-list */
      EE_tc_csa_set_fcx(pcx);
      /* "Rewind" previous CSA list */
      EE_tc_csa_set_pcxi(cp);
    }

    /* If the ISR at the lowest level is ended, restore old
       SP, reset CCPN and call the scheduler. */
    /* Check for scheduling point */
    if ( EE_is_inside_ISR_call() == 0U )
    {
      /* Restore Original ISR2 TOS value */
      EE_tc_system_tos[interrupted_tos].ram_tos =
        EE_tc_system_bos[interrupted_tos].base_stack;
      /* Reset ORTI ISR2 value */
      EE_ORTI_set_runningisr2(EE_NO_ISR2);
      /* Set CCPN to unmask next IRQ, it would have been
         done by RFE but we are not returning yet */
      EE_tc_set_int_prio( EE_ISR_UNMASKED );
      /* Configure "User" Stack */
      EE_tc_set_psw_user_stack();
      /* Restore task stack pointer */
      EE_tc_set_SP( isr_stack_ptr->TerminationTOS );
      /* Call the scheduler */
      EE_std_after_IRQ_schedule();
      /* The following call restore global OSApplication tracking variable and
         check if stacked task has been terminated. If yes, it calls
        EE_hal_terminate_task */
      EE_as_after_IRQ_interrupted_app(interrupted_appid);
      /* If the stacked TASK is not terminated restart the TASK timing
         protection */
      EE_as_tp_active_update_budgets_and_restart();
    } else {
      /* Get Interrupted SP */
      EE_ADDR interrupted_sp = isr_stack_ptr->TerminationTOS;

      /* If the Os-Application is already Terminated I don't check stack
         coherence */
      if ( EE_as_Application_RAM[interrupted_appid].ApplState !=
        APPLICATION_ACCESSIBLE )
      {
        /* Restore Original ISR2 TOS value */
        EE_tc_system_tos[interrupted_tos].ram_tos = EE_tc_system_bos[
          interrupted_tos].base_stack;
        /* We have to terminate the next stacked ISR too */
        continue;
      } else {
        /* Restore interrupted OSApplication */
        EE_as_active_app = interrupted_appid;
        /* Restore ORTI ISR2 Tracing */
        EE_ORTI_running_isr2_restore_preempted();
        /* Restore old TOS... */
        EE_tc_system_tos[interrupted_tos].ram_tos = interrupted_sp;
        /* ...And check it. */
        EE_tc_check_and_handle_stack_overflow_with_sp(interrupted_appid,
          interrupted_tos, interrupted_sp);
        /* Restore the ISR2 timing protection set and restart it */
        EE_as_tp_active_set_from_id_with_restart(EE_AS_TP_ID_FROM_ISR2(
          isr_stack_ptr->ISR_ID));
      }
    }

    /* Reset PSW Call Depth Counter to let this function be called by
       other primitives like EE_as_TerminateApplication */
    EE_tc_set_psw(EE_tc_get_psw() & EE_TC_PSW_CDC_CLEAN_MASK);

    /* In case of OS-Application mapped on protection set I don't need to
       explicitly restore PSW.PSR. Hardware does that in Context Restoring. */
    EE_tc_restore_os_app(interrupted_appid);

    /* I DON'T NEED TO EXPLICITLY RESTORE AN INTERRUPTED ISR2/TASK USER STACK,
       NEITHER PSW.PSR. IT WILL BE CONTEXT RESTORING TO DO THAT FOR ME */
    break;
  }

  EE_TC_RFE();
  return E_OK;
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /* __GNUC__ */

#endif /* EE_MAX_ISR_ID > 0 */

/* Used by TerminateApplication to Terminate Other TASKs   */
void EE_hal_terminate_other_task( EE_TID tid ) {
  /* TID converted as struct index */
  EE_UTID utid;
  /* TOS index */
  EE_UREG tos;
  /* TOS structure pointer */
  struct EE_TC_TOS * p_tos;
  /* Free-list head */
  EE_CSA_LINK fcx;
  /* TASK previous CSAs list head */
  EE_CSA_LINK task_queue_head;
  /* TASK previous CSAs list tail */
  EE_CSA_LINK task_queue_tail;

  /* REWIND THE CALL STACK */
  utid   = EE_tc_tid_as_index(tid);
  tos    = EE_std_thread_tos[utid];
  p_tos  = &EE_tc_system_tos[tos];

  /* Stack already rewinded */
  if ( p_tos->pcxi_tos == 0U ) {
    return;
  }

  /* Flush shadow CSAs & any kind of write before access */
  EE_tc_dsync();
  /* Obtain free-list head */
  fcx = EE_tc_csa_get_fcx();
  /* Obtain the TASK pcxi head */
  task_queue_head.reg = p_tos->pcxi_tos;
  /* Obtain the TASK pcxi tail */
  task_queue_tail.reg = p_tos->pcxi_bos;
  /* Link tail of dead CSAs to free-list */
  EE_tc_csa_set_next(task_queue_tail, fcx);
  /* Dead CSAs become head of free-list */
  EE_tc_csa_set_fcx(task_queue_head);

  /* Clear TOS structure */
  p_tos->ram_tos = EE_tc_system_bos[tos].base_stack;
  p_tos->pcxi_tos = 0U;
  p_tos->pcxi_bos = 0U;
}

void EE_TC_CHANGE_STACK_POINTER
  EE_hal_call_app_hook_implentation ( EE_HOOKTYPE hook, ApplicationType app )
{
  /* XXX: Switch to ISR2 User Stack ( I don't know in what else
          stack switch to ) */
  EE_tc_set_SP(EE_tc_system_tos[EE_as_Application_ROM[app].ISRTOS].ram_tos);
  EE_tc_set_os_app_prot_set_from_appid(app);
  hook();
  /*                         !!! IMPORTANT !!!
     I DON'T NEED TO EXPLICITLY RESTORE PREVIOUS STACK AND PREVIOUS PROTECTION
     REGISTER SET, IT WILL BE CONTEXT RESTORING TO DO THAT FOR ME.
  */

}

void EE_hal_call_app_hook ( EE_HOOKTYPE hook, ApplicationType app )
{
  /* Save Active OsApplication */
  ApplicationType prev_app = EE_as_active_app;
  /* Force the function call */
  __asm volatile ("call EE_hal_call_app_hook_implentation");
  /* This function can be called by the kernel only, so after hook execution
     I restore kernel as executing OS-Application */
  EE_as_active_app = prev_app;
}

void EE_TC_CHANGE_STACK_POINTER EE_hal_call_app_status_hook_implementation (
  StatusType Error, EE_STATUSHOOKTYPE status_hook, ApplicationType app )
{
  /* XXX: Switch to ISR2 User Stack ( I don't know in what else
          stack switch to ) */
  EE_tc_set_SP(EE_tc_system_tos[EE_as_Application_ROM[app].ISRTOS].ram_tos);
  EE_tc_set_os_app_prot_set_from_appid(app);
  status_hook(Error);
  /*                         !!! IMPORTANT !!!
     I DON'T NEED TO EXPLICITLY RESTORE PREVIOUS STACK AND PREVIOUS PROTECTION
     REGISTER SET, IT WILL BE CONTEXT RESTORING TO DO THAT FOR ME.
  */
}

void EE_hal_call_app_status_hook ( StatusType Error, EE_STATUSHOOKTYPE
  status_hook, ApplicationType app )
{
  /* Save Active OsApplication that generate the error */
  ApplicationType prev_app = EE_as_active_app;
  /* Force the function call */
  __asm volatile ("call EE_hal_call_app_status_hook_implementation");
  /* This function can be called by the kernel only, so after hook execution
     I restore kernel as executing OS-Application */
  EE_as_active_app = prev_app;
}

#ifdef __EE_MEMORY_PROTECTION__

/* Ram Begin Address */
extern EE_UINT32 ee_skernel_ram[];
/* Kernel Ram End Adddress */
extern EE_UINT32 ee_ekernel_ram[];

/* Begin Code Range */
extern void ee_sall_code ( void );
/* End Code Range */
extern void ee_eall_code ( void );

/* API Ram Begin Address */
extern EE_UINT32 ee_sapi_ram[];
/* API Ram End Adddress */
extern EE_UINT32 ee_eapi_ram[];

/* Begin Const API Section Address */
extern EE_UINT32 ee_sapi_const[];
/* End Const API Flash End Address */
extern EE_UINT32 ee_eapi_const[];

/* Symbols set by the linker, used for section boundaries. Declared as arrays,
 * so the compiler doesn't think they are in the small data area. */

#if defined(RTDRUID_CONFIGURATOR_NUMBER) \
 && (RTDRUID_CONFIGURATOR_NUMBER >= RTDRUID_CONFNUM_STACK_IN_APP_SEC_INFO)
#define check_stack(b,e,i) (((b) >= (EE_ADDR)((i)->stacks_begin))  \
  && ((e) <= (EE_ADDR)(i)->stacks_end))

#else /* RTDRUID_CONFNUM_STACK_IN_APP_SEC_INFO */
/* No stack address available: always return false */
#define check_stack(b,e,i) 0
#endif /* RTDRUID_CONFNUM_STACK_IN_APP_SEC_INFO */

/* Function intruced to prevent from misra error:
 * "cast from pointer to unsigned int"
 * and
 * "cast from unsigned int to pointer"
 */
static EE_ADDR EE_tc_compute_end_bound(EE_ADDR beg[], EE_UREG size){
  /* This op simply performs: beg+size */
  return &((EE_BIT *)beg)[size];
}

/* Return the access permission for the given memory area. */
AccessType EE_hal_get_app_mem_access(ApplicationType app,
  MemoryStartAddressType beg, MemorySizeType size)
{
  AccessType ret = (AccessType)0;
  EE_ADDR end = NULL;

  /* If the input region is not contained within a single section (e.g.,
   * starting is inside the stack section and the end is inside BSS) we
   * return 0 even if the task would have some permissions to the region,
   * because an application should not rely upon the layout of sections
   * to work correctly. */
#ifdef __MSRP__
  /* Shared data */
  extern EE_UINT32 ee_mcglobald_begin[];
  extern EE_UINT32 ee_mcglobald_end[];
  /* Shared const */
  extern EE_UINT32 ee_mcglobalc_begin[];
  extern EE_UINT32 ee_mcglobalc_end[];
  /* Shared code */
  extern void ee_mcglobalt_begin( void );
  extern void ee_mcglobalt_end( void );

  if ( (beg >= (EE_ADDR)ee_mcglobald_begin) &&
      (beg <= (EE_ADDR)ee_mcglobald_end) )
  {
    if ( EE_as_Application_ROM[app].Mode == EE_MEMPROT_TRUST_MODE ) {
      ret = EE_ACCESS_READ | EE_ACCESS_WRITE;
    } else {
      ret = EE_ACCESS_READ;
    }
  } else if ( (beg >= (EE_ADDR)ee_mcglobalc_begin) &&
    (beg <= (EE_ADDR)ee_mcglobalc_end) )
  {
    ret = EE_ACCESS_READ;
  } else if ( (beg >= (EE_ADDR)ee_mcglobalt_begin) &&
    (beg <= (EE_ADDR)ee_mcglobalt_end) )
  {
    ret = EE_ACCESS_EXEC;
  } else
#endif /*  __MSRP__ */
  {
    /* New code to compute end (misra compliant)*/
    if ( (beg != NULL) && (size > 0U) ) {
      end = EE_tc_compute_end_bound(beg, size);
    }

    if ( end != NULL ) {
      if ( (beg >= (EE_ADDR)ee_sall_code) && (end <= (EE_ADDR)ee_eall_code) ) {
        /* Executable code */
        ret = EE_ACCESS_EXEC;
      } else if ( (beg >= (EE_ADDR)ee_sapi_const) &&
        (end <= (EE_ADDR)ee_eapi_const) ) {
        /* Global constant data section */
        ret = EE_ACCESS_READ;
      } else {
        /* Check the OS-Application sections */
        EE_APP_SEC_INFO_T const * const info = &EE_as_Application_ROM[app].
          sec_info;

        if ( check_stack(beg,end,info) ) {
          /* Stack section. For the sake of efficiency, we don't
           * check individual stacks. */
          ret = EE_ACCESS_READ | EE_ACCESS_WRITE | EE_ACCESS_STACK;
        } else if ( EE_as_Application_ROM[app].Mode == EE_MEMPROT_TRUST_MODE ) {
          /* If the application is Trusted it can access to all the RAM ->
             Kernel Ram */
          if ( (beg >= (EE_ADDR)ee_skernel_ram) && (end <=
            (EE_ADDR)ee_ekernel_ram) ) {
            /* Data section. For the sake of efficiency, we don't
             * check individual sections. */
             ret = EE_ACCESS_READ | EE_ACCESS_WRITE;
          }
        } else if ( (beg >= (EE_ADDR)info->ram_begin) && (end <=
          (EE_ADDR)info->ram_end) ) {
          /* Data section. For the sake of efficiency, we don't
           * check individual sections. */
          ret = EE_ACCESS_READ | EE_ACCESS_WRITE;
        } else {
          /* No more sections; return the default (0) - Added For MISRA */
        }
      }
    }
  }

  return ret;
}

#endif /* __EE_MEMORY_PROTECTION__ */

#endif /* EE_AS_OSAPPLICATIONS__ */
