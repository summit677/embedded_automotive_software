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

 /** @file   ee_tc_mem_prot.c
  *  @brief  Memory protection CSFR configuration.
  *  @author Errico Guidieri
  *  @date 2012
  */

/******************************************************************************
              Memory Protection & Timing Protection Configuration
 ******************************************************************************/
#if defined(__EE_MEMORY_PROTECTION__) || defined(EE_TIMING_PROTECTION__)
/* N.B EE_TIMING_PROTECTION__ is just a placeholder not implemented yet */
/* Compiler check has been already done inside EE_tc_cpu.h */

/* Include internals to let this function see the Kernel data structures for
   OS-Applications configuration */
#include "ee_internal.h"

#ifdef __EE_MEMORY_PROTECTION__
/*
 * Utility function that group the common configuration code between the two
 * memory implemented protection configurations(i.e. OS-Application directly
 * mapped on Protection sets or dynamically associated).
 */
static EE_UREG EE_tc_conf_common_memory_prot( void ) {

  /* Begin Code Range */
  extern void ee_sall_code ( void );
  /* End Code Range */
  extern void ee_eall_code ( void );

  /* Ram Begin Address */
  extern EE_UINT32 ee_skernel_ram[];
  /* Kernel Ram End Address */
  extern EE_UINT32 ee_ekernel_ram[];

  /* Begin Const API Section Address */
  extern EE_UINT32 ee_sapi_const[];
  /* End Const API Flash End Address */
  extern EE_UINT32 ee_eapi_const[];

  /* API Ram Begin Address */
  extern EE_UINT32 ee_sapi_ram[];
  /* API Ram End Address */
  extern EE_UINT32 ee_eapi_ram[];

  /* Kernel Data Structures range Begin Address */
  extern EE_UINT32 ee_sbss_kernel[];
  /* Kernel Data Structures End Address
     (These two symbols are the boundaries of kernel data structures sections:
      all the OSApplications will have read permission for this segment */
  extern EE_UINT32 ee_edata_kernel[];

  /* Kernel Data Structures range Begin Address */
  extern EE_UINT32 ee_sbss_kernel[];
  /* Kernel Data Structures End Address
     (These two symbols are the boundaries of kernel data structures sections:
      all the OSApplications will have read permission for this segment */
  extern EE_UINT32 ee_edata_kernel[];

#ifdef __MSRP__
  /* Shared code */
  extern void ee_mcglobalt_begin( void );
  extern void ee_mcglobalt_end( void );
  /* Shared const */
  extern EE_UINT32 ee_mcglobalc_begin[];
  extern EE_UINT32 ee_mcglobalc_end[];
  /* Shared data */
  extern EE_UINT32 ee_mcglobald_begin[];
  extern EE_UINT32 ee_mcglobald_end[];
#endif /* __MSRP__ */

  /* Code protection is not yet implemented so all the code will be
     made executable */
  EE_tc_set_csfr(EE_TC_ALL_CODE_L, (EE_UREG)ee_sall_code);
  EE_tc_set_csfr(EE_TC_ALL_CODE_U, (EE_UREG)ee_eall_code);

  /* Range Registers 0 are kernel Range Registers for RAM  */
  EE_tc_set_csfr(EE_TC_ALL_RAM_L, (EE_UREG)ee_skernel_ram);
  EE_tc_set_csfr(EE_TC_ALL_RAM_U, (EE_UREG)ee_ekernel_ram);

  /* READ ONLY data are accessible by all the code */
  EE_tc_set_csfr(EE_TC_CONST_L, (EE_UREG)ee_sapi_const);
  EE_tc_set_csfr(EE_TC_CONST_U, (EE_UREG)ee_eapi_const);

  /* Range Registers 2 are ERIKA API Range Registers for RAM  */
  EE_tc_set_csfr(EE_TC_ERIKA_API_DATA_L, (EE_UREG)ee_sapi_ram);
  EE_tc_set_csfr(EE_TC_ERIKA_API_DATA_U, (EE_UREG)ee_eapi_ram);

  /* Range Registers 6 are ERIKA Data Structures Range Registers:
     All OSApplication will have reading permission for this segment */
  EE_tc_set_csfr(EE_TC_ERIKA_DATA_L, (EE_UREG)ee_sbss_kernel);
  EE_tc_set_csfr(EE_TC_ERIKA_DATA_U, (EE_UREG)ee_edata_kernel);

#ifdef __MSRP__
  /* Range Register 7 are ERIKA Multi-core shared data */
  EE_tc_set_csfr(EE_TC_ERIKA_SHARED_L, (EE_UREG)ee_mcglobald_begin);
  EE_tc_set_csfr(EE_TC_ERIKA_SHARED_U, (EE_UREG)ee_mcglobald_end);

  /* Shared code */
  EE_tc_set_csfr(EE_TC_SHARED_CODE_L, (EE_UREG)ee_mcglobalt_begin);
  EE_tc_set_csfr(EE_TC_SHARED_CODE_U, (EE_UREG)ee_mcglobalt_end);

  /* Shared const */
  EE_tc_set_csfr(EE_TC_SHARED_CONST_L, (EE_UREG)ee_mcglobalc_begin);
  EE_tc_set_csfr(EE_TC_SHARED_CONST_U, (EE_UREG)ee_mcglobalc_end);
#endif /* __MSRP__ */

  /*  Range Registers 15 will be used for peripheral space
      so EE_BIT(15) will be added in or to all data protection sets active */
  EE_tc_set_csfr(EE_TC_PERIPHERAL_L, 0xF0000000U);
  /* Range Addresses have to be 8 byte aligned so I'll use 0xFFFFFFF8U
     instead of 0xFFFFFFFFU */
  EE_tc_set_csfr(EE_TC_PERIPHERAL_U, 0xFFFFFFF8U);

  /* PSW.PRS is automatically set to 0 by hardware after a TRAP or an IRQ, so
     set 0 of DPR is implicitly reserved to the kernel */

  /* Set 0 of Data Protection Registers is reserved for kernel ->
     enable ranges 0-1 registers (RAM and FLASH) for Read Access */
  EE_tc_set_csfr(EE_TC_KERNEL_R, EE_TC_PERIPHERAL | EE_TC_SHARED_CONST |
    EE_TC_ERIKA_SHARED | EE_TC_ERIKA_API_DATA | EE_TC_CONST | EE_TC_ALL_RAM);

  /* Set 0 of Data Protection Registers are reserved for kernel ->
     enable ranges 0 registers (RAM) for Write Access */
  EE_tc_set_csfr(EE_TC_KERNEL_W, EE_TC_PERIPHERAL | EE_TC_ERIKA_SHARED |
    EE_TC_ERIKA_API_DATA | EE_TC_ALL_RAM);

  /* XXX: Temporarily all the code is executable */
  EE_tc_set_csfr(EE_TC_KERNEL_X, EE_TC_ALL_CODE | EE_TC_SHARED_CODE);

  /* Start to configure the syscon register value to be set */
  return EE_TC_ENABLE_MEMORY_PROTECTION |
    EE_TC_USER1_PERIPHERAL_ACCESS_AS_SUPERVISOR;
}
#else /* __EE_MEMORY_PROTECTION__ */
#define EE_tc_conf_common_memory_prot()   (0U)
#endif /* __EE_MEMORY_PROTECTION__ */

