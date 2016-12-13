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

 /** @file      ee_tc_cpu.h
  *  @brief     CPU-dependent part of HAL
  *  @author    Errico Guidieri
  *  @date      2012
  */

/* Infineon modifications, integration to Infineon Build environment:
 * Fixes for compilation issues with Tasking Compiler:
 * Updates for compilation issues with Dcc Compiler:
 * Usage of generic compiler independent register header file:
 * Author: 
 *         Ashok Abbi, <Ashok.Abbi@infineon.com> 18.07.2013
 */

#ifndef INCLUDE_EE_TC_CPU_H__
#define INCLUDE_EE_TC_CPU_H__

/*******************************************************************************
                          HAL Types and structures
 ******************************************************************************/
#if defined(__TC13__) || defined(__TC131__) || defined(__TC161__) || \
    defined(__CORE_TC16X__)
/* All defines needed by kernel too */
#include "eecfg.h"
#include "ee_tc_cpu_reg.h"
#ifdef __GNUC__

/* GNUC Intrinsic functions */
#include <machine/intrinsics.h>
/* GNUC compiler inclusion */
#include "cpu/common/inc/ee_compiler_gcc.h"

/* HELP Macros to generate the right MCU SFR inclusion */
#define EE_INCLUDE_MCU_REGS2(DIR, FILE) EE_PREPROC_STRING(mcu/DIR/inc/FILE)

#define EE_TC_MCU_DIR(CPU)        infineon_ ## CPU
#define EE_TC_MCU_SFR_FILE(CPU)   EE_PREPROC_JOIN(\
  ee_ ## CPU, _gnu_sfr.h)

#define EE_INCLUDE_MCU_REGS1(CPU) EE_INCLUDE_MCU_REGS2(\
  EE_TC_MCU_DIR(CPU), EE_TC_MCU_SFR_FILE(CPU))

#define EE_INCLUDE_MCU_REGS       EE_INCLUDE_MCU_REGS1(__CPU__)

/* MCU SFR inclusion */
/* #include EE_INCLUDE_MCU_REGS */

/* The above solution: smart and clean, cannot be used with "ERIKA flattening"
   and for MISRA Compliance. So
   TODO: Add some horrible switch to select the right file for registers in
   case of multiple MCUs support */
#ifndef EE_NO_SFR
#ifdef EE_TC27X__
#ifndef EE_MM_OPT

#ifdef EE_APPKIT_TC2X5
#include <tc27xc/Ifx_reg.h>
#elif defined(EE_APPKIT_TC2X7)
#include <tc29xb/Ifx_reg.h>
#else
#include <tc27xa/Ifx_reg.h>
#endif
#else /* !EE_MM_OPT */
#include "Ifx_reg.h"
#endif /* !EE_MM_OPT */
#elif defined(EE_TC26X__)
/* Hightec Tricore compiler has a bugged support for tc26xx mcpu */
/* #include <tc26xx/Ifx_reg.h> */
#include "mcu/infineon_tc26x/sfr/Ifx_reg.h"
#elif defined(EE_TC29X__)
#if defined(EE_APPKIT_TC2X7)
#include <tc29xb/Ifx_reg.h>
#else
#include <tc29xa/Ifx_reg.h>	
#endif
#endif /* EE_TC27X__ || EE_TC26X__ || EE_TC29X__*/
#endif /* EE_NO_SFR */

/* Macros to abstract mfcr & mtcr intrinsic functions */
#define EE_tc_get_csfr(reg_id) __mfcr(reg_id)

/* When used this isync have to be put by hand */
#define EE_tc_set_csfr(reg_id, reg) __mtcr((reg_id), (reg));

#elif defined (__TASKING__)
/* This let you include right SFR (Special Function Registers).
   1.3.2. Accessing Hardware from C */
#ifndef EE_NO_SFR
#define   EE_INCLUDE_MCU_REGS __SFRFILE__(__CPU__)
#include  EE_INCLUDE_MCU_REGS
#endif /* EE_NO_SFR */
#include "cpu/common/inc/ee_compiler_tasking.h"

/* Macros to abstract mfcr & mtcr intrinsic functions */
#define EE_tc_get_csfr(reg_id) __mfcr(reg_id)

/* When used this isync have to be put by hand */
#define EE_tc_set_csfr(reg_id, reg) __mtcr((reg_id), (reg));

#elif defined (__DCC__)
/* DIAB MCU SFR inclusion. TODO: handle this better */
#define  EE_INCLUDE_MCU_REGS <sfr/TC27x/Ifx_reg.h>
#include EE_INCLUDE_MCU_REGS

/* DCC Intrinsic functions */
#include <diab/tcasm.h>
#include "cpu/common/inc/ee_compiler_diab.h"

/* Macros to abstract mfcr & mtcr intrinsic functions */
#define EE_tc_get_csfr(reg_id) _mfcr(reg_id)

/* When used this isync have to be put by hand */
#define EE_tc_set_csfr(reg_id, reg) _mtcr((reg_id), (reg));

#else
#error Unsupported compiler!
#endif

/* Primitive data types */
#include "cpu/common/inc/ee_types.h"

/* Remote TID, to be used with Remote notifications */
#define EE_REMOTE_TID   0x80000000U

/* Stack Alignment Macros */
#define EE_STACK_ALIGN          0xFFFFFFF8U
#define EE_STACK_ALIGN_SIZE     8U

/******************************************************************************
                        Useful Generic Defines
 ******************************************************************************/
#define EE_KILO         1000U
#define EE_MEGA         1000000U
/* Single bit bitmask generator. */
#define EE_BIT(bit)     (1U << ((EE_UREG)(bit)))

