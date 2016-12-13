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

 /** @file   ee_tc2Yx_internal.h
  *  @brief  MCU-dependent internal part of HAL
  *  @author Errico Guidieri
  *  @date 2012
  */
#ifndef INCLUDE_EE_TC2YX_INTERNAL_H__
#define INCLUDE_EE_TC2YX_INTERNAL_H__


#if defined (EE_TRIBOARD_TC2X5)
#include "board/infineon_TriBoard_TC2X5/inc/ee_tc2x5_board.h"
#define EE_SCU_CCUCON0 (EE_TC2YX_CCUCON0_BAUD1DIV(1U) |\
  EE_TC2YX_CCUCON0_SRI(1U) | EE_TC2YX_CCUCON0_SPB(2U) |\
  EE_TC2YX_CCUCON0_FSI2(2U) | EE_TC2YX_CCUCON0_FSI(2U) |\
  EE_TC2YX_CCUCON0_CLKSEL(1U))
#elif defined (EE_APPKIT_TC2X5)
#include "board/infineon_Applikation_Kit_TC2X5/inc/ee_board.h"
#define EE_SCU_CCUCON0 (EE_TC2YX_CCUCON0_BAUD1DIV(1U) |\
  EE_TC2YX_CCUCON0_SRI(1U) | EE_TC2YX_CCUCON0_SPB(2U) |\
  EE_TC2YX_CCUCON0_FSI2(2U) | EE_TC2YX_CCUCON0_FSI(2U) |\
  EE_TC2YX_CCUCON0_CLKSEL(1U) | EE_TC2YX_CCUCON0_BAUD2DIV(1U))
#elif defined (EE_APPKIT_TC2X7)
#include "board/infineon_Applikation_Kit_TC2X7/inc/ee_board.h"
#define EE_SCU_CCUCON0 (EE_TC2YX_CCUCON0_BAUD1DIV(1U) |\
  EE_TC2YX_CCUCON0_SRI(1U) | EE_TC2YX_CCUCON0_SPB(2U) |\
  EE_TC2YX_CCUCON0_FSI2(2U) | EE_TC2YX_CCUCON0_FSI(2U) |\
  EE_TC2YX_CCUCON0_CLKSEL(1U) | EE_TC2YX_CCUCON0_BAUD2DIV(1U))  
#else
/* Some tests do not define a board, but EE_TC2YX_BOARD_FOSC macro is needed
   I will use the define from EE_TRIBOARD_TC2X5 */
/* Board Oscillator Frequency default (20MHz) - TriBoard quartz */
#include "mcu/infineon_common_tc2Yx/inc/ee_tc2Yx_mcu.h"
#define EE_TC2YX_BOARD_FOSC 20000000U
#define EE_SCU_CCUCON0 (EE_TC2YX_CCUCON0_BAUD1DIV(1U) |\
  EE_TC2YX_CCUCON0_SRI(1U) | EE_TC2YX_CCUCON0_SPB(2U) |\
  EE_TC2YX_CCUCON0_FSI2(2U) | EE_TC2YX_CCUCON0_FSI(2U) |\
  EE_TC2YX_CCUCON0_CLKSEL(1U))
#endif

/** Interrupt table */
extern void EE_tc_interrupt_table ( void );
/** Trap table */
extern void EE_tc_trap_table ( void );

#ifdef  EE_MASTER_CPU
/*****************************************************************************
                          CCU Clock Control Support
 ****************************************************************************/