/* Set the given bitmask on SYSCON register */
static void EE_tc_set_evaluated_syscon( EE_UREG syscon_value )
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
 * structures for OS-Application need to be seen by this here. */
void EE_tc_enable_protections( void )
{
  /* Configure Memory Protection common part and obtain the starting mask for
     syscon register */
  register EE_UREG syscon_value = EE_tc_conf_common_memory_prot();

#ifdef __EE_MEMORY_PROTECTION__
  /* Utility pointer to OS_Application_ROM configuration for configuration */
  register EE_as_Application_ROM_type const * app_ROM_ptr;

  /* OS-Application configurations. N.B. mtcr use constant addressing so
     I cannot use a cycle (incrementing a offset) to configure the following
     register sets. The code is already thought to be used in "Erika as library"
     configuration. So it won't use ifdef on EE_MAX_APP (define/const global
     value). */
  if ( EE_MAX_APP >= 2U )
  {
    /* Trusted bitmask */
    EE_UREG trusted = 0U;
    app_ROM_ptr = &EE_as_Application_ROM[1];

    if( app_ROM_ptr->Mode == EE_MEMPROT_TRUST_MODE )
    {
      /* Add "Kernel" ranges at trusted application */
      trusted = EE_TC_ERIKA_SHARED | EE_TC_ALL_RAM;
    }

    /* Range Registers 3 are Range Registers for OS-Application 1 */
    EE_tc_set_csfr(EE_TC_OSAPP1_L, (EE_UREG)app_ROM_ptr->sec_info.ram_begin);
    EE_tc_set_csfr(EE_TC_OSAPP1_U, (EE_UREG)app_ROM_ptr->sec_info.ram_end);

    /* Set 1 of Protection Registers are reserved for OS-Applications 1 */
    EE_tc_set_csfr(EE_TC_OSAPP1_R, EE_TC_PERIPHERAL | EE_TC_ERIKA_DATA |
      EE_TC_SHARED_CONST | EE_TC_ERIKA_SHARED | EE_TC_OSAPP1 |
      EE_TC_ERIKA_API_DATA | EE_TC_CONST | trusted);

    EE_tc_set_csfr(EE_TC_OSAPP1_W, EE_TC_PERIPHERAL | EE_TC_OSAPP1 |
      EE_TC_ERIKA_API_DATA | trusted);

    /* XXX: Temporarily all the code is executable */
    EE_tc_set_csfr(EE_TC_OSAPP1_X, EE_TC_ALL_CODE | EE_TC_SHARED_CODE);
  }

  if ( EE_MAX_APP >= 3U )
  {
    /* Trusted bitmask */
    EE_UREG trusted = 0U;
    app_ROM_ptr = &EE_as_Application_ROM[2];

    if( app_ROM_ptr->Mode == EE_MEMPROT_TRUST_MODE )
    {
      /* Add "Kernel" ranges at trusted application */
      trusted = EE_TC_ERIKA_SHARED | EE_TC_ALL_RAM;
    }

    /* Range Registers 4 are Range Registers for OS-Application 2 */
    EE_tc_set_csfr(EE_TC_OSAPP2_L, (EE_UREG)app_ROM_ptr->sec_info.ram_begin);
    EE_tc_set_csfr(EE_TC_OSAPP2_U, (EE_UREG)app_ROM_ptr->sec_info.ram_end);

    /* Set 2 of Protection Registers are reserved for OS-Applications 1 */
    EE_tc_set_csfr(EE_TC_OSAPP2_R, EE_TC_PERIPHERAL | EE_TC_ERIKA_DATA |
      EE_TC_SHARED_CONST | EE_TC_ERIKA_SHARED | EE_TC_OSAPP2 |
      EE_TC_ERIKA_API_DATA | EE_TC_CONST | trusted);

    EE_tc_set_csfr(EE_TC_OSAPP2_W, EE_TC_PERIPHERAL | EE_TC_OSAPP2 |
      EE_TC_ERIKA_API_DATA | trusted);

    /* XXX: Temporarily all the code is executable */
    EE_tc_set_csfr(EE_TC_OSAPP2_X, EE_TC_ALL_CODE | EE_TC_SHARED_CODE);
  }

  if ( EE_MAX_APP == 4U )
  {
    /* Trusted bitmask */
    EE_UREG trusted = 0U;
    app_ROM_ptr = &EE_as_Application_ROM[3];

    if( app_ROM_ptr->Mode == EE_MEMPROT_TRUST_MODE )
    {
      /* Add "Kernel" ranges at trusted application protection set */
      trusted = EE_TC_ERIKA_SHARED | EE_TC_ALL_RAM;
    }

    /* Range Registers 5 are Range Registers for OS-Application 3 */
    EE_tc_set_csfr(EE_TC_OSAPP3_L, (EE_UREG)app_ROM_ptr->sec_info.ram_begin);
    EE_tc_set_csfr(EE_TC_OSAPP3_U, (EE_UREG)app_ROM_ptr->sec_info.ram_end);

    /* Set 3 of Protection Registers are reserverd for OS-Applications 1 */
    /* N.B IN ANY CASE READS ALL OVER THE MEMORY ARE ALLOWED SO
       "Kernel RAM range" (EE_BIT(0U)) IS ADDED TO ALL DPRE */
    EE_tc_set_csfr(EE_TC_OSAPP3_R, EE_TC_PERIPHERAL | EE_TC_ERIKA_DATA |
      EE_TC_SHARED_CONST | EE_TC_ERIKA_SHARED | EE_TC_OSAPP3 |
      EE_TC_ERIKA_API_DATA | EE_TC_CONST | trusted);

    EE_tc_set_csfr(EE_TC_OSAPP3_W, EE_TC_PERIPHERAL | EE_TC_OSAPP3 |
      EE_TC_ERIKA_API_DATA | trusted);

    /* XXX: Temporarily all the code is executable */
    EE_tc_set_csfr(EE_TC_OSAPP3_X, EE_TC_ALL_CODE | EE_TC_SHARED_CODE);
  }
#endif /* __EE_MEMORY_PROTECTION__ */

#ifdef EE_TIMING_PROTECTION__
  syscon_value |= EE_TC_ENABLE_TEMPORAL_PROTECTION;
  /* Start the CPU Clock Cycle Counter used as Free Timer */
  EE_tc_start_CCNT();
  /* EE_tc_set_csfr(EE_CPU_REG_CCTRL, 0x2U); */
#endif /* EE_TIMING_PROTECTION__ */

  /* Set the given bitmask on SYSCON register */
  EE_tc_set_evaluated_syscon(syscon_value);
}
#else /* EE_OS_APP_MAPPED_ON_PROTECTION_SETS */
/*
 *  When OS-Application are not directly mapped on protection set,
 *  OS-Application boundaries are dynamically set in data protection range
 *  set 2 at each OS-Application Context switch.
 */
