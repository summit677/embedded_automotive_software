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

/**  @file      ee_tc_as_api.c
  *  @brief     CPU-dependent part of Autosar API
  *  @author    Errico Guidieri
  *  @date      2012
  */
/* ee + Kernel API + IRQ callbacks */
#include "ee.h"
/* Internal data structures */
#include "ee_internal.h"
/* CPU Memory Protection Internal declarations */
#include "cpu/tricore/inc/ee_tc_mem_prot_internal.h"

/*                        IMPLEMENTATION NOTES:

  "Service functions" are only simple wrappers to syscall instructions with the
  same signature of "kernel implementation service functions".

  So we rely on compiler to populate parameter passing register in the way that
  TriCore EABI dictate.

  This means no-pointer parameters in d[4]-d[7] registers and pointer parameters
  in a[4]-a[7] registers in order from sx to dx in function declaration.

  (N.B. In any case we don't need parameter passing on the stack, so we
   won't bother with that).

  Inside EE_tc_syscall_handler we will guarantee that those registers won't be
  touched, so the handler has just to access to system call table and get the
  function pointer to the "kernel implementation service function" to be
  called (this information is stored in d15 register as TriCore architecture
  states).

*/

/* In case of OS-Application mapped on protection sets, set that the number of
 * User OS-Application is less than 3 (the maxium alloweb.
 * N.B OS application ID 0 is always used by the Kernel
 */
#ifdef EE_OS_APP_MAPPED_ON_PROTECTION_SETS
#if defined( EE_MAP_APP ) && ( EE_MAP_APP > 4 )
#error With OS-Application directly mapped on protection sets you cannot have\
 more than 3 OS-Applications configurated
#endif /* EE_MAP_APP > 4 */
#endif /* EE_OS_APP_MAPPED_ON_PROTECTION_SETS */

/* Put the following code in ee_kernel_text */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* Get the RA register after the syscall instruction */
__INLINE__ EE_FADDR __ALWAYS_INLINE__ EE_tc_get_syscall_RA ( void )
{
  /* Return Return Address */
  return (EE_FADDR)EE_tc_get_RA();
}

/* Get the PCXI register after a trap */
__INLINE__ EE_PCXI __ALWAYS_INLINE__ EE_tc_get_trap_PCXI ( void )
{
  /* Read actual PCXI register as EE_PCXI CSA data structure */
  register EE_PCXI pcxi = EE_tc_csa_get_pcxi();
  /* Flush shadow CSAs & any kind of write before access */
  EE_tc_dsync();
  /* Move Up once because svlcx. */
  return EE_tc_csa_make_addr(pcxi)->next;
}

/* Get the RA register after a trap */
__INLINE__ EE_FADDR __ALWAYS_INLINE__ EE_tc_get_trap_RA ( void )
{
  /* Get PCXI after a trap */
  register EE_PCXI pcxi = EE_tc_get_trap_PCXI();
  /* Get the expected lower context and access RA and return it */
  return  (EE_FADDR)EE_tc_csa_make_addr(pcxi)->cx.lcx.A11;
}

#ifdef __GNUC__
/* This disable the selected warning of unused variables only for this code
   (available in gcc from version 4.6), the warning is suppressed between
   until "#pragma GCC diagnostic pop" */
#pragma GCC diagnostic push
/* I declare some volatile registers located variables to "discourage" compiler
   to use that registers */
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif /* __GNUC__ */

#ifdef __EE_MEMORY_PROTECTION__
/******************************************************************************
            Global System Call Handler + TerminateISR2 implementation
 *****************************************************************************/

#ifdef __GNUC__
/* I don't want read or write on register variables. Just I don't want them be
   touched */
#pragma GCC diagnostic ignored "-Wvolatile-register-var"
/* d12 and d13 are initialized by The Assembler Handler (Unfortunately seem to
   not work... ) */
/* #pragma GCC diagnostic ignored "-Wuninitialized" */

/* Prepare an alias for return register */
#define EE_DECLARE_RETURN_STATUS() register StatusType status __asm ("d2")
#define EE_RETURN_STATUS() return status

#define EE_DECLARE_ISRTYPE() register ISRType isr_id __asm ("d2")
#define EE_RETURN_ISRTYPE() return isr_id