/* Define HAL types */
typedef EE_UINT32 EE_UREG;
typedef EE_INT32  EE_SREG;
typedef EE_UINT32 EE_FREG;

/* Thread IDs */
typedef EE_INT32 EE_TID;

/* Thread IDs - unsigned version*/
typedef EE_UINT32 EE_UTID;

/* ISR Priority representation type */
typedef EE_UINT8 EE_TYPEISR2PRIO;

/* Type pointing to an ISR (Here because ISR macro code generation) */
typedef void (*EE_tc_ISR_handler)(void);

#ifdef __OO_ORTI_RUNNINGISR2__
/* ISR2 ORTI types and NO_ISR2 value define */
typedef EE_tc_ISR_handler EE_ORTI_runningisr2_type;
#define EE_NO_ISR2  ((EE_ORTI_runningisr2_type)0)
#if defined(__OO_ORTI_USE_OTM__)
__INLINE__ void __ALWAYS_INLINE__
  EE_ORTI_send_otm_runningisr2(EE_ORTI_runningisr2_type isr2)
{
  /* TODO: If possible/needed: add OTM messages */
}
#else /* __OO_ORTI_USE_OTM__ */
#define EE_ORTI_send_otm_runningisr2(isr2)    ((void)0)
#endif /* __OO_ORTI_USE_OTM__ */
#endif /* __OO_ORTI_RUNNINGISR2__ */

/*******************************************************************************
                    Data Structures for Multi-Stack
  They must be visible in API because eecfg.c ERIKA configuration module define
  these data multi-stack structures so struct definition need to be seen by
  this file. (As reminder I point that eecfg.c include ee.h API collector).
 ******************************************************************************/
#ifdef __MULTI__

/* Used to save "context info" for multistack switch */
struct EE_TC_TOS {
  EE_ADDR ram_tos;        /* Top of the Stack in RAM */
  EE_UREG pcxi_tos;       /* Top of the Stack in CSA */
#ifdef EE_AS_OSAPPLICATIONS__
  EE_UREG pcxi_bos;       /* Bottom of Stack in CSA */
#ifdef EE_AS_KERNEL_STACKS__
  EE_ADDR kernel_ram_tos; /* Top of the Kernel Stack in RAM */
#endif /* EE_AS_KERNEL_STACKS__ */
#endif /* EE_AS_OSAPPLICATIONS__ */
};

/* Stack Entry Type (I use uint32 to match the type of the filler pattern,
   otherwise I would have used uint8) */
typedef EE_UINT32 EE_STACK_T;

/* Used to initialize stack arrays with the right size. */
#define EE_STACK_WLEN(size) (((((EE_UINT32)size) + EE_STACK_ALIGN_SIZE) - 1U) \
  / sizeof(EE_STACK_T))

/* Used to initialize TOS structures. TriCore EABI states that stacks grows
   towards small addresses if I know that the base is alligned (as I will
   enforce with pragmas) so I only need that "index" is aligned to 8 */
#define EE_STACK_INITP(stack) \
  ((EE_ADDR)&stack[((sizeof(stack)/sizeof(stack[0])) - 1U) & EE_STACK_ALIGN])

/* Used to initialize TOS structures. TriCore EABI states that stacks grows
   towards small addresses so the end of the stack is the begin of the
   memory buffer holding it */
#define EE_STACK_ENDP(stack)  ((EE_ADDR)&stack[0U])

/* Used to place ERIKA Stacks in right section for memory protection and ORTI
   Stack filling, and handling stack alignment */
#define EE_STACK_ATTRIB EE_COMPILER_ALIGN(EE_STACK_ALIGN_SIZE) \
  EE_COMPILER_SECTION("ee_kernel_stack")

/*******************************************************************************
                Multistack Data Structures for Context Handling
 ******************************************************************************/
/*
 * These structures are used by the Multistack HAL to contain the
 * information about a "stack". This type is used internally by the HAL, but I
 * cannot move them to ee_tc_internal.h otherwise definition in eecfg.c won't
 * see correct definition in case of Memory Mapping active.
 */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_AS_OSAPPLICATIONS__
/* The following variables belong to ERIKA API section: ee_kernel_bss */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */

/* Index of the current stack. */
extern EE_UREG EE_tc_active_tos;

/* Used in common context */
#define EE_hal_active_tos EE_tc_active_tos

/* Used to hold the Tasks return addresses (that can be lost in context
   switches) */
#ifdef EE_AS_KERNEL_STACKS__
typedef struct EE_tc_task_save_data_type {
  EE_ADDR   ra;         /* Task Return Address */
  EE_ADDR   kernel_sp;  /* Task Kernel SP to be set in kernel TOS */
} EE_tc_task_save_data;
#else /* EE_AS_KERNEL_STACKS__ */
typedef EE_ADDR EE_tc_task_save_data;
#endif /* EE_AS_KERNEL_STACKS__ */

extern EE_tc_task_save_data EE_tc_tasks_RA[];

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_AS_OSAPPLICATIONS__
#define OS_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_AS_OSAPPLICATIONS__
/* The following variables belong to ERIKA API section: ee_kernel_data */
#define OS_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */

/* Top of each private stack. */
extern struct EE_TC_TOS EE_tc_system_tos[];

/* Stack used by IRQ handlers */
#ifdef __IRQ_STACK_NEEDED__
/* This could be declared 'const', but to keep RT-Druid behave as usual
   is NOT declared as const */
extern struct EE_TOS EE_tc_IRQ_tos;
#endif /* __IRQ_STACK_NEEDED__ */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_AS_OSAPPLICATIONS__
#define OS_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */

#endif /* __MULTI__ */

/*******************************************************************************
                          Stack Monitoring Utilities
 ******************************************************************************/
