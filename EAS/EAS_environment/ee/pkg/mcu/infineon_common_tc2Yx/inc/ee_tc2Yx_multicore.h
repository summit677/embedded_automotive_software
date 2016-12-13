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

 /** @file      ee_tc_multicore
  *  @brief     Multicore dependent part of HAL
  *  @author    Errico Guidieri
  *  @date      2012
  */

/*******************************************************************************
                    Multicore and multiprocessor support
 ******************************************************************************/
#ifndef INCLUDE_EE_TC2YX_MULTICORE_H__
#define INCLUDE_EE_TC2YX_MULTICORE_H__

#ifdef __MSRP__
/*******************************************************************************
                          Multicore System startup
 ******************************************************************************/
#include "cpu/tricore/inc/ee_tc_irq.h"

/** @brief The following macros MAY be redefined by a user that want use
      custom startup code (setting the EEOPT=EE_USE_CUSTOM_STARTUP_CODE in OIL
      file) to tell the kernel Start(NonAutosar)Core which is the core start
      address */
#ifndef EE_CPU1_START_ADDR
#define EE_CPU1_START_ADDR  EE_tc2Yx_cpu1_start
#endif  /* EE_CPU1_START_ADDR */

#if (EE_NUMBER_OF_CORES > 2)
#ifndef EE_CPU2_START_ADDR
#define EE_CPU2_START_ADDR  EE_tc2Yx_cpu2_start
#endif  /* EE_CPU2_START_ADDR */
#endif /* EE_NUMBER_OF_CORES > 2 */

/**
 *  @brief  Setup the intercore IRQs used by ERIKA for multicore support.
 *          Software interrupt sourcers (registers) SRC_GPSR00 SRC_GPSR10
 *          SRC_GPSR20, will be used for this communication mechanism,
 *          respectively, for core 0,1 and 2.
 */
__INLINE__ void __ALWAYS_INLINE__ EE_tc2Yx_setup_inter_irqs( void )
{
  /*  Assign TOS (Type of Service Control:Read CPU) enable and assign
      entry(priority) in INTTAB. (Priority is statically chosen as the smallest
      possible, that is EE_ISR_PRI_1. RT-Druid MUST ensure that no other ISR
      will be generated with this priority) */
  SRC_GPSR00.U = EE_TC2YX_SRN_TYPE_OF_SERVICE(0U) | EE_TC2YX_SRN_ENABLE |
    EE_TC2YX_SRN_PRIORITY(EE_ISR_PRI_1);
  SRC_GPSR10.U = EE_TC2YX_SRN_TYPE_OF_SERVICE(1U) | EE_TC2YX_SRN_ENABLE |
    EE_TC2YX_SRN_PRIORITY(EE_ISR_PRI_1);
#if (EE_NUMBER_OF_CORES > 2)
  SRC_GPSR20.U = EE_TC2YX_SRN_TYPE_OF_SERVICE(2U) | EE_TC2YX_SRN_ENABLE |
    EE_TC2YX_SRN_PRIORITY(EE_ISR_PRI_1);
#endif /* EE_NUMBER_OF_CORES > 2 */
}

/*******************************************************************************
                            Multicore CPU Signal
 ******************************************************************************/
/* Signal the core "cpu" (0/1/2) by sending an IIRQ. This API Must handle more
   than a signal before an acknowledge without interrupting the serving IISR.
   This is implicitly assumed by the code write inside EE_rn_send and EE_as_rpc
   where this function is used (redefineted as: EE_hal_IRQ_interprocessor by a
   macro). */
__INLINE__ void __ALWAYS_INLINE__ EE_tc2Yx_signal_cpu( EE_TYPECOREID cpu )
{
  if ( cpu == 0U ) {
    SRC_GPSR00.U |= EE_TC2YX_SRN_SET_REQUEST;
  } else if ( cpu == 1U ) {
    SRC_GPSR10.U |= EE_TC2YX_SRN_SET_REQUEST;
  } else
#if (EE_NUMBER_OF_CORES > 2)
  if ( cpu == 2U ) {
    SRC_GPSR20.U |= EE_TC2YX_SRN_SET_REQUEST;
  } else
#endif /* EE_NUMBER_OF_CORES > 2 */
  {
    /* Empty else statement to comply with MISRA 14.10 */
  }
}

/* Acknowledge the signal received by the core "cpu" (0/1/2) */
__INLINE__ void __ALWAYS_INLINE__ EE_tc2Yx_ack_signal( EE_TYPECOREID cpu )
{
  if ( cpu == 0U ) {
    SRC_GPSR00.U |= EE_TC2YX_SRN_CLEAR_REQUEST;
  } else if ( cpu == 1U ) {
    SRC_GPSR10.U |= EE_TC2YX_SRN_CLEAR_REQUEST;
  } else
#if (EE_NUMBER_OF_CORES > 2)
  if ( cpu == 2U ) {
    SRC_GPSR20.U |= EE_TC2YX_SRN_CLEAR_REQUEST;
  } else
#endif /* EE_NUMBER_OF_CORES > 2 */
  {
    /* Empty else statement to comply with MISRA 14.10 */
  }
}

#define EE_tc_ack_signal(cpu)   EE_tc2Yx_ack_signal(cpu)
#define EE_tc_signal_cpu(cpu)   EE_tc2Yx_signal_cpu(cpu)

/*******************************************************************************
                          Multicore Symbols Remapping
 ******************************************************************************/
#ifdef __TASKING__
#ifdef EE_MASTER_CPU
/* Symbol remapping already done */

