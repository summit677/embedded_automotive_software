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

 /** @file      ee_tc_cpu_reg.h
  *  @brief     Compiler independent CPU register header file
  *  @author    Ashok Abbi, <Ashok.Abbi@infineon.com>
  *  @date      18.07.2013
  */

/* Infineon modifications, integration to Infineon Build environment:
 * Fixes for compilation issues with Tasking Compiler:
 * Updates for compilation issues with Dcc Compiler:
 * Usage of generic compiler independent register header file:
 * Author: 
 *         Ashok Abbi, <Ashok.Abbi@infineon.com> 18.07.2013
 */

#ifndef EE_TC_CPU_REG_H__
#define EE_TC_CPU_REG_H__

/* Program Control 0 */
#define EE_CPU_REG_PCON0      0x920C
/* Data Memory Control Register */
#define EE_CPU_REG_DCON0      0x9040
/* Compatibility Control Register */
#define EE_CPU_REG_COMPAT     0x9400

/* Counter Control */
#define EE_CPU_REG_CCTRL      0xFC00
/* CPU Clock Cycle Count */
#define EE_CPU_REG_CCNT       0xFC04
/* Previous Context Information Register */
#define EE_CPU_REG_PCXI       0xFE00
/* Program Status Word */
#define EE_CPU_REG_PSW        0xFE04
/* Program Counter */
#define EE_CPU_REG_PC         0xFE08
/* System Configuration Register */
#define EE_CPU_REG_SYSCON     0xFE14
/* CPUn Identification Register TC1.6P */
#define EE_CPU_REG_CPU_ID     0xFE18
/* CPUn Core Identification Register */
#define EE_CPU_REG_CORE_ID    0xFE1C
/* Base Interrupt Vector Table Pointer */
#define EE_CPU_REG_BIV        0xFE20
/* Base Trap Vector Table Pointer */
#define EE_CPU_REG_BTV        0xFE24
/* Interrupt Stack Pointer */
#define EE_CPU_REG_ISP        0xFE28
/* Interrupt Control Register */
#define EE_CPU_REG_ICR        0xFE2C
/* Free CSA List Head Pointer */
#define EE_CPU_REG_FCX        0xFE38
/* Free CSA List Limit Pointer */
#define EE_CPU_REG_LCX        0xFE3C

/* CPUn Data Protection Range 0, Lower Bound Register */
#define EE_CPU_REG_DPR0_L     0xC000
/* CPUn Data Protection Range 1, Lower Bound Register */
#define EE_CPU_REG_DPR1_L     0xC008
/* CPUn Data Protection Range 2, Lower Bound Register */
#define EE_CPU_REG_DPR2_L     0xC010
/* CPUn Data Protection Range 3, Lower Bound Register */
#define EE_CPU_REG_DPR3_L     0xC018
/* CPUn Data Protection Range 4, Lower Bound Register */
#define EE_CPU_REG_DPR4_L     0xC020
/* CPUn Data Protection Range 5, Lower Bound Register */
#define EE_CPU_REG_DPR5_L     0xC028
/* CPUn Data Protection Range 6, Lower Bound Register */
#define EE_CPU_REG_DPR6_L     0xC030
/* CPUn Data Protection Range 7, Lower Bound Register */
#define EE_CPU_REG_DPR7_L     0xC038
/* CPUn Data Protection Range 8, Lower Bound Register */
#define EE_CPU_REG_DPR8_L     0xC040
/* CPUn Data Protection Range 9, Lower Bound Register */
#define EE_CPU_REG_DPR9_L     0xC048
/* CPUn Data Protection Range 10, Lower Bound Register */
#define EE_CPU_REG_DPR10_L    0xC050
/* CPUn Data Protection Range 11, Lower Bound Register */
#define EE_CPU_REG_DPR11_L    0xC058
/* CPUn Data Protection Range 12, Lower Bound Register */
#define EE_CPU_REG_DPR12_L    0xC060
/* CPUn Data Protection Range 13, Lower Bound Register */
#define EE_CPU_REG_DPR13_L    0xC068
/* CPUn Data Protection Range 14, Lower Bound Register */
#define EE_CPU_REG_DPR14_L    0xC070
/* CPUn Data Protection Range 15, Lower Bound Register */
#define EE_CPU_REG_DPR15_L    0xC078
/* CPUn Data Protection Range 0, Upper Bound Register */
#define EE_CPU_REG_DPR0_U     0xC004
/* CPUn Data Protection Range 1, Upper Bound Register */
#define EE_CPU_REG_DPR1_U     0xC00C
/* CPUn Data Protection Range 2, Upper Bound Register */
#define EE_CPU_REG_DPR2_U     0xC014
/* CPUn Data Protection Range 3, Upper Bound Register */
#define EE_CPU_REG_DPR3_U     0xC01C
/* CPUn Data Protection Range 4, Upper Bound Register */
#define EE_CPU_REG_DPR4_U     0xC024
/* CPUn Data Protection Range 5, Upper Bound Register */
#define EE_CPU_REG_DPR5_U     0xC02C
/* CPUn Data Protection Range 6, Upper Bound Register */
#define EE_CPU_REG_DPR6_U     0xC034
/* CPUn Data Protection Range 7, Upper Bound Register */
#define EE_CPU_REG_DPR7_U     0xC03C
/* CPUn Data Protection Range 8, Upper Bound Register */
#define EE_CPU_REG_DPR8_U     0xC044
/* CPUn Data Protection Range 9, Upper Bound Register */
#define EE_CPU_REG_DPR9_U     0xC04C
/* CPUn Data Protection Range 10, Upper Bound Register */
#define EE_CPU_REG_DPR10_U    0xC054
/* CPUn Data Protection Range 11, Upper Bound Register */
#define EE_CPU_REG_DPR11_U    0xC05C
/* CPUn Data Protection Range 12, Upper Bound Register */
#define EE_CPU_REG_DPR12_U    0xC064
/* CPUn Data Protection Range 13, Upper Bound Register */
#define EE_CPU_REG_DPR13_U    0xC06C
/* CPUn Data Protection Range 14, Upper Bound Register */
#define EE_CPU_REG_DPR14_U    0xC074
/* CPUn Data Protection Range 15, Upper Bound Register */
#define EE_CPU_REG_DPR15_U    0xC07C

