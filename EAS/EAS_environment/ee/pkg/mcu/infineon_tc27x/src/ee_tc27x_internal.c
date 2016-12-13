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

 /** @file   ee_tc27x_internal.c
  *  @brief  MCU-dependent internal part of HAL implementation
  *  @author Errico Guidieri
  *  @date 2012
  */

/******************************************************************************
              Memory Protection & Timing Protection Configuration
 ******************************************************************************/
#if defined(__EE_MEMORY_PROTECTION__) || defined(EE_TIMING_PROTECTION__)
/* N.B EE_TIMING_PROTECTION__ is just a placeholder not implemented yet */
/* Compiler check has been already done inside EE_tc_cpu.h */

#ifdef EE_OS_APP_MAPPED_ON_PROTECTION_SETS

/* Include internals to let this function see the Kernel data structures for
   OS-Applications Configurtation */
#include "ee_internal.h"

/* In this configuration each Os-Application is directly mappet on a protection
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
void EE_tc27x_enable_protections( void )
{
  /* Configure Memory Protection common part and obtain the starting mask for
     syscon register */
  register EE_UREG syscon_value = EE_tc27x_conf_common_memory_prot();

#ifdef __EE_MEMORY_PROTECTION__
  /* Utility pointer to OS_Application_ROM configuration for configuartion */
  register EE_as_Application_ROM_type const * app_ROM_ptr;

  /* OS-Application configurations. N.B. mtcr use constant addressing so
     I cannot use a cycle (incrementing a offset) to configure the following
     register sets. The code is already thought to be used in "Erika as library"
     configuration. So it wont use ifdef on EE_MAX_APP (define/const global
     value). */
  if ( EE_MAX_APP >= 2U )
  {
    app_ROM_ptr = &EE_as_Application_ROM[1];

    /* Trusted bitmask */
    EE_UREG trusted = 0U;
    if( app_ROM_ptr->Mode == EE_MEMPROT_TRUST_MODE )
    {
      /* Add "Kernel" range at trusted application */
      trusted |= EE_BIT(0U);
    }

    /* Range Registers 3 are Range Registers for OS-Application 1 */
    EE_tc_set_csfr(EE_CPU_REG_DPR3_L, (EE_UREG)app_ROM_ptr->sec_info.ram_begin);
    EE_tc_set_csfr(EE_CPU_REG_DPR3_U, (EE_UREG)app_ROM_ptr->sec_info.ram_end);

    /* Set 1 of Protection Registers are reserverd for OS-Applications 1 */
    /* N.B IN ANY CASE READS ALL OVER THE MEMORY ARE ALLOWED SO
       "Kernel RAM range" (EE_BIT(0U)) IS ADDED TO ALL DPRE */
    EE_tc_set_csfr(EE_CPU_REG_DPRE_1, EE_BIT(15U) | EE_BIT(6U) | EE_BIT(3U) |
      EE_BIT(2U) | EE_BIT(1U) | EE_BIT(0U));

    EE_tc_set_csfr(EE_CPU_REG_DPWE_1, EE_BIT(15U) | EE_BIT(3U) | EE_BIT(2U) |
      trusted);

    /* XXX: Temporarily all the code is executable */
    EE_tc_set_csfr(EE_CPU_REG_CPXE_1, EE_BIT(0U));
  }

  if ( EE_MAX_APP >= 3U )
  {
    app_ROM_ptr = &EE_as_Application_ROM[2];

    /* Trusted bitmask */
    EE_UREG trusted = 0U;
    if( app_ROM_ptr->Mode == EE_MEMPROT_TRUST_MODE )
    {
      /* Add "Kernel" range at trusted application */
      trusted |= EE_BIT(0U);
    }

    /* Range Registers 4 are Range Registers for OS-Application 2 */
    EE_tc_set_csfr(EE_CPU_REG_DPR4_L, (EE_UREG)app_ROM_ptr->sec_info.ram_begin);
    EE_tc_set_csfr(EE_CPU_REG_DPR4_U, (EE_UREG)app_ROM_ptr->sec_info.ram_end);

    /* Set 2 of Protection Registers are reserverd for OS-Applications 1 */
    /* N.B IN ANY CASE READS ALL OVER THE MEMORY ARE ALLOWED SO
       "Kernel RAM range" (EE_BIT(0U)) IS ADDED TO ALL DPRE */
    EE_tc_set_csfr(EE_CPU_REG_DPRE_2, EE_BIT(15U) | EE_BIT(6U) | EE_BIT(4U) |
      EE_BIT(2U) | EE_BIT(1U) | EE_BIT(0U));

    EE_tc_set_csfr(EE_CPU_REG_DPWE_2, EE_BIT(15U) | EE_BIT(4U) | EE_BIT(2U) |
      trusted);

    /* XXX: Temporarily all the code is executable */
    EE_tc_set_csfr(EE_CPU_REG_CPXE_2, EE_BIT(0U));
  }

  if ( EE_MAX_APP == 4U )
  {
    app_ROM_ptr = &EE_as_Application_ROM[3];
    
    /* Trusted bitmask */
    EE_UREG trusted = 0U;
    if( app_ROM_ptr->Mode == EE_MEMPROT_TRUST_MODE )
    {
      /* Add "Kernel" range at trusted application protection set */
      trusted |= EE_BIT(0U);
    }

    /* Range Registers 5 are Range Registers for OS-Application 3 */
    EE_tc_set_csfr(EE_CPU_REG_DPR5_L, (EE_UREG)app_ROM_ptr->sec_info.ram_begin);
    EE_tc_set_csfr(EE_CPU_REG_DPR5_U, (EE_UREG)app_ROM_ptr->sec_info.ram_end);

    /* Set 3 of Protection Registers are reserverd for OS-Applications 1 */
    /* N.B IN ANY CASE READS ALL OVER THE MEMORY ARE ALLOWED SO
       "Kernel RAM range" (EE_BIT(0U)) IS ADDED TO ALL DPRE */
    EE_tc_set_csfr(EE_CPU_REG_DPRE_3, EE_BIT(15U) | EE_BIT(6U) | EE_BIT(5U) |
      EE_BIT(2U) | EE_BIT(1U) | EE_BIT(0U));

    EE_tc_set_csfr(EE_CPU_REG_DPWE_3, EE_BIT(15U) | EE_BIT(5U) | EE_BIT(2U) |
      trusted);

    /* XXX: Temporarily all the code is executable */
    EE_tc_set_csfr(EE_CPU_REG_CPXE_3, EE_BIT(0U));
  }
#endif /* __EE_MEMORY_PROTECTION__ */

#ifdef EE_TIMING_PROTECTION__
  syscon_value |= EE_TC_ENABLE_TEMPORAL_PROTECTION;
  /* XXX TODO: Add here TC Temporal Protection Code */
#endif /* EE_TIMING_PROTECTION__ */

  /* Set the given bitmask on SYSCON register */
  EE_tc27x_set_syscon(syscon_value);
}
#endif /* EE_OS_APP_MAPPED_ON_PROTECTION_SETS */

#endif /* __EE_MEMORY_PROTECTION__ || EE_TIMING_PROTECTION__ */