#define EE_DECLARE_APPLICATIONTYPE()  \
  register ApplicationType application_id __asm ("d2")
#define EE_RETURN_APPLICATIONTYPE() return application_id

#define EE_DECLARE_ACCESSTYPE() register AccessType access __asm ("d2")
#define EE_RETURN_ACCESSTYPE() return access

#define EE_DECLARE_OBJECTACCESSTYPE() \
  register CheckObjectAccess object_access __asm ("d2")
#define EE_RETURN_OBJECTACCESSTYPE() return object_access

#elif defined(__DCC__)

#define EE_DECLARE_RETURN_STATUS() register StatusType status
#define EE_RETURN_STATUS()  \
  do { status = EE_tc_read_return_value(); return status; } while ( 0 )

#define EE_DECLARE_ISRTYPE() register ISRType isr_id
#define EE_RETURN_ISRTYPE()  \
  do { isr_id = EE_tc_read_return_value(); return isr_id; } while ( 0 )

#define EE_DECLARE_APPLICATIONTYPE() register ApplicationType application_id
#define EE_RETURN_APPLICATIONTYPE() \
  do { application_id = EE_tc_read_return_value(); return application_id; } \
    while ( 0 )

#define EE_DECLARE_ACCESSTYPE() register AccessType access
#define EE_RETURN_ACCESSTYPE()  \
  do { access = EE_tc_read_return_value(); return access; } while ( 0 )

#define EE_DECLARE_OBJECTACCESSTYPE() register CheckObjectAccess object_access
#define EE_RETURN_OBJECTACCESSTYPE()  \
  do { object_access = EE_tc_read_return_value(); return object_access; } \
    while ( 0 )
#endif /* __GNUC__ || __DCC__ */

/* Used to cast pointer to a IRQ handling function and call it */
typedef EE_FREG (*EE_as_IRQ_handling_func_ptr)(EE_FREG);

