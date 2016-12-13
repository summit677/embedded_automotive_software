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
   *  @file   ee_tc27x_system.c
   *  @brief  This file contains System Timer Initialization code and
   *  eventually all other code needed for ERIKA Start-Up. (The things
   *  that have to be provided to teh kernell for his behaviour)
   *  @author Errico Guidieri
   *  @date 2012
   */

#include <time.h>
#include "ee_internal.h"
#include "cpu/tricore/inc/ee_tc_irq.h"
#include "mcu/infineon_tc27x/inc/ee_tc27x_endinit.h"

/* ENDINIT and SAFETY ENDINIT WATCHDOG Support */
void EE_tc_endint_disable( void ) { 
  EE_tc27x_endinit_set(EE_TC_ENDINIT_DISABLE);
}

void EE_tc_endint_enable( void ) { 
  EE_tc27x_endinit_set(EE_TC_ENDINIT_ENABLE);
}

void EE_tc_safety_endinit_disable( void ) { 
  EE_tc27x_safety_endinit_set(EE_TC_ENDINIT_DISABLE);
}

void EE_tc_safety_endinit_enable( void ) { 
  EE_tc27x_safety_endinit_set(EE_TC_ENDINIT_ENABLE);
}

/* STM Compare Register Selector */
#ifdef EE_MASTER_CPU
/* registers */
#define EE_STM_CMCON    STM0_CMCON
#define EE_STM_ICR      STM0_ICR
#define EE_STM_CMP0     STM0_CMP0
#define EE_STM_CMP1     STM0_CMP1
#define EE_STM_SR0      SRC_STM0SR0
#define EE_STM_SR1      SRC_STM0SR1
#elif (EE_CURRENTCPU == 1)
#define EE_STM_CMCON    STM1_CMCON
#define EE_STM_ICR      STM1_ICR
#define EE_STM_CMP0     STM1_CMP0
#define EE_STM_CMP1     STM1_CMP1
#define EE_STM_SR0      SRC_STM1SR0
#define EE_STM_SR1      SRC_STM1SR1
#elif (EE_CURRENTCPU == 2)
#define EE_STM_CMCON    STM2_CMCON
#define EE_STM_ICR      STM2_ICR
#define EE_STM_CMP0     STM2_CMP0
#define EE_STM_CMP1     STM2_CMP1
#define EE_STM_SR0      SRC_STM2SR0
#define EE_STM_SR1      SRC_STM2SR1
#else
#error Unknown CPU ID
#endif

/* STM_SR Function Static storage declaration (if needed) */
#if (EE_SYSTEM_TIMER_DEVICE == EE_TC_STM_SR0)
#define EE_STM_SR0_STORAGE static
static void EE_tc27x_stm_set_sr0(EE_UINT32 usec, EE_TYPEISR2PRIO intvec);
static void EE_tc27x_stm_set_sr0_next_match(EE_UINT32 usec);
#else
#define EE_STM_SR0_STORAGE
#endif /* EE_SYSTEM_TIMER_DEVICE == EE_TC_STM_SR0 */

#if (EE_SYSTEM_TIMER_DEVICE == EE_TC_STM_SR1)
#define EE_STM_SR1_STORAGE static
static void EE_tc27x_stm_set_sr1(EE_UINT32 usec, EE_TYPEISR2PRIO intvec);
static void EE_tc27x_stm_set_sr1_next_match(EE_UINT32 usec);
#else
#define EE_STM_SR1_STORAGE
#endif /* EE_SYSTEM_TIMER_DEVICE == EE_TC_STM_SR1 */

/* This file is needed only if System Timer is defined with a device */
#if defined(ENABLE_SYSTEM_TIMER) && defined(EE_SYSTEM_TIMER_DEVICE)
/****************************************************************
                    System Timer Support
 ****************************************************************/

