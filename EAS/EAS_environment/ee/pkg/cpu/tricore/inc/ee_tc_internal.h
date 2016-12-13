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

 /** @file   ee_tc_internal.h
  *  @brief  CPU-dependent internal part of HAL
  *  @author Errico Guidieri
  *  @date 2012
  */

/* Infineon modifications, integration to Infineon Build environment:
 * Fixes for compilation issues with Tasking Compiler:
 * Updates for compilation issues with Dcc Compiler:
 * Usage of generic compiler independent register header file:
 * Author: 
 *         Ashok Abbi, <Ashok.Abbi@infineon.com> 18.07.2013
 */

#ifndef INCLUDE_EE_TC_INTERNAL_H__
#define INCLUDE_EE_TC_INTERNAL_H__

/* Public API must be seen by Kernel internals */
#include "cpu/tricore/inc/ee_tc_cpu.h"

/* Customize internal compilation with pragmas */
#ifdef __TASKING__
/*
 * Disable Warnings for Infinite Loops, inside the Kernel
 */
EE_DO_PRAGMA(warning 557)
/* Disable Warnings for dead assignment to "np_flags" eliminated inside the
   EE_oo_TerminateTask Kernel Function. We want the code in the way it is. */
/* It should have been worked according to documentation but it doesn't */
/* EE_DO_PRAGMA(EE_oo_TerminateTask:warning 588) */
#endif /* __TASKING__ */

/* I have to inform DCC when a function touch SP adding the following attribute
   to the function definition */
#if defined(__DCC__)
#define EE_TC_CHANGE_STACK_POINTER __attribute__(( use_frame_pointer ))
#else
#define EE_TC_CHANGE_STACK_POINTER
#endif /* __DCC__ */

#ifdef __GNUC__
#define EE_TC_INTERRUPT_HANDER  __attribute__(( interrupt_handler, used ))
#else  /* __GNUC__ */
#define EE_TC_INTERRUPT_HANDER
#endif /* __GNUC__ */

#if defined(__DCC__) || defined(__TASKING__)
#define EE_TC_RFE() do { EE_tc_rslcx(); __asm volatile("rfe"); } while ( 0 )
#else /* __DCC__ || __TASKING__ */
#define EE_TC_RFE() ((void)0U)
#endif /* __DCC__ || __TASKING__ */

/*******************************************************************************
                              Generic Primitives
 ******************************************************************************/

#include "cpu/common/inc/ee_primitives.h"

/*******************************************************************************
      System startup Handling is Moved in MCU folder, because TriCore
      architecture doesn't define any standard core timer to be used as
      system timer. So the system timer must be a MCU Timer.
 ******************************************************************************/
/* Inform StartOS kernel primitive that have to call EE_cpu_startos
   start-up routine */
#define __OO_CPU_HAS_STARTOS_ROUTINE__

#ifdef __MSRP__
/*******************************************************************************
                        HAL For Multicore Startup
 ******************************************************************************/

__INLINE__ void __ALWAYS_INLINE__ EE_hal_start_core( EE_TYPECOREID core_id,
  EE_ADDR start_up_addr )
{
  EE_tc_start_core(core_id, start_up_addr);
}

/** @brief HAL for core synchronization inside the Kernel */
void EE_hal_sync_barrier ( EE_TYPEBARRIER *bar,
  EE_UREG volatile * p_wait_mask );

#ifndef EE_USE_CUSTOM_STARTUP_CODE
/* EE Default Startup code used: declare Slave Cores entry points */
extern void EE_tc2Yx_cpu1_start ( void );
extern void EE_tc2Yx_cpu2_start ( void );
#endif /* !EE_USE_CUSTOM_STARTUP_CODE */
#endif /* __MSRP__ */

/*******************************************************************************
                    HAL For Primitives Synchronization
 ******************************************************************************/

/* Mask used to reset CCPN field in flags dull variable */
#define EE_TC_RESET_ICR_CCPN 0xFFFFFF00U
/* Macro used to adjust flags dull variable with new priority */
#define EE_TC_ADJUST_FLAGS_WITH_NEW_PRIO(flags, prio) \
  (((flags) & EE_TC_RESET_ICR_CCPN) | (prio))

/* Called as _first_ function of a primitive that can be called from within
 * an IRQ and from within a task. */
__INLINE__ EE_FREG __ALWAYS_INLINE__ EE_hal_begin_nested_primitive( void )
{
  return EE_hal_suspendIRQ();
}