void EE_TC_INTERRUPT_HANDER EE_TC_CHANGE_STACK_POINTER
  EE_tc_syscall_handler ( void )
{
#ifdef __GNUC__
  /* The TIN system-call param is passed through d15 register */
  register EE_TIN tin __asm ("d15");

  /* The following place holders are used to try to prevent the compiler to
     mess with parameter passing registers */
  register EE_UREG volatile data_return __asm ("d2");

  register EE_UREG volatile data_param1 __asm ("d4");
  register EE_UREG volatile data_param2 __asm ("d5");
  register EE_UREG volatile data_param3 __asm ("d6");
  register EE_UREG volatile data_param4 __asm ("d7");

  register EE_ADDR volatile pointer_param1 __asm ("a4");
  register EE_ADDR volatile pointer_param2 __asm ("a5");
  register EE_ADDR volatile pointer_param3 __asm ("a6");
  register EE_ADDR volatile pointer_param4 __asm ("a7");

  /* Previous PCXI Saved By ASM Handler (This to work around to
     uninitialized warning) */
  register EE_UREG ucx_d12 __asm("d12");
  register EE_UREG ucx_d13 __asm("d13");

  register EE_CSA_LINK original_ucx_pcxi = { .reg = ucx_d12 };
  register EE_CSA_LINK original_lcx_pcxi = { .reg = ucx_d13 };

#elif defined (__DCC__)
  /* Previous PCXI Saved By ASM Handler (This to work around to
     uninitialized warning) */
  static EE_UREG ucx_d12 __asm("d12");
  static EE_UREG ucx_d13 __asm("d13");
  /* Previous PCXI Saved By ASM Handler */
  /* DIAB compiler don't let you declare a scratch register as register
     variable... */
  static EE_UREG tin                    __asm("d14");
  /* static */ EE_CSA_LINK original_ucx_pcxi /* __asm("d12") */;
  /* static */ EE_CSA_LINK original_lcx_pcxi /* __asm("d13") */;

  original_ucx_pcxi.reg = ucx_d12;
  original_lcx_pcxi.reg = ucx_d13;
  tin = EE_tc_get_TIN();
#endif /* __GNUC__ || __DCC__ */

  /* Get the service pointer or get back in the Kernel */
  register EE_FADDR service_ptr;

#if defined(__OO_EXTENDED_STATUS__) || (!defined(EE_MM_OPT))
  /* Begin Kernel Code Range */
  extern void ee_skernel_code ( void );
  /* End Kernel Code Range */
  extern void ee_ekernel_code ( void );
  /* Begin API Code Range */
  extern void ee_sapi_code ( void );
  /* End API Code Range */
  extern void ee_eapi_code ( void );
  /* Check Who Executed The System Call: we accept call only from ee_api_text
     and ee_kernel_text sections. */
  register EE_FADDR ra = EE_tc_get_syscall_RA();
#endif /* __OO_EXTENDED_STATUS__ || !EE_MM_OPT */

  /* I do not handle stack passing parameters because I don't need them */

  /* PSW.PRS is automatically set to 0 by hardware so Set 0 of DPR is yet
     active here + PSW.IO is in supervisor mode (Done by the hardware) */
#if defined(__OO_EXTENDED_STATUS__) || (!defined(EE_MM_OPT))
  if( !((( ra >= ee_skernel_code ) && ( ra <= ee_ekernel_code )) ||
        (( ra >= ee_sapi_code ) && ( ra <= ee_eapi_code ))) )
  {
    /* Wrong syscall call site: call the ProtectionHook */
    EE_as_call_protection_error(EE_as_active_app, E_OS_PROTECTION_EXCEPTION);
    /* the following won't be called: added just in case */
    EE_tc_restore_os_app(EE_as_active_app);
  } else
#endif /* __OO_EXTENDED_STATUS__ || !EE_MM_OPT */
  {
    /* Pause active TP... !!!WARNING!!! I CANNOT do this here...
       This will clutter scratch registers needed for syscalls parameters */
    /* EE_as_tp_active_pause_and_update_budgets(); */
    /* We need to set back "User Stack" Stack Before execute a service.
       It may generate a Context Switch, so I need to be in User Stack.
       Do This before anything else: if generated code WOULD hit the stack it
       SHALL be TASK Stack */
    /* Flush Shadow CSA Accesses */
    EE_tc_dsync();
#ifdef EE_AS_KERNEL_STACKS__
    {
      /* Handle kernel stack switching */
      register EE_UREG calling_tos;
      if ( EE_IRQ_nesting_level > 0U ) {
        calling_tos = EE_as_Application_ROM[app_from].ISRTOS;
      } else {
        /* I'm in a TASK this primitive can cause rescheduling I have to save
           interrupted TOS value on system TOS */
        calling_tos = EE_tc_active_tos;
        EE_tc_system_tos[calling_tos].ram_tos =
          EE_tc_csa_make_addr(original_ucx_pcxi)->cx.ucx.A10;
      }
      /* Switch to "Kernel Stack" */
      EE_tc_set_SP( EE_tc_system_tos[calling_tos].kernel_ram_tos );
    }
#else /* EE_AS_KERNEL_STACKS__ */
    /* Restore calling Stack */
    EE_tc_set_interrupted_stack(EE_tc_csa_make_addr(original_ucx_pcxi)->cx.
      ucx.A10);
#endif /* EE_AS_KERNEL_STACKS__ */
    /* EE_MAX_SYS_SERVICEID is EE_ID_CallTrustedFunction that have to be
       handled in a special way */
    /* A TRUST FUNCTION CALL is executed as if the original OS-Application
       were active */
    /* [OS266] When CallTrustedFunction() calls the function <FunctionIndex>,
        that function shall be executed with the same processor mode and memory
        protection boundaries as the OS-Application to which it belongs.
        It shall however retain the service protection limitations of
        the calling Task or ISR, and the notion of "current application" shall
        remain that of the calling Task or Category 2 ISR. */
    /* MAYBE TODO: Add data structure to get information to which OS-Application
       the TRUSTED function belong to to set the right protection set */
    if ( tin < EE_MAX_SYS_SERVICEID ) {
      service_ptr = EE_syscall_table[tin];
      /* If the tin correspond to a IRQ handling service I have to handle it in
         a special way  */
      if( tin <= EE_ID_interrupts_last ) {
        /* Let the return be the new pcxi value */
        original_ucx_pcxi.reg =
          ((EE_as_IRQ_handling_func_ptr)service_ptr)( original_ucx_pcxi.reg );

        /* Write the new manipulated PCXI on the original CSA */
        EE_tc_csa_make_addr(original_lcx_pcxi)->next = original_ucx_pcxi;
        /* Make sure that the write on CSA will be flushed */
        EE_tc_dsync();

        /* Restore Original Protection Set */
        EE_tc_restore_os_app(EE_as_active_app);
      } else
#if defined(EE_MAX_ISR2) && (EE_MAX_ISR2 > 0)
      if ( tin == EE_ID_TerminateISR2 ) {
        /* Monitor the stack before entering TerminateISR Function (In case that
           we'll be called by EE_as_handle_protection_error) */
        EE_as_check_and_handle_stack_overflow(EE_as_active_app,
          EE_as_Application_ROM[EE_as_active_app].ISRTOS);
        /* Terminate the ISR2 */
        EE_as_TerminateISR2();
      } else
#endif /* EE_MAX_ISR2 > 0 */
      {
        /* The service function parameters are already in the right place so
           I can just do the CALL */
        service_ptr();

        /* Save return value after the service call on private lower saved lower
           context */
        EE_tc_csa_make_addr(original_lcx_pcxi)->cx.lcx.D2 =
          EE_tc_read_return_value();
        /* Make sure that the write on CSA will be flushed */
        EE_tc_dsync();

        /* Restore Original Protection Set */
        EE_tc_restore_os_app(EE_as_active_app);
      }
    }
    /* else Called just to get back in Kernel OS-Application */
    /* Restart active TP... !!!WARNING!!! Done in syscall service..
       I CANNOT Pause and Restart TP here, because that will clutter scratch
       registers needed for syscalls parameters */
    /* EE_as_tp_active_update_budgets_and_restart(); */
  }
  /* I DON'T NEED TO EXPLICITLY RESTORE CALLING ISR2/TASK USER STACK,
     IT WILL BE CONTEXT RESTORING TO DO THAT FOR ME */
  EE_TC_RFE();
}
#endif /* __EE_MEMORY_PROTECTION__ */