/* Map the right device that will be used as system timer
 * Legit Devices for System Timer Defines.
 * N.B:
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

#if (EE_SYSTEM_TIMER_DEVICE != EE_TC_STM_SR0) && \
    (EE_SYSTEM_TIMER_DEVICE != EE_TC_STM_SR1)
#error Unsupported Device as System Timer!
#endif

DeclareIsr(EE_tc_system_timer_handler);

ISR2(EE_tc_system_timer_handler) {
#if (EE_SYSTEM_TIMER_DEVICE == EE_TC_STM_SR0)
  EE_tc27x_stm_set_sr0_next_match(OSTICKDURATION / 1000U);
#elif (EE_SYSTEM_TIMER_DEVICE == EE_TC_STM_SR1)
  EE_tc27x_stm_set_sr1_next_match(OSTICKDURATION / 1000U);
#endif /* EE_SYSTEM_TIMER_DEVICE */

#ifndef EE_AS_OSAPPLICATIONS__
  EE_as_set_execution_context( Kernel_Context );
#endif /* !EE_AS_OSAPPLICATIONS__ */

#if defined(__OO_BCC1__) || defined(__OO_BCC2__) || defined(__OO_ECC1__) || \
    defined(__OO_ECC2__)
  (void)IncrementCounterHardware(EE_SYSTEM_TIMER);
#else /* OO Kernels */
  CounterTick(EE_SYSTEM_TIMER);
#endif /* OO Kernels */
}

/* System Timer Initialization */
void EE_tc27x_initialize_system_timer(void) {
#if (EE_SYSTEM_TIMER_DEVICE == EE_TC_STM_SR0)
  EE_tc27x_stm_set_sr0(OSTICKDURATION / 1000U,
    (EE_TYPEISR2PRIO)EE_ISR2_ID_EE_tc_system_timer_handler);
#elif (EE_SYSTEM_TIMER_DEVICE == EE_TC_STM_SR1)
  EE_tc27x_stm_set_sr1(OSTICKDURATION / 1000U,
    (EE_TYPEISR2PRIO)EE_ISR2_ID_EE_tc_system_timer_handler);
#endif
}

#endif /* ENABLE_SYSTEM_TIMER && EE_SYSTEM_TIMER_DEVICE */

/* If MemMap. support is enabled (i.e. because protection memory): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define API_START_SEC_CODE
#define API_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/****************************************************************
                    SCU Clock Support
 ****************************************************************/
#ifdef EE_MASTER_CPU
void EE_tc27x_configure_clock(EE_UREG fpll) {
  EE_tc27x_configure_clock_internal(fpll);
}
#endif /* EE_MASTER_CPU */

EE_UREG EE_tc27x_get_clock( void ) {
  /* PLL dividers */
  EE_UREG k1, k2, p, n;
  /*  PLL Clock Frequency */
  EE_UREG fpll;

  /* Prescaler mode */
  if ( SCU_PLLSTAT.B.VCOBYST )
  {
    k1 = (EE_UREG)SCU_PLLCON1.B.K1DIV + (EE_UREG)1U;
    fpll = EE_TC27X_BOARD_FOSC / k1;
  } else {
    /* Free running mode */
    if ( SCU_PLLSTAT.B.FINDIS )
    {
      k2 = (EE_UREG)SCU_PLLCON1.B.K2DIV + (EE_UREG)1U;
      fpll = EE_TC27X_BOARD_FOSC / k2;
    } else {
      /* PLL Normal mode */
      k2 = (EE_UREG)SCU_PLLCON1.B.K2DIV + (EE_UREG)1U;
      p = (EE_UREG)SCU_PLLCON0.B.PDIV + (EE_UREG)1U;
      n = (EE_UREG)SCU_PLLCON0.B.NDIV + (EE_UREG)1U;

      /* cpu clock value fclk = (fosc * n)/(P * k2) */
      fpll = n * (EE_TC27X_BOARD_FOSC / (p * k2));
    }
  }
  return fpll;
}

/****************************************************************
                        STM Support
 ****************************************************************/
/* Global variable with freq in Khz value */
static EE_UINT32 EE_tc27x_stm_freq_khz;

static EE_UINT32 EE_tc27x_stm_us_ticks ( EE_UINT32 usec ) {
  EE_UINT32 ticks;
  if ( EE_tc27x_stm_freq_khz >= EE_KILO ) {
    ticks = usec * ( EE_tc27x_stm_freq_khz / EE_KILO );
  } else if ( usec >= EE_KILO ) {
    ticks = ( usec / EE_KILO ) * EE_tc27x_stm_freq_khz;
  } else {
    ticks = ( usec * EE_tc27x_stm_freq_khz ) / EE_KILO;
  }
  return ticks;
}