/* Called as _last_ function of a primitive that can be called from
 * within an IRQ or a task. */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_end_nested_primitive( EE_FREG flag )
{
  EE_hal_resumeIRQ(flag);
}

/* Used to get internal CPU priority. */
__INLINE__ EE_TYPEISR2PRIO __ALWAYS_INLINE__ EE_hal_get_int_prio( void )
{
  return EE_tc_get_int_prio();
}

/* Used to set internal CPU priority. */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_set_int_prio( EE_TYPEISR2PRIO prio )
{
  EE_tc_set_int_prio(prio);
}

/*
 * Used to change internal CPU priority and return a status flags mask.
 *
 * Note:    EE_FREG param flags and return value needed only for according to
 *          HAL interface.
 */
__INLINE__ EE_FREG __ALWAYS_INLINE__ EE_hal_change_int_prio(
  EE_TYPEISR2PRIO prio, EE_FREG flags )
{
  EE_tc_set_int_prio(prio);
  return EE_TC_ADJUST_FLAGS_WITH_NEW_PRIO(flags, prio);
}

/*
 * Used to raise internal CPU interrupt priority if param new_prio is greater
 * than actual priority.
 *
 * Note:    EE_FREG param flags and return value needed only for according to
 *          HAL interface.
 */

__INLINE__ EE_FREG __ALWAYS_INLINE__ EE_hal_raise_int_prio_if_less(
  EE_TYPEISR2PRIO new_prio, EE_FREG flags )
{
  register EE_TYPEISR2PRIO prev_prio = EE_tc_get_int_prio();
  if ( prev_prio < new_prio )
  {
    EE_tc_set_int_prio(new_prio);
    /* Mask CCPN in flags and set the new one */
    flags = EE_TC_ADJUST_FLAGS_WITH_NEW_PRIO(flags, new_prio);
  }
  return flags;
}

/*
    Used to check if interrupt priority is less than new priority to be
    set.
*/
__INLINE__ EE_BIT __ALWAYS_INLINE__ EE_hal_check_int_prio_if_higher(
    EE_TYPEISR2PRIO new_prio )
{
  register EE_TYPEISR2PRIO actual_prio = EE_tc_get_int_prio();
  return (actual_prio > new_prio)? 1U: 0U;
}

#if defined(EE_AS_OSAPPLICATIONS__) || defined(EE_STACK_MONITORING__)
/* Constant boundaries of each private stack and for GLOBAL Stack */
extern struct EE_TC_BOS const EE_tc_system_bos[];
#endif /* EE_AS_OSAPPLICATIONS__ || EE_STACK_MONITORING__ */

/*******************************************************************************
                            Standard Context Handling
 ******************************************************************************/

/* Flag (OR'ed to an EE_TID) to mark a task as stacked. */
#define TID_IS_STACKED_MARK 0x80000000U

#include "cpu/common/inc/ee_context.h"

/* typically called at the end of an interrupt */
#define EE_hal_IRQ_stacked  EE_hal_endcycle_stacked
#define EE_hal_IRQ_ready    EE_hal_endcycle_ready

 /* Unmark the tid and add one to access the kernel data structures
    (tid->index).
    This is rely on implicit cast rule between signed and unsigned numbers
    valid only for two's complements integer representations,
    IS NOT PORTABLE IN NOT INTEGER TWO'S COMPLEMENT ARCHITECTURES. */
__INLINE__ EE_UTID __ALWAYS_INLINE__ EE_tc_tid_as_index( EE_TID tid )
{
  return (((EE_UTID)tid + 1U) & (~TID_IS_STACKED_MARK));
}

/*******************************************************************************
                        OO TerminateTask related stuffs
 ******************************************************************************/
#if defined(__OO_BCC1__) || defined(__OO_BCC2__) || defined(__OO_ECC1__) || \
    defined(__OO_ECC2__)

/** Save the context and call the body of the task `tid'. */
void EE_hal_terminate_savestk( EE_TID tid );

/** Restore the context saved by EE_hal_terminate_savestk() for the task `tid'
    and return from EE_hal_terminate_savestk(). */
void EE_hal_terminate_task( EE_TID tid );

#endif /* __OO_BCCx__  || __OO_ECCx__ */