#if defined(__EE_MEMORY_PROTECTION__) && defined(__EE_USE_MMU__)
void EE_TC_INTERRUPT_HANDER EE_tc_mmu_handler ( EE_TIN tin )
{
  /* TODO: when will be added support for a TriCore Family with MMU */
  EE_TC_RFE();
}
#endif /* __EE_MEMORY_PROTECTION__ && __EE_USE_MMU__ */

#if defined(__EE_MEMORY_PROTECTION__)
void EE_TC_INTERRUPT_HANDER EE_tc_protection_handler ( EE_TIN tin )
{
  /*  Macros for Protection Class Trap Numbers:
   *  TIN Name Sync/Async  H/S      Description
   *  1   PRIV Synchronous Hardware Privileged Instruction
   *  2   MPR  Synchronous Hardware Memory Protection: Read Access
   *  3   MPW  Synchronous Hardware Memory Protection: Write Access
   *  4   MPX  Synchronous Hardware Memory Protection: Execution Access
   *  5   MPP  Synchronous Hardware Memory Protection: Peripheral Access
   *  6   MPN  Synchronous Hardware Memory Protection: Null Address
   *  7   GRWP Synchronous Hardware Global Register Write Protection
   */
  /* Set a break point in debug */
  EE_BREAK_POINT();
  switch ( tin ) {
    case EE_TRAPPROT_MPR:
    case EE_TRAPPROT_MPW:
    case EE_TRAPPROT_MPN:
      EE_as_call_protection_error(EE_as_active_app, E_OS_PROTECTION_MEMORY);
      break;
    case EE_TRAPPROT_PRIV:
    case EE_TRAPPROT_MPX:
    case EE_TRAPPROT_MPP:
    case EE_TRAPPROT_GRWP:
      EE_as_call_protection_error(EE_as_active_app,
        E_OS_PROTECTION_EXCEPTION);
      break;
    default:
      /* Unexpected: set a break point in debug */
      EE_BREAK_POINT();
      EE_as_call_protection_error(EE_as_active_app,
        E_OS_PROTECTION_EXCEPTION);
      break;
  }
  EE_TC_RFE();
}
#endif /* __EE_MEMORY_PROTECTION__ */

