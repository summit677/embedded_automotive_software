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

 /** @file   ee_tc27x_endinit.h
  *  @brief  ENDINIT Support Must Be Added by One and no more of
  *          Kernel modules, by default is included in start-up
  *          code for Tasking.
  *  @author Errico Guidieri
  *  @date 2012
  */

#ifndef INCLUDE_EE_TC27X_ENDINIT_H__
#define INCLUDE_EE_TC27X_ENDINIT_H__

/* Public API must be seen by Kernel internals */
#include "mcu/infineon_tc27x/inc/ee_tc27x_mcu.h"

/* Multicore ENDINIT Support */
#ifdef EE_MASTER_CPU
#if defined (__DCC__)
/* DIAB support new naming convention for SCU WDT registers */
#define EE_WDTCPUCON0   SCU_WDTCPU0_CON0
#define EE_WDTCPUCON1   SCU_WDTCPU0_CON1
#else
#define EE_WDTCPUCON0   SCU_WDTCPU0CON0
#define EE_WDTCPUCON1   SCU_WDTCPU0CON1
#endif /* __DCC__ */
#elif (EE_CURRENTCPU == 1)
#if defined (__DCC__)
/* DIAB support new naming convention for SCU WDT registers */
#define EE_WDTCPUCON0   SCU_WDTCPU1_CON0
#define EE_WDTCPUCON1   SCU_WDTCPU1_CON1
#else
#define EE_WDTCPUCON0   SCU_WDTCPU1CON0
#define EE_WDTCPUCON1   SCU_WDTCPU1CON1
#endif /* __DCC__ */
#elif (EE_CURRENTCPU == 2)
#if defined (__DCC__)
/* DIAB support new naming convention for SCU WDT registers */
#define EE_WDTCPUCON0   SCU_WDTCPU2_CON0
#define EE_WDTCPUCON1   SCU_WDTCPU2_CON1
#else
#define EE_WDTCPUCON0   SCU_WDTCPU2CON0
#define EE_WDTCPUCON1   SCU_WDTCPU2CON1
#endif /* __DCC__ */
#else
#error Unknown CPU ID
#endif

/**************************************************************************
 *
 * FUNCTION:     endinit_set
 *
 * DESCRIPTION:  Sets or Clears the ENDINIT bit in the WDT_CON0 register
 *               in order to enabled or disable the write-protection for
 *               registers protected via the EndInit feature
 *               (ie. BTV, BIV, ISP, PCON0, DCON0).
 *
 *************************************************************************/
__INLINE__ void __ALWAYS_INLINE__ EE_tc27x_endinit_set( EE_tc_endinit_t
  endinit_value )
{
  EE_UINT32 wdt_con0;
  /*
   * 1st step: Password access (create password and send to WDT_CON0)
   */
  wdt_con0 = EE_WDTCPUCON0.U;

  wdt_con0 &= 0xFFFFFF01U;    /* clear WDTLCK, WDTHPW0, WDTHPW1 */
  wdt_con0 |= 0xF1U;          /* set WDTHPW1 to 0xF  1 must be written to
                                 ENDINIT for password access 
                                (but this will not actually modify the bit) */
  EE_WDTCPUCON0.U = wdt_con0;

  /*
   * 2nd step: Modify access, set the bit ENDINIT to 1 or 0 to allow access to
   *           registers: WDT_CON1, BTV, BIV, ISP and mod_CLC
   */
  wdt_con0 &= 0xFFFFFFF0U;          /* clear WDTHPW0, WDTLCK, ENDINIT  */
  /* WDTHPW0=0, WDTLCK=1, ENDINIT=endinit_value */
  wdt_con0 |= (EE_UINT32)0x02U | (EE_UINT32)endinit_value;
  EE_tc_isync();
  EE_WDTCPUCON0.U = wdt_con0;

  EE_WDTCPUCON0.U;                  /* read is required */
}

/**************************************************************************
 *
 * FUNCTION:     safety_endinit_set
 *
 * DESCRIPTION:  Sets or Clears the ENDINIT bit in the WDTSCON0 register
 *               in order to enabled or disable the write-protection for
 *               safety-critical registers protected via the EndInit feature.
 *
 *************************************************************************/

__INLINE__ void __ALWAYS_INLINE__ EE_tc27x_safety_endinit_set( EE_tc_endinit_t
  endinit_value )
{
  EE_UINT32 wdtscon0;

  /*
   * 1st step: Password access (create password and send to WDTSCON0)
   */
  wdtscon0 = SCU_WDTSCON0.U;

  wdtscon0 &= 0xFFFFFF01U;    /* clear WDTLCK, WDTHPW0, WDTHPW1 */
  wdtscon0 |= 0xF1U;          /* set WDTHPW1 to 0xF  1 must be written to
                                 ENDINIT for password access 
                                (but this will not actually modify the bit) */
  SCU_WDTSCON0.U = wdtscon0;

  /*
   * 2nd step: Modify access, set the bit ENDINIT to 1 or 0 to allow access to
   *           registers: SCU_WDTSCON1, BTV, BIV, ISP and mod_CLC
   */
  wdtscon0 &= 0xFFFFFFF0U;        /* clear WDTHPW0, WDTLCK, ENDINIT  */
  /* WDTHPW0=0, WDTLCK=1, ENDINIT=endinit_value */
  wdtscon0 |= (EE_UINT32)0x02 | (EE_UINT32)endinit_value;
  EE_tc_isync();
  SCU_WDTSCON0.U = wdtscon0;
  SCU_WDTSCON0.U;                 /* read is required */
}

#endif /* INCLUDE_EE_TC27X_ENDINIT_H__ */