/*******************************************************************************
                  ISR (and TRAPs) PCXI/Stack handling Code
 ******************************************************************************/
 
 /**
 *  This code crawl the CSA list and search the one after an IRQ or a TRAP.
 *  It makes some assumption about Interrupt and Vector tables implementations.
 */
 __INLINE__ EE_CSA * __ALWAYS_INLINE__ EE_tc_get_original_csa( void )
{
  register EE_PCXI pcxi = EE_tc_csa_get_pcxi();
  /* Flush shadow CSAs & any kind of write before access */
  EE_tc_dsync();

  /* If Interrupt Vector use a call instead of a jump
     the actual Previous Context is an Upper Context, so I have to move up
     twice in the CSA list to find the right context where TASK SP is saved.
     Uncomment the following */
  /* pcxi = EE_tc_csa_make_addr(pcxi)->next; */

  /* Move-Up from expected lower context saved */
  return EE_tc_csa_make_addr(pcxi);
}

/**
 *  This code crawl the CSA list and search the PCXI after an IRQ or a TRAP.
 *  It makes some assumption about Interrupt and Vector tables implementations.
 */
__INLINE__ EE_PCXI __ALWAYS_INLINE__ EE_tc_get_original_pcxi( void )
{
  EE_CSA_LINK res = {0U};
  EE_CSA * csa_ptr = EE_tc_get_original_csa();
  if (csa_ptr != NULL) {
    res = csa_ptr->next;
  }
  return res;
}

#ifdef __IRQ_STACK_NEEDED__

/**
 *  This code crawl the CSA list and search the Context Holding SP to be set
 *  back before scheduling. It makes some assumption about Interrupt and
 *  Vector tables implementations.
 */
__INLINE__ EE_ADDR __ALWAYS_INLINE__ EE_tc_get_prev_stack( void )
{
  /* Read saved SP */
  EE_ADDR res = (EE_ADDR)NULL;
  EE_CSA* csa_ptr = EE_tc_csa_make_addr(EE_tc_get_original_pcxi());
  if (csa_ptr != NULL) {
    res = csa_ptr->cx.ucx.A10;
  }
  return res;
}

/* Masked used to clean bit corresponding to PSW.IS in bitmask. */
#define EE_TC_PSW_IS_CLEAN_MASK (~((EE_UINT32)1U << 9U))

/* Clean PSW.IS bit in PSW register */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_psw_user_stack( void )
{
  EE_tc_set_psw((EE_UINT32)EE_tc_get_psw() & EE_TC_PSW_IS_CLEAN_MASK);
}

/**
 *  This code crawl the CSA list and search the Context Holding SP and set it
 *  back before scheduling. It makes some assumption about Interrupt and
 *  Vector tables implementations.
 */
__INLINE__ void __ALWAYS_INLINE__ EE_TC_CHANGE_STACK_POINTER
  EE_tc_set_prev_stack_back( void )
{
  /* Switch back to interrupted "User Stack" */
  EE_tc_set_SP( EE_tc_get_prev_stack( ) );
  /* Configure "User" Stack */
  EE_tc_set_psw_user_stack();
}

__INLINE__ void __ALWAYS_INLINE__ EE_TC_CHANGE_STACK_POINTER
  EE_tc_set_interrupted_stack( EE_CONST_ADDR sp )
{
  /* Switch back to interrupted "User Stack" */
  EE_tc_set_SP( sp );
  /* Configure "User" Stack */
  EE_tc_set_psw_user_stack();
}
#else /* __IRQ_STACK_NEEDED__ */
__INLINE__ EE_ADDR __ALWAYS_INLINE__ EE_tc_get_prev_stack( void )
{
  return EE_tc_get_SP();
}
/* Do not clean anything, I can't */
#define EE_TC_PSW_IS_CLEAN_MASK                     ((EE_UREG)-1)
/* If there's only one stack you must be in "Global Shared Stack" flagged by
   PSW.IS bit at true: that means, no PSW.IS clean up is allowed */
#define EE_tc_set_psw_user_stack()                  ((void)0)
/* Automatic switch to ISR stack is disabled: I'm always in "previous" stack */
#define EE_tc_set_prev_stack_back()                 ((void)0)
/* "Shared Global Stack": nothing to do */
#define EE_tc_set_interrupted_stack(isr_stack_ptr)  ((void)0)
#endif /* __IRQ_STACK_NEEDED__ */

/*******************************************************************************
          Erika internal software free running counter HAL support

  N.B. Software Free Running Timer (SWFRT) could be configured in any case, but
       for now it is tied to Timing Protection.
*******************************************************************************/
#ifdef EE_TIMING_PROTECTION__
/** CCNT has a bit that is set once until software reset:
    "obnoxious sticky bit" */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_hal_swfrt_get_current_time ( void )
{
  /* The "obnoxious sticky bit" masking is not needed here, added only because
     it makes easier read tp data structures values */
  return (EE_tc_get_CCNT() & EE_HAL_SWFRT_TIMER_DURATION);
}

