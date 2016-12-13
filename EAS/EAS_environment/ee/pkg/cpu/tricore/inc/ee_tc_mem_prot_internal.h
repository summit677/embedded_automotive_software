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

 /** @file   ee_tc_internal.h
  *  @brief  Here you can find the code for changing a protection set.
             This is supposed to be included only in implentation .c
             files of TriCore CPU layer, So it will include the
             a collector file (ee.h).
  *  @author Errico Guidieri
  *  @date 2012
  */

#ifndef INCLUDE_EE_TC_MEM_PROT_INTERNAL_H__
#define INCLUDE_EE_TC_MEM_PROT_INTERNAL_H__

/* For Kernel Data Structures Types and for protection function */
#include "ee_internal.h"

#ifdef EE_AS_OSAPPLICATIONS__

/* If memory protection or stack monitoring are not defined this file will be
   empty */
#ifdef __EE_MEMORY_PROTECTION__
/******************************************************************************
                      Memory Protection Internal Support
 ******************************************************************************/

/* Adjust OSApplication Range registers */
#ifndef EE_OS_APP_MAPPED_ON_PROTECTION_SETS
__INLINE__ void __ALWAYS_INLINE__
  EE_tc_set_os_app_range_registers(EE_as_Application_ROM_type const * const
    app_ROM_ptr )
{
  /* Configure the protection domain */
  /* Range Registers 3 are Range Registers for OS-Application untrusted RAM */
  EE_tc_set_csfr(EE_TC_OSAPPS_L, (EE_UREG)app_ROM_ptr->sec_info.ram_begin);
  EE_tc_set_csfr(EE_TC_OSAPPS_U, (EE_UREG)app_ROM_ptr->sec_info.ram_end);
}
#else /* !EE_OS_APP_MAPPED_ON_PROTECTION_SETS */
#define EE_tc_set_os_app_range_registers(app_ROM_ptr) ((void)0)
#endif /* !EE_OS_APP_MAPPED_ON_PROTECTION_SETS */

/**
  * This Function read OS-Application configuration data structures starting
  * from a Application ID and set the right value in
  * DPR2_x (Range Data Protection Registers for OS-Application RAM)
  */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_os_app_prot_set_from_appid(
  ApplicationType appid )
{
  /* Used to read and handle PSW */
  register EE_UREG temp_psw;
  /* Retrieve OS-Application Informations */
  register EE_as_Application_ROM_type const * const app_ROM_ptr =
    &EE_as_Application_ROM[appid];
  /* Set OSApplication Range Registers */
  EE_tc_set_os_app_range_registers(app_ROM_ptr);

  /* Set protection set active (PSW.PSR bits) + active PSW.IO
    (Trusted [supervisor] or Untrusted[User-1] ) */
  temp_psw = (EE_tc_get_psw() & EE_TC_PSW_PRS_IO_CLEAN_MASK) |
    EE_TC_PSW_APP_TO_PRS(appid) | app_ROM_ptr->Mode;

  /* Save the new active OS-Application */
  EE_as_active_app = appid;

  /* Activate the new user protection set */
  EE_tc_set_psw(temp_psw);
}

/**
  * This Function read OS-Application configuration data structures starting
  * from a TASK ID converted to an index and set the right value in
  * DPR2_x (Range Data Protection Registers for OS-Application RAM)
  */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_os_app_prot_set_from_task_utid(
  EE_UTID utid )
{
  register ApplicationType const appid = EE_th_app[utid];
  EE_tc_set_os_app_prot_set_from_appid(appid);
}

/* In case of OS-Application mapped on protection set I don't need to
   explicitly restore PSW.PSR. Hardware does that in Context Restoring. */
#ifndef EE_OS_APP_MAPPED_ON_PROTECTION_SETS
__INLINE__ void __ALWAYS_INLINE__
  EE_tc_restore_os_app( ApplicationType app_to_be_restored )
{
  register EE_as_Application_ROM_type const * const app_ROM_ptr =
    &EE_as_Application_ROM[app_to_be_restored];
  /* Set OSApplication Range Registers */
  EE_tc_set_os_app_range_registers(app_ROM_ptr);
}
#else /* !EE_OS_APP_MAPPED_ON_PROTECTION_SETS */
#define EE_tc_restore_os_app( app_to_be_restored ) ((void)0)
#endif /* !EE_OS_APP_MAPPED_ON_PROTECTION_SETS */

#else /* __EE_MEMORY_PROTECTION__ */
/* In the case with OS-Application with no memory protection, the following are
   useful */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_os_app_prot_set_from_appid(
  ApplicationType appid )
{
  EE_as_active_app = appid;
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc_set_os_app_prot_set_from_task_utid(
  EE_UTID utid )
{
  EE_as_active_app = EE_th_app[utid];
}

#define EE_tc_restore_os_app( app_to_be_restored ) ((void)0)
#endif /* __EE_MEMORY_PROTECTION__ */

#endif /* EE_AS_OSAPPLICATIONS__ */

#if defined(EE_STACK_MONITORING__) && defined(EE_AS_OSAPPLICATIONS__)
__INLINE__ void __ALWAYS_INLINE__
  EE_tc_check_and_handle_stack_overflow_with_sp( ApplicationType appid,
    EE_UREG tos, EE_ADDR sp )
{
  if ( EE_tc_check_stack_overflow_with_sp(tos, sp) ) {
    EE_as_call_protection_error( appid, E_OS_STACKFAULT );
  }
}

#else /* EE_STACK_MONITORING__ && EE_AS_OSAPPLICATIONS__ */
#define EE_tc_check_and_handle_stack_overflow_with_sp(appid, tos, sp)\
  ((void)0U)
#endif /* EE_STACK_MONITORING__ && EE_AS_OSAPPLICATIONS__ */

#endif /* INCLUDE_EE_TC_MEM_PROT_INTERNAL_H__ */