void EE_tc_enable_protections( void )
{
  /* Configure Memory Protection common part and obtain the starting mask for
     syscon register */
  register EE_UREG syscon_value = EE_tc_conf_common_memory_prot();

#ifdef __EE_MEMORY_PROTECTION__
  /* Set 1 of Data Protection Registers are reserved for OS-Applications ->
     enable ranges 2-1 registers (RAM and FLASH) for Read Access.
     N.B IN ANY CASE READS ALL OVER THE MEMORY ARE ALLOWED SO
     "All RAM range" (EE_BIT(0U)) IS ADDED TO DPRE_1 bitmask */
  EE_tc_set_csfr(EE_TC_OSAPPS_R,EE_TC_PERIPHERAL | EE_TC_ERIKA_DATA |
    EE_TC_SHARED_CONST | EE_TC_ERIKA_SHARED | EE_TC_OSAPPS |
    EE_TC_ERIKA_API_DATA | EE_TC_CONST | EE_TC_ALL_RAM);

  /* Set 1 of Data Protection Registers are reserved for OS-Applications ->
     enable ranges 2 registers (RAM) for Write Access */
  EE_tc_set_csfr(EE_TC_OSAPPS_W, EE_TC_PERIPHERAL | EE_TC_OSAPPS |
    EE_TC_ERIKA_API_DATA);

  /* XXX: Temporarily all the code is executable */
  EE_tc_set_csfr(EE_TC_OSAPPS_X, EE_TC_ALL_CODE | EE_TC_SHARED_CODE);

  /* XXX TODO: !!! Add Code Protection Configuration !!! */
#endif /* __EE_MEMORY_PROTECTION__ */

#ifdef EE_TIMING_PROTECTION__
  syscon_value |= EE_TC_ENABLE_TEMPORAL_PROTECTION;
  /* Start the CPU Clock Cycle Counter used as Free Timer */
  EE_tc_start_CCNT();
  /* EE_tc_set_csfr(EE_CPU_REG_CCTRL, 0x2U); */
#endif /* EE_TIMING_PROTECTION__ */

  /* Set the given bitmask on SYSCON register */
  EE_tc_set_evaluated_syscon(syscon_value);
}
#endif /* EE_OS_APP_MAPPED_ON_PROTECTION_SETS */
#endif /* __EE_MEMORY_PROTECTION__ || EE_TIMING_PROTECTION__ */

