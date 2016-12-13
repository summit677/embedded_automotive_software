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

 /** @file      ee_tc_multicore.h
  *  @brief     Multicore dependent part of HAL
  *  @author    Errico Guidieri
  *  @date      2012
  */

#ifndef INCLUDE_EE_TC_MULTICORE_H__
#define INCLUDE_EE_TC_MULTICORE_H__
/*********************************************************************
                Multicore and multiprocessor support
 *********************************************************************/
#ifdef __MSRP__

#ifndef EE_AS_OSAPPLICATIONS__
/* Shared data use separate sections; potentially, three different sections
 * could be used for constant, uninitialized, and initialized data */
#if defined(__TASKING__)
#define EE_SHARED_CDATA  EE_COMPILER_SECTION("ee_mcglobalc") EE_FAR EE_COMPILER_KEEP
#define EE_SHARED_UDATA  EE_COMPILER_SECTION("ee_mcglobald") EE_FAR EE_COMPILER_KEEP
#define EE_SHARED_IDATA  EE_COMPILER_SECTION("ee_mcglobald") EE_FAR EE_COMPILER_KEEP
#define EE_SHARED_TDATA  EE_COMPILER_SECTION("ee_mcglobalt") EE_FAR EE_COMPILER_KEEP
#elif defined(__GNUC__) || defined(__DCC__)
#define EE_SHARED_CDATA  EE_COMPILER_SECTION("ee_mcglobalc")
#define EE_SHARED_UDATA  EE_COMPILER_SECTION("ee_mcglobald")
#define EE_SHARED_IDATA  EE_COMPILER_SECTION("ee_mcglobald")
#define EE_SHARED_TDATA  EE_COMPILER_SECTION("ee_mcglobalt")
#endif

#else /* EE_AS_OSAPPLICATIONS__ */
/* If MemMap.h is used do not use attributes */
#define EE_SHARED_CDATA
#define EE_SHARED_UDATA
#define EE_SHARED_IDATA
#endif /* EE_AS_OSAPPLICATIONS__ */

/*
  ISR priority 1 (and this means ID 1 for TriCore Architecture) is already
  taken by Inter Cores Interrupts in multicore environment. So I raise an
  error if another ISR with the same priority is configured in RT-Druid.
 */
#if defined(EE_TC_1_ISR) || defined(EE_TC_1_ISR_PRI) ||\
    defined(EE_TC_1_ISR_CAT)
#error In multicore environment IRQ priority 1 is already used by ERIKA\
 for Inter Cores Communication.
#endif /* EE_TC_1_ISR || EE_TC_1_ISR_PRI || EE_TC_1_ISR_CAT */

#define EE_TC_1_ISR     EE_TC_iirq_handler
#define EE_TC_1_ISR_PRI EE_ISR_PRI_1
#define EE_TC_1_ISR_CAT 2

/* the MSB in a MUTEX type indicates if a resource is global (1) or
   local (0) to a processor */
#define EE_GLOBAL_MUTEX ((ResourceType)0x80000000U)

/** @brief Type for spinlocks: an ID */
typedef EE_UINT32 EE_TYPESPIN;

/** @brief Type for inter cpu barriers */
typedef struct ee_tc_barrier {
  volatile EE_UINT32 value;
} EE_TYPEBARRIER;

/** 
 *  @brief Synchronize on a barrier. Wait until all CPUs have called this
 *    function on the same barrier.
 *  @param bar Pointer to barrier to synchronize on.
 */
void EE_tc_sync_barrier( EE_TYPEBARRIER *bar );

#ifdef EE_AS_OSAPPLICATIONS__
#define SHARED_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */
/* declaration EE_TYPEBARRIER EE_SHARED_UDATA EE_tc2Yx_start_barrier */
extern EE_TYPEBARRIER EE_SHARED_UDATA EE_tc_kernel_barrier;
#ifdef EE_AS_OSAPPLICATIONS__
#define SHARED_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */

/* Standard Name Mapping for kernel */
#define EE_startos_before_hook_barrier          EE_tc_kernel_barrier
#define EE_startos_before_scheduling_barrier    EE_tc_kernel_barrier
#define EE_shutdownos_barrier                   EE_tc_kernel_barrier

