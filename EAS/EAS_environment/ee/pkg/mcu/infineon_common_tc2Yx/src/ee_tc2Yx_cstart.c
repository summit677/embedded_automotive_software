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

 /** @file   ee_tc2Yx_cstart.c
  *  @brief  The system startup code
  *
  *  The system startup code initializes the processor's registers
  *  and the application C variables.
  *  Has been created starting from cstart.c TASKING start-up code and has
  *  been customized to compile with HIGHTEC GNUC compiler (v4.6.1.2) too.
  *  We used TASKING 4.0r1 as compiler and you can find the
  *  documentation pdf file for compiler at:
  *  http://www.tasking.com/support/tricore/tc_user_guide_v4.0.pdf
  *  In following comments, sometime, paragraph reference of this document
  *  are added.
  *  For HIGHTEC documentation visit site: http://www.hightec-rt.com/
  *
  *  @author Errico Guidieri
  *  @date 2012
  */

#include <stdlib.h>

/* ERIKA CPU API + Internal inclusion */
#include "cpu/tricore/inc/ee_tc_internal.h"
/* Resolve ERIKA trap vectors. (They are inside EE_tc_trapvect.c) */
#include "cpu/tricore/inc/ee_tc_trap.h"
/* Resolve ERIKA interrupt vectors. (They are inside EE_tc_intvect.c) */
#include "cpu/tricore/inc/ee_tc_irq.h"
/* MCU defines to populate registers + PLL configuration utility */
#include "mcu/infineon_common_tc2Yx/inc/ee_tc2Yx_internal.h"
/* ENDINIT Support */
#include "mcu/infineon_common_tc2Yx/inc/ee_tc2Yx_endinit.h"

#ifdef __TASKING__

#pragma weak   exit
#pragma extern _Exit
/* prevent profiling information on cstart */
#pragma profiling off
/* preset tradeoff level (for size) */
#pragma tradeoff 4
/* disable runtime error checking for cstart */
#pragma runtime BCM
/* disable MISRA-C checking */
#pragma nomisrac
/* bss clearing not before cinit */
#pragma noclear
/* no external ROM access before bus configuration */
#pragma immediate_in_code

/* libc exit function remapping  */
#define EE_EXIT exit

/* far data access specificator */
#define EE_FAR __far

/* C initialization function for TASKING is inside libc */
#define EE_tc2Yx_cinit _c_init
/* C initialization function declaration */
extern void EE_tc2Yx_cinit( void );

/* Utility macros to set registers: */

/* Two pre-processor steps to let macros explode */
#define EE_tc2Yx_extru2(src,start,size) \
  ({ int res; __asm volatile (" extr.u\t %0,%1,%2,%3" : "=d" (res) : "d" (src),\
  "i" (start),"i" (size) : "memory"); res; })
#define EE_tc2Yx_extru(src,start,size) EE_tc2Yx_extru2(src,start,size)