/* Structure that hold boundaries of a stack */
struct EE_TC_BOS {
  EE_ADDR base_stack;  /* Base Of the Stack in RAM (Highest Address) */
  EE_ADDR end_stack;   /* End Of the Stack in RAM (Lowest Address) */
};

/* Fill Pattern Used for Stack Monitoring */
#ifndef EE_TC_STACK_FILL_PATTERN
#define EE_TC_STACK_FILL_PATTERN 0xA5A5A5A5U
#endif /* EE_TC_STACK_FILL_PATTERN */

#if ((defined(__OO_ORTI_STACK__) || defined(EE_STACK_MONITORING__)) &&\
  defined(EE_EXECUTE_FROM_RAM)) && (!defined(__DCC__))
/* Use Range Designated Initializers (GNU extension implemented by TASKING
   too) */
/* ! THIS WON'T BE USED TO INITIALIZE STACKS WHEN THE APPLICATION IS NOT
     LOCATED IN RAM BECAUSE USELESS COPY IN FLASH ! */
#define EE_TC_FILL_STACK(stack) \
  = {[0 ... (sizeof(stack)/sizeof(stack[0]) - 1U)] = EE_TC_STACK_FILL_PATTERN}
#else /* (__OO_ORTI_STACK__ || EE_STACK_MONITORING__) && EE_EXECUTE_FROM_RAM
  && !__DCC__ */
#define EE_TC_FILL_STACK(stack)
#endif /* (__OO_ORTI_STACK__ || EE_STACK_MONITORING__) && EE_EXECUTE_FROM_RAM
  && !__DCC__ */

/*******************************************************************************
            Common Context Types and Data Structures Declarations
 ******************************************************************************/
/* I need to put this here because it needs to see HAL TYPES
   declaration in eecf.c */
#include "cpu/common/inc/ee_hal_structs.h"

/*******************************************************************************
                    Multicore and multiprocessor support
 ******************************************************************************/
#if (!defined(EE_CURRENTCPU)) || (EE_CURRENTCPU == 0)
/* Used as short-cut for previous condition */
#define EE_MASTER_CPU
#endif /* EE_CURRENTCPU == 0 */
/* Include multicore support there's a guard inside */
#include "cpu/tricore/inc/ee_tc_multicore.h"

/*******************************************************************************
 *  IMPORTANT:
 *  These struct layouts are packet by construction so I don't add 
 *  __attribute__((packed)) because it could harm performances.
 *  Look at:
 *  http://digitalvampire.org/blog/index.php/2006/07/31/\
 *  why-you-shouldnt-use-__attribute__packed
 ******************************************************************************/

/* Previous Context Information Register */
typedef union {
  EE_UINT32 reg;
  struct {
    EE_UINT32 PCXO  :16; /* Previous Context Pointer Offset Address  */
    EE_UINT32 PCXS  :4;  /* Previous Context Pointer Segment Address */
#if defined(__TC13__) || defined(__TC131__)
    EE_UINT32       :1;  /* In TC 1.3 bit20 is Reserved */
    EE_UINT32       :1;  /* In TC 1.3 bit21 is Reserved */
#endif /* __TC13__ || __TC131__ */
    EE_UINT32 UL    :1;  /* Upper or Lower Context Tag */
    EE_UINT32 PIE   :1;  /* Previous Interrupt Enable */
    EE_UINT32 PCPN  :8;  /* Previous CPU Priority Number */ 
  } bits;
} EE_PCXI;

/* Context Save Area Link Word (only PCXS and PCXO are valid)*/
typedef EE_PCXI EE_CSA_LINK;

/* Context Save Area (16 words, upper or lower context). */
typedef struct {
  EE_CSA_LINK next;
  union {
    struct {
      /* underscore needed otherwise TASKING preprocessor will change
       * it in a integer constant, because PSW SFR definition */
      EE_UREG PSW_;
      EE_ADDR A10;
      EE_ADDR A11;
      EE_UREG D8;
      EE_UREG D9;
      EE_UREG D10;
      EE_UREG D11;
      EE_ADDR A12;
      EE_ADDR A13;
      EE_ADDR A14;
      EE_ADDR A15;
      EE_UREG D12;
      EE_UREG D13;
      EE_UREG D14;
      EE_UREG D15;
    } ucx;
    struct {
      EE_ADDR A11;
      EE_ADDR A2;
      EE_ADDR A3;
      EE_UREG D0;
      EE_UREG D1;
      EE_UREG D2;
      EE_UREG D3;
      EE_ADDR A4;
      EE_ADDR A5;
      EE_ADDR A6;
      EE_ADDR A7;
      EE_UREG D4;
      EE_UREG D5;
      EE_UREG D6;
      EE_UREG D7;
    } lcx;
  } cx;
} EE_CSA;

