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

 /** @file   ee_tc2Yx_mcu.h
  *  @brief  MCU-dependent part of API
  *  @author Errico Guidieri
  *  @date 2012
  */
#ifndef INCLUDE_EE_TC2YX_MCU_H__
#define INCLUDE_EE_TC2YX_MCU_H__

/* Include CPU API */
#include "cpu/tricore/inc/ee_tc_cpu.h"

/****************************************************************
          Services Requests Node (SRN) Utility Macros
 ****************************************************************/
#define EE_TC2YX_SRN_PRIORITY(intvec)     (((EE_UINT32)(intvec)) & \
  (EE_UINT32)0x7FU)
#define EE_TC2YX_SRN_ENABLE               ((EE_UINT32)1U << 10U)
#define EE_TC2YX_SRN_TYPE_OF_SERVICE(tos) ((((EE_UINT32)(tos)) & \
  (EE_UINT32)0x3U) << 11U)
#define EE_TC2YX_SRN_CLEAR_REQUEST        ((EE_UINT32)1U << 25U)
#define EE_TC2YX_SRN_SET_REQUEST          ((EE_UINT32)1U << 26U)


/******************************************************************************
              Multicore with single ELF x Core Symbols Remapping
 *****************************************************************************/
/* Generate the CORE_SYM suffux has is done by HIGHTEC ld binutils */
#if defined(__MSRP__) && defined(EE_BUILD_SINGLE_ELF)
#ifndef __GNUC__
#error Multicore Single ELF build is supported only by HIGHTEC GNU Compiler
#endif /* __GNUC__ */

#define EE_CPU_SUFFIX3(sym3)      EE_PREPROC_JOIN(sym3,_)
#define EE_CPU_SUFFIX2(sym2,cpu)  EE_CPU_SUFFIX3(EE_PREPROC_JOIN(sym2,cpu))
#define EE_CPU_SUFFIX(sym)        EE_CPU_SUFFIX2(EE_PREPROC_JOIN(sym,_CPU),\
  EE_CURRENTCPU)

#define __USTACK_BEGIN  EE_CPU_SUFFIX(__USTACK_BEGIN) /* user stack base */
#define __USTACK        EE_CPU_SUFFIX(__USTACK)       /* user stack end */
#define __ISTACK        EE_CPU_SUFFIX(__ISTACK)       /* interrupt stack end */
/* centre of A0 addressable area */
#define _SMALL_DATA_    EE_CPU_SUFFIX(_SMALL_DATA_)
/* centre of A1 addressable area */
#define _SMALL_DATA2_   EE_CPU_SUFFIX(_SMALL_DATA2_)
/* centre of A8 addressable area */
#define _SMALL_DATA3_   EE_CPU_SUFFIX(_SMALL_DATA3_)
/* centre of A9 addressable area */
#define _SMALL_DATA4_   EE_CPU_SUFFIX(_SMALL_DATA4_)
/* Context Save Area base */
#define __CSA_BEGIN     EE_CPU_SUFFIX(__CSA_BEGIN)
/* Context Save Area end  */
#define __CSA_END       EE_CPU_SUFFIX(__CSA_END)

/* Begin Code Range */
#define ee_sall_code    EE_CPU_SUFFIX(ee_sall_code)
/* End Code Range */
#define ee_eall_code    EE_CPU_SUFFIX(ee_eall_code)

/* Ram Begin Address */
#define ee_skernel_ram  EE_CPU_SUFFIX(ee_skernel_ram)
/* Kernel Ram End Address */
#define ee_ekernel_ram  EE_CPU_SUFFIX(ee_ekernel_ram)

/* Begin Const API Section Address */
#define ee_sapi_const   EE_CPU_SUFFIX(ee_sapi_const)
/* End Const API Flash End Address */
#define ee_eapi_const   EE_CPU_SUFFIX(ee_eapi_const)

/* API Ram Begin Address */
#define ee_sapi_ram     EE_CPU_SUFFIX(ee_sapi_ram)
/* API Ram End Address */
#define ee_eapi_ram     EE_CPU_SUFFIX(ee_eapi_ram)

/* Kernel Data Structures range Begin Address */
#define ee_sbss_kernel  EE_CPU_SUFFIX(ee_sbss_kernel)
/* Kernel Data Structures End Address */
#define ee_edata_kernel EE_CPU_SUFFIX(ee_edata_kernel)