__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_hal_swfrt_eval_elapsed_time (
  EE_UREG after, EE_UREG before )
{
  /* Handle the obnoxious sticky bit */
  return ((after - before) & EE_HAL_SWFRT_TIMER_DURATION);
}

__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_hal_swfrt_get_elapsed_time (
  EE_UREG before )
{
  return EE_hal_swfrt_eval_elapsed_time(EE_hal_swfrt_get_current_time(),
    before);
}
#endif /* EE_TIMING_PROTECTION__ */

/*******************************************************************************
                OS-Applications & Memory Protection Support
*******************************************************************************/
#ifdef EE_AS_OSAPPLICATIONS__
/** @brief Used by TerminateApplication to Terminate Other TASKs   */
void EE_hal_terminate_other_task( EE_TID tid );

#ifdef __EE_MEMORY_PROTECTION__
/*******************************************************************************
                      Memory Protection Internal Support
 ******************************************************************************/

/** @brief Ranges Mapping Abstraction MAcros */
/* XXX: No code protection implementation yet */
#define EE_TC_ALL_CODE_L        EE_CPU_REG_CPR0_L
#define EE_TC_ALL_CODE_U        EE_CPU_REG_CPR0_U
#define EE_TC_ALL_CODE          EE_BIT(0U)

/* XXX: Shared code have to set explicitly */
#define EE_TC_SHARED_CODE_L     EE_CPU_REG_CPR1_L
#define EE_TC_SHARED_CODE_U     EE_CPU_REG_CPR1_U
#define EE_TC_SHARED_CODE       EE_BIT(1U)

/* XXX: Protection set for Kernel */
#define EE_TC_KERNEL_R          EE_CPU_REG_DPRE_0
#define EE_TC_KERNEL_W          EE_CPU_REG_DPWE_0
#define EE_TC_KERNEL_X          EE_CPU_REG_CPXE_0

/* XXX: Protection set for OS-Applications: OS-Applications NOT mapped on
   protection sets */
#define EE_TC_OSAPPS_L          EE_CPU_REG_DPR3_L
#define EE_TC_OSAPPS_U          EE_CPU_REG_DPR3_U
#define EE_TC_OSAPPS_R          EE_CPU_REG_DPRE_1
#define EE_TC_OSAPPS_W          EE_CPU_REG_DPWE_1
#define EE_TC_OSAPPS_X          EE_CPU_REG_CPXE_1
#define EE_TC_OSAPPS            EE_BIT(3U)

/* XXX: Kernel and Trusted OSApplication can access the whole RAM */
#define EE_TC_ALL_RAM_L         EE_CPU_REG_DPR0_L
#define EE_TC_ALL_RAM_U         EE_CPU_REG_DPR0_U
#define EE_TC_ALL_RAM           EE_BIT(0U)

/* XXX: READ ONLY data are accessible by all the code */
#define EE_TC_CONST_L           EE_CPU_REG_DPR1_L
#define EE_TC_CONST_U           EE_CPU_REG_DPR1_U
#define EE_TC_CONST             EE_BIT(1U)

/* XXX: ERIKA API data */
#define EE_TC_ERIKA_API_DATA_L  EE_CPU_REG_DPR2_L
#define EE_TC_ERIKA_API_DATA_U  EE_CPU_REG_DPR2_U
#define EE_TC_ERIKA_API_DATA    EE_BIT(2U)

/* XXX: ERIKA kernel data: readable by all */
#define EE_TC_ERIKA_DATA_L      EE_CPU_REG_DPR6_L
#define EE_TC_ERIKA_DATA_U      EE_CPU_REG_DPR6_U
#define EE_TC_ERIKA_DATA        EE_BIT(6U)

/* XXX: ERIKA Multi-core kernel shared data: Accessible only by the Kernel */
#define EE_TC_ERIKA_SHARED_L    EE_CPU_REG_DPR7_L
#define EE_TC_ERIKA_SHARED_U    EE_CPU_REG_DPR7_U
#define EE_TC_ERIKA_SHARED      EE_BIT(7U)

/* XXX: Shared const have to set explicitly */
#define EE_TC_SHARED_CONST_L     EE_CPU_REG_DPR8_L
#define EE_TC_SHARED_CONST_U     EE_CPU_REG_DPR8_U
#define EE_TC_SHARED_CONST       EE_BIT(8U)

