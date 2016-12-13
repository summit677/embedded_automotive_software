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

 /** @file      ee_as_multicore_api.h
  *  @brief     Kernel API For Autosar Multicore Support
  *  @author    Errico Guidieri
  *  @date      2012
  */


#ifndef INCLUDE_EE_KERNEL_AS_API__
#define INCLUDE_EE_KERNEL_AS_API__

#if defined(__OO_BCC1__) || defined(__OO_BCC2__) || defined(__OO_ECC1__) ||\
    defined(__OO_ECC2__)
/* These API are always called directly, never by a syscall */
#ifndef GetNumberOfActivatedCores
#define GetNumberOfActivatedCores   EE_as_GetNumberOfActivatedCores
#endif /* GetNumberOfActivatedCores */

#ifndef GetCoreID
#define GetCoreID                   EE_as_GetCoreID
#endif /* GetCoreID */

#ifndef StartCore
#define StartCore                   EE_as_StartCore
#endif /* StartCore */

#ifndef StartNonAutosarCore
#define StartNonAutosarCore         EE_as_StartNonAutosarCore
#endif /* StartNonAutosarCore */

/* API Called by syscall when memory protection is enabled */
#ifndef __EE_MEMORY_PROTECTION__
#ifndef GetSpinlock
#define GetSpinlock                 EE_as_GetSpinlock
#endif /* GetSpinlock */

#ifndef ReleaseSpinlock
#define ReleaseSpinlock             EE_as_ReleaseSpinlock
#endif /* ReleaseSpinlock */

#ifndef TryToGetSpinlock
#define TryToGetSpinlock            EE_as_TryToGetSpinlock
#endif /* TryToGetSpinlock */

#ifndef ShutdownAllCores
#define ShutdownAllCores            EE_as_ShutdownAllCores
#endif /* ShutdownAllCores */

#endif /* !__EE_MEMORY_PROTECTION__ */

#endif /*  __OO_BCC1__ || __OO_BCC2__ || __OO_ECC1__ || __OO_ECC2__  */

#endif /* INCLUDE_EE_KERNEL_AS_API__ */