/* The following are added here to fulfil some Autosar kernel requirements that
   are architecture dependent */
#if (defined(EE_TC27X__) || defined(EE_TC29X__))
#define EE_NUMBER_OF_CORES    3
#elif defined(EE_TC26X__)
#define EE_NUMBER_OF_CORES    2
#endif /* EE_TC29X__ || EE_TC27X__ || EE_TC26X__ */

/* [OS583]: The number of cores that can be controlled by the AUTOSAR OS shall
    be configured offline.
    A new configuration item (OsNumberOfCores) within the "OsOS" container is
    used to specify the maximum number of cores that are controlled by the
    AUTOSAR OS. If no value for (OsNumberOfCores) has been specified the number
    of cores shall be one. (BSW4080001, BSW4080011)
    FIXME: This SHOULD NOT be here */
#define OsNumberOfCores   EE_NUMBER_OF_CORES

/* [OS627] An implementation shall define a set of constants OS_CORE_ID_<No>
   of the type CoreIDType with <No> a value from 0 to "OsNumberOfCores - 1".
   (BSW4080001) */
/* [OS628] An implementation shall offer a constant OS_CORE_ID_MASTER of the
   type CoreIDType that refers to the master core. (BSW4080001) */
/** @brief ID type for cores */
typedef enum EE_tc_core_id {
  OS_CORE_ID_0 = 0x0,
  OS_CORE_ID_MASTER = OS_CORE_ID_0,
  OS_CORE_ID_1 = (OS_CORE_ID_MASTER + 0x1),
#if (EE_NUMBER_OF_CORES > 2)
  OS_CORE_ID_2 = (OS_CORE_ID_MASTER + 0x2)
#endif /* EE_NUMBER_OF_CORES > 2 */
} EE_TYPECOREID;

#define INVALID_CORE_ID ((EE_TYPECOREID)-1)

/** @brief Reference to a core ID */
typedef EE_TYPECOREID *       EE_TYPECOREIDREF;
typedef EE_TYPECOREID const * EE_TYPECOREIDCONSTREF;

/** @brief retun information abpout wich core is executing */
__INLINE__ EE_TYPECOREID __ALWAYS_INLINE__ EE_tc_get_core_id( void )
{
  return (EE_TYPECOREID)EE_tc_get_csfr(EE_CPU_REG_CORE_ID);
}

/*******************************************************************************
                      HAL For Multicore Management
   (This should be placed in internal but inline implementation of GetCoreID
    Services need that this symbol is visible to user code).
 ******************************************************************************/
#define EE_hal_get_core_id() EE_tc_get_core_id()

/* Value that have to be write to DBGSR to put the Core on RUN at reset time */
#define EE_RESET_DBGSR_HALT 2
/** @brief start the core represented by id param from start_up_addr */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_start_core( EE_TYPECOREID core_id,
    EE_ADDR start_up_addr)
{
  switch( core_id )
  {
    case OS_CORE_ID_0:
    /* Nothing to do in this case */
    break;
    case OS_CORE_ID_1:
      CPU1_PC.U = (EE_UINT32)start_up_addr;
      CPU1_DBGSR.B.HALT = EE_RESET_DBGSR_HALT;
    break;
#if (EE_NUMBER_OF_CORES > 2)
    case OS_CORE_ID_2:
      CPU2_PC.U = (EE_UINT32)start_up_addr;
      CPU2_DBGSR.B.HALT = EE_RESET_DBGSR_HALT;
    break;
#endif /* EE_NUMBER_OF_CORES > 2 */
    default:
    break;
  }
}

/*  Spin Lock implementation valid fore new TriCore core 1.6x that has cmpswapw
    instruction */
#if defined(__CORE_TC16X__) || defined(__TC161__)

/* Internal implementation of spin lock */

/* Values SWAP utilities */
#ifdef __TASKING__
#define EE_tc_cmpswapw(p_var, new_val, expected_val) \
  __cmpswapw((p_var),(new_val),(expected_val))