/* XXX: Range Registers 3 are reserved for OS-Application 1 in case of
   OSApplications mapped on protection sets */
#define EE_TC_OSAPP1_L          EE_CPU_REG_DPR3_L
#define EE_TC_OSAPP1_U          EE_CPU_REG_DPR3_U
#define EE_TC_OSAPP1_R          EE_CPU_REG_DPRE_1
#define EE_TC_OSAPP1_W          EE_CPU_REG_DPWE_1
#define EE_TC_OSAPP1_X          EE_CPU_REG_CPXE_1
#define EE_TC_OSAPP1            EE_BIT(3U)

/* XXX: Range Registers 4 are reserved for OS-Application 2 in case of
   OSApplications mapped on protection sets */
#define EE_TC_OSAPP2_L          EE_CPU_REG_DPR4_L
#define EE_TC_OSAPP2_U          EE_CPU_REG_DPR4_U
#define EE_TC_OSAPP2_R          EE_CPU_REG_DPRE_2
#define EE_TC_OSAPP2_W          EE_CPU_REG_DPWE_2
#define EE_TC_OSAPP2_X          EE_CPU_REG_CPXE_2
#define EE_TC_OSAPP2            EE_BIT(4U)

/* XXX: Range Registers 5 are reserved for OS-Application 3 in case of
   OSApplications mapped on protection sets */
#define EE_TC_OSAPP3_L          EE_CPU_REG_DPR5_L
#define EE_TC_OSAPP3_U          EE_CPU_REG_DPR5_U
#define EE_TC_OSAPP3_R          EE_CPU_REG_DPRE_3
#define EE_TC_OSAPP3_W          EE_CPU_REG_DPWE_3
#define EE_TC_OSAPP3_X          EE_CPU_REG_CPXE_3
#define EE_TC_OSAPP3            EE_BIT(5U)

/* XXX: Peripheral space */
#define EE_TC_PERIPHERAL_L      EE_CPU_REG_DPR15_L
#define EE_TC_PERIPHERAL_U      EE_CPU_REG_DPR15_U
#define EE_TC_PERIPHERAL        EE_BIT(15U)

/* AS Interrupt Handling Macros */

/* TC13 and TC131 cpus have PCXI.PIE in a different position respect AURIX */
#if defined(__TC13__) || defined(__TC131__)
#define EE_TC_PCXI_ICR_PIE_BIT  (1U << 23U)
#else
#define EE_TC_PCXI_ICR_PIE_BIT  (1U << 21U)
#endif

/*
   The following MACROS are used inside AS Kernel to handle bitmask
   representing the machine IRQ status.
 */

/* Initial state for ISR global variable, IRQ state value at the time
   DisableAllInterrupts() is called. */
#define EE_HAL_IRQSTATE_INVALID ((EE_FREG)EE_TC_PCXI_ICR_PIE_BIT)
/* There's nothig like a valid flag for TriCore */
#define EE_hal_set_irq_valid_flag(f) ((f) | EE_TC_PCXI_ICR_PIE_BIT)
/* Clear the IRQ flag */
#define EE_hal_clear_irq_flag(f) ((f) & ~EE_TC_PCXI_ICR_PIE_BIT)
/* Copy the IRQ status from a flag to another */
#define EE_hal_copy_irq_flag(from, to) (((to) & ~EE_TC_PCXI_ICR_PIE_BIT) | \
  ((from) & EE_TC_PCXI_ICR_PIE_BIT))

/* Name remapping for compiler Abstraction (I cannot use inline function,
   because _syscall is a macro that stringify her param, so I have to rely
   on preprocessor macro substitution) */
#ifdef __TASKING__
#define EE_tc_syscall(tin) __syscallfunc(tin)
#else
#define EE_tc_syscall(tin) _syscall(tin)
#endif

#endif /* __EE_MEMORY_PROTECTION__ */
#endif /* EE_AS_OSAPPLICATIONS__ */

#ifdef EE_TIMING_PROTECTION__
/*******************************************************************************
                    Timing Protection Internal Support
 ******************************************************************************/
/* XXX: Timing protection will use only TPS_TIMER0. In this way we will have a
        "reference implementation" more portable on multiple architectures */