/* Interrupt control register */
#if defined(__TC13__) || defined(__TC131__)
typedef union {
  struct {
    EE_UREG CCPN    :8; /* Current CPU Priority Number */
    EE_UREG IE      :1; /* Interrupt Enable */
    EE_UREG bit9_   :1;
    EE_UREG bit10_  :1;
    EE_UREG bit11_  :1;
    EE_UREG bit12_  :1;
    EE_UREG bit13_  :1;
    EE_UREG bit14_  :1;
    EE_UREG bit15_  :1;
    EE_UREG PIPN    :8; /* Pending Interrupt Priority Number */
    EE_UREG CARBCYC :2; /* Number of Arbitration Cycles */
    EE_UREG CONECYC :1; /* No. of Clks per Arb. Cycle Control */
    EE_UREG bit27_  :1;
    EE_UREG bit28_  :1;
    EE_UREG bit29_  :1;
    EE_UREG bit30_  :1;
    EE_UREG bit31_  :1;
  } bits;
  EE_UREG reg;
} EE_ICR;
#else /* __TC13__ || __TC131__ */
typedef union {
  struct {
    EE_UREG CCPN    :8; /* Current CPU Priority Number */
    EE_UREG bit8_   :1;
    EE_UREG bit9_   :1;
    EE_UREG bit10_  :1;
    EE_UREG bit11_  :1;
    EE_UREG bit12_  :1;
    EE_UREG bit13_  :1;
    EE_UREG bit14_  :1;
    EE_UREG IE      :1; /* Interrupt Enable */
    EE_UREG PIPN    :8; /* Pending Interrupt Priority Number */
    EE_UREG bit24_  :1;
    EE_UREG bit25_  :1;
    EE_UREG bit26_  :1;
    EE_UREG bit27_  :1;
    EE_UREG bit28_  :1;
    EE_UREG bit29_  :1;
    EE_UREG bit30_  :1;
    EE_UREG bit31_  :1;
  } bits;
  EE_UREG reg;
} EE_ICR;
#endif /* __TC13__ || __TC131__ */

/*******************************************************************************
                              Utility Functions
 ******************************************************************************/
#ifdef __TASKING__
/* Count Leading Zeros */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_clz( EE_UREG data )
{
  return (EE_UREG)__clz(data);
}

/* Data barrier */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_dsync( void )
{
  __dsync();
}

/* Instruction barrier */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_isync( void )
{
  __isync();
}

/* Debug instruction */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_debug( void )
{
  __debug();
}

/*Core info*/
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_cpu_model( void )
{
  register EE_UREG r = __mfcr(EE_CPU_REG_CPU_ID);
  return (r >> 2U);
}

__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_cpu_revision( void )
{
  register EE_UREG r = __mfcr(EE_CPU_REG_CPU_ID);
  return (r & 0xFFU);
}

/* Start performance counter [CCNT] in normal mode */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_start_CCNT( void )
{
  __mtcr(EE_CPU_REG_CCTRL, 0x2U);
}

/* Reads the CPU Clock Cycle Counter (includes overflow bit) */
__INLINE__  EE_UREG __ALWAYS_INLINE__ EE_tc_get_CCNT( void )
{
  return __mfcr(EE_CPU_REG_CCNT);
}

/* Read temporal protection configuration register */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_get_TPS_CON( void )
{
  return __mfcr(EE_CPU_REG_TPS_CON);
}

/* Write temporal protection configuration register */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_TPS_CON( EE_UREG tps_con )
{
  __mtcr(EE_CPU_REG_TPS_CON, tps_con);
}