#elif (EE_CURRENTCPU == 1)
/* Start-Up Symbols Remapping */
#define EE_B_USTACK       _lc_ub_ustack_tc1   /* user stack base */
#define EE_E_USTACK       _lc_ue_ustack_tc1   /* user stack end */
#define EE_E_ISTACK       _lc_ue_istack_tc1   /* interrupt stack end */
#define EE_INT_TAB        _lc_u_int_tab_tc1   /* interrupt table */
#define EE_TRAP_TAB       _lc_u_trap_tab_tc1  /* trap table */
#define EE_SMALL_DATA     _SMALL_DATA_        /* centre of A0 addr. area */
#define EE_LITERAL_DATA   _LITERAL_DATA_      /* centre of A1 addr. area */
#define EE_A8_DATA        _A8_DATA_           /* centre of A8 addr. area */
#define EE_A9_DATA        _A9_DATA_           /* centre of A9 addr. area */
#define EE_B_CSA          _lc_ub_csa_tc1      /* Context Save Area base */
#define EE_E_CSA          _lc_ue_csa_tc1      /* Context Save Area end  */

#define EE_C_INIT_TC      _c_init_tc1         /* C initialization function */
#define EE_TC2YX_START    EE_COMPILER_SECTION(EE_tc2Yx_cpu1_start) EE_tc2Yx_cpu1_start /* Core Start-up code entry */

#elif (EE_CURRENTCPU == 2)
/* Start-Up Symbols Remapping */
#define EE_B_USTACK       _lc_ub_ustack_tc2   /* user stack base */
#define EE_E_USTACK       _lc_ue_ustack_tc2   /* user stack end */
#define EE_E_ISTACK       _lc_ue_istack_tc2   /* interrupt stack end */
#define EE_INT_TAB        _lc_u_int_tab_tc2   /* interrupt table */
#define EE_TRAP_TAB       _lc_u_trap_tab_tc2  /* trap table */
#define EE_SMALL_DATA     _SMALL_DATA_        /* centre of A0 addr. area */
#define EE_LITERAL_DATA   _LITERAL_DATA_      /* centre of A1 addr. area */
#define EE_A8_DATA        _A8_DATA_           /* centre of A8 addr. area */
#define EE_A9_DATA        _A9_DATA_           /* centre of A9 addr. area */
#define EE_B_CSA          _lc_ub_csa_tc2      /* Context Save Area base */
#define EE_E_CSA          _lc_ue_csa_tc2      /* Context Save Area end  */

#define EE_C_INIT_TC      _c_init_tc2         /* C initialization function */
#define EE_TC2YX_START    EE_COMPILER_SECTION(EE_tc2Yx_cpu2_start) EE_tc2Yx_cpu2_start /* Core Start-up code entry */

#else
#error Unknown CPU ID
#endif /* EE_MASTER_CPU */
#elif defined (__GNUC__) || defined(__DCC__)
#ifdef EE_MASTER_CPU
/* Symbol remapping already done */

#elif (EE_CURRENTCPU == 1) 
/* Start-Up Symbols Remapping */
#define EE_B_USTACK     __USTACK_BEGIN    /* user stack base */
#define EE_E_USTACK     __USTACK          /* user stack end */
#define EE_E_ISTACK     __ISTACK          /* interrupt stack end */

/* Generated in code */
#define EE_INT_TAB      EE_tc_interrupt_table  /* interrupt table */
#define EE_TRAP_TAB     EE_tc_trap_table       /* trap table */

#define EE_SMALL_DATA     _SMALL_DATA_    /* centre of A0 addressable area */
#define EE_LITERAL_DATA   _SMALL_DATA2_   /* centre of A1 addressable area */
#define EE_A8_DATA        _SMALL_DATA3_   /* centre of A8 addressable area */
#define EE_A9_DATA        _SMALL_DATA4_   /* centre of A9 addressable area */
#define EE_B_CSA          __CSA_BEGIN     /* Context Save Area base */
#define EE_E_CSA          __CSA_END       /* Context Save Area end  */

#define EE_TC2YX_START  EE_tc2Yx_cpu1_start /* Core Start-up code entry */

#elif (EE_CURRENTCPU == 2)
/* Start-Up Symbols Remapping */
#define EE_B_USTACK     __USTACK_BEGIN    /* user stack base */
#define EE_E_USTACK     __USTACK          /* user stack end */
#define EE_E_ISTACK     __ISTACK          /* interrupt stack end */

/* Generated in code */
#define EE_INT_TAB      EE_tc_interrupt_table  /* interrupt table */
#define EE_TRAP_TAB     EE_tc_trap_table       /* trap table */

#define EE_SMALL_DATA     _SMALL_DATA_    /* centre of A0 addressable area */
#define EE_LITERAL_DATA   _SMALL_DATA2_   /* centre of A1 addressable area */
#define EE_A8_DATA        _SMALL_DATA3_   /* centre of A8 addressable area */
#define EE_A9_DATA        _SMALL_DATA4_   /* centre of A9 addressable area */
#define EE_B_CSA          __CSA_BEGIN     /* Context Save Area base */
#define EE_E_CSA          __CSA_END       /* Context Save Area end  */

#define EE_TC2YX_START  EE_tc2Yx_cpu2_start /* Core Start-up code entry */

#else
#error Unknown CPU ID
#endif /* EE_MASTER_CPU */
#else /* __TASKING__ || __GNUC__ || __DCC__ */
#error Unsupported compiler!
#endif /* __TASKING__ || __GNUC__ || __DCC__ */
#else /* __MSRP__ */
/* DUMMY EE_tc2Yx_setup_inter_irqs declaration for Singlecore EE_cpu_startos */
#define EE_tc2Yx_setup_inter_irqs() ((void)0)
#endif /* __MSRP__ */
#endif /* INCLUDE_EE_TC2YX_MULTICORE_H__ */