#if defined(EE_TIMING_PROTECTION__)
void EE_TC_INTERRUPT_HANDER EE_tc_bus_handler ( EE_TIN tin )
{
  /*  Macros for System Bus and Peripheral Errors Class Trap Numbers:
   *  TIN Name Sync/Async   H/S      Description
   *  1   PSE  Synchronous  Hardware Program fetch bus error
   *  2   DSE  Synchronous  Hardware Data access bus error
   *  3   DAE  Asynchronous Hardware Data access bus error
   *  4   CAE  Asynchronous Hardware Coprocessor Trap Asynchronous (TriCore 1.6)
   *  5   PIE  Synchronous  Hardware Program Memory Integrity Error.
   *  6   DIE  Asynchronous Hardware Data Memory Integrity Error (TriCore 1.6)
   *  7   TAE  Asynchronous Hardware Temporal Asynchronous Error
   */

  /* Actual PCXI is the one after svlcx: this handler is not called but
     jumped into. */
  EE_PCXI pcxi;
  switch ( tin ) {
    /* TPS_TIMERX trap: timing protection budget expired */
    case EE_TRAPBUS_TAE:
      /* I need previous PCXI register to check PIE bit */
      pcxi = EE_tc_get_trap_PCXI();
      /* Write zero on TPS_TIMER to reset TRAP request... */
      EE_tc_set_csfr(EE_CPU_REG_TPS_TIMER0, 0U);
      /* And update reset TTRAP to re-enable the TP. */
      EE_tc_tps_reset_ttrap();
      /* XXX: There is a small chance of race condition between TP and the
          beginning of a ISR2, following code handle that.
          If the (PCXI.PIE == 0) && (EE_as_tp_active.active_tp <=
          EE_RESOURCE_LOCK_BUDGET) means that we was entering in an ISR2, we
          just postpone the budget expiration handling at the end of
          EE_tc_isr2_wrapper_body or EE_as_TerminateISR2.
          If the active tp is invalid means that we are here because Time
          Frame Reclamation budget: it is always harmless handle that. */
      if ( (EE_as_tp_active.active_tp == INVALID_TIMING_PROTECTION) ||
          (pcxi.bits.PIE != 0U) || (EE_as_tp_budget_confs[EE_as_tp_active.
            active_tp_RAM_ref->first_expiring].budget_type >
              EE_RESOURCE_LOCK_BUDGET) )
      {
        EE_as_tp_active_budget_expired();
      }
    break;

    case EE_TRAPBUS_PSE:
    case EE_TRAPBUS_DSE:
    case EE_TRAPBUS_DAE:
    case EE_TRAPBUS_CAE:
    case EE_TRAPBUS_PIE:
    case EE_TRAPBUS_DIE:
      /* Set a break point in debug */
      EE_BREAK_POINT();
      EE_as_call_protection_error(EE_as_active_app,
        E_OS_PROTECTION_EXCEPTION);
      break;
    default:
      /* Unexpected: set a break point in debug */
      EE_BREAK_POINT();
      EE_as_call_protection_error(EE_as_active_app,
        E_OS_PROTECTION_EXCEPTION);
      break;
  }
  EE_TC_RFE();
}
#endif /* EE_TIMING_PROTECTION__ */

/* The default TRAP handler is just a normal function because we call it and
   not just jump on it.
   We do that because in this way we have a call stack that make us better
   understand which TRAP happened */
