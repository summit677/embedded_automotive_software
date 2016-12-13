/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2008  Evidence Srl
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

/*
 * Author: Paolo Gai
 * CVS: $Id: ee_api.h,v 1.3 2007/05/25 15:55:12 pj Exp $
 */


#ifndef __INCLUDE_KERNEL_EDF_EE_API__
#define __INCLUDE_KERNEL_EDF_EE_API__

#ifdef __EDF__

#ifndef Schedule
#define Schedule			EE_edf_Schedule
#endif

#ifndef ActivateTask
#define ActivateTask		        EE_edf_ActivateTask
#endif

#ifndef GetResource
#define GetResource			EE_edf_GetResource
#endif

#ifndef ReleaseResource
#define ReleaseResource			EE_edf_ReleaseResource
#endif

#ifndef GetTime
#ifdef __TIME_SUPPORT__
#ifndef __JANUS__
#define GetTime		                EE_edf_GetTime
#endif
#endif
#endif

#endif // __EDF__

#endif