__INLINE__ void __ALWAYS_INLINE__ EE_tc2Yx_configure_clock_ctrl ( void )
{
  /* Configure CCUCON0 */
  SCU_CCUCON0.U = EE_SCU_CCUCON0;

  /* Configure CCUCON1 and Update all CCU */
  SCU_CCUCON1.U =  EE_TC2YX_CCUCON1_STMDIV(1U) |
    EE_TC2YX_CCUCON1_GTMDIV(1U) | EE_TC2YX_CCUCON1_INSEL(1U) |
    EE_TC2YX_CCUCONX_UP;
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc2Yx_configure_osc_ctrl ( void )
{
  /*
   * Default System Oscillator Configuration
   * MODE:    0   -> External Crystal
   */
  SCU_OSCCON.U= EE_TC2YX_SCU_OSCCON_OSCRES | EE_TC2YX_SCU_OSCCON_GAINSEL |
      EE_TC2YX_SCU_OSCCON_MODE(0U) | EE_TC2YX_SCU_OSCCON_OSCVAL(7U);

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
#define EE_TC2YX_PLL_NDIV           60U
#define EE_TC2YX_PLL_PDIV           2U

/* This function accept fpll in HZ */
 __INLINE__ void __ALWAYS_INLINE__ EE_tc2Yx_configure_clock_internal(
    EE_UREG fpll)
{
  /* k2 divider value */
  EE_UINT32 k2;
  /* Adjust fcpu if needed */
  if(fpll > EE_TC2YX_CLOCK_MAX) {
      fpll = EE_TC2YX_CLOCK_MAX;
  } else if (fpll < EE_TC2YX_CLOCK_MIN) {
      fpll = EE_TC2YX_CLOCK_MIN;
  }
  else {
      /* Empty else to comply with MISRA 14.10 */
  }

  /* Because fpll =  (fosc * n) / (k2 * p(=2)). k2 is ... */
  k2 = (((EE_TC2YX_BOARD_FOSC / EE_MEGA) * EE_TC2YX_PLL_NDIV) /
      ((fpll / EE_MEGA) * EE_TC2YX_PLL_PDIV)) - 1U;

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
  SCU_PLLCON0.B.PDIV= EE_TC2YX_PLL_PDIV - 1U;
  SCU_PLLCON0.B.NDIV= EE_TC2YX_PLL_NDIV - 1U;

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
#define EE_tc2Yx_configure_clock_ctrl() ((void)0)
#define EE_tc2Yx_configure_osc_ctrl()   ((void)0)
#define EE_tc2Yx_configure_clock()      ((void)0)

#endif /* EE_MASTER_CPU */

#if defined(__OO_ORTI_STACK__) || defined(EE_STACK_MONITORING__)
/******************************************************************************
                            ORTI Stack Filling
 ******************************************************************************/
/**
  * @brief Fill User Stack and TASKs Private Stacks with known pattern
  *   for monitoring
  */
__INLINE__ void __ALWAYS_INLINE__ EE_tc2Yx_fill_stacks( void )
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
#endif /* (__GNUC__ && !EE_EXECUTE_FROM_RAM) ||  __DCC__*/

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
  while (1)
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
#elif ((!defined(__GNUC__)) && (!defined(__DCC__)) && (!defined(__TASKING__))) && (!defined(EE_EXECUTE_FROM_RAM))
#error Fix Stack Filling code in Other compiler Than GNUC and DCC !
#endif /* (!__GNUC__ && !EE_EXECUTE_FROM_RAM) ||  __DCC__ */
}
#else  /* __OO_ORTI_STACK__ || EE_STACK_MONITORING__ */
/* If ORTI STACKs is not enabled stack filling is not active */
#define EE_tc2Yx_fill_stacks() ((void)0U)
#endif /* __OO_ORTI_STACK__ || EE_STACK_MONITORING__ */

/******************************************************************************
                              System startup
 *****************************************************************************/
/* If system is defined I have to initialize it*/
#if defined(ENABLE_SYSTEM_TIMER) && defined(EE_SYSTEM_TIMER_DEVICE)

void EE_tc2Yx_initialize_system_timer(void);
#else /* ENABLE_SYSTEM_TIMER && EE_SYSTEM_TIMER_DEVICE */
#define EE_tc2Yx_initialize_system_timer() ((void) 0)
#endif /* ENABLE_SYSTEM_TIMER && EE_SYSTEM_TIMER_DEVICE */

__INLINE__ int __ALWAYS_INLINE__ EE_cpu_startos( void )
{
#if (defined(__EE_MEMORY_PROTECTION__) || defined(EE_TIMING_PROTECTION__)) &&\
  (defined(EE_USE_CUSTOM_STARTUP_CODE) || defined(EE_MM_OPT))
  /* In case of "User Boot" and "sytem protections" play SAFE and force
     the set of the right Trap Table. */
  EE_tc_endint_disable();
  EE_tc_set_csfr(EE_CPU_REG_BTV, (EE_UINT32)&EE_tc_trap_table);
  EE_tc_endint_enable();
#endif /* (__EE_MEMORY_PROTECTION__ || EE_TIMING_PROTECTION__) &&
   (EE_USE_CUSTOM_STARTUP_CODE || EE_MM_OPT) */
#ifdef EE_MASTER_CPU
/* If a CPU CLOCK frequency is defined configure the SCU registers */
#if defined(EE_CPU_CLOCK) && (!defined(EE_MM_OPT))
  /* Disable SAFETY ENDINIT Protection */
  EE_tc_safety_endinit_disable();

  /********* Configure CCU Clock Control. *********/
  EE_tc2Yx_configure_clock_ctrl();
  /********* Configure Oscillator Control. *********/
  EE_tc2Yx_configure_osc_ctrl();
  /***************** Configure PLL *****************/
  EE_tc2Yx_configure_clock( EE_CPU_CLOCK );

  /* Re-enable SAFETY ENDINIT Protection */
  EE_tc_safety_endinit_enable();
#endif /* EE_CPU_CLOCK && !EE_MM_OPT */

  /* Initialize intercore IRQs (in multicore environment) */
  EE_tc2Yx_setup_inter_irqs();
#endif /* EE_MASTER_CPU */

  /* Fill Stacks With Known Path for monitoring usage, if ORTI is enabled */
  EE_tc2Yx_fill_stacks();

  /* Configure hardware for memory protection or timing protection:
     if enabled */
  EE_tc_enable_protections();

  /* Initialize stdlib time reference (or internal variable) with STM
      frequency. */
  EE_tc2Yx_stm_set_clockpersec();
  EE_tc2Yx_initialize_system_timer();

  return 0;
}

#endif /* INCLUDE_EE_TC2YX_INTERNAL_H__ */