void EE_tc_default_trap_handler ( EE_TIN tin )
{
#ifdef EE_DEBUG
  /* Read TRAP return address */
  EE_FADDR volatile ra = EE_tc_get_trap_RA();
  /* Set a break point in debug */
  EE_BREAK_POINT();
#endif /* EE_DEBUG */
  /* [OS245]: If an instruction exception occurs (e.g. division by zero)
      the Operating System module shall call the protection hook with
      E_OS_PROTECTION_EXCEPTION. (BSW11011) */
  /* Just call the protection Hook */
  EE_as_call_protection_error(EE_as_active_app, E_OS_PROTECTION_EXCEPTION);
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif /* __GNUC__ */

/* Stop ee_kernel_text section */
#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef __EE_MEMORY_PROTECTION__
/******************************************************************************
                            System Services API
 ******************************************************************************/

/* The following code belong to ERIKA API section ee_api_text */
#define API_START_SEC_CODE
#include "MemMap.h"

/***************************************************************************
 * 13.3 Interrupt handling 
 ***************************************************************************/

/* 13.3.2.1: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_ENABLEALLINTERRUPTS__
void EnableAllInterrupts( void )
{
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    EE_oo_EnableAllInterrupts();
  } else {
    EE_tc_syscall(EE_ID_EnableAllInterrupts);
  }
}
#endif /* ! __PRIVATE_ENABLEALLINTERRUPTS__ */

/* 13.3.2.2: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_DISABLEALLINTERRUPTS__
void DisableAllInterrupts( void )
{
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    EE_oo_DisableAllInterrupts();
  } else {
    EE_tc_syscall(EE_ID_DisableAllInterrupts);
  }
}
#endif /* ! __PRIVATE_DISABLEALLINTERRUPTS__ */

/* 13.3.2.3: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_RESUMEALLINTERRUPTS__
void ResumeAllInterrupts( void )
{
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    EE_oo_ResumeAllInterrupts();
  } else {
    EE_tc_syscall(EE_ID_ResumeAllInterrupts);
  }
}
#endif /* ! __PRIVATE_RESUMEALLINTERRUPTS__ */

/* 13.3.2.4: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_SUSPENDALLINTERRUPTS__
void SuspendAllInterrupts( void )
{
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    EE_oo_SuspendAllInterrupts();
  } else {
    EE_tc_syscall(EE_ID_SuspendAllInterrupts);
  }
}
#endif /* ! __PRIVATE_SUSPENDALLINTERRUPTS__ */

/* 13.3.2.5: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_RESUMEOSINTERRUPTS__
void ResumeOSInterrupts( void )
{
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    EE_oo_ResumeOSInterrupts();
  } else {
    EE_tc_syscall(EE_ID_ResumeOSInterrupts);
  }
}
#endif /* ! __PRIVATE_RESUMEOSINTERRUPTS__ */

/* 13.3.2.6: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_SUSPENDOSINTERRUPTS__
void SuspendOSInterrupts( void )
{
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    EE_oo_SuspendOSInterrupts();
  } else {
    EE_tc_syscall(EE_ID_SuspendOSInterrupts);
  }
}
#endif /* ! __PRIVATE_SUSPENDOSINTERRUPTS__ */

/***************************************************************************
 * 13.2 Task management 
 ***************************************************************************/

/* 13.2.3 System services                                                  */
/* ----------------------------------------------------------------------- */

/* 13.2.3.1: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_ACTIVATETASK__
StatusType ActivateTask(TaskType TaskID)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_ActivateTask(TaskID);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_ActivateTask);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_ACTIVATETASK__ */

/* 13.2.3.2: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_TERMINATETASK__
StatusType TerminateTask(void)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_TerminateTask();
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_TerminateTask);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_TERMINATETASK__ */

/* 13.2.3.3: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_CHAINTASK__
StatusType ChainTask(TaskType TaskID)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_ChainTask(TaskID);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_ChainTask);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_CHAINTASK__ */

/* 13.2.3.4: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_SCHEDULE__
StatusType Schedule(void)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_Schedule();
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_Schedule);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_SCHEDULE__ */

/* Needed for counters (see later) */
#ifndef __PRIVATE_FORCESCHEDULE__
StatusType ForceSchedule(void)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_ForceSchedule();
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_ForceSchedule);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_FORCESCHEDULE__ */

/***************************************************************************
 * 13.4 Resource management 
 ***************************************************************************/

#ifndef __OO_NO_RESOURCES__

/* 13.4.3 System services                                                  */
/* ----------------------------------------------------------------------- */

/* 13.4.3.1: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_GETRESOURCE__
StatusType GetResource(ResourceType ResID)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_GetResource(ResID);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_GetResource);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_GETRESOURCE__ */

/* 13.4.3.2: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_RELEASERESOURCE__
StatusType ReleaseResource(ResourceType ResID)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_ReleaseResource(ResID);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_ReleaseResource);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_RELEASERESOURCE__ */

#endif /* !__OO_NO_RESOURCES__ */

/***************************************************************************
 * 13.5 Event control 
 ***************************************************************************/

/* 13.5.3 System services                                                  */
/* ----------------------------------------------------------------------- */

/* see also internal.h */
/* 13.5.3.1: ECC1, ECC2 */
#if defined(__OO_ECC1__) || defined(__OO_ECC2__)

#ifndef __PRIVATE_SETEVENT__
StatusType SetEvent(TaskType TaskID, EventMaskType Mask)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_SetEvent(TaskID, Mask);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_SetEvent);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_SETEVENT__ */