/* Set inside std time reference  */
void EE_tc27x_stm_set_clockpersec(void)
{
#ifdef __TASKING__
  /* I don't know where is declared */
  extern unsigned long long setfoschz( unsigned long long );
#endif /* __TASKING__ */
  /*  PLL Clock Frequency */
  EE_UREG const fpll = EE_tc27x_get_clock();
  /* Standard Timer Module period */
  EE_UREG const fstm = fpll / SCU_CCUCON1.B.STMDIV;

  /* Set Global variable with freq in Khz value */
  EE_tc27x_stm_freq_khz = fstm / EE_KILO;

#ifdef __TASKING__
  setfoschz ( fstm );
#endif /* __TASKING__ */
}

/*
    STM set_sr function implementation. It will use SFR
    Types. These types are already volatile so I don't need to put
    that qualifier on pointers.
 */

EE_STM_SR0_STORAGE void EE_tc27x_stm_set_sr0(EE_UINT32 usec,
    EE_TYPEISR2PRIO intvec)
{
  EE_UREG compare_value;

  EE_UREG size_of_compare;

  /* I need to make this setup atomic */
  /* USING SUSPEND/RESUME IRQ WOULD BE BETTER (code executable even with isr
     disabled) BUT IS NOT COMPATIBLE WITH NOT-TRUSTED USER-1 PROFILE */
  /* EE_FREG flags = EE_tc_suspendIRQ(); */
    EE_tc_disableIRQ();

  /*  Evaluate next compare value (actual value + increment,
      I don't need to handle wrap around) */
  compare_value = EE_tc27x_stm_us_ticks(usec) +
    EE_tc27x_stm_get_time_lower_word();
  /* Adjust the size of the mask */
  size_of_compare = 31U - EE_tc_clz(compare_value);

  /* Set Compare Value Register */
  EE_STM_CMP0.U = compare_value;

  if(intvec != (EE_TYPEISR2PRIO)EE_ISR_UNMASKED) {
    EE_STM_CMCON.B.MSTART0 = 0U;
    EE_STM_CMCON.B.MSIZE0  = (EE_UINT8)size_of_compare;
    /* Tie STM Service Request 0 with Compare Register 0 */
    EE_STM_ICR.B.CMP0OS = 0U;
    /* Enable STM Service Request Source */
    EE_STM_ICR.B.CMP0EN = 1U;
    /*
     *  STM service Request configuration
     *  [0..7] SRPN = INTERRUPT_NR
     *  [10] Service Request enable
     *  [11..12] Type Of Service (means which CPU will handle it)
     */
    EE_STM_SR0.U =  EE_TC27X_SRN_TYPE_OF_SERVICE(EE_CURRENTCPU) |
      EE_TC27X_SRN_ENABLE | EE_TC27X_SRN_PRIORITY(intvec);
  } else {
    /* Disable STM Service Request Source */
    EE_STM_ICR.B.CMP0EN = 0U;
    EE_STM_SR0.U = 0U;
  }

  /* EE_tc_resumeIRQ(flags); */
  EE_tc_enableIRQ();
}

EE_STM_SR0_STORAGE void EE_tc27x_stm_set_sr0_next_match(EE_UINT32 usec)
{
  EE_UREG compare_value;

  EE_UREG size_of_compare;

  /* I need to make this setup atomic */
  /* USING SUSPEND/RESUME IRQ WOULD BE BETTER (code executable even with isr
     disabled) BUT IS NOT COMPATIBLE WITH NOT-TRUSTED USER-1 PROFILE */
  /* EE_FREG flags = EE_tc_suspendIRQ(); */
    EE_tc_disableIRQ();
  /* Evaluate next compare value (previous one + increment,
     I don't need to handle wrap around) */
  compare_value = EE_tc27x_stm_us_ticks(usec) +
    EE_tc27x_stm_get_time_lower_word();
  /* Adjust the size of the mask */
  size_of_compare = 31U - EE_tc_clz(compare_value);

  /* Set Compare Value Register */
  EE_STM_CMP0.U = compare_value;
  /* Set Compare Size Value Register */
  EE_STM_CMCON.B.MSIZE0 = (EE_UINT8)size_of_compare;

  /* EE_tc_resumeIRQ(flags); */
  EE_tc_enableIRQ();
}