/* Kernel Data Structures range Begin Address */
#define ee_sbss_kernel  EE_CPU_SUFFIX(ee_sbss_kernel)
/* Kernel Data Structures End Address */
#define ee_edata_kernel EE_CPU_SUFFIX(ee_edata_kernel)

/* Kernel Code Section Begin Address */
#define ee_skernel_code EE_CPU_SUFFIX(ee_skernel_code)
/* Kernel Code Section End Address */
#define ee_ekernel_code EE_CPU_SUFFIX(ee_ekernel_code)

/* API Code Section Begin Address */
#define ee_sapi_code EE_CPU_SUFFIX(ee_sapi_code)
/* API Code Section End Address */
#define ee_eapi_code EE_CPU_SUFFIX(ee_eapi_code)

/* Stacks Table Address */
#define ee_stacks_table EE_CPU_SUFFIX(ee_stacks_table)
/* Kernel Stacks Section Start Address */
#define ee_sstack_kernel EE_CPU_SUFFIX(ee_sstack_kernel)
/* Kernel Stacks Section End Address */
#define ee_estack_kernel EE_CPU_SUFFIX(ee_estack_kernel)

/* Clear table entry */
#define __clear_table EE_CPU_SUFFIX(__clear_table)
/* Copy table entry */
#define __copy_table  EE_CPU_SUFFIX(__copy_table)

#else /* __MSRP__ && EE_BUILD_SINGLE_ELF */
#define EE_CPU_SUFFIX(sym)  sym
#endif /* __MSRP__ && EE_BUILD_SINGLE_ELF */

#ifdef  EE_MASTER_CPU
/******************************************************************************
                        Startup Symbols Remapping
 *****************************************************************************/

#ifdef __TASKING__
/* Start-Up Symbols Remapping */
#define EE_B_USTACK       _lc_ub_ustack_tc0 /* user stack base */
#define EE_E_USTACK       _lc_ue_ustack     /* user stack end */
#define EE_E_ISTACK       _lc_ue_istack     /* interrupt stack end */
#define EE_INT_TAB        _lc_u_int_tab     /* interrupt table */
#define EE_TRAP_TAB       _lc_u_trap_tab    /* trap table */
#define EE_SMALL_DATA     _SMALL_DATA_      /* centre of A0 addressable area */
#define EE_LITERAL_DATA   _LITERAL_DATA_    /* centre of A1 addressable area */
#define EE_A8_DATA        _A8_DATA_         /* centre of A8 addressable area */
#define EE_A9_DATA        _A9_DATA_         /* centre of A9 addressable area */
#define EE_B_CSA          _lc_ub_csa_01     /* Context Save Area base */
#define EE_E_CSA          _lc_ue_csa_01     /* Context Save Area end  */

/* Core Start-up code entry */
#define EE_TC2YX_START      EE_COMPILER_SECTION(ee_kernel_start) EE_tc2Yx_start

#elif defined (__GNUC__) || defined (__DCC__)

#define EE_B_USTACK     __USTACK_BEGIN  /* user stack base */
#define EE_E_USTACK     __USTACK        /* user stack end */
#define EE_E_ISTACK     __ISTACK        /* interrupt stack end */

/* #define EE_INT_TAB  __inttab_start   interrupt table (linker symbol) */
/* #define EE_TRAP_TAB __traptab_start trap table (linker symbol) */
/* Generated in code */
#define EE_INT_TAB      EE_tc_interrupt_table /* interrupt table */
#define EE_TRAP_TAB     EE_tc_trap_table      /* trap table */

#define EE_SMALL_DATA     _SMALL_DATA_    /* centre of A0 addressable area */
#define EE_LITERAL_DATA   _SMALL_DATA2_   /* centre of A1 addressable area */
#define EE_A8_DATA        _SMALL_DATA3_   /* centre of A8 addressable area */
#define EE_A9_DATA        _SMALL_DATA4_   /* centre of A9 addressable area */
#define EE_B_CSA          __CSA_BEGIN     /* Context Save Area base */
#define EE_E_CSA          __CSA_END       /* Context Save Area end  */

/* Core Start-up code entry */
#define EE_TC2YX_START  EE_tc2Yx_start

#else
#error Unsupported compiler!
#endif /* __TASKING__ || __GNUC__ || __DCC__ */
#endif /* EE_MASTER_CPU */

/*********************************************************************
                Multicore and multiprocessor support
 *********************************************************************/
/* Include multicore support there's a guard inside */
#include "mcu/infineon_common_tc2Yx/inc/ee_tc2Yx_multicore.h"

