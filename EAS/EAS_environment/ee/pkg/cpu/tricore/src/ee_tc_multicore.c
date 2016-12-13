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

 /** @file      ee_tc_multicore.c
  *  @brief     MCU Independent part of Multicore HAL
  *  @author    Errico Guidieri
  *  @date      2012
  */
#include "ee_internal.h"

#ifdef EE_MASTER_CPU
EE_TYPEBARRIER EE_SHARED_UDATA EE_tc_kernel_barrier;
#endif /* EE_MASTER_CPU (EE_CURRENTCPU == 0) */

void EE_hal_sync_barrier( EE_TYPEBARRIER * bar, EE_UREG volatile * p_wait_mask )
{
  /* Not OK for MISRA: taken as reference
  EE_UINT32 const exit_mask  = (0xFFFFFFFFU << EE_MAX_CPU); */

  EE_UINT32 const exit_mask  = (0xFFFFFFFFU ^
    (((EE_UINT32)0x1U << EE_MAX_CPU) - 1U));

  EE_UINT32 wait_mask, all_exited;

  if ( (bar != NULL) && (p_wait_mask != NULL) ) {
    while( (bar->value & exit_mask) != 0U ) {
      ; /* If the barrier is still not completed exited: wait */
    }
    /* I need a spinlock so I will reuse spinlock for core 0 */
    EE_hal_spin_in(EE_SPINLOCK_CORE0);
    /* Set current CPU as entered */
    bar->value |= ((EE_UINT32)1U << EE_CURRENTCPU);
    EE_hal_spin_out(EE_SPINLOCK_CORE0);
    do {
      wait_mask = (*p_wait_mask);
      /* Wait for all other cores/CPUs */
    } while ( (bar->value & wait_mask) != wait_mask );

    all_exited = (wait_mask << EE_MAX_CPU) | wait_mask;
    EE_hal_spin_in(EE_SPINLOCK_CORE0);
    /* Set current CPU as exited */
    bar->value |= ((EE_UINT32)1U << (EE_MAX_CPU + EE_CURRENTCPU));
    if ( bar->value == all_exited ) {
      /* Reset the barrier */
      bar->value = 0U;
    }
    EE_hal_spin_out(EE_SPINLOCK_CORE0);
  }
}

void EE_tc_sync_barrier( EE_TYPEBARRIER * bar )
{
  EE_UINT32 all_entered = ((EE_UINT32)1U << EE_MAX_CPU) - 1U;
  EE_hal_sync_barrier(bar, &all_entered);
}

/* Prototype in scope required */
DeclareIsr(EE_TC_iirq_handler);

/* Declare ISR handler */
ISR2(EE_TC_iirq_handler)
{
#ifdef EE_AS_RPC__
  EE_as_rpc_handler();
#elif defined(__RN__)
  EE_rn_handler();
#endif /*  EE_AS_RPC__ || __RN__ */

}