/* CPUn Code Protection Range 0 Lower Bound Register */
#define EE_CPU_REG_CPR0_L     0xD000
/* CPUn Code Protection Range 1 Lower Bound Register */
#define EE_CPU_REG_CPR1_L     0xD008
/* CPUn Code Protection Range 2 Lower Bound Register */
#define EE_CPU_REG_CPR2_L     0xD010
/* CPUn Code Protection Range 3 Lower Bound Register */
#define EE_CPU_REG_CPR3_L     0xD018
/* CPUn Code Protection Range 4 Lower Bound Register */
#define EE_CPU_REG_CPR4_L     0xD020
/* CPUn Code Protection Range 5 Lower Bound Register */
#define EE_CPU_REG_CPR5_L     0xD028
/* CPUn Code Protection Range 6 Lower Bound Register */
#define EE_CPU_REG_CPR6_L     0xD030
/* CPUn Code Protection Range 7 Lower Bound Register */
#define EE_CPU_REG_CPR7_L     0xD038
/* CPUn Code Protection Range 0 Upper Bound Register */
#define EE_CPU_REG_CPR0_U     0xD004
/* CPUn Code Protection Range 1 Upper Bound Register */
#define EE_CPU_REG_CPR1_U     0xD00C
/* CPUn Code Protection Range 2 Upper Bound Register */
#define EE_CPU_REG_CPR2_U     0xD014
/* CPUn Code Protection Range 3 Upper Bound Register */
#define EE_CPU_REG_CPR3_U     0xD01C
/* CPUn Code Protection Range 4 Upper Bound Register */
#define EE_CPU_REG_CPR4_U     0xD024
/* CPUn Code Protection Range 5 Upper Bound Register */
#define EE_CPU_REG_CPR5_U     0xD02C
/* CPUn Code Protection Range 6 Upper Bound Register */
#define EE_CPU_REG_CPR6_U     0xD034
/* CPUn Code Protection Range 7 Upper Bound Register */
#define EE_CPU_REG_CPR7_U     0xD03C

/* CPUn Data Protection Read Enable Register Set 0 */
#define EE_CPU_REG_DPRE_0     0xE010
/* CPUn Data Protection Read Enable Register Set 1 */
#define EE_CPU_REG_DPRE_1     0xE014
/* CPUn Data Protection Read Enable Register Set 2 */
#define EE_CPU_REG_DPRE_2     0xE018
/* CPUn Data Protection Read Enable Register Set 3 */
#define EE_CPU_REG_DPRE_3     0xE01C

/* CPUn Data Protection Write Enable Register Set 0 */
#define EE_CPU_REG_DPWE_0     0xE020
/* CPUn Data Protection Write Enable Register Set 1 */
#define EE_CPU_REG_DPWE_1     0xE024
/* CPUn Data Protection Write Enable Register Set 2 */
#define EE_CPU_REG_DPWE_2     0xE028
/* CPUn Data Protection Write Enable Register Set 3 */
#define EE_CPU_REG_DPWE_3     0xE02C

/* CPUn Code Protection Execute Enable Register Set 0 */
#define EE_CPU_REG_CPXE_0     0xE000
/* CPUn Code Protection Execute Enable Register Set 1 */
#define EE_CPU_REG_CPXE_1     0xE004
/* CPUn Code Protection Execute Enable Register Set 2 */
#define EE_CPU_REG_CPXE_2     0xE008
/* CPUn Code Protection Execute Enable Register Set 3 */
#define EE_CPU_REG_CPXE_3     0xE00C

/* CPUn Temporal Protection System Timer Register 0 */
#define EE_CPU_REG_TPS_TIMER0 0xE404
/* CPUn Temporal Protection System Timer Register 0 */
#define EE_CPU_REG_TPS_TIMER1 0xE408
/* CPUn Temporal Protection System Timer Register 0 */
#define EE_CPU_REG_TPS_TIMER2 0xE40C
/* CPUn Temporal Protection System Control Register */
#define EE_CPU_REG_TPS_CON    0xE400

#endif /* EE_TC_CPU_REG_H__ */