/****************************************************************
                    System Timer Support
 ****************************************************************/

/* Legit Devices for System Timer Defines. */
/* N.B:
 * For the system timer we will use STM periperal. This periferal
 * is composed by a 64 bit upper cunter in free-run, two compare
 * registers (32 bit with offset and mask-length configurable),
 * and two services (read: interrupt sources). Each Compare
 * register can be tied to both service, and that would mean
 * 4 STM meaningfull configuration, but only two independent.
 * For semplicity i fix a degrees of freedom tying compare register
 * with corrisponding service number. So the configuration will be
 * easyer, still having two independent services source.
 *
 * (Check the documentention for more information)
 */

#define EE_TC_STM_SR0 1
#define EE_TC_STM_SR1 2

/****************************************************************
                    SCU OSCCON Support
 ****************************************************************/
/* Command to reset Watchdog Oscillator. Bits[2] */
#define EE_TC2YX_SCU_OSCCON_OSCRES ( (EE_UINT32)1U << 2U )
/* Default Reset Value for OSCCCON.GAIN Should not be Changed. Bits[3:4] */
#define EE_TC2YX_SCU_OSCCON_GAINSEL ( (EE_UINT32)3U << 3U )
/* Extern Cristall is 00 mode so this define doesn't change the OSSCON
   Value. Bits[5:6] */
#define EE_TC2YX_SCU_OSCCON_MODE(mode) ( (((EE_UINT32)(mode)) & \
  (EE_UINT32)0x3U) << 5U )
/* OSCCCON.OSCVAL OSC Frequency Value
   fOSC is divided by OSCVAL + 1. Bits [16:20]
   (15 is The only value that work. Found empirically) */
#define EE_TC2YX_SCU_OSCCON_OSCVAL(oscval) ( (((EE_UINT32)(oscval)) & \
  (EE_UINT32)0xFU) << 16U )

__INLINE__ void __ALWAYS_INLINE__ EE_tc2Yx_set_osccon( EE_UREG value )
{
  EE_tc_safety_endinit_disable();
  SCU_OSCCON.U = value;
  EE_tc_safety_endinit_enable();
}

/****************************************************************
                    SCU Clock Support
 ****************************************************************/
/*  This bit defined the update for the CCU.
    Please note that setting this bit request a CCU
    update based on all three registers,
    so only one UP bit have to be set.

    1B A new complete parameter set is transferred
    to the CCU. All three registers CCUCON0, 1
    and 5 content is taken by CCU. */
#define EE_TC2YX_CCUCONX_UP  ( (EE_UINT32)0x1U << 30U )

/** CCU0 defines **/

/*  Baud1 Divider Reload Value. 0001B fBAUD1 = fsource. Bits [0:3] */
#define EE_TC2YX_CCUCON0_BAUD1DIV(baud1)  (((EE_UINT32)(baud1)) & \
  (EE_UINT32)0xFU)
/*  Baud2 Divider Reload Value. 0001B fBAUD2 = fsource. Bits[4:7] */
#define EE_TC2YX_CCUCON0_BAUD2DIV(baud2)  ((((EE_UINT32)(baud2)) & \
  (EE_UINT32)0xF) << 4U)
/*  SRI Divider Reload Value 0001B fSRI = fsource. Bits [8:11] */
#define EE_TC2YX_CCUCON0_SRI(sri)         ((((EE_UINT32)(sri)) & \
  (EE_UINT32)0xFU) << 8U)

/*  Low Power Divider Reload Value 0001B fSRI, fSPB, and fBBB = fsource/30;
    fMAX = fsource/15. Bits [15:12] */
#define EE_TC2YX_CCUCON0_LPDIV(lpdiv) ((((EE_UINT32)(lpdiv)) & \
  (EE_UINT32)0xFU) << 12U)
/*  SPB Divider Reload Value 0001B Reserved 0010B fSPB = fsource/2.
    Bits [16:19] */
#define EE_TC2YX_CCUCON0_SPB(spb)     (((((EE_UINT32)(spb)) & \
  (EE_UINT32)0xFU) == 1U)? ((EE_UINT32)0x2U << 16U):((((EE_UINT32)(spb)) & \
  (EE_UINT32)0xFU) << 16U))
/*  FSI2 Divider Reload Value 10B fFSI2 = fSRI / 2 for SRIDIV = 0001B
    or 0010B, else fFSI2 = fSRI. Bits [20:21] */