#elif defined (__GNUC__)
__INLINE__ EE_UINT32 __ALWAYS_INLINE__ EE_tc_cmpswapw( EE_UINT32 * const p_var,
  EE_UINT32 new_val, EE_UINT32 expected_val )
{
  /* The following force new_val in da with a even and expected_val in d(a+1) */
  register /*__extension__ */ EE_UINT64 e_reg = ((EE_UINT64)expected_val << 32U)
    | new_val;

  /* Register modifier A tell to reference the alternative name of the
     register (e.g. e2). "+d" constraint tell the compiler that the parameter
     is an input-output parameter */
  __asm volatile ("cmpswap.w [%1]0, %A0" : "+d"(e_reg) : "a"(p_var) : "memory");

  return (EE_UINT32)e_reg;
}
#elif defined (__DCC__)
  /* extern void _cmpswapw( unsigned long long,  void*, const unsigned int) 
     This is the signature for diab intrinsic cmpswap.w support. I suppose that
     the last parameter is the offset from address value: in this case it will
     be always 0 for us */
  /* The following force new_val in da with a even and expected_val in d(a+1) */
  /* !!! INTRINSIC FUNCTIONS FOR SWAP ARE BUGGED IN DIAB 5.9.2 COMPILER !!! */
/* __INLINE__ EE_UINT32 __ALWAYS_INLINE__ EE_tc_cmpswapw( EE_UINT32 * const p_var,
  EE_UINT32 new_val, EE_UINT32 expected_val )
{
  EE_UINT64 e_reg = ((EE_UINT64)expected_val << 32U) | new_val;

  _cmpswapw(e_reg, p_var, 0); 

  return (EE_UINT32)e_reg;
} */
/* DIAB doesn't support extended register in  */

asm volatile EE_UINT32 EE_tc_cmpswapw( EE_UINT32 * const p_var,
  EE_UINT32 new_val, EE_UINT32 expected_val )
{
% reg p_var, new_val, expected_val
! "%d2", "%d3" /*, "%e2" You cannot declare extended registers */
  mov %d3, expected_val
  mov %d2, new_val
  cmpswap.w %e2, [p_var]0
}
#else
#error Unsupported compiler
#endif /* __TASKING__ || __GNUC__ || __DCC__ */

#if defined(__TASKING__) || defined(__GNUC__)
__INLINE__ EE_UINT32 __ALWAYS_INLINE__ EE_tc_swapw( EE_UINT32 * const p_var,
  EE_UINT32 new_val )
{
  /* I use the following way, instead of "+d" constraint, to tell the compiler
     that new_val is input-output parameter because it is shared between
     TASKING and GNUC compilers */
  __asm("swap.w [%1]0,%0": "=d"(new_val) : "a"(p_var), "0"(new_val) : "memory");
  return new_val;
}
#elif defined(__DCC__)
/*
  p_var and new_val have to be located in registers
  d2 is scratch registers used */
asm volatile EE_UINT32 EE_tc_swapw( EE_UINT32 * const p_var,
  EE_UINT32 new_val )
{
% reg p_var, new_val
! "%d2" 
  swap.w new_val, [p_var]0
  mov %d2, new_val
}

#else
#error Unsupported compiler
#endif /* __TASKING__ || __GNUC__ || __DCC__ */

/* Spinlocks HAL data types */
typedef EE_UINT32           EE_TYPESPINSTATUS;
typedef EE_UINT16           EE_TYPESPINVALUE;
typedef EE_TYPESPINVALUE  * EE_TYPESPINVALUEREF;

#ifdef EE_AS_OSAPPLICATIONS__
#define SHARED_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */

/* Global shared variables used as real spin locks */
/* definition: */
extern EE_TYPESPINSTATUS EE_SHARED_UDATA EE_hal_spinlock_status[];

#ifdef EE_AS_OSAPPLICATIONS__
#define SHARED_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */

#ifdef EE_QUEUEING_SPINLOCKS
#ifdef __TASKING__
/* Suppress infinite loop warning */
EE_DO_PRAGMA(warning 557)
#endif /* __TASKING__ */

