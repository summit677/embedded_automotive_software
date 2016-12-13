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

 /** @file   ee_tc27x_internal.h
  *  @brief  MCU-dependent internal part of HAL
  *  @author Errico Guidieri
  *  @date 2012
  */
#ifndef INCLUDE_EE_TC27X_INTERNAL_H__
#define INCLUDE_EE_TC27X_INTERNAL_H__

/* Internals must view MCU API */
#include "mcu/infineon_tc27x/inc/ee_tc27x_mcu.h"
/* Board inclusion for Oscillator Frequency Define EE_TC27X_BOARD_FOSC */
/* TODO: Protect this with some kind of selector */
#include "board/infineon_TriBoard_TC2X5/inc/ee_tc2x5_board.h"

#ifdef  EE_MASTER_CPU
/*****************************************************************************
                          CCU Clock Control Support
 ****************************************************************************/

__INLINE__ void __ALWAYS_INLINE__ EE_tc27x_configure_clock_ctrl ( void )
{
  /* Configure CCUCON0 */
  SCU_CCUCON0.U = EE_TC27X_CCUCON0_BAUD1DIV(1U) |
    EE_TC27X_CCUCON0_SRI(1U) | EE_TC27X_CCUCON0_SPB(2U) |
    EE_TC27X_CCUCON0_FSI2(2U) | EE_TC27X_CCUCON0_FSI(2U) |
    EE_TC27X_CCUCON0_CLKSEL(1U);

  /* Configure CCUCON1 and Update all CCU */
  SCU_CCUCON1.U =  EE_TC27X_CCUCON1_STMDIV(1U) |
    EE_TC27X_CCUCON1_GTMDIV(1U) | EE_TC27X_CCUCON1_INSEL(1U) |
    EE_TC27X_CCUCONX_UP;
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc27x_configure_osc_ctrl ( void )
{
  /*
   * Default System Oscillator Configuration
   * MODE:    0   -> External Crystal
   */
  SCU_OSCCON.U= EE_TC27X_SCU_OSCCON_OSCRES | EE_TC27X_SCU_OSCCON_GAINSEL |
      EE_TC27X_SCU_OSCCON_MODE(0U) | EE_TC27X_SCU_OSCCON_OSCVAL(7U);

  while ( SCU_OSCCON.B.PLLLV == 0U ) {
    ; /* Oscillator not too low */
  }

  while ( SCU_OSCCON.B.PLLHV == 0U ) {
    ; /* Oscillator not too high */
  }
}

/******************************************************************************
                                SCU PLL Support
 *****************************************************************************/

/*  I will use n=80 and p=2. Because I can get al the
    needed values */
#define EE_TC27X_PLL_NDIV           80U
#define EE_TC27X_PLL_PDIV           2U

/* This function accept fpll in HZ */
 __INLINE__ void __ALWAYS_INLINE__ EE_tc27x_configure_clock_internal(
    EE_UREG fpll)
{
  /* k2 divider value */
  EE_UINT32 k2;
  /* Adjust fcpu if needed */
  if(fpll > EE_TC27X_CLOCK_MAX) {
      fpll = EE_TC27X_CLOCK_MAX;
  } else if (fpll < EE_TC27X_CLOCK_MIN) {
      fpll = EE_TC27X_CLOCK_MIN;
  }
  else {
      /* Empty else to comply with MISRA 14.10 */
  }

  /* Because fpll =  (fosc * n) / (k2 * p(=2)). k2 is ... */
  k2 = (((EE_TC27X_BOARD_FOSC / EE_MEGA) * EE_TC27X_PLL_NDIV) /
      ((fpll / EE_MEGA) * EE_TC27X_PLL_PDIV)) - 1U;

  /* Divide by K2DIV + 1 */
  SCU_PLLCON1.B.K2DIV = (EE_UINT8)k2;

  while( SCU_PLLSTAT.B.K2RDY == 0U ) {
    ; /* Wait until K2-Divider is ready to operate */
  }

  /* K1 divider default value */

  /* Enabled the VCO Bypass Mode */
  SCU_PLLCON0.B.VCOBYP = 1U;

  while( SCU_PLLSTAT.B.VCOBYST == 0U ) {
    ; /* Wait until prescaler mode is entered */
  }

  /*  I will use n=80 and p=2. Because I can get al the
      needed values */
  SCU_PLLCON0.B.PDIV= EE_TC27X_PLL_PDIV - 1U;
  SCU_PLLCON0.B.NDIV= EE_TC27X_PLL_NDIV - 1U;

  /* power down VCO Normal Behaviour */
  SCU_PLLCON0.B.VCOPWD = 0U;

  /***** Configure PLL normal mode. *****/

  /* Automatic oscillator disconnect disabled */
  SCU_PLLCON0.B.OSCDISCDIS = 1U;
  /* Connect VCO to the oscillator */
  SCU_PLLCON0.B.CLRFINDIS = 1U;

  while ( SCU_PLLSTAT.B.FINDIS == 1U ) {
    ; /* Wait until oscillator is connected to the VCO */
  }

  /* Restart VCO lock detection */
  SCU_PLLCON0.B.RESLD = 1U;

  while ( SCU_PLLSTAT.B.VCOLOCK == 0U ) {
    ; /* Wait until the VCO becomes locked */
  }

  /* Disable the VCO Bypass Mode */
  SCU_PLLCON0.B.VCOBYP = 0U;

  while ( SCU_PLLSTAT.B.VCOBYST == 1U ) {
    ; /* Wait until normal mode is entered */
  }

  /* Automatic oscillator disconnect enabled */
  SCU_PLLCON0.B.OSCDISCDIS = 0U;
}

#else
#define EE_tc27x_configure_clock_ctrl() ((void)0)
#define EE_tc27x_configure_osc_ctrl()   ((void)0)
#define EE_tc27x_configure_clock()      ((void)0)

#endif /* EE_MASTER_CPU */

#ifdef __OO_ORTI_STACK__
/******************************************************************************
                            ORTI Stack Filling
 ******************************************************************************/
/**
  * @brief Fill User Stack and TASKs Private Stacks with known pattern
  *   for monitoring
  */
__INLINE__ void __ALWAYS_INLINE__ EE_tc27x_fill_stacks( void )
{
  /* User Stack end */
  extern EE_UINT32 EE_E_USTACK[];
  /* User Stack Base. */
  extern EE_UINT32 EE_B_USTACK[];

#if (defined(__GNUC__) && (!defined(EE_EXECUTE_FROM_RAM))) || defined(__DCC__)
  /* ERIKA stacks table entry */
  extern EE_UINT32 ee_stacks_table[];
  /* Pointer used to traverse stack table */
  EE_UINT32 *stack_table_ptr;
  /* Actual Stack length (first in bytes then, in words) */
  EE_UINT32 stack_length;
#endif /* (__GNUC__ && !EE_EXECUTE_FROM_RAM) ||  __DCC__ */

  /* Pointer used to traverse stacks */
  register EE_UINT32 * stack_fill_ptr;

  /* Initialize the main stack with fill pattern */
  for(stack_fill_ptr = EE_B_USTACK;
      stack_fill_ptr < EE_E_USTACK; ++stack_fill_ptr)
  {
    *stack_fill_ptr = EE_TC_STACK_FILL_PATTERN;
  }

#if (defined(__GNUC__) && (!defined(EE_EXECUTE_FROM_RAM))) || defined(__DCC__)
  /* Stack table */
  stack_table_ptr = ee_stacks_table;
  /* Traverse it */
  while (stack_table_ptr)
  {
    /* Get a stack section base address */
    stack_fill_ptr = (EE_UINT32 *)*stack_table_ptr;
    /* next field of the table */
    stack_table_ptr++;
    /* Get the stack section length */
    stack_length = *stack_table_ptr;
    
    /* we have finished when next length == -1 */
    if ( stack_length == 0xFFFFFFFFU ) {
      break;
    }

    /* Normalize the length with the variable's size used to fill the stack */
    stack_length = stack_length / (EE_UINT32)sizeof(*stack_fill_ptr);

    while ( stack_length ) {
      *stack_fill_ptr = EE_TC_STACK_FILL_PATTERN;
      stack_length--;
      stack_fill_ptr++;
    }
    /* Prepare to access to next entry on stacks table */
    stack_table_ptr++;
  }
#elif ((!defined(__GNUC__)) && (!defined(__DCC__))) && (!defined(EE_EXECUTE_FROM_RAM))
#error Fix Stack Filling code in Other compiler Than GNUC and DCC !
#endif /* (!__GNUC__ && !EE_EXECUTE_FROM_RAM) ||  __DCC__ */
}
#else
/* If ORTI STACKs is not enabled stack filling is not active */
#define EE_tc27x_fill_stacks() ((void)0U)
#endif /* __OO_ORTI_STACK__ */

/******************************************************************************
              Memory Protection & Timing Protection Configuration
 ******************************************************************************/
#if defined(__EE_MEMORY_PROTECTION__) || defined(EE_TIMING_PROTECTION__)
/* N.B EE_TIMING_PROTECTION__ is just a placeholder not implemented yet */
/* Compiler check has been already done inside EE_tc_cpu.h */

#ifdef __EE_MEMORY_PROTECTION__

/*
 * Utility function that group the common configuration code between the two
 * memory implemented protection configurations(i.e. OS-Application directly
 * mapped on Protection sets or dinamically associated).
 */
__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_tc27x_conf_common_memory_prot( void )
{
  /* Ram Begin Address */
  extern EE_UINT32 ee_skernel_ram[];
  /* Kernel Ram End Address */
  extern EE_UINT32 ee_ekernel_ram[];

  /* Kernel Data Structures range Begin Address */
  extern EE_UINT32 ee_sbss_kernel[];
  /* Kernel Data Structures End Address
     (These two symbols are the boundaries of kernel data structures sections:
      all the OSApplications will have read permission for this segment */
  extern EE_UINT32 ee_edata_kernel[];

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

  /* Code protection is not yet implemented so all the code will be
     made executable */
  EE_tc_set_csfr(EE_CPU_REG_CPR0_L, (EE_UREG)ee_sall_code);
  EE_tc_set_csfr(EE_CPU_REG_CPR0_U, (EE_UREG)ee_eall_code);

  /* Range Registers 0 are kernel Range Registers for RAM  */
  EE_tc_set_csfr(EE_CPU_REG_DPR0_L, (EE_UREG)ee_skernel_ram);
  EE_tc_set_csfr(EE_CPU_REG_DPR0_U, (EE_UREG)ee_ekernel_ram);

  /* READ ONLY Parameters are accessible by all the code */
  EE_tc_set_csfr(EE_CPU_REG_DPR1_L, (EE_UREG)ee_sapi_const);
  EE_tc_set_csfr(EE_CPU_REG_DPR1_U, (EE_UREG)ee_eapi_const);

  /* Range Registers 2 are ERIKA API Range Registers for RAM  */
  EE_tc_set_csfr(EE_CPU_REG_DPR2_L, (EE_UREG)ee_sapi_ram);
  EE_tc_set_csfr(EE_CPU_REG_DPR2_U, (EE_UREG)ee_eapi_ram);

  /* Range Registers 6 are ERIKA Data Structures Range Registers:
     All OSApplication will have reading permission for this segment */
  EE_tc_set_csfr(EE_CPU_REG_DPR6_L, (EE_UREG)ee_sbss_kernel);
  EE_tc_set_csfr(EE_CPU_REG_DPR6_U, (EE_UREG)ee_edata_kernel);

  /* !!! XXX WORKAROUND: TriCore aurix specification states that in supervisor
     mode you can access Peripheral Space (Segment 0xF) even without a
     configured data protection range for that. Moreover states that
     setting bit 17 in SYSCON  the same behaviour should be granted even for
     User-1 mode.
     Well said that for TC275TE MCU SIMPLY THIS IS NOT THE TRUTH !!! 

     Range Registers 15 will be used for peripheral space WORKAROUND:
     so EE_BIT(15) will be added in oring in all data protection sets active
   */
  EE_tc_set_csfr(EE_CPU_REG_DPR15_L, 0xF0000000U);
  /* Range Addresses have to be 8 byte aligned so I'll use 0xFFFFFFF8U
     instead of 0xFFFFFFFFU */
  EE_tc_set_csfr(EE_CPU_REG_DPR15_U, 0xFFFFFFF8U);

  /* PSW.PRS is automatically set to 0 by hardware after a TRAP or an IRQ, so
     set 0 of DPR is implicitly reserved to the kernel */

  /* Set 0 of Data Protection Registers is reserved for kernel ->
     enable ranges 0-1 registers (RAM and FLASH) for Read Access */
  EE_tc_set_csfr(EE_CPU_REG_DPRE_0, EE_BIT(15U) |  EE_BIT(2U) | EE_BIT(1U) |
    EE_BIT(0U));

  /* Set 0 of Data Protection Registers are reserved for kernel ->
     enable ranges 0 registers (RAM) for Write Access */
  EE_tc_set_csfr(EE_CPU_REG_DPWE_0, EE_BIT(15U) | EE_BIT(2U) | EE_BIT(0U));

  /* XXX: Temporarily all the code is executable */
  EE_tc_set_csfr(EE_CPU_REG_CPXE_0, EE_BIT(0U));

  /* Start to configure the syscon register value to be set */
  return EE_TC_ENABLE_MEMORY_PROTECTION |
    EE_TC_USER1_PERIPHERAL_ACCESS_AS_SUPERVISOR;
}
#else /* __EE_MEMORY_PROTECTION__ */
#define EE_tc27x_conf_common_memory_prot() 0U;
#endif /* __EE_MEMORY_PROTECTION__ */

/*
 * Set the given bitmask on SYSCON register
 */
__INLINE__ void __ALWAYS_INLINE__ EE_tc27x_set_syscon( EE_UREG syscon_value )
{
  /* Disable SAFETY ENDINIT Protection */
  EE_tc_safety_endinit_disable();

  /* Disable Safety Compatibility with TC 1.3 (I want use all the AURIX
     features). This maybe should not be here, but it doesn't harm */
  EE_tc_set_csfr(EE_CPU_REG_COMPAT, ~EE_BIT(4U));

  /* Assure that all the changes to CFSR are taken in count after this point */
  EE_tc_isync();

  /* Enable protections */
  EE_tc_set_syscon(syscon_value);

  /* Re-enable SAFETY ENDINIT Protection */
  EE_tc_safety_endinit_enable();
}

#ifdef EE_OS_APP_MAPPED_ON_PROTECTION_SETS
/* In this configuration each Os-Application is directly mapped on a protection
 * set register, so each  OS-Application context switch consist of setting the
 * right PSW.PSR and PSW.IO values, in other words a bitmask operation plus a
 * CSFR write.
 * Configuration is made once here during the StartOS procedure, so Kernel data
 * structures for OS-Application need to be seen by this here.
 *
 * N.B. Implementation has been moved in a "dedicated" file, because this
 *      function need to access at some AS Kernel data structures and types
 *      and it wasn't easy do that in this header file.
 */
void EE_tc27x_enable_protections( void );

#else /* EE_OS_APP_MAPPED_ON_PROTECTION_SETS */
/*
 *  When OS-Application are not directly mapped on protection set,
 *  OS-Application boundaries are dynamically set in data protection range
 *  set 2 at each OS-Application Context switch.
 */
__INLINE__ void __ALWAYS_INLINE__ EE_tc27x_enable_protections( void )
{
  /* Configure Memory Protection common part and obtain the starting mask for
     syscon register */
  register EE_UREG syscon_value = EE_tc27x_conf_common_memory_prot();

  /* Set 1 of Data Protection Registers are reserved for OS-Applications ->
     enable ranges 2-1 registers (RAM and FLASH) for Read Access.
     N.B IN ANY CASE READS ALL OVER THE MEMORY ARE ALLOWED SO
     "Kernel RAM range" (EE_BIT(0U)) IS ADDED TO DPRE_1 bitmask */
  EE_tc_set_csfr(EE_CPU_REG_DPRE_1, EE_BIT(15U) | EE_BIT(6U) | EE_BIT(3U) |
    EE_BIT(2U) | EE_BIT(1U) | EE_BIT(0U));

  /* Set 1 of Data Protection Registers are reserverd for OS-Applications ->
     enable ranges 2 registers (RAM) for Write Access */
  EE_tc_set_csfr(EE_CPU_REG_DPWE_1, EE_BIT(15U) | EE_BIT(3U) | EE_BIT(2U));

  /* XXX: Temporarily all the code is executable */
  EE_tc_set_csfr(EE_CPU_REG_CPXE_1, EE_BIT(0U));

  /* XXX TODO: !!! Add Code Protection Configuration !!! */

#ifdef EE_TIMING_PROTECTION__
  syscon_value |= EE_TC_ENABLE_TEMPORAL_PROTECTION;
  /* XXX TODO: Add here TC Temporal Protection Code */
#endif /* EE_TIMING_PROTECTION__ */

  /* Set the given bitmask on SYSCON register */
  EE_tc27x_set_syscon(syscon_value);
}
#endif /* EE_OS_APP_MAPPED_ON_PROTECTION_SETS */
#else /* __EE_MEMORY_PROTECTION__ || EE_TIMING_PROTECTION__ */

/* Memory protection is not enabled */
#define EE_tc27x_enable_protections() ((void)0U)
#endif /* __EE_MEMORY_PROTECTION__ || EE_TIMING_PROTECTION__ */


/******************************************************************************
                              System startup
 *****************************************************************************/
/* If system is defined I have to initialize it*/
#if defined(ENABLE_SYSTEM_TIMER) && defined(EE_SYSTEM_TIMER_DEVICE)

void EE_tc27x_initialize_system_timer(void);
#else /* ENABLE_SYSTEM_TIMER && EE_SYSTEM_TIMER_DEVICE */
#define EE_tc27x_initialize_system_timer() ((void) 0)
#endif /* ENABLE_SYSTEM_TIMER && EE_SYSTEM_TIMER_DEVICE */

__INLINE__ int __ALWAYS_INLINE__ EE_cpu_startos( void )
{
#ifdef EE_MASTER_CPU
/* If a CPU CLOCK frequency is defined configure the SCU registers */
#ifdef EE_CPU_CLOCK
  /* Disable SAFETY ENDINIT Protection */
  EE_tc_safety_endinit_disable();

  /********* Configure CCU Clock Control. *********/
  EE_tc27x_configure_clock_ctrl();
  /********* Configure Oscillator Control. *********/
  EE_tc27x_configure_osc_ctrl();
  /***************** Configure PLL *****************/
  EE_tc27x_configure_clock( EE_CPU_CLOCK );

  /* Re-enable SAFETY ENDINIT Protection */
  EE_tc_safety_endinit_enable();
#endif /* EE_CPU_CLOCK */

  /* Initialize intercore IRQs (in multicore environment) */
  EE_tc27x_setup_inter_irqs();
#endif /* EE_MASTER_CPU */

  /* Fill Stacks With Known Path for monitoring usage, if ORTI is enabled */
  EE_tc27x_fill_stacks();

  /* Configure hardware for memory protection or timing protection:
     if enabled */
  EE_tc27x_enable_protections();

  /* Initialize stdlib time reference (or internal variable) with STM
      frequency. */
  EE_tc27x_stm_set_clockpersec();
  EE_tc27x_initialize_system_timer();

  return 0;
}

#endif /* INCLUDE_EE_TC27X_INTERNAL_H__ */