#define EE_TC2YX_CCUCON0_FSI2(fsi2)   ((((EE_UINT32)(fsi2)) & \
  (EE_UINT32)0x3U) << 20U)
/*  FSI Divider Reload Value 10B fFSI = fSRI / 2 for SRIDIV = 0001B
    or 0010B, else fFSI = fSR. Bits [24:25] */
#define EE_TC2YX_CCUCON0_FSI(fsi)     ((((EE_UINT32)(fsi)) & \
  (EE_UINT32)0x3U) << 24U)
/*  ADC Clock Selection This bit field defines the clock source that is
    used for the clock fADC of the ADCs. Bits[26:27]*/
#define EE_TC2YX_CCUCON0_ADC(adc)     ((((EE_UINT32)(adc)) & \
  (EE_UINT32)0x3U) << 26U)
/*  Clock Selection: fERAY and fFSI. 1B fPLL is used as clock source fsource.
    Bits [28:29] */
#define EE_TC2YX_CCUCON0_CLKSEL(clk)  ((((EE_UINT32)(clk)) & \
  (EE_UINT32)0x1U) << 28U)

/** CCU1 defines **/
/** TODO: Complete them **/
/* STM Divider Reload Value. 0001B fSTM = fsource. Bits[8:11] */
#define EE_TC2YX_CCUCON1_STMDIV(stm)  ((((EE_UINT32)(stm)) & \
  (EE_UINT32)0x3U) << 8U)
/* GTM Divider Reload Value. 0001B fGTM = fsource. Bits[12:15] */
#define EE_TC2YX_CCUCON1_GTMDIV(gtm)  ((((EE_UINT32)(gtm)) & \
  (EE_UINT32)0x3U) << 12U)
/* Input Selection source for the PLL and PLL_ERAY. 01B fOSC0
   is used as clock source. Bits [28:29] */
#define EE_TC2YX_CCUCON1_INSEL(in)    ((((EE_UINT32)(in)) & \
  (EE_UINT32)0x1U) << 28U)

/* PLL Frequencies Bound Defines */
#define EE_TC2YX_CLOCK_MIN          20000000U
#define EE_TC2YX_CLOCK_MAX          200000000U

#ifdef EE_MASTER_CPU
/** @brief  Set PLL frequency. This function accept fpll HZ **/
void EE_tc2Yx_configure_clock( EE_UREG fpll );
#endif /* EE_MASTER_CPU */

/** @brief  Return PLL frequency in HZ. **/
EE_UREG EE_tc2Yx_get_clock( void );

/****************************************************************
                        STM Support
 ****************************************************************/

/** @brief Initialize a global variable with STM frequency. */
void EE_tc2Yx_stm_set_clockpersec( void );

#if (EE_SYSTEM_TIMER_DEVICE != EE_TC_STM_SR0)
/**
  *  @brief Programs STM compare register 0 to trigger an IRQ after
  *         usec microseconds
  *  @param usec microseconds after you want get an IRQ on intvec
  *  @param intvec Interrupt Vector Table entry that will handle STM interrupt.
  *         With intvec == 0, the correponding service request node is left
  *         unprogrammed or resetted.
  */
void EE_tc2Yx_stm_set_sr0( EE_UINT32 usec, EE_TYPEISR2PRIO intvec );

/**
  *  @brief Change previous programmed STM compare register 0 to trigger next
            IRQ after usec microseconds
  *  @param usec microseconds after you want get an IRQ on intvec
  */
void EE_tc2Yx_stm_set_sr0_next_match( EE_UINT32 usec );
#endif /* EE_SYSTEM_TIMER_DEVICE != EE_TC_STM_SR0 */
#if (EE_SYSTEM_TIMER_DEVICE != EE_TC_STM_SR1)
/**
  *  @brief Programs STM compare register 1 to trigger an IRQ after
  *         usec microseconds
  *  @param usec microseconds after you want get an IRQ on intvec
  *  @param intvec Interrupt Vector Table entry that will handle STM interrupt.
  *         With intvec == 0, the correponding service request node is left
  *         unprogrammed or resetted.
  */
void EE_tc2Yx_stm_set_sr1( EE_UINT32 usec, EE_TYPEISR2PRIO intvec );

/**
  * @brief  Change previous programmed STM compare register 1 to trigger next
  *         IRQ after usec microseconds
  * @param  usec microseconds after you want get an IRQ on intvec
  */