EE_STM_SR1_STORAGE void EE_tc27x_stm_set_sr1(EE_UINT32 usec,
  EE_TYPEISR2PRIO intvec)
{
  EE_UREG compare_value;

  EE_UREG size_of_compare;

  /* I need to make this setup atomic */
  /* USING SUSPEND/RESUME IRQ WOULD BE BETTER (code executable even with isr
     disabled) BUT IS NOT COMPATIBLE WITH NOT-TRUSTED USER-1 PROFILE */
  /* EE_FREG flags = EE_tc_suspendIRQ(); */
  EE_tc_disableIRQ();
  /*  Evaluate next compare value (actual value + increment,
      I don't need to handle wrap around) */
  compare_value = EE_tc27x_stm_us_ticks(usec) +
    EE_tc27x_stm_get_time_lower_word();
  /* Adjust the size of the mask */
  size_of_compare = 31U - EE_tc_clz(compare_value);

  /* Set Compare Value Register */
  EE_STM_CMP1.U = compare_value;

  if(intvec != (EE_TYPEISR2PRIO)EE_ISR_UNMASKED) {
    EE_STM_CMCON.B.MSTART1 = 0U;
    EE_STM_CMCON.B.MSIZE1  = (EE_UINT8)size_of_compare;
    /* Tie STM Service Request 1 with Compare Register 1 */
    EE_STM_ICR.B.CMP1OS = 1U;
    /* Enable STM Service Request Source */
    EE_STM_ICR.B.CMP1EN = 1U;
    /*
     *  STM service Request configuration
     *  [0..7] SRPN = INTERRUPT_NR
     *  [10] Service Request enable
     *  [11..12] Type Of Service (means which CPU will handle it)
     */
    EE_STM_SR1.U = EE_TC27X_SRN_TYPE_OF_SERVICE(EE_CURRENTCPU) |
      EE_TC27X_SRN_ENABLE | EE_TC27X_SRN_PRIORITY(intvec);
  } else {
    /* Disable STM Service Request Source */
    EE_STM_ICR.B.CMP1EN = 0U;
    EE_STM_SR1.U = 0U;
  }
  /* EE_tc_resumeIRQ(flags); */
  EE_tc_enableIRQ();
}

EE_STM_SR1_STORAGE void EE_tc27x_stm_set_sr1_next_match(EE_UINT32 usec)
{
  EE_UREG compare_value;

  EE_UREG size_of_compare;

  /* I need to make this setup atomic */
  /* USING SUSPEND/RESUME IRQ WOULD BE BETTER (code executable even with isr
     disabled) BUT IS NOT COMPATIBLE WITH NOT-TRUSTED USER-1 PROFILE */
  /* EE_FREG flags = EE_tc_suspendIRQ(); */
  EE_tc_disableIRQ();
  /* Evaluate next compare value (previous one + increment,
     I don't need to handle wrap around) */
  compare_value = EE_tc27x_stm_us_ticks(usec) +
    EE_tc27x_stm_get_time_lower_word();
  /* Adjust the size of the mask */
  size_of_compare = 31U - EE_tc_clz(compare_value);

  /* Set Compare Value Register */
  EE_STM_CMP1.U = compare_value;

  /* Set Compare Size Value Register */
  EE_STM_CMCON.B.MSIZE1 = (EE_UINT8)size_of_compare;
  /* EE_tc_resumeIRQ(flags); */
  EE_tc_enableIRQ();
}

void EE_tc27x_delay( EE_UREG usec )
{
  /* Read Start Point */
  EE_UREG const start = EE_tc27x_stm_get_time_lower_word();
  /* Evaluate End Point */
  EE_UREG const ticks = EE_tc27x_stm_us_ticks(usec);

  while ( ticks > EE_tc27x_stm_get_time_lower_word() - start ) {
    ; /* Wait */
  }
}