/* Functions to Access ICR register */
__INLINE__ EE_ICR __ALWAYS_INLINE__ EE_tc_get_ICR( void )
{
  register EE_ICR icr;
  icr.reg = __mfcr(EE_CPU_REG_ICR);
  return icr;
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_ICR( EE_ICR icr )
{
  __mtcr(EE_CPU_REG_ICR, icr.reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
}

/* Enable interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_enableIRQ( void )
{
  __enable();
}
/* Disable interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_disableIRQ( void )
{
  __disable();
}

/*Functions to select the call method*/
__INLINE__ void __ALWAYS_INLINE__ EE_tc_call( EE_THREAD_PTR t )
{
  __asm volatile ("calli %0" : : "a"(t));
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_jump( EE_THREAD_PTR t )
{
  __asm volatile ("ji %0" : : "a"(t));
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_jump_and_link( EE_THREAD_PTR t )
{
  __asm volatile ("jli %0" : : "a"(t));
}

/*Context handling functions for Tasking*/
__INLINE__ EE_ADDR __ALWAYS_INLINE__ EE_tc_get_RA( void )
{
  register EE_ADDR reg = 0U;
  __asm volatile ("mov.aa %0, a11" : "=a"(reg));
  return reg;
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_RA( EE_CONST_ADDR reg )
{
  __asm volatile ("mov.aa a11, %0" : : "a"(reg));
}

__INLINE__ EE_ADDR __ALWAYS_INLINE__ EE_tc_get_SP( void )
{
  register EE_ADDR reg = 0U;
  __asm volatile ("mov.aa %0, sp" : "=a"(reg));
  return reg;
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_SP( EE_CONST_ADDR reg )
{
  __asm volatile ("mov.aa sp, %0" : : "a"(reg));
}

/* Lower Context Handling Functions */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_svlcx( void )
{
  __svlcx();
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_rslcx( void )
{
  __rslcx();
}

/*Functions to access the CSFRs*/

#define EE_tc_get_psw()        __mfcr(EE_CPU_REG_PSW)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_psw( EE_UREG reg )
{
  __mtcr(EE_CPU_REG_PSW, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
}

#define EE_tc_get_pcxi()       __mfcr(EE_CPU_REG_PCXI)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_pcxi( EE_UREG reg )
{
  __mtcr(EE_CPU_REG_PCXI, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
}

#define EE_tc_get_fcx()       __mfcr(EE_CPU_REG_FCX)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_fcx( EE_UREG reg )
{
  __mtcr(EE_CPU_REG_FCX, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
}

#define EE_tc_get_syscon() __mfcr(EE_CPU_REG_SYSCON)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_syscon( EE_UREG reg )
{
  __mtcr(EE_CPU_REG_SYSCON, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
}

#elif defined(__GNUC__)
/* Count Leading Zeros */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_clz( EE_UREG data )
{
  return (EE_UREG)__CLZ(data);
}

/* Implement Push and Pop registers on Stack */
/* Stacks grows toward smaller addresses */
#define EE_tc_push_data_reg(reg) \
  __asm volatile ("st.w [%%a10+]-4, %%" EE_PREPROC_STRING(reg) : : : "memory")
#define EE_tc_push_address_reg(reg) \
  __asm volatile ("st.a [%%a10+]-4, %%" EE_PREPROC_STRING(reg) : : : "memory")

#define EE_tc_pop_data_reg(reg) \
  __asm volatile ("ld.w %%" EE_PREPROC_STRING(reg) ", [+%%a10]4" : : : "memory")
#define EE_tc_pop_address_reg(reg) \
  __asm volatile ("ld.a %%" EE_PREPROC_STRING(reg) ", [+%%a10]4" : : : "memory")

/* Move a variable in a data register */
#define EE_tc_mov_to_data_reg(reg, var) \
  __asm volatile ("mov %%" EE_PREPROC_STRING(reg) ", %0" : : "d"(var))
/* Move an address in an address register */
#define EE_tc_mov_to_addr_reg(reg, addr) \
  __asm volatile ("mov.aa %%" EE_PREPROC_STRING(reg) ", %0" : : "a"(addr))

__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_read_return_value( void )
{
  register EE_UREG reg = 0U;
  __asm volatile ("mov %0, %%d2" : "=d"(reg));
  return reg;
}

/* Data barrier */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_dsync( void )
{
  _dsync();
}

/* Instruction barrier */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_isync( void )
{
  _isync();
}

/* Debug instruction */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_debug( void )
{
  _debug();
}

/* Core info */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_cpu_model( void )
{
  register EE_UREG r = _mfcr(EE_CPU_REG_CPU_ID);
  return (r >> 2U);
}

__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_cpu_revision( void )
{
  register EE_UREG r = _mfcr(EE_CPU_REG_CPU_ID);
  return (r & 0xFFU);
}

/* Start performance counter [CCNT] in normal mode */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_start_CCNT( void )
{
  _mtcr(EE_CPU_REG_CCTRL, 0x2U);
}

/* Reads the CPU Clock Cycle Counter (includes overflow bit) */
__INLINE__  EE_UREG __ALWAYS_INLINE__ EE_tc_get_CCNT( void )
{
  return _mfcr(EE_CPU_REG_CCNT);
}

/* Read temporal protection configuration register */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_get_TPS_CON( void )
{
  return _mfcr(EE_CPU_REG_TPS_CON);
}

/* Write temporal protection configuration register */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_TPS_CON( EE_UREG tps_con )
{
  _mtcr(EE_CPU_REG_TPS_CON, tps_con);
  EE_tc_isync();
}

/* Functions to Access ICR register */
__INLINE__ EE_ICR __ALWAYS_INLINE__ EE_tc_get_ICR( void )
{
  register EE_ICR icr;
  icr.reg = _mfcr(EE_CPU_REG_ICR);
  return icr;
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_ICR( EE_ICR icr )
{
  _mtcr(EE_CPU_REG_ICR, icr.reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
  /* HIGHTEC GNUC intrinsic __mtcr do not put an isync natively */
  EE_tc_isync();
}

/* Enable interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_enableIRQ( void )
{
  _enable();
}
/* Disable interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_disableIRQ( void )
{
  _disable();
}

/* Functions to select the call method */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_call( EE_THREAD_PTR t )
{
  __asm volatile ("calli %0" : : "a"(t));
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_jump( EE_THREAD_PTR t )
{
  __asm volatile ("ji %0" : : "a"(t));
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_jump_and_link( EE_THREAD_PTR t )
{
  __asm volatile ("jli %0" : : "a"(t));
}

/* Context handling functions for Gnuc */

__INLINE__ EE_ADDR __ALWAYS_INLINE__ EE_tc_get_RA( void )
{
  register EE_ADDR reg = 0U;
  __asm volatile ("mov.aa %0, %%a11" : "=a"(reg));
  return reg;
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_RA( EE_CONST_ADDR reg )
{
  __asm volatile ("mov.aa %%a11, %0" : : "a"(reg));
}

__INLINE__ EE_ADDR __ALWAYS_INLINE__ EE_tc_get_SP( void )
{
  register EE_ADDR reg = 0U;
  __asm volatile ("mov.aa %0, %%a10" : "=a"(reg));
  return reg;
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_SP( EE_CONST_ADDR reg )
{
  /* Inform The GCC that this instruction "clobber" the memory (stack) */
  __asm volatile ("mov.aa %%a10, %0" : : "a"(reg) : "memory");
}

/* Lower Context Handling Functions */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_svlcx( void )
{
  _svlcx();
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_rslcx( void )
{
  _rslcx();
}

/* Functions to access the CSFRs */

#define EE_tc_get_psw()        __mfcr(EE_CPU_REG_PSW)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_psw( EE_UREG reg )
{
  __mtcr(EE_CPU_REG_PSW, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
  /* HIGHTEC GNUC intrinsic __mtcr do not put an isync natively */
  EE_tc_isync();
}

#define EE_tc_get_pcxi()       __mfcr(EE_CPU_REG_PCXI)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_pcxi( EE_UREG reg )
{
  __mtcr(EE_CPU_REG_PCXI, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
  /* HIGHTEC GNUC intrinsic __mtcr do not put an isync natively */
  EE_tc_isync();
}

#define EE_tc_get_fcx()       __mfcr(EE_CPU_REG_FCX)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_fcx( EE_UREG reg )
{
  __mtcr(EE_CPU_REG_FCX, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
  /* HIGHTEC GNUC intrinsic __mtcr do not put an isync natively */
  EE_tc_isync();
}

#define EE_tc_get_syscon() __mfcr(EE_CPU_REG_SYSCON)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_syscon( EE_UREG reg )
{
  __mtcr(EE_CPU_REG_SYSCON, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
  /* HIGHTEC GNUC intrinsic __mtcr do not put an isync natively */
  EE_tc_isync();
}

#elif defined(__DCC__)

asm volatile EE_UREG EE_tc_read_return_value( void )
{
! "%d2"
  mov %d2, %d2
}

/* Count Leading Zeros */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_clz( EE_UREG data )
{
  return (EE_UREG)_clz(data);
}

/* Data barrier */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_dsync( void )
{
  _dsync();
}

/* Instruction barrier */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_isync( void )
{
  _isync();
}

/* Debug instruction */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_debug( void )
{
  _debug();
}

/* Core info */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_cpu_model( void )
{
  register EE_UREG r = _mfcr(EE_CPU_REG_CPU_ID);
  return (r >> 2U);
}

__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_cpu_revision( void )
{
  register EE_UREG r = _mfcr(EE_CPU_REG_CPU_ID);
  return (r & 0xFFU);
}

/* Start performance counter [CCNT] in normal mode */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_start_CCNT( void )
{
  _mtcr(EE_CPU_REG_CCTRL, 0x2U);
}

/* Reads the CPU Clock Cycle Counter (includes overflow bit) */
__INLINE__  EE_UREG __ALWAYS_INLINE__ EE_tc_get_CCNT( void )
{
  return _mfcr(EE_CPU_REG_CCNT);
}

/* Read temporal protection configuration register */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc_get_TPS_CON( void )
{
  return _mfcr(EE_CPU_REG_TPS_CON);
}

/* Write temporal protection configuration register */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_TPS_CON( EE_UREG tps_con )
{
  _mtcr(EE_CPU_REG_TPS_CON, tps_con);
  EE_tc_isync();
}

/* Functions to Access ICR register */
__INLINE__ EE_ICR __ALWAYS_INLINE__ EE_tc_get_ICR( void )
{
  register EE_ICR icr;
  icr.reg = _mfcr(EE_CPU_REG_ICR);
  return icr;
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_ICR( EE_ICR icr )
{
  _mtcr(EE_CPU_REG_ICR, icr.reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
  /* DCC intrinsic __mtcr do not put an isync natively */
  EE_tc_isync();
}

/* Enable interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_enableIRQ( void )
{
  _enable();
}
/* Disable interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_disableIRQ( void )
{
  _disable();
}

/* Functions to select the call method */
asm volatile void EE_tc_call(EE_THREAD_PTR t)
{
% reg t
  calli t
}

asm volatile void EE_tc_jump( EE_THREAD_PTR t )
{
% reg t
  ji t
}

asm volatile void EE_tc_jump_and_link(EE_THREAD_PTR ptr)
{
% reg ptr
  jli ptr
}

/* Context handling functions for Diab */

asm volatile EE_ADDR EE_tc_get_RA( void )
{
! "%a2"
  mov.aa %a2, %a11
}

asm volatile void EE_tc_set_RA(EE_CONST_ADDR ptr)
{
% reg ptr
  mov.aa %a11, ptr
}

asm volatile EE_ADDR EE_tc_get_SP(void)
{
! "%a2"
  mov.aa %a2, %a10
}

asm volatile __attribute__((use_frame_pointer)) void EE_tc_set_SP(EE_CONST_ADDR ptr)
{
% reg ptr
  mov.aa %a10, ptr
}

/* Lower Context Handling Functions */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_svlcx( void )
{
  _svlcx();
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_rslcx( void )
{
  _rslcx();
}

/* Functions to access the CSFRs */

#define EE_tc_get_psw()        _mfcr(EE_CPU_REG_PSW)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_psw( EE_UREG reg )
{
  _mtcr(EE_CPU_REG_PSW, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
  /* DCC intrinsic __mtcr do not put an isync natively */
  EE_tc_isync();
}

#define EE_tc_get_pcxi()       _mfcr(EE_CPU_REG_PCXI)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_pcxi( EE_UREG reg )
{
  _mtcr(EE_CPU_REG_PCXI, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
  /* DCC intrinsic __mtcr do not put an isync natively */
  EE_tc_isync();
}

#define EE_tc_get_fcx()       _mfcr(EE_CPU_REG_FCX)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_fcx( EE_UREG reg )
{
  _mtcr(EE_CPU_REG_FCX, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
  /* DCC intrinsic __mtcr do not put an isync natively */
  EE_tc_isync();
}

#define EE_tc_get_syscon() _mfcr(EE_CPU_REG_SYSCON)

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_syscon( EE_UREG reg )
{
  _mtcr(EE_CPU_REG_SYSCON, reg);
  /* When you write a CSFR you need to force synchronization (isync) to
     avoid side-effects, TASKING __mtcr do that by implementation */
  /* DCC intrinsic __mtcr do not put an isync natively */
  EE_tc_isync();
}

#endif /* __TASKING__ || __GNUC__ || __DCC__ */

/* Utility Macros for debugging and tracing purposes */

/* One step indirection more to let a macro explode */
#define EE_STRINGIFY(m) EE_PREPROC_STRING(m)

#ifdef EE_DEBUG
#define EE_BREAK_POINT()        EE_tc_debug()
#define EE_GLOBAL_LABEL(label)  __asm__(".globl " #label "\n" #label ":")
#else  /* EE_DEBUG */
#define EE_BREAK_POINT()        ((void)0U)
#define EE_GLOBAL_LABEL(label)  ((void)0U)
#endif /* EE_DEBUG */

/* Suspend/Resume Interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_resumeIRQ( EE_FREG flags )
{
  register EE_ICR icr;
  icr.reg = flags;
  EE_tc_set_ICR(icr);
}

__INLINE__ EE_FREG __ALWAYS_INLINE__ EE_tc_suspendIRQ( void )
{
  register EE_ICR icr = EE_tc_get_ICR();
  EE_tc_disableIRQ();
  return icr.reg;
}

/* FIXME: In Erika HAL doesn't have a method to check IRQ status! */
__INLINE__ EE_FREG __ALWAYS_INLINE__ EE_tc_isIRQEnabled( void )
{
  register EE_ICR icr = EE_tc_get_ICR();
  return (EE_FREG)(icr.bits.IE);
}

/* New functions needed for Resource sharing with ISR  */
__INLINE__ EE_TYPEISR2PRIO __ALWAYS_INLINE__ EE_tc_get_int_prio( void )
{
  return (EE_UINT8)EE_tc_get_ICR().bits.CCPN;
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_int_prio( EE_TYPEISR2PRIO prio )
{
  register EE_ICR icr = EE_tc_get_ICR();
  icr.bits.CCPN = prio;
  EE_tc_set_ICR(icr);
}

/*******************************************************************************
                            TRICORE CSA handling
 ******************************************************************************/

/* Makes a CSA pointer from a CSA link word */
__INLINE__ EE_CSA* __ALWAYS_INLINE__ EE_tc_csa_make_addr( EE_CSA_LINK link )
{
  EE_CSA* res = (EE_CSA*)(((EE_UINT32)(link.bits.PCXS) << 28U) |
    (((EE_UINT32)link.bits.PCXO) << 6U));
  return res;
}

/* Returns the first CSA in the Free Context List */
__INLINE__ EE_CSA_LINK __ALWAYS_INLINE__ EE_tc_csa_get_fcx( void )
{
  register EE_CSA_LINK head;
  head.reg = EE_tc_get_fcx();
  return head;
}

/* Set the first CSA in the Free Context List */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_csa_set_fcx( EE_CSA_LINK link )
{
  EE_tc_set_fcx(link.reg);
}

/* Returns the first CSA in the Previous Context List */
__INLINE__ EE_CSA_LINK __ALWAYS_INLINE__ EE_tc_csa_get_pcxi()
{
  register EE_CSA_LINK head;
  head.reg = EE_tc_get_pcxi();
  return head;
}

/* Set the first CSA in the Previous Context List */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_csa_set_pcxi( EE_CSA_LINK link )
{
  EE_tc_set_pcxi(link.reg);
}

/* Returns the link work of a given CSA 'at' */
__INLINE__ EE_CSA_LINK __ALWAYS_INLINE__ EE_tc_csa_get_next( EE_CSA_LINK at )
{
  EE_CSA_LINK res = {0U};
  EE_CSA* csa_ptr = EE_tc_csa_make_addr(at);
  if (csa_ptr != NULL) {
    res = csa_ptr->next;
  }
  return res;
}

/* Sets the link word of a given CSA 'at' to 'link' */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_csa_set_next( EE_CSA_LINK at,
    EE_CSA_LINK link )
{
  EE_CSA* csa_ptr = EE_tc_csa_make_addr(at);
  if (csa_ptr != NULL) {
    csa_ptr->next = link;
  }
}

/*******************************************************************************
                        HAL Interrupt Handling
  (These SHOULD be placed in internal but I MUST place them here, because
   interrupt handling kernel primitives inlining)
 ******************************************************************************/

/* Disable/Enable Interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_enableIRQ( void )
{
  EE_tc_enableIRQ();
}

__INLINE__ void __ALWAYS_INLINE__ EE_hal_disableIRQ( void )
{
  EE_tc_disableIRQ();
}
/* Suspend/Resume Interrupts */
__INLINE__ void __ALWAYS_INLINE__ EE_hal_resumeIRQ( EE_FREG flag )
{
  EE_tc_resumeIRQ(flag);
}

__INLINE__ EE_FREG __ALWAYS_INLINE__ EE_hal_suspendIRQ( void )
{
  return EE_tc_suspendIRQ();
}

/*******************************************************************************
                  ENDINIT e SAFETY_ENDINIT support functions
 This function are declared here but defined inside MCU because completely MCU
 dependent. TriCore architecture states only that ENDINIT protection will exist
 in a TriCore implementation.
*******************************************************************************/

/* endinit types */
typedef enum
{
  EE_TC_ENDINIT_DISABLE = 0x0U,
  EE_TC_ENDINIT_ENABLE  = 0x1U
} EE_tc_endinit_t;

#ifdef EE_AS_OSAPPLICATIONS__
#define API_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */

void EE_tc_endint_disable( void );
void EE_tc_endint_enable( void );

#if defined(__TC161__) || defined(__CORE_TC16X__)
void EE_tc_safety_endinit_disable( void );
void EE_tc_safety_endinit_enable( void );
#endif /* __TC161__ || __CORE_TC16X__ */

#ifdef EE_AS_OSAPPLICATIONS__
#define API_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */

/******************************************************************************
                OS-Applications & Memory Protection Support
*******************************************************************************/
/* Utility Macros to let rt-druid easily locate structures in the right
   sections. (RT-Druid still has some issues in using MemMap.h file) */

/* Kernel private uninitialized data (BSS) */
#define EE_KERNEL_UDATA EE_COMPILER_SECTION("ee_kernel_bss")
/* Kernel private initialized data */
#define EE_KERNEL_IDATA EE_COMPILER_SECTION("ee_kernel_data")
/* API (core-shared) uninitialized data (BSS) */
#define EE_API_UDATA EE_COMPILER_SECTION("ee_kernel_bss")
/* API (core-shared) initialized data */
#define EE_API_IDATA EE_COMPILER_SECTION("ee_kernel_data")

#ifdef EE_AS_OSAPPLICATIONS__
/* Flag Used to inform Kernel/User code that can use MemMap.h inclusion to
   locate code in right sections */
#define EE_SUPPORT_MEMMAP_H

#if (!defined(__GNUC__)) && (!defined(__DCC__))
#error Unsupported Compiler! OS-Applications and Memory Protection has been\
 implemented only for GNUC and DIAB Compilers!
#endif /* !__GNUC__ && !__DCC__ */

/*
 * Markers for application private areas (OLD Custom Erika Method to locate
 * OS-Applications data and code
 */

/* Application private constant data (read-only) */
/* XXX: In the following macros I MUST use stringify preprocessor operator (#)
        directly, without using a wrapping macro, because OSApplications ID's
        macros are equal to the string that have to be passed to the following
        macros, so with a layer of indirection we will incur in a WRONG macro
        expansion */
/* The default is OK */
#define EE_APPLICATION_CDATA(app)  /* EE_COMPILER_SECTION(".rodata") */
/* Application private uninitialized data (BSS) */
#define EE_APPLICATION_UDATA(app)  \
  EE_COMPILER_SECTION("ee_" #app "_bss")
/* Application private initialized data */
#define EE_APPLICATION_IDATA(app)  \
  EE_COMPILER_SECTION("ee_" #app "_data")
/* Application private code */
#define EE_APPLICATION_CODE(app)  \
  EE_COMPILER_SECTION("ee_" #app "_text")
/* Application private stack */
#define EE_STACK_ATTRIB_NAME(app) EE_COMPILER_ALIGN(EE_STACK_ALIGN_SIZE) \
  EE_COMPILER_SECTION("ee_" #app "_stack")

/**
 * Data type for Information data structure of os-application's section
 */
typedef struct {
  /* RAM range to be set in DPR registers */
  const EE_ADDR ram_begin;
  const EE_ADDR ram_end;
  /* Stack Ranges used to check access */
  const EE_ADDR stacks_begin;
  const EE_ADDR stacks_end;
} EE_APP_SEC_INFO_T;

/* Used to read current PSW.PSR field bit [13:12] */
#define EE_TC_READ_PSW_PSR(psw) (((psw) & ((EE_UREG)0x3U << 12U)) >> 12U)

/* Used to clean bit [13:12] corresponding to PSW.PSR field position and bit
   [11:10] corresponding to PSW.IO field in a bitmask */
#define EE_TC_PSW_PRS_IO_CLEAN_MASK  0xFFFFC3FFU

/* Used to clean bit [6:0] corresponding to PSW.CDC (call depth counter) */
#define EE_TC_PSW_CDC_CLEAN_MASK     0xFFFFFF80U

/*
 * Choose between two mode:
 * 1) OS-Application directly mapped on PSR.
 *    PRO: Configuration done Once, Context Switch is just a
 *      read/modify/write of PSW register.
 *    CONS: Only 3 OS-Applications configurable.
 * 2) OS-Application dynamically set.
 *    PRO: No theoretical upper limit in OS-Applications configuration.
 *    CONS: Context switch need two more memory reads and two more CSFR writes.
 */
#ifdef EE_OS_APP_MAPPED_ON_PROTECTION_SETS

/* Convert 0 (Kernel) - 4 valid app ID's in the right bitmast to set the value
   in PSW.PRS */
#define EE_TC_PSW_APP_TO_PRS(appid) (((EE_UREG)(appid) & 0x3U) << 12U)
/* PSW.IO 2 for trusted code (IO Supervisor) */
#define EE_MEMPROT_TRUST_MODE       (0x2U << 10U)
/* PSW.IO 1 for user code (IO User-1 Can disable interrupts +
   Peripheral Access) */
/* [OS058] If supported by hardware, the Operating System module shall execute
   non-trusted OS-Applications in non-privileged mode. */
/* [OS096] As far as supported by hardware, the Operating System module shall
   not allow non-trusted OS-Applications to access control registers managed by
   the Operating System module. (BSW11011) */
#define EE_MEMPROT_USR_MODE         (0x1U << 10U)
#else /* EE_OS_APP_MAPPED_ON_PROTECTION_SETS */

/* Unused if EE_OS_APP_MAPPED_ON_PROTECTION_SETS is not defined */
#define EE_TC_PSW_APP_TO_PRS(appid) (0U)
/* PSW.PRS 0 and PSW.IO 2 for trusted code (IO Supervisor) */
#define EE_MEMPROT_TRUST_MODE ( (0x0U << 12U) | (0x2U << 10U) )
/* PSW.PRS 1 and PSW.IO 1 for user code (IO User-1 Can disable interrupts +
   Peripheral Access) */
#define EE_MEMPROT_USR_MODE   ( (0x1U << 12U) | (0x1U << 10U) )
#endif /* EE_OS_APP_MAPPED_ON_PROTECTION_SETS */

#endif /* EE_AS_OSAPPLICATIONS__ */

/******************************************************************************
                           Timing Protection Support

  N.B. Software Free Running Timer (SWFRT) could be configured in any case, but
       for now it is tied to Timing Protection
*******************************************************************************/
#ifdef EE_TIMING_PROTECTION__
/** Macro to abstract free running timer duration in Kernel Layer */
#define EE_HAL_SWFRT_TIMER_DURATION (((EE_UREG)-1) >> 1U)
#endif /* EE_TIMING_PROTECTION__ */

#else /* __TC13__ || __TC131__ || __TC161__ || __CORE_TC16X__ */
#error CPU not supported.
#endif /* __TC13__ || __TC131__ || __TC161__ ||__CORE_TC16X__ */
#endif /* INCLUDE_EE_TC_CPU_H__ */