/* Internal API to deal with TPS_CON Register */
#define EE_TC_TPS_CON_TTRAP         EE_BIT(16)
#define EE_TC_TPS_CON_RESET_TTRAP   (~EE_TC_TPS_CON_TTRAP)
__INLINE__ void __ALWAYS_INLINE__ EE_tc_tps_reset_ttrap ( void )
{
  /* XXX: EG: Probably ALL the following are too much, but just to be
          conservative... */
  EE_UREG tps_con = (EE_tc_get_TPS_CON() & EE_TC_TPS_CON_RESET_TTRAP);
  EE_tc_set_TPS_CON(tps_con);
}

__INLINE__ void __ALWAYS_INLINE__ EE_hal_tp_stop ( void ) {
  /* Write zero on TPS_TIMER registers stop the corresponding timer */
  EE_tc_set_csfr(EE_CPU_REG_TPS_TIMER0, 0U);
}

__INLINE__ EE_UREG __ALWAYS_INLINE__
  EE_hal_tp_get_current_time_and_pause ( void )
{
  /* Write zero on TPS_TIMER registers stop the corresponding timer ( Pause ) */
  EE_tc_set_csfr(EE_CPU_REG_TPS_TIMER0, 0U);
  return EE_hal_swfrt_get_current_time();
}

__INLINE__ void __ALWAYS_INLINE__ EE_hal_tp_set_expiration ( EE_UREG
  expiration )
{
  /* Write zero on TPS_TIMER register something != from zero restart the
     protection */
  EE_tc_set_csfr(EE_CPU_REG_TPS_TIMER0, expiration);
}
#else /* EE_TIMING_PROTECTION__ */
/* Moved in ee_as_timing_prot.h because it is directly used in
   EE_oo_ShutdownOS_internal */
/* #define EE_hal_tp_stop()                        ((void)0) */
#define EE_hal_tp_get_current_time_and_pause()  ((void)0)
#define EE_hal_tp_set_expiration(expiration)    ((void)0)
#endif /* EE_TIMING_PROTECTION__ */

/*******************************************************************************
           Enable Protection Procedure called inside EE_cpu_startos
 ******************************************************************************/
#if defined(__EE_MEMORY_PROTECTION__) || defined(EE_TIMING_PROTECTION__)
/* Set of Macro to configure SYSCON for Memory Protection */ 
/* User Code Can Access ALL Peripherals registers */
#define EE_TC_USER1_PERIPHERAL_ACCESS_AS_SUPERVISOR ( 1U << 17U )
/* Disable default behaviour for User-1 to eneble/disable interrupts */
#define EE_TC_USER1_DISABLE_INTERRUPT_HANDLING      ( 1U << 16U)
/* Define that TRAP Handlers are Safe by default (PSW.S value after a TRAP) */
#define EE_TC_TRAP_HANDLER_SAFE                     ( 1U << 4U )
/* Define that ISR are Safe by default (PSW.S value inside an ISR) */
#define EE_TC_ISR_SAFE                              ( 1U << 3U )
/* Define that temporal protection is enabled */
#define EE_TC_ENABLE_TEMPORAL_PROTECTION            ( 1U << 2U )
/* Define that memory protection is enabled */
#define EE_TC_ENABLE_MEMORY_PROTECTION              ( 1U << 1U )

/** @brief Memory and Timing protection global configuration function */
void EE_tc_enable_protections( void );
#else  /* __EE_MEMORY_PROTECTION__ || EE_TIMING_PROTECTION__ */
#define EE_tc_enable_protections()      ((void)0)
#endif /* __EE_MEMORY_PROTECTION__ || EE_TIMING_PROTECTION__ */

#ifdef EE_STACK_MONITORING__
/*******************************************************************************
                      Stack Monitoring Internal Support
 ******************************************************************************/

__INLINE__ EE_INT32 __ALWAYS_INLINE__
  EE_tc_check_stack_overflow_with_sp( EE_UREG tos, EE_ADDR sp)
{
  return ( (*((EE_STACK_T *)EE_tc_system_bos[tos].end_stack)) !=
      EE_TC_STACK_FILL_PATTERN  ) ||
    ( sp < EE_tc_system_bos[tos].end_stack ) ||
    ( sp > EE_tc_system_bos[tos].base_stack );
}

__INLINE__ EE_INT32 __ALWAYS_INLINE__ EE_TC_CHANGE_STACK_POINTER
  EE_tc_check_stack_overflow( EE_UREG tos )
{
  return EE_tc_check_stack_overflow_with_sp(tos, EE_tc_get_SP());
}
#endif /* EE_STACK_MONITORING__ */

#endif /* INCLUDE_EE_TC_INTERNAL_H__ */