/*  QUEUING SPINLOCK:
 *  EE_hal_spinlock_status:
 *
 *  change the definition:
 *  EE_UINT32 EE_SHARED_IDATA EE_hal_spinlock_status[EE_MAX_CPU] = { ... };
 *
 *  When enabled queuing spinlock implementation EE_hal_spinlock_status does not
 *  contain more 0 for free and 1 for taken spinlocks, but a 'spinlock status'
 *  composed by:
 *
 *  Something referring to the cpu that could have taken the spinlock
 *  (in this implementation the address of the entry of
 *   EE_tc_spin_value[][] 'matrix') and the value that means: 'still locked'
 *  (In this implementation is stored in the least significant bit of status,
 *    therefore it will be used an EE_tc_spin_value[][] matrix with all the
 *    elements aligned at least at 2 bytes).
 *
 *  EE_tc_spin_value:
 *  This contains the actual value of the lock, given spinlock id and cpu Id.
 *
 *  The first version of this data structure was declared as following:
 *  extern EE_UINT16 EE_tc_spin_value[][EE_MAX_CPU];
 *
 *  That way of declaring the variable does not work when using a binary
 *  distribution, because the number of CPUs is not specified in eecfg.h as a
 *  define, but it is an external variable.
 *
 *  The data structure we are currently using is basically an array of
 *  pointers. Each pointer points to a vector of data.
 *
 *  The EE_tc_spin_value must point to other spinlock arrays, one for each
 *  spinlock. Each spinlock array must be allocated in RAM and each value
 *  initialized to '0'.
 *
 *  To contains the value it would have been enough EE_UINT8
 *  (or better Standard C _Bool), but 2 bytes alignment it's needed for
 *  each spinlock value so the easier solution is using EE_UINT16 as type.
 *
 *  About the initialization values of the spinlocks data structutures:
 *
 *  Bit 0 of EE_hal_spinlock_status[i] MUST BE DIFFERENT to bit 0 of
 *  EE_hal_spinlock_value[cpu][i] {cpu from 0 to EE_MAX_CPU}
 *
 *  For that reason initialization values will be:
 * 
 *  EE_hal_spinlock_status[i] = ((EE_UINT32)&EE_hal_spin_cpu0[i])+1U);
 *  EE_hal_spinlock_value[i][cpu] = 0;
 *
 *  for each cpu, for each i.
 *
 */
/* definition: */
#ifdef EE_AS_OSAPPLICATIONS__
#define SHARED_START_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */
extern EE_TYPESPINVALUEREF const EE_SHARED_CDATA EE_hal_spinlock_value[];

#ifdef EE_AS_OSAPPLICATIONS__
#define SHARED_STOP_SEC_CONST_DATA
#include "MemMap.h"
#endif /* EE_AS_OSAPPLICATIONS__ */

/* Utility macros */
#define EE_TC_WAIT_ADDR_MASK 0xFFFFFFFEU
#define EE_TC_LOCK_VALUE_ADDRESS(status) \
  ((EE_UINT16 *)((status) & EE_TC_WAIT_ADDR_MASK))
#define EE_TC_LOCK_WHAT_MEANS_LOCKED(status)  ((status) & 0x1U)

/** @brief Acquire a spinlock, spin inside if the spinlock is already busy.
    @param spin_id Spinlock to be aquired id.
 */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_spin_in( EE_TYPESPIN spin_id )
{
  /*  volatile qualifier is mandatory otherwise compiler could "optimize" in
      an infinite loop without subsequent readings */
  register  EE_UINT16 volatile *  wait_address;
  register  EE_UINT32             previous_lock_status;
  register  EE_UINT16             what_means_locked;

  /* Get current core */
  register  EE_TYPECOREID         current_core = EE_tc_get_core_id();
  /*  New Lock Status: is obtained "oring" address of spin lock value
      with his value that will be assume meaning of still locked value.
      (This approach works if each element of EE_hal_spinlock_value is aligned
       at least at 2 bytes: 16bit - half-word alignment) */
  register  EE_UINT32             new_lock_status =
    ((EE_UINT32) &EE_hal_spinlock_value[spin_id][current_core]) |
      EE_hal_spinlock_value[spin_id][current_core];

  /*  The following read-write at &EE_hal_spinlock_status[spin_id] must be
      atomic. EE_tc_swap grants that. */
  previous_lock_status = EE_tc_swapw(&EE_hal_spinlock_status[spin_id],
    new_lock_status);

  /* Extract lock value address (where to spin on) and lock value
     (the value that means still locked) */
  wait_address      = EE_TC_LOCK_VALUE_ADDRESS(previous_lock_status);
  what_means_locked = EE_TC_LOCK_WHAT_MEANS_LOCKED(previous_lock_status);

  while ( (*wait_address) == what_means_locked ) {
    ; /* Spin until wait_address pointed value is equal to what_means_locked */
  }
}

