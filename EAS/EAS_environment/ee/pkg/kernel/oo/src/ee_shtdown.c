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
 * Author: 2002 Paolo Gai
 * CVS: $Id: ee_shtdown.c,v 1.1 2005/07/16 12:23:42 pj Exp $
 */

#include "ee_internal.h"

/* ShutdownOS

  - This function simply calls ShutdownHook.

  - NOTE: it is not specified into the standard if the ShutdownHook
    have to be called with interrupts disabled or not. The behavior I
    chose is the same for the StartupHook.

  - It seems that the only thing that this function has to do is to call
    the ShutdownHook... In any case, if the ShutdownHook returns, a system
    reset is issued!

*/


#ifndef __PRIVATE_SHUTDOWNOS__

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* Put the following code in ee_kernel_text */
#define OS_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/* StartOS Flag defined inside ee_startos.c */
extern EE_UREG volatile EE_oo_started;

static void EE_oo_shutdown(void)
{
  EE_oo_started = 0U;
  for(;;) {
    ;
  }
}

#ifdef __OO_HAS_SHUTDOWNHOOK__
static void EE_oo_call_ShutdownHook(StatusType Error)
{
  /* Set the context excution at ShutdownHook */
  EE_as_set_execution_context( ShutdownHook_Context );
  ShutdownHook(Error);
}
#else
__INLINE__ void __ALWAYS_INLINE__ EE_oo_call_ShutdownHook(StatusType Error)
{
  /* Useless operations to meet Linters requirements */
  volatile StatusType dummy_assignment = Error;
  (void)dummy_assignment;
}
#endif

void EE_oo_ShutdownOS_internal(StatusType Error)
{
  /* [OS071]: If the PostTaskHook() is configured, the Operating System shall
      not call the hook if ShutdownOS() is called. */
  /* EE_oo_call_PostTaskHook(); */

  /* Definitely stop the timing protection. I cannot use EE_as_tp_active_stop,
     because it will restart Reclamation Time Frames Budget */
  EE_hal_tp_stop();
  EE_as_tp_active_set(INVALID_TIMING_PROTECTION);

#ifdef EE_AS_USER_SPINLOCKS__
  /* [OS620]: ShutdownOS shall release all spinlocks which are occupied by the
       calling core. (BSW4080021) */
  {
    register EE_TYPESPIN spinlock_id;

    spinlock_id = EE_as_spinlocks_last[EE_CURRENTCPU];
    while ( spinlock_id != INVALID_SPINLOCK )
    {
      /* Remove the last entry from the data structure */
      EE_as_spinlocks_last[EE_CURRENTCPU] = EE_as_spinlocks_stack[spinlock_id];

      /* Free the spinlock */
      EE_as_spinlocks_locker_core[spinlock_id] = INVALID_CORE_ID;
      EE_as_spinlocks_locker_task_or_isr2[spinlock_id] = EE_NIL;

      /* Release the lock */
      EE_hal_spin_out(spinlock_id);

      spinlock_id = EE_as_spinlocks_last[EE_CURRENTCPU];
    }
  }
#endif /* EE_AS_USER_SPINLOCKS__ */

#ifdef EE_AS_OSAPPLICATIONS__
  /* [OS586]: During the shutdown, the OS-Application specific ShutdownHook
      shall be called on the core on which the corresponding OS-Application
      is bound. (BSW4080007) */
  {
    register  ApplicationType i;
    /* Set the context execution at ShutdownHook Context */
    EE_as_set_execution_context( ShutdownHook_Context );
    /* ApplID 0 is always reserved by the Kernel */
    for ( i = 1U; i < EE_MAX_APP; ++i ) {
      register EE_STATUSHOOKTYPE shutdown_hook =
        EE_as_Application_shutdownhook[i];

      if ( shutdown_hook != 0U ) {
        /* [OS225]: The Operating System module shall execute an
            application-specific shutdown hook with the access rights of the
            associated OS-Application. */
        EE_hal_call_app_status_hook(Error, shutdown_hook, i);
      }
    }
  }
#endif /* EE_AS_OSAPPLICATIONS__ */

#ifdef EE_AS_RPC__
  {
    register EE_TYPESPIN spinlock_id = EE_as_core_spinlocks[EE_CURRENTCPU];

    /* Check global ShutdownAllCores flag in a critical section */
    EE_hal_spin_in( spinlock_id );

    if ( EE_as_shutdown_all_cores_flag != 0U )
    {
      /* Release this core spinlock */
      EE_hal_spin_out( spinlock_id );

      /* [OS587]: Before calling the global ShutdownHook, all cores shall be
          synchronized. (BSW4080007) */
      EE_hal_sync_barrier(&EE_shutdownos_barrier, &EE_as_shutdown_mask);
    } else {
      /* Remove this core from the waiting mask: this core has already reached
         the barrier */
      EE_as_shutdown_mask &= (~( (EE_UREG)1U << EE_CURRENTCPU ));

      /* Release this core spinlock */
      EE_hal_spin_out( spinlock_id );
    }
  }
#endif /* EE_AS_RPC__ */

  /* [OS588]: The global ShutdownHook shall be called on all cores.
      (BSW4080007) */
  EE_oo_call_ShutdownHook(Error);

  EE_ORTI_set_service_out(EE_SERVICETRACE_SHUTDOWNOS);

  EE_oo_shutdown();
}

void EE_oo_ShutdownOS( StatusType Error )
{
  /* Primitive Lock Procedure */
  EE_OS_FOREVER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_SHUTDOWNOS);

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      "invalid value" of the service. (BSW11009, BSW11013) */
  /* ShutdownOS is callable by Task and ISR2, ErrorHook and StartupHook */
  if ( EE_oo_check_disableint_error() ) {
    EE_ORTI_set_service_out(EE_SERVICETRACE_SHUTDOWNOS);
  } else if ( (EE_as_execution_context > ErrorHook_Context) &&
    (EE_as_execution_context != StartupHook_Context) )
  {
    EE_ORTI_set_service_out(EE_SERVICETRACE_SHUTDOWNOS);
  } else
#endif /* EE_SERVICE_PROTECTION__ */
#ifdef EE_AS_OSAPPLICATIONS__
  /* [OS054]: The Operating System module shall ignore calls to ShutdownOS()
      from non-trusted OS-Applications. */
  if ( EE_as_Application_ROM[EE_as_active_app].Mode != EE_MEMPROT_TRUST_MODE ) {
    EE_ORTI_set_service_out(EE_SERVICETRACE_SHUTDOWNOS);
  } else
#endif /* EE_AS_OSAPPLICATIONS__ */
  {
    /* This won't never return... */
    EE_oo_ShutdownOS_internal(Error);
  }

  return;
}

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* Put the following code in ee_kernel_text */
#define OS_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* __PRIVATE_SHUTDOWNOS__ */
