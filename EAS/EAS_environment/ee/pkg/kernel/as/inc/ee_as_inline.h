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

 /** @file      ee_as_inline.h
  *  @brief     Kernel API Implementation Inline For Autosar Multicore Support
  *  @author    Errico Guidieri
  *  @date      2012
  */

#ifndef INCLUDE_EE_KERNEL_AS_INLINE__
#define INCLUDE_EE_KERNEL_AS_INLINE__

#ifdef __MSRP__

/* EG: The following should have been declared in ee_as_internal.h,
       but in-line services implementation force me to declare it here. */
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
/** @brief counter for Autosar cores started (OS_CORE_ID_MASTER is always an
      AUTOSAR by default) */
extern EE_UREG volatile EE_SHARED_IDATA EE_as_core_started;
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */
/** @brief mask for non Autosar cores started */
extern EE_UREG volatile  EE_SHARED_UDATA EE_as_not_as_core_mask;
#ifdef EE_SUPPORT_MEMMAP_H
#define SHARED_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifndef EE_PRIVATE_GETNUMBEROFACTIVATEDCORES__
__INLINE__ EE_UINT32 __ALWAYS_INLINE__
  EE_as_GetNumberOfActivatedCores( void )
{
  /* EG: XXX Add Service Protection? (I think is just overkilling, can we
     just count this as features extension?) */
  /* Both assignment to enable smart debuggers to notice the entry and
     exit from getactiveapplicationmode.
     Note that the variable is volatile, so both the writings succeeds */
  /* [OS673]: The return value of GetNumberOfActivatedCores shall be less or
      equal to the configured value of "OsNumberOfCores". */
  EE_ORTI_set_service_in(EE_SERVICETRACE_GETNUMBEROFACTIVATEDCORES);
  EE_ORTI_set_service_out(EE_SERVICETRACE_GETNUMBEROFACTIVATEDCORES);
  return EE_as_core_started;
}
#endif /* EE_PRIVATE_GETNUMBEROFACTIVATEDCORES__ */

#ifndef EE_PRIVATE_GETCOREID__
__INLINE__ CoreIdType __ALWAYS_INLINE__ EE_as_GetCoreID( void )
{
  /* [OS675] The function GetCoreID shall return the unique logical CoreID of
     the core on which the function is called. The mapping of physical cores
     to logical CoreIDs is implementation specific. (BSW4080001) */
  register CoreIdType core_id;
  /* XXX: CoreID service trace doesn't work, because the tracing variable should
          be shared or dispatched beetwen cores. */
  /* EE_ORTI_set_service_in(EE_SERVICETRACE_GETCOREID); */
  core_id = (CoreIdType)EE_hal_get_core_id();
  /* EE_ORTI_set_service_out(EE_SERVICETRACE_GETCOREID); */
  return  core_id;
}
#endif /* EE_PRIVATE_GETCOREID__ */

#endif /* __MSRP__ */

#endif /* INCLUDE_EE_KERNEL_AS_INLINE__ */