/** @brief Release a spin lock
    @param spin_id Spinlock to be released id */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_spin_out( EE_TYPESPIN spin_id )
{
  /* Get current core */
  register  EE_TYPECOREID         current_core = EE_tc_get_core_id();
  /*  Free the lock. All the writes in
      EE_hal_spinlock_value[spin_id][EE_CURRENTCPU] location are made by current
      cpu so I don't need atomicity */
  EE_hal_spinlock_value[spin_id][current_core] =
    (EE_hal_spinlock_value[spin_id][current_core] == 0U) ? 1U : 0U;
}

/** @brief Try to acquire a spinlock, immediatly exit if it is already busy.
    @param spin_id Spinlock to be aquired id.
 */
__INLINE__ EE_BIT __ALWAYS_INLINE__ EE_tc_try_to_spin_in( EE_TYPESPIN spin_id )
{
  /* FIXME: It is impossible to implement try_to_get semantic with
      queuing spinlocks */
  EE_tc_spin_in(spin_id);
  return 1;
}

#else /* EE_QUEUEING_SPINLOCKS */

/* Spinlocks Trivial Implementation */

/** @brief Acquire a spinlock, spin inside if the spinlock is already busy.
    @param spin_id Spinlock to be aquired id.
 */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_spin_in( EE_TYPESPIN spin_id )
{
  while ( EE_tc_cmpswapw(&EE_hal_spinlock_status[spin_id], 1U, 0U) ) {
    ; /* Wait until you get the spinlock (i.e. write 1 on it)*/
  }
}

/** @brief Release a spin lock
    @param spin_id Spinlock to be released id */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_spin_out( EE_TYPESPIN spin_id )
{
  EE_hal_spinlock_status[spin_id] = 0U;
}

/** @brief Try to acquire a spinlock, immediatly exit if it is already busy.
    @param spin_id Spinlock to be aquired id.
 */
__INLINE__ EE_BIT __ALWAYS_INLINE__ EE_tc_try_to_spin_in( EE_TYPESPIN spin_id )
{
  EE_INT32 res = ( EE_tc_cmpswapw(&EE_hal_spinlock_status[spin_id], 1U, 0U) == 0U );
  return (EE_BIT)res;
}

#endif /* EE_QUEUEING_SPINLOCKS */

/* Map Spin Locks API on HAL */
#define EE_hal_spin_in                          EE_tc_spin_in
#define EE_hal_spin_out                         EE_tc_spin_out
#define EE_hal_try_to_spin_in                   EE_tc_try_to_spin_in
/* Map RN signaling functions */
#define EE_hal_IRQ_interprocessor_served(cpu)   EE_tc_ack_signal(cpu)
#define EE_hal_IRQ_interprocessor(cpu)          EE_tc_signal_cpu(cpu)

/* Multicore name remapping */
#ifdef EE_MASTER_CPU
/* Symbol remapping not needed */
#elif (EE_CURRENTCPU == 1)
#define EE_tc_IRQ_tos                           EE_tc_cpu1_IRQ_tos
#define EE_tc_endint_disable                    EE_tc_cpu1_endint_disable
#define EE_tc_endint_enable                     EE_tc_cpu1_endint_enable
#elif (EE_CURRENTCPU == 2)
#define EE_tc_IRQ_tos                           EE_tc_cpu2_IRQ_tos
#define EE_tc_endint_disable                    EE_tc_cpu2_endint_disable
#define EE_tc_endint_enable                     EE_tc_cpu2_endint_enable
#else
#error Unknown CPU ID
#endif /* EE_MASTER_CPU */

#endif /* __CORE_TC16X__ || __TC161__ */

#endif /* __MSRP__ */
#endif /* INCLUDE_EE_TC_MULTICORE_H__ */