void EE_tc2Yx_stm_set_sr1_next_match( EE_UINT32 usec );
#endif /* EE_SYSTEM_TIMER_DEVICE != EE_TC_STM_SR1 */


/* STM TIM0 and CAP(ture) Register Selector */
#ifdef EE_MASTER_CPU
/* registers */
#define EE_STM_TIM0     STM0_TIM0
#define EE_STM_CAP      STM0_CAP
#elif (EE_CURRENTCPU == 1)
#define EE_STM_TIM0     STM1_TIM0
#define EE_STM_CAP      STM1_CAP
#elif (EE_CURRENTCPU == 2)
#define EE_STM_TIM0     STM2_TIM0
#define EE_STM_CAP      STM2_CAP
#else 
#error Unknown CPU ID
#endif /* EE_CURRENTCPU */

/**
  * @brief  Used to read lower word of STM peripheral's 64 bit counter.
  *         To guarantee coherency lower word must be read before than upper
  *         word.
 */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc2Yx_stm_get_time_lower_word( void )
{
 return EE_STM_TIM0.U;
}

/**
  * @brief  Used to read upper word of STM peripheral's 64 bit counter.
  *         To guarantee coherency lower word must be read before than upper
  *         word.
  */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc2Yx_stm_get_time_upper_word( void )
{
  return EE_STM_CAP.U;
}

/**
  * @brief  Used as bounded busy wait.
  * @param  usec the number of microseconds you want to wait
  */
void EE_tc2Yx_delay( EE_UREG usec );

/****************************************************************
                        Ports Support
 ****************************************************************/
/* Defines to populate Ports registers */
/*  Input - No input pull device connected, tri-state mode
    (same as 0x03U) */
#define EE_TC2YX_INPUT_TRISTATE         0x00U
/*  Input - Pull-Down device connected */
#define EE_TC2YX_INPUT_PULL_DOWN        0x01U
/*  Input - Pull-Up device connected. RESET Default */
#define EE_TC2YX_INPUT_PULL_UP          0x02U

/*  Output - Push-pull General-purpose output */
#define EE_TC2YX_OUTPUT_PUSH_PULL_GP    0x10U
/*  Output - Push-pull Alternate output function 1 */
#define EE_TC2YX_OUTPUT_PUSH_PULL_ALT1  0x11U
/*  Output - Push-pull Alternate output function 2 */
#define EE_TC2YX_OUTPUT_PUSH_PULL_ALT2  0x12U
/*  Output - Push-pull Alternate output function 3 */
#define EE_TC2YX_OUTPUT_PUSH_PULL_ALT3  0x13U
/*  Output - Push-pull Alternate output function 4 */
#define EE_TC2YX_OUTPUT_PUSH_PULL_ALT4  0x14U
/*  Output - Push-pull Alternate output function 5 */
#define EE_TC2YX_OUTPUT_PUSH_PULL_ALT5  0x15U
/*  Output - Push-pull Alternate output function 6 */
#define EE_TC2YX_OUTPUT_PUSH_PULL_ALT6  0x16U
/*  Output - Push-pull Alternate output function 7 */
#define EE_TC2YX_OUTPUT_PUSH_PULL_ALT7  0x17U

/*  Output - Open-drain General-purpose output */
#define EE_TC2YX_OUTPUT_OPEN_DRAIN_GP   0x24U
/*  Output - Open-drain Alternate output function 1 */
#define EE_TC2YX_OUTPUT_OPEN_DRAIN_ALT1 0x25U
/*  Output - Open-drain Alternate output function 2 */
#define EE_TC2YX_OUTPUT_OPEN_DRAIN_ALT2 0x26U
/*  Output - Open-drain Alternate output function 3 */
#define EE_TC2YX_OUTPUT_OPEN_DRAIN_ALT3 0x27U
/*  Output - Open-drain Alternate output function 4 */
#define EE_TC2YX_OUTPUT_OPEN_DRAIN_ALT4 0x28U
/*  Output - Open-drain Alternate output function 5 */
#define EE_TC2YX_OUTPUT_OPEN_DRAIN_ALT5 0x29U
/*  Output - Open-drain Alternate output function 6 */
#define EE_TC2YX_OUTPUT_OPEN_DRAIN_ALT6 0x30U
/*  Output - Open-drain Alternate output function 7 */
#define EE_TC2YX_OUTPUT_OPEN_DRAIN_ALT7 0x31U

#endif /* INCLUDE_EE_TC2YX_MCU_H__ */