/* 13.5.3.2: ECC1, ECC2 */
#ifndef __PRIVATE_CLEAREVENT__
StatusType ClearEvent(EventMaskType Mask)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_ClearEvent(Mask);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_ClearEvent);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_CLEAREVENT__ */

/* 13.5.3.4: ECC1, ECC2 */
#ifndef __PRIVATE_WAITEVENT__
StatusType WaitEvent(EventMaskType Mask)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_WaitEvent(Mask);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_WaitEvent);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_WAITEVENT__ */

#endif /* __OO_ECC1__ || __OO_ECC2__ */

/***************************************************************************
 * 13.6 Alarms 
 ***************************************************************************/

#ifndef __OO_NO_ALARMS__

/* Implementation defined functions */

/* Counter initialization
   ----------------------

   Counter initialization is done at initialization time OUTSIDE
   StartOS.  Since software counters are stored in RAM, their
   initialization values are typically set to 0. That also conforms to
   note 10 page 43 of the specification version 2.2.1 that says:

   "Counters are - if possible - set to zero by the system
   initialization before alarms are autostarted. Exception: calendar
   timers, etc. For autostarted alarms, all values are relative
   values.

*/

/*
   AS 4.0 OS SWS 8.4.16
   AS OS requirement OS399:

   This function notifies a tick to a counter. That is, the counter is
   incremented by 1.

   The function will also implement the notification of expired alarms
   (calling an alarm callback, setting an event, or activating a
   task). 

   And DO RESCHEDULING.
   see also internal.h
*/
#if (!defined(__PRIVATE_INCREMENTCOUNTER__)) && (EE_MAX_COUNTER > 0U)
StatusType IncrementCounter(CounterType CounterID)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_IncrementCounter(CounterID);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_IncrementCounter);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif  /* __PRIVATE_INCREMENTCOUNTER__ */

#ifndef __PRIVATE_GETALARMBASE__
StatusType GetAlarmBase(AlarmType AlarmID, AlarmBaseRefType Info) {
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_GetAlarmBase(AlarmID, Info);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_GetAlarmBase);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_GETALARMBASE__ */

/* 13.6.3.2 BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_GETALARM_
StatusType GetAlarm(AlarmType AlarmID, TickRefType Tick) {
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_GetAlarm(AlarmID, Tick);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_GetAlarm);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_GETALARM_ */

/* 13.6.3.3 BCC1, BCC2, ECC1, ECC2; Events only ECC1, ECC2 */
#ifndef __PRIVATE_SETRELALARM__
StatusType SetRelAlarm(AlarmType AlarmID, TickType increment, TickType cycle)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_SetRelAlarm(AlarmID, increment, cycle);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_SetRelAlarm);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_SETRELALARM__ */

/* 13.6.3.4 BCC1, BCC2, ECC1, ECC2; Events only ECC1, ECC2 */
#ifndef __PRIVATE_SETABSALARM__
StatusType SetAbsAlarm(AlarmType AlarmID, TickType start, TickType cycle)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_SetAbsAlarm(AlarmID, start, cycle);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_SetAbsAlarm);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_SETABSALARM__ */

/* 13.6.3.5 BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_CANCELALARM__
StatusType CancelAlarm(AlarmType AlarmID)
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_oo_CancelAlarm(AlarmID);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_CancelAlarm);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* __PRIVATE_CANCELALARM__ */

#endif /* !__OO_NO_ALARMS__ */

/***************************************************************************
 * 13.7 Operating system execution control 
 ***************************************************************************/

/* 13.7.2.3: BCC1, BCC2, ECC1, ECC2 */
#ifndef __PRIVATE_SHUTDOWNOS__
void ShutdownOS(StatusType Error)
{
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    EE_oo_ShutdownOS(Error);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_ShutdownOS);
  }
}
#endif /* __PRIVATE_SHUTDOWNOS__ */

#ifdef EE_AS_SCHEDULETABLES__
/*******************************************************************************
 *                             Schedule Tables API
 ******************************************************************************/
StatusType StartScheduleTableRel( ScheduleTableType ScheduleTableID,
  TickType Offset )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_StartScheduleTableRel(ScheduleTableID, Offset);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_StartScheduleTableRel);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}