#define EE_tc2Yx_setareg2(areg,val) \
  __asm volatile ( "mov.aa " #areg ",%0"::"a"(&val) : "memory" )
#define EE_tc2Yx_setareg(areg,val) EE_tc2Yx_setareg2(areg,val)

#elif defined (__GNUC__)

extern void _exit (int status);

/* libc exit function remapping  */
#define EE_EXIT _exit

/* GNUC meaningless function or data specificators */
#define EE_FAR
#define JUMP

/* Utility macros to set registers: */

/* Two pre-processor steps to let macros explode */
#define EE_tc2Yx_extru2(src,start,size) \
  ({ int res; __asm volatile (" extr.u\t %0,%1,%2,%3" : "=d" (res) : "d" (src),\
  "i" (start),"i" (size) : "memory"); res; })
#define EE_tc2Yx_extru(src,start,size) EE_tc2Yx_extru2(src,start,size)

#define EE_tc2Yx_setareg2(areg,val) \
{ __asm volatile (" movh.a\t %%" EE_PREPROC_STRING(areg) ",\
  hi:" EE_PREPROC_STRING(val) "\n\tlea\t %%"EE_PREPROC_STRING(areg)",\
  [%%" EE_PREPROC_STRING(areg) "]lo:"EE_PREPROC_STRING(val)"" : : : "memory"); }
#define EE_tc2Yx_setareg(areg,val) EE_tc2Yx_setareg2(areg,val)

/* C initialization function */
#ifdef EE_EXECUTE_FROM_RAM
#define EE_tc2Yx_cinit() ((void)0)
#else /* EE_EXECUTE_FROM_RAM */
static void EE_tc2Yx_cinit( void );
#endif /* EE_EXECUTE_FROM_RAM */

__asm ("\t .weak " EE_STRINGIFY(_SMALL_DATA3_));
__asm ("\t .weak " EE_STRINGIFY(_SMALL_DATA4_));

#elif defined(__DCC__)
/* never profile nor run-time check this code */
#pragma option -Xprof-all=0
#pragma option -Xrtc=0

extern void exit (int status);

#ifndef EE_EXECUTE_FROM_RAM
/*
  DCC Library does not contain copytable.h where find struct copytable
  declaration end __copy_multiple function declaration (what a shame). So
  instead do the clean thing I have to work around with void * */
/* #pragma weak __COPY_TABLE_BEGIN__
#pragma weak __COPY_TABLE_END__ */
__asm ("\t .weak __COPY_TABLE_BEGIN__,__COPY_TABLE_END__");
extern /* struct copytable */ void * __COPY_TABLE_BEGIN__[];
extern /* struct copytable */ void * __COPY_TABLE_END__[];
extern void __copy_multiple (/* struct copytable */void *begin,
  /* struct copytable */ void *end);
#endif /* !EE_EXECUTE_FROM_RAM */

/* libc exit function remapping  */
#define EE_EXIT exit

/* DCC meaninless fucnctions or data specificators */
#define EE_FAR
#define JUMP

/* Utility macros to set registers: */

/* we need to use a15 for the address register and not direct that the compiler
   this will not remove */
#define EE_tc2Yx_setareg2(areg, val)          \
  { __asm("#$$bp");                           \
    __asm ("  movh.a\t %a15,"#val"@ha\n");    \
    __asm ("  lea\t %a15,[%a15]"#val"@l\n");  \
    __asm ("  mov.aa %"#areg", %a15\n");      \
    __asm ("#$$ep");                          \
  }
#define EE_tc2Yx_setareg(areg,val) EE_tc2Yx_setareg2(areg,val)

asm volatile int EE_tc2Yx_extru (int src, int start, int size)
{
%reg src; con start, size;
!"%d2" 
  mov.d	 %d2, src               # pointer src is not casted by compiler
  extr.u %d2, %d2, start, size
}

/* C initialization function */
#ifdef EE_EXECUTE_FROM_RAM
#define EE_tc2Yx_cinit() ((void)0)
#else /* EE_EXECUTE_FROM_RAM */
#define EE_tc2Yx_cinit()  \
  __copy_multiple(__COPY_TABLE_BEGIN__, __COPY_TABLE_END__)
#endif /* EE_EXECUTE_FROM_RAM */

/* __asm ("\t .weak __SDA8_BASE, __SDA9_BASE"); */
__asm ("\t .weak " EE_STRINGIFY(_SMALL_DATA3_));
__asm ("\t .weak " EE_STRINGIFY(_SMALL_DATA4_));

#else
#error Unsupported compiler!
#endif /* __TASKING__ || __GNUC__ || __DCC__ */

extern EE_FAR EE_UINT32 EE_E_USTACK[];    /* user stack end */
/* extern EE_FAR EE_UINT32 EE_LC_UE_ISTACK[]; interrupt stack end */

extern EE_FAR void * EE_SMALL_DATA;       /* centre of A0 addressable area */
extern EE_FAR void * EE_LITERAL_DATA;     /* centre of A1 addressable area */
#ifdef __TASKING__
extern EE_FAR void * EE_A8_DATA;          /* centre of A8 addressable area */
extern EE_FAR void * EE_A9_DATA;          /* centre of A9 addressable area */
extern EE_FAR void * EE_INT_TAB;          /* interrupt table */
extern EE_FAR void * EE_TRAP_TAB;         /* trap table */
#endif /* __TASKING__ */

#ifdef EE_C_INIT_TC
/* Specific core initialization function */
extern void EE_C_INIT_TC ( void );
#endif /* EE_C_INIT_TC */

/* passed to exit libc function */
extern int main( void );

/* start-up functions */
void __NEVER_INLINE__ JUMP EE_tc2Yx_start( void );
/* This MUST be inline because you cannot make a real function call
   before initialize CSAs lists */
__INLINE__ void __ALWAYS_INLINE__ EE_tc2Yx_csa_init( void );

/* Declare user start-up callback before endinit  */
#ifdef EE_START_UP_USER_ENDINIT
extern void EE_START_UP_USER_ENDINIT( void );
#endif /* EE_START_UP_ENDINIT */

#if defined(__TASKING__) && defined(EE_ERIKA_TRAP_HANDLING_OFF)
/* If ERIKA support for trap handling is turned off, but still user will
   use default boot, enable trap vector from the C-library. */

EE_COMPILER_EXTERN(_trapmmu)
EE_COMPILER_EXTERN(_trapprotection)
EE_COMPILER_EXTERN(_trapinstruction)
EE_COMPILER_EXTERN(_trapcontext)
EE_COMPILER_EXTERN(_trapbus)
EE_COMPILER_EXTERN(_trapassertion)
EE_COMPILER_EXTERN(_trapsystem)
EE_COMPILER_EXTERN(_trapnmi)
#endif /* __TASKING__ && EE_DEFAULT_TRAP_HANDLING_OFF */

#ifdef EE_MASTER_CPU

/* Boot Mode Headers are generated by the linker script with Tasking compiler */
#ifndef __TASKING__
/*******************************************************************************
**                      Boot Mode Headers                                     **
*******************************************************************************/
/*Boot Mode Header 0 sections to inform linker to locate them at 0x8000 0000 */
#if defined(__GNUC__)
#pragma section
#pragma section ".bmhd0" a
#endif

#if defined(__DCC__)
#pragma section CONST ".bmhd0" R
#endif
/** \brief Boot Mode Header 0
 * Boot mode header at memory location 0c8000 0000.
 */
const EE_UINT32 BootModeHeader0[] = {
    0x00000000,                 /* STADBM first user code at 0x8000 0020h */
    0xb3590070,                 /* BMI = 0070h BMHDID = B359h */
    0x00000000,                 /* ChkStart */
    0x00000000,                 /* ChkEnd */
    0x00000000,                 /* CRCrange */
    0x00000000,                 /* !CRCrange */
    0x791eb864,                 /* CRChead */
    0x86e1479b                  /* !CRChead */
};

/*reset the sections defined above */
#if defined(__GNUC__)
#pragma section
#endif
#if defined(__DCC__)
#pragma section CONST
#endif

#ifndef EE_TC_BMH01_NOT_NEEDED
/*Boot Mode Header 1 sections to inform linker to locate them at 0x8002 0000 */
#if defined(__GNUC__)
#pragma section
#pragma section ".bmhd1" a
#endif
#if defined(__DCC__)
#pragma section CONST ".bmhd1" R
#endif

/** \brief Boot Mode Header 1
 * Boot mode header at memory location 0c8002 0000.
 */
const EE_UINT32 BootModeHeader1[] = {
    0x00000000,                 /* STADBM first user code at 0x8000 0020h */
    0xB3590070,                 /* BMI = 0070h BMHDID = B359h */
    0x00000000,                 /* ChkStart */
    0x00000000,                 /* ChkEnd */
    0x00000000,                 /* CRCrange */
    0x00000000,                 /* !CRCrange */
    0x791eb864,                 /* CRChead */
    0x86e1479b                  /* !CRChead */
};
/*reset the sections defined above */
#if defined(__GNUC__)
#pragma section
#endif
#if defined(__DCC__)
#pragma section CONST
#endif
#endif /* !EE_TC_BMH01_NOT_NEEDED */
#endif /* !__TASKING__*/
#endif /* EE_MASTER_CPU */

/******************************************************************************
 * reset vector address, user section to inform linker to locate the code at
 * (0x8000 0020)
 *****************************************************************************/
#if defined(__GNUC__)
#pragma section
#pragma section ".startup" x
#endif
#if defined(__TASKING__)
#pragma protect on
#pragma section code "startup"
#endif
#if defined(__DCC__)
#pragma section CODE ".startup" X
#endif

#if defined(EE_MASTER_CPU)
void _START(void)
{
#if defined(__GNUC__) || defined(__TASKING__)
  __asm ("ja EE_tc2Yx_start");
#elif defined(__DCC__)
  __asm ("  movh.a %a15,EE_tc2Yx_start@ha");
  __asm ("  lea  %a15,[%a15]EE_tc2Yx_start@l");
  __asm ("  ji %a15");
#endif /* __GNUC__ || __TASKING__ || __DCC__ */
}
#endif /* EE_MASTER_CPU */

#if !defined(EE_MASTER_CPU) && defined(__TASKING__)
void _START(void){}
#endif


/* reset the sections defined above, to normal region */
#if defined(__GNUC__)
#pragma section
#endif
#if defined(__TASKING__)
#pragma protect restore
#pragma section code restore
#endif
#if defined(__DCC__)
#pragma section CODE
#endif


/*******************************************************************************
 * @brief startup code
 ******************************************************************************/
#define EE_TC_START_PSW         0x00000B80U
#define EE_TC_START_PSW_ISP     0x00000980U

#if defined(__MSRP__) && defined(EE_MASTER_CPU) && defined(__GNUC__)
#pragma section "ee_kernel_start" ax 4
#endif /* __MSRP__ && EE_MASTER_CPU && __GNUC__ */

void __NEVER_INLINE__ JUMP EE_TC2YX_START( void )
{
  EE_UREG pcxi;

#if defined (__MULTI__) && defined(__IRQ_STACK_NEEDED__)
  EE_UINT32 isp;
#endif

  /* Load user stack pointer. First thing to do */
  EE_ADDR const sp = (EE_ADDR)
      ((EE_UINT32)(EE_E_USTACK) & EE_STACK_ALIGN);
  EE_tc_set_SP(sp);

#if (!defined(__OO_BCC1__)) && (!defined(__OO_BCC2__)) && \
    (!defined(__OO_ECC1__)) && (!defined(__OO_ECC2__))
/* Multicore initialization needed for non OSEK Kernels */
#ifdef EE_MASTER_CPU
#ifdef EE_START_CPU1
  /*
   * Set start address of CPU1
   * Reset value is 0xAFFFC000
   */
  extern void EE_tc2Yx_cpu1_start ( void );
  CPU1_PC.U = (EE_UINT32)EE_tc2Yx_cpu1_start;
#endif /* EE_START_CPU1 */

#ifdef EE_START_CPU2
  /*
   * Set start address of CPU2
   * Reset value is 0xAFFFC000
   */
  extern void EE_tc2Yx_cpu2_start ( void );
  CPU2_PC.U = (EE_UINT32)EE_tc2Yx_cpu2_start;
#endif /* EE_START_CPU2 */
#endif /* EE_MASTER_CPU */
#endif /* !__OO_BCC1__ && !__OO_BCC2__ && !__OO_ECC1__ && !__OO_ECC2__ */

  /* Do a dsync before changing any of the csfr values, thus any previous
   * background state gets flushed first. Required for applications that jump
   * to the reset address.
   */
  EE_tc_dsync();

  /* Set the PSW to its reset value in case of a warm start */
#if defined (__MULTI__) && defined(__IRQ_STACK_NEEDED__)
  /* clear PSW.IS */
  EE_tc_set_psw(EE_TC_START_PSW_ISP);
#else /* __MULTI__ &&  __IRQ_STACK_NEEDED__  */
  EE_tc_set_psw(EE_TC_START_PSW);
#endif /* __MULTI__ &&  __IRQ_STACK_NEEDED__  */

  /* Set the PCXS and PCXO to its reset value in case of a warm start */
  pcxi = EE_tc_get_pcxi();
  pcxi &= 0xFFF00000U;
  EE_tc_set_pcxi(pcxi);

  /*
   * Clear the ENDINIT bit in the WDT_CON0 register in order
   * to disable the write-protection for registers protected
   * via the EndInit feature (for example: WDT_CON1).
   */
  EE_tc2Yx_endinit_set(EE_TC_ENDINIT_DISABLE);

  /*
   * Disable the Watchdog if requested. Watchdog is enabled by default.
   * The Watchdog is disabled after ENDINIT is set by endinit_set().
   */
  EE_WDTCPUCON1.U |= 0x8;

#ifdef EE_MASTER_CPU
  /*
   * Clear the ENDINIT bit in the WDTSCON0 register in order
   * to disable the write-protection for safety-critical registers
   * protected via the safety EndInit feature.
   */
  EE_tc2Yx_safety_endinit_set(EE_TC_ENDINIT_DISABLE);

  /*
   * Disable the safety watchdog if requested. Safety watchdog
   * is enabled by default. The safety watchdog is disabled after
   * ENDINIT is set by safety_endinit_set().
   */
  SCU_WDTSCON1.U |= 0x8;

#endif /* EE_MASTER_CPU */

  /*
   * Load Base Address of Trap Vector Table.
   * Disable this if not started from RESET vector. (E.g.
   * ROM monitors require to keep in control of vectors)
   */
  EE_tc_set_csfr(EE_CPU_REG_BTV, (EE_UINT32)&EE_TRAP_TAB);

  /*
   * Load Base Address of Interrupt Vector Table.
   * Disable this if not started from RESET vector. (E.g.
   * ROM monitors require to keep in control of vectors)
   */
  EE_tc_set_csfr(EE_CPU_REG_BIV, (EE_UINT32)&EE_INT_TAB);

#ifdef EE_ICACHE_ENABLED
  /*
   * PCON0 configuration.
   */
  EE_tc_set_csfr(EE_CPU_REG_PCON0, 0U);
#endif

#ifdef EE_DCACHE_ENABLED
  /*
   * DCON0 configuration.
   */
  EE_tc_set_csfr(EE_CPU_REG_DCON0, 0U);
#endif

#if (!defined(__OO_BCC1__)) && (!defined(__OO_BCC2__)) && \
    (!defined(__OO_ECC1__)) && (!defined(__OO_ECC2__))
/* Clock Initialization needed for non OSEK Kernels */
#if defined(EE_MASTER_CPU) && defined(EE_CPU_CLOCK)
/******** Configure CCU Clock Control. ********/
  EE_tc2Yx_configure_clock_ctrl();

/********* Configure Oscillator Control. *********/
  EE_tc2Yx_configure_osc_ctrl();

/***************** Configure PLL *****************/

  /* The internal version of EE_tc2Yx_configure_clock is an inline
     function (to which the MCU API simple delegate) I use this version
     here because I cannot CALL a fuction before configuring CSA
     (thing done later) */
  EE_tc2Yx_configure_clock_internal(EE_CPU_CLOCK);

#ifdef __TASKING__
  /*
   * Enable the GTM to get MCS memory access,
   * required for MCS initialization which
   * is performed by _c_init. (TASKING)
   */
  GTM_CLC.U = 0U;
#endif /* __TASKING__ */
#endif /* !__OO_BCC1__ && !__OO_BCC2__ && !__OO_ECC1__ && !__OO_ECC2__ */

#endif /* EE_MASTER_CPU && EE_CPU_CLOCK */

  /*
   * Inititialize global address registers a0/a1 to support
   * __a0/__a1 storage qualifiers of the C compiler.
   */
  EE_tc2Yx_setareg(a0, EE_SMALL_DATA);

  EE_tc2Yx_setareg(a1, EE_LITERAL_DATA);
#ifdef __TASKING__
  /* NOT YET SUPPORTED BY OUR LINKER SCRIPTS FOR GNUC
   * Inititialize global address registers a8/a9 to support
   * __a8/__a9 storage qualifiers of the C compiler. A8 and A9
   * are reserved for OS use, or for application use in cases
   * where the application ans OS are tightly coupled.
   */

  EE_tc2Yx_setareg(a8, EE_A8_DATA);

  EE_tc2Yx_setareg(a9, EE_A9_DATA);
#endif /* __TASKING__ */

  /* Setup the context save area linked list. This MUST be inline because
     you cannot make a real function call before initialize CSAs lists */
  EE_tc2Yx_csa_init();

  /*
   * Initialize and clear C variables.
   */
  EE_tc2Yx_cinit();      /* initialize data */

#ifdef EE_C_INIT_TC
  EE_C_INIT_TC();      /* initialize core specific data */
#endif /* EE_C_INIT_TC */

#if defined (__MULTI__) && defined(__IRQ_STACK_NEEDED__)
  /*
   * Load interrupt stack pointer.
   * Disable this if not started from RESET vector. (E.g.
   * ROM monitors require to keep in control of vectors)
   */
  /* EE_UINT32 isp = (EE_UINT32)(_lc_ue_istack) & EE_STACK_ALIGN; */
  isp = (EE_UINT32)EE_tc_IRQ_tos.SYS_tos & EE_STACK_ALIGN;
  EE_tc_set_csfr(EE_CPU_REG_ISP, isp);
#endif /* __MULTI__ &&  __IRQ_STACK_NEEDED__ */

  /*
   * Set the ENDINIT bit in the WDT_CON0 register again
   * to enable the write-protection.
   */
  EE_tc2Yx_endinit_set(EE_TC_ENDINIT_ENABLE);

#ifdef EE_MASTER_CPU
  /*
   * Set the ENDINIT bit in the WDTSCON0 register to enable the
   * safety-critical register write-protection.
   */
  EE_tc2Yx_safety_endinit_set(EE_TC_ENDINIT_ENABLE);
#endif /* EE_MASTER_CPU */

#ifdef EE_START_UP_USER_ENDINIT
  /*  Call the user callback to let him do initial configuration with
      endinit & safe_endinit protections disabled. Beware that protected
      registers are unlocked for the duration of the Time-out
      Period only! */
  EE_tc2Yx_endinit_set(EE_TC_ENDINIT_DISABLE);
#ifdef EE_MASTER_CPU
  EE_tc2Yx_safety_endinit_set(EE_TC_ENDINIT_DISABLE);
#endif /* EE_MASTER_CPU */
  EE_START_UP_USER_ENDINIT();
  EE_tc2Yx_endinit_set(EE_TC_ENDINIT_ENABLE);
#ifdef EE_MASTER_CPU
  EE_tc2Yx_safety_endinit_set(EE_TC_ENDINIT_ENABLE);
#endif /* EE_MASTER_CPU */
#endif /* EE_START_UP_USER_ENDINIT */

  /*
   * Call C main program.
   */
  EE_EXIT( main() );
}

/**************************************************************************
 *
 * FUNCTION:     EE_tc2Yx_csa_init
 *
 * DESCRIPTION: Setup the context save area linked list. This MUST be
 *              inline because you cannot make a real function call before
 *              initialize CSAs lists
 *
 *************************************************************************/
__INLINE__ void __ALWAYS_INLINE__ EE_tc2Yx_csa_init( void )
{
  /* Context Save Area base */
  extern EE_CSA EE_B_CSA[];
  /* Context Save Area end  */
  extern EE_CSA EE_E_CSA[];
  /* Nr of CSAs in area 1      */
  EE_INT32 const no_of_csas = EE_E_CSA - EE_B_CSA;
  /* Current CSA segment (I handle only one segment) */
  EE_UINT32 const pcxi_seg = (((EE_UINT32)EE_E_CSA >> 28U) & 0xFU) << 16U;
  /* for cycle index */
  EE_UINT32 i;
  /* Current CSA offset (I handle only one segment) */
  EE_UINT32 pcxi_o;
  /* Previous Context Pointer (CSA Link Word) */
  EE_UINT32 pcxi_val = 0U;
  /*  This variable hold the number of CSA that I have to leave free to
      handle FCD trap 3 are needed by this trap and 3 more are needed for
      potential nested asynchronous trap.
      To handle a trap 3 CSA are needed because class trap handler entry
      call TNI switch routine that call TNI sub-handler.
  */
  EE_UINT32 fcd_needed_csa = 6U;
  /* Current CSA pointer */
  EE_CSA * csa;

  /* IMPORTANT:
     I initialize the Free Context List in direct order.
     So I will end with a list that will grow toward smaller addresses.
   */
  for (i = 0U; i < (EE_UINT32)no_of_csas; ++i)
  {
    /* Get th current CSA pointer */
    csa = &EE_B_CSA[i];
    /* Store in current CSA previous pointer
       (null pointer in last CSA = first time!) */
    csa->next.reg = pcxi_val;
    /* Evaluate the CSA Segment Offset */
    pcxi_o = (((EE_UINT32)csa >> 6U) & 0xFFFFU);
    /* Compose pcxi_seg and pcxi_o in the pcxi_val to be stored in next CSA */
    pcxi_val = pcxi_seg | pcxi_o;

    /* Check if you have to populate LCX that point to the 'almost empty'
       position  */
    --fcd_needed_csa;
    if (fcd_needed_csa == 0U)
    {
      EE_tc_set_csfr(EE_CPU_REG_LCX, pcxi_val);
    }
  }
  /* Initialize the HEAD of Free Context List */
  EE_tc_set_fcx(pcxi_val);
}

#if defined(__GNUC__) && (!defined(EE_EXECUTE_FROM_RAM))
/**************************************************************************
 *
 * FUNCTION:     EE_tc2Yx_cinit
 *
 * DESCRIPTION: Initialize C Data
 *
 *************************************************************************/
/* Clear, Copy Tables pointers */
typedef volatile union
{
  EE_UINT8  ucPtr;
  EE_UINT16 usPtr;
  EE_UINT32 uiPtr;
  EE_UINT64 ullPtr;
} EE_TABLE_PTR;

static void EE_tc2Y_apply_clear_table ( EE_UINT32 * pTable ) {
  EE_UINT32 uiLength, uiCnt;
  EE_TABLE_PTR *pBlockDest;

  while (pTable)
  {
    pBlockDest = (EE_TABLE_PTR *) * pTable;
    pTable++;
    uiLength = *pTable;
    pTable++;
    /* we are finished when length == -1 */
    if (uiLength == 0xFFFFFFFFU) {
      break;
    }
    uiCnt = uiLength / 8U;
    while (uiCnt) {
      (*pBlockDest).ullPtr = (EE_UINT64)0U;
      pBlockDest++;
      uiCnt--;
    }
    if ((uiLength) & 0x4U) {
      (*pBlockDest).uiPtr = (EE_UINT32)0U;
      pBlockDest++;
    }
    if ((uiLength) & 0x2U) {
      (*pBlockDest).usPtr = (EE_UINT16)0U;
      pBlockDest++;
    }
    if ((uiLength) & 0x1U) {
      (*pBlockDest).ucPtr = 0U;
    }
  }
}

static void EE_tc2Y_apply_copy_table ( EE_UINT32 * pTable ) {
  EE_UINT32 uiLength, uiCnt;
  EE_TABLE_PTR *pBlockDest;
  EE_TABLE_PTR *pBlockSrc;

  while (pTable)
  {
    pBlockSrc = (EE_TABLE_PTR *) * pTable;
    pTable++;
    pBlockDest = (EE_TABLE_PTR *) * pTable;
    pTable++;
    uiLength = *pTable;
    pTable++;
    /* we are finished when length == -1 */
    if (uiLength == 0xFFFFFFFFU) {
      break;
    }
    uiCnt = uiLength / 8U;
    while (uiCnt) {
      (*pBlockDest).ullPtr = (*pBlockSrc).ullPtr;
      uiCnt--;
      pBlockDest++;
      pBlockSrc++;
    }
    if ((uiLength) & 0x4U) {
      (*pBlockDest).uiPtr = (*pBlockSrc).uiPtr;
      pBlockDest++;
      pBlockSrc++;
    }
    if ((uiLength) & 0x2U) {
      (*pBlockDest).usPtr = (*pBlockSrc).usPtr;
      pBlockDest++;
      pBlockSrc++;
    }
    if ((uiLength) & 0x1U) {
      (*pBlockDest).ucPtr = (*pBlockSrc).ucPtr;
    }
  }
}

static void EE_tc2Yx_cinit( void ) {
  /* clear table entry */
  extern EE_UINT32 __clear_table[];
  /* copy table entry */
  extern EE_UINT32 __copy_table[];
#if (defined(__MSRP__) && defined(EE_BUILD_SINGLE_ELF)) && defined(EE_MASTER_CPU)
  /* copy table entry */
  extern EE_UINT32 ee_mcglobald_copy_table[];
#endif /* __MSRP__ && EE_BUILD_SINGLE_ELF && EE_MASTER_CPU */

  /* clear table */
  EE_tc2Y_apply_clear_table(__clear_table);
  /* copy table */
  EE_tc2Y_apply_copy_table(__copy_table);

#if (defined(__MSRP__) && defined(EE_BUILD_SINGLE_ELF)) && defined(EE_MASTER_CPU)
  /* Initialize global data */
  EE_tc2Y_apply_copy_table(ee_mcglobald_copy_table);
#endif /* __MSRP__ && EE_BUILD_SINGLE_ELF && EE_MASTER_CPU */
}
#endif /* __GNUC__ && !EE_EXECUTE_FROM_RAM */

