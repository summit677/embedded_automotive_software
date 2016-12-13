/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2011  Evidence Srl
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
 * Author: 2003- Paolo Gai
 *         2011 Bernardo  Dal Seno
 */


#ifndef __KERNEL_RN_RN_H__
#define __KERNEL_RN_RN_H__

#ifdef __MSRP__
#define EE_MARK_REMOTE_TID(tid)   ((tid) + (EE_TID)EE_REMOTE_TID)
#define EE_UNMARK_REMOTE_TID(tid) ((tid) - (EE_TID)EE_REMOTE_TID)
#define EE_IS_TID_REMOTE(tid)     ((EE_UTID)(tid) & (EE_UTID)EE_REMOTE_TID)
#else /* __MSRP__ */
#define EE_MARK_REMOTE_TID(tid)   (tid)
#define EE_UNMARK_REMOTE_TID(tid) (tid)
#define EE_IS_TID_REMOTE(tid)     (0)
#endif /* __MSRP__ */

#ifdef  __RN__

/*
 * Remote Notification
 *
 * This library implements the remote notification procedure
 *
 * Symbols to be defined to enable remote notification:
 * __RN__           Remote Notification in general
 * __RN_COUNTER__   Counter RN
 * __RN_EVENT__     Event RN
 * __RN_TASK__      Task RN
 * __RN_FUNC__      Func RN
 * __RN_BIND__      Bind RN, for FRSH
 *
 * Constants to be defined by the application:
 * - EE_MAX_RN - the total number of remote notifications
 * - EE_MAX_CPU - the total number of CPUs
 *
 * Types defined in this file that can be modified by the application
 * EE_TYPERN - a SIGNED integer - EE_INT8 should be enough in most cases
 * EE_TYPERN_SWITCH - a bit field with at least 3 bits
 * EE_TYPERN_NOTIFY - a bit field with at least 5 bits
 *
 * These types should be required by other parts of the Kernel...
 * EE_UINT8
 * EE_TYPECOUNTER
 * EE_TYPEEVENTMASK
 * EE_TID
 * EE_ADDR
 * EE_UREG
 * EE_TYPESPIN
 *
 */

/* types */

#ifndef EE_TYPERN
#define EE_TYPERN EE_SREG
#endif

#ifndef EE_TYPERN_SWITCH
#define EE_TYPERN_SWITCH EE_UREG
#endif

#ifndef EE_TYPERN_NOTIFY
#define EE_TYPERN_NOTIFY EE_UREG
#endif

/* CPU numbers start from 0: 0,1,2,... */

/* Notification types */
#ifdef __RN_COUNTER__
#define EE_RN_COUNTER   ((EE_TYPERN_NOTIFY)1U)
#endif
       
#ifdef __RN_EVENT__
#define EE_RN_EVENT     ((EE_TYPERN_NOTIFY)2U)
#endif

#ifdef __RN_TASK__
#define EE_RN_TASK      ((EE_TYPERN_NOTIFY)4U)
#endif

#ifdef __RN_FUNC__
#define EE_RN_FUNC      ((EE_TYPERN_NOTIFY)8U)
#endif

#ifdef __RN_BIND__
#define EE_RN_BIND     ((EE_TYPERN_NOTIFY)16U)
#endif

#ifdef __RN_UNBIND__
#define EE_RN_UNBIND   ((EE_TYPERN_NOTIFY)32U)
#endif


/* For each RN: The CPU to which the RN is related to */
#ifdef USE_PRAGMAS
extern const EE_UINT8 EE_rn_cpu[];
#else
extern const EE_UINT8 EE_SHARED_CDATA EE_rn_cpu[];
#endif

/* For each RN: The type of notification that must be used
   initvalue: 0
*/
#ifdef USE_PRAGMAS
extern EE_TYPERN_NOTIFY EE_rn_type[][2];
#else
extern EE_TYPERN_NOTIFY EE_SHARED_IDATA EE_rn_type[][2];
#endif
       
/* For each RN: The counter number if EE_RN_COUNTER, or -1 */
#ifdef __RN_COUNTER__
#ifdef USE_PRAGMAS
extern const EE_TYPECOUNTER EE_rn_counter[];
#else
extern const EE_TYPECOUNTER EE_SHARED_CDATA EE_rn_counter[];
#endif
#endif

/* For each RN, 2 times: all 0 (this data structures will contain the
   event mask set by rn_send */
#ifdef __RN_EVENT__
#ifdef USE_PRAGMAS
extern EE_TYPEEVENTMASK EE_rn_event[][2];
#else
extern EE_TYPEEVENTMASK EE_SHARED_IDATA EE_rn_event[][2];
#endif
#endif

/* For each RN: a TID */
#if defined( __RN_EVENT__ ) || defined( __RN_TASK__ ) || defined( __RN_BIND__ ) || defined( __RN_UNBIND )
#ifdef USE_PRAGMAS
extern const EE_TID EE_rn_task[];
#else
extern const EE_TID EE_SHARED_CDATA EE_rn_task[];
#endif
#endif

/* For each RN: EE_VRES_NIL */
#if defined( __RN_BIND__ )
#ifdef USE_PRAGMAS
extern EE_TYPECONTRACT EE_rn_vres[][2];
#else
extern EE_TYPECONTRACT EE_SHARED_IDATA EE_rn_vres[][2];
#endif
#endif

/* For each RN: a function name if EE_RN_FUNC or -1 */
#ifdef __RN_FUNC__
#ifdef USE_PRAGMAS
extern const EE_ADDR EE_rn_func[];
#else
extern const EE_ADDR EE_SHARED_CDATA EE_rn_func[];
#endif
#endif

/* For each RN, 2 times: -1 */
#ifdef USE_PRAGMAS
extern EE_TYPERN EE_rn_next[][2];
#else
extern EE_TYPERN EE_SHARED_IDATA EE_rn_next[][2];
#endif

/* For each RN: Number of pending notifications. Init value all 0 */
#ifdef USE_PRAGMAS
extern EE_UREG EE_rn_pending[][2];
#else
extern EE_UREG EE_SHARED_IDATA EE_rn_pending[][2];
#endif

/* For each CPU: -1 */
#ifdef USE_PRAGMAS
extern EE_TYPERN EE_rn_first[][2];
#else
extern EE_TYPERN EE_SHARED_IDATA EE_rn_first[][2];
#endif

/* For each CPU: an index of the spin lock to use (may be different
   for each CPU, or may be the same; the value is a valid index that
   must work with EE_hal_spin_in/out */
#ifdef USE_PRAGMAS
extern const EE_TYPESPIN EE_rn_spin[];
#else
extern const EE_TYPESPIN EE_SHARED_CDATA EE_rn_spin[];
#endif

/* For each CPU: initialized to 0 */
#ifdef USE_PRAGMAS
extern EE_TYPERN_SWITCH EE_rn_switch[];
#else
extern EE_TYPERN_SWITCH EE_SHARED_IDATA EE_rn_switch[];
#endif

#endif /* __RN__ */
#endif