StatusType StartScheduleTableAbs( ScheduleTableType ScheduleTableID,
  TickType Start )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_StartScheduleTableAbs(ScheduleTableID, Start);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_StartScheduleTableAbs);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}

StatusType StopScheduleTable( ScheduleTableType ScheduleTableID )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_StopScheduleTable(ScheduleTableID);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_StopScheduleTable);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}

/* The following primitives are available only on local objects */
#if defined(EE_MAX_SCHEDULETABLE) && (EE_MAX_SCHEDULETABLE > 0)

StatusType NextScheduleTable( ScheduleTableType ScheduleTableID_From,
  ScheduleTableType ScheduleTableID_To )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_NextScheduleTable(ScheduleTableID_From, ScheduleTableID_To);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_NextScheduleTable);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}

StatusType SyncScheduleTable( ScheduleTableType ScheduleTableID,
  TickType Value )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_SyncScheduleTable(ScheduleTableID, Value);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_SyncScheduleTable);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* EE_MAX_SCHEDULETABLE > 0 */
#endif /* EE_AS_SCHEDULETABLES__ */

#ifdef EE_AS_OSAPPLICATIONS__
/*******************************************************************************
                        Autosar Kernel Functions
 ******************************************************************************/

StatusType TerminateApplication( ApplicationType Application,
  RestartType RestartOption )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_TerminateApplication(Application, RestartOption);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_TerminateApplication);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}

StatusType AllowAccess ( void )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_AllowAccess();
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_AllowAccess);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}

#if defined(EE_SYSCALL_NR) && defined(EE_MAX_SYS_SERVICEID) &&\
  (EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID)
StatusType CallTrustedFunction( TrustedFunctionIndexType FunctionIndex,
  TrustedFunctionParameterRefType FunctionParams )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_CallTrustedFunction(FunctionIndex, FunctionParams);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_CallTrustedFunction);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* EE_SYSCALL_NR > EE_MAX_SYS_SERVICEID */
#endif /* EE_AS_OSAPPLICATIONS__ */

#ifdef __OO_HAS_ERRORHOOK__
void EE_os_notify_error_from_us( OSServiceIdType ServiceID,
  const EE_oo_ErrorHook_parameters * const error_parameters_ref, StatusType Error )
{
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    EE_oo_notify_error_from_us_internal(ServiceID, error_parameters_ref, Error);
    EE_hal_end_nested_primitive(flag);
  } else {
    EE_tc_syscall(EE_ID_notify_error_from_us_internal);
  }
}
#endif /* __OO_HAS_ERRORHOOK__ */

#if defined(EE_AS_USER_SPINLOCKS__)  && (!defined(EE_PRIVATE_SPINLOCK__))
StatusType GetSpinlock( SpinlockIdType SpinlockId )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_GetSpinlock(SpinlockId);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_GetSpinlock);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}

StatusType ReleaseSpinlock( SpinlockIdType SpinlockId )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_ReleaseSpinlock(SpinlockId);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_ReleaseSpinlock);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}

StatusType TryToGetSpinlock( SpinlockIdType SpinlockId,
  TryToGetSpinlockType* Success )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_TryToGetSpinlock(SpinlockId, Success);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_TryToGetSpinlock);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* EE_AS_USER_SPINLOCKS__ && !EE_PRIVATE_SPINLOCK__ */

#ifdef EE_AS_RPC__
void ShutdownAllCores( StatusType Error ) {
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    EE_as_ShutdownAllCores(Error);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_ShutdownAllCores);
  }
}

StatusType EE_as_rpc_from_us( OSServiceIdType ServiceId,
  EE_os_param param1, EE_os_param param2, EE_os_param param3 )
{
  /* Prepare an alias for return register */
  EE_DECLARE_RETURN_STATUS();
  if ( EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_TRUST_MODE ) {
    register const EE_FREG flag = EE_hal_begin_nested_primitive();
    status = EE_as_rpc(ServiceId, param1, param2, param3);
    EE_hal_end_nested_primitive(flag);
  } else {
    /* make the system call */
    EE_tc_syscall(EE_ID_rpc);
  }
  /* just forward d2 as return value */
  EE_RETURN_STATUS();
}
#endif /* EE_AS_RPC__ */

#endif /* __EE_MEMORY_PROTECTION__ */


