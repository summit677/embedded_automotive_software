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

 /** @file      ee_as_prot_error.c
  *  @brief     Protection Error Handling and calls to ProtectionHook
  *  @author    Errico Guidieri
  *  @date      2012
  */

#include "ee_internal.h"

#ifdef EE_AS_OSAPPLICATIONS__
void EE_as_handle_protection_error( ApplicationType error_app,
  StatusType error )
{
  /* Call the ProtectionHook and get the policy */
  ProtectionReturnType policy;
  /* ISR Nesting level  */
  EE_UREG current_nesting;
  /* Stacked TASK ID */
  EE_TID  current_task;
  /* Handle Protection Hook execution context */
  EE_TYPECONTEXT prev_context;

  /* Saving the execution context make sense only in case of
     E_OS_PROTECTION_ARRIVAL, otherwise there will some kind of Termination
     (TASK,ISR2,Application,OS) that will handle that */
  if ( E_OS_PROTECTION_ARRIVAL == error  ) {
    prev_context = EE_as_get_execution_context();
  }

  /* Set the context on ProtectionHook */
  EE_as_set_execution_context( ProtectionHook_Context );
  policy = ProtectionHook(error);

  /* [OS506]: If the ProtectionHook() is called with E_OS_PROTECTION_ARRIVAL
      the only valid return values are PRO_IGNORE or PRO_SHUTDOWN.
      Returning other values will result in a call to ShutdownOS(). */
  if ( E_OS_PROTECTION_ARRIVAL == error ) {
    if ( policy == PRO_IGNORE ) {
      /* Restore the previous execution context */
      EE_as_set_execution_context(prev_context);
    } else {
      EE_oo_ShutdownOS_internal(error);
    }
  } else {
#ifdef EE_TIMING_PROTECTION__
    if ( error == E_OS_PROTECTION_LOCKED ) {
      /* Reset ISR counter here to not call ErrorHook too in addition of
         ProtectionHook in case of ISR2 Termination (the ErrorHook would be call
         by EE_IRQ_end_post_stub()) */
      EE_oo_IRQ_disable_count = 0U;
    }
#endif /* EE_TIMING_PROTECTION__ */

    switch ( policy )
    {
      case PRO_IGNORE:
        /* [OS475]: If the ProtectionHook() returns PRO_IGNORE and the
            ProtectionHook() was not called with E_OS_PROTECTION_ARRIVAL then the
            Operating System module shall call ShutdownOS(). */
        EE_oo_ShutdownOS_internal(error);
        break;

      case PRO_TERMINATETASKISR:
        /* [OS243]: If the reaction is to forcibly terminate the Task/Category 2
            OsIsr and no Task or OsIsr can be associated with the error,
            the running OS-Application is forcibly terminated by the Operating
            System. */
        current_nesting = EE_hal_get_IRQ_nesting_level();
        current_task    = EE_stk_queryfirst();
        if ( current_nesting > 0U ) {
#if defined(EE_MAX_ISR2) && (EE_MAX_ISR2 > 0)
        ISRType irq = EE_as_ISR_stack[current_nesting - 1U].ISR_ID;
          if ( EE_as_ISR_ROM[irq].ApplID == error_app ) {
            /* I can terminate the ISR2 because it belong to the application
               that generate the error */
            EE_as_TerminateISR2();
          } else
#endif /* EE_MAX_ISR2 > 0 */
          if ( (error_app != INVALID_OSAPPLICATION) &&
            (error_app != KERNEL_OSAPPLICATION) ) {
            EE_as_TerminateApplication_internal(error_app, NO_RESTART);
          } else {
            /* In any other case here I have to shutdown the OS */
            EE_oo_ShutdownOS_internal(error);
          }
        } else if ( current_task != EE_NIL ) {
          if ( EE_th_app[current_task + 1] == error_app ) {
            /* I can terminate the TASK because it belong to the application
               that generate the error */
            /* [OS108]: If the Operating System module forcibly terminates a
                task, it terminates the task, releases all allocated OSEK
                resources and calls EnableAllInterrupts() /
                ResumeOSInterrupts() / ResumeAllInterrupts() if the Task called
                DisableAllInterrupts() / SuspendOSInterrupts() /
                SuspendAllInterrupts() before without the corresponding
                EnableAllInterrupts() / ResumeOSInterrupts() /
                ResumeAllInterrupts() call. */
            (void)EE_oo_release_all_resources(current_task);
            (void)EE_as_release_all_spinlocks(current_task);
#ifndef __OO_NO_CHAINTASK__
            EE_th_terminate_nextask[current_task] = EE_NIL;
#endif /* __OO_NO_CHAINTASK__ */
            EE_hal_terminate_task(current_task);
          } else if ( (error_app != INVALID_OSAPPLICATION) &&
              (error_app != KERNEL_OSAPPLICATION) ) {
            EE_as_TerminateApplication_internal(error_app, NO_RESTART);
          } else {
            /* In any other case here I have to shutdown the OS */
            EE_oo_ShutdownOS_internal(error);
          }
        } else if ( (error_app != INVALID_OSAPPLICATION) &&
            (error_app != KERNEL_OSAPPLICATION) ) {
          EE_as_TerminateApplication_internal(error_app, NO_RESTART);
        } else {
          /* In any other case here I have to shutdown the OS */
          EE_oo_ShutdownOS_internal(error);
        }
        break;

      case PRO_TERMINATEAPPL:
        if ( EE_as_active_app == error_app ) {
          EE_as_TerminateApplication_internal( EE_as_active_app, NO_RESTART );
          EE_as_terminate_current_app();
        } else if ( (error_app != INVALID_OSAPPLICATION) &&
            (error_app != KERNEL_OSAPPLICATION) ) {
          EE_as_TerminateApplication_internal(error_app, NO_RESTART);
        } else {
          /* [OS244] If the ProtectionHook() returns PRO_TERMINATEAPPL or
             PRO_TERMINATEAPPL_RESTART and no OS-Application can be assigned,
             ShutdownOS() is called. (BSW11014) */
          EE_oo_ShutdownOS_internal(error);
        }
        break;

      case PRO_TERMINATEAPPL_RESTART:
        if ( EE_as_active_app == error_app ) {
          EE_as_TerminateApplication_internal( EE_as_active_app, RESTART );
          EE_as_terminate_current_app();
        }  else if ( (error_app != INVALID_OSAPPLICATION) &&
            (error_app != KERNEL_OSAPPLICATION) ) {
          EE_as_TerminateApplication_internal(error_app, NO_RESTART);
        } else {
          /* [OS244] If the ProtectionHook() returns PRO_TERMINATEAPPL or
             PRO_TERMINATEAPPL_RESTART and no OS-Application can be assigned,
             ShutdownOS() is called. (BSW11014) */
           EE_oo_ShutdownOS_internal(error);
        }
        break;

      case PRO_SHUTDOWN:
        EE_oo_ShutdownOS_internal(error);
        break;

      default:
        /* ProtectionHook returned a wrong policy: shutdown the OS */
        EE_oo_ShutdownOS_internal(error);
        break;
    }
  }
}
#else /* EE_AS_OSAPPLICATIONS__ */
void EE_as_handle_protection_error( StatusType error )
{
  /* Call the ProtectionHook and get the policy */
  ProtectionReturnType policy;
  /* ISR Nesting level  */
  EE_UREG current_nesting;
  /* Stacked TASK ID */
  EE_TID  current_task;
  /* Handle Protection Hook execution context */
  EE_TYPECONTEXT prev_context;

  /* Saving the execution context make sense only in case of
     E_OS_PROTECTION_ARRIVAL, otherwise there will some kind of Termination
     (TASK,ISR2,Application,OS) that will handle that */
  if ( E_OS_PROTECTION_ARRIVAL == error  ) {
    prev_context = EE_as_get_execution_context();
  }

  /* Set the context on ProtectionHook */
  EE_as_set_execution_context( ProtectionHook_Context );
  policy = ProtectionHook(error);

  /* [OS506]: If the ProtectionHook() is called with E_OS_PROTECTION_ARRIVAL
      the only valid return values are PRO_IGNORE or PRO_SHUTDOWN.
      Returning other values will result in a call to ShutdownOS(). */
  if ( E_OS_PROTECTION_ARRIVAL == error ) {
    if ( policy == PRO_IGNORE ) {
      /* Restore the previous execution context */
      EE_as_set_execution_context(prev_context);
    } else {
      EE_oo_ShutdownOS_internal(error);
    }
  } else {

#ifdef EE_TIMING_PROTECTION__
    if ( error == E_OS_PROTECTION_LOCKED ) {
      /* Reset ISR counter here to not call ErrorHook too in addition of
         ProtectionHook in case of ISR2 Termination (the ErrorHook would be call
         by EE_IRQ_end_post_stub()) */
      EE_oo_IRQ_disable_count = 0U;
    }
#endif /* EE_TIMING_PROTECTION__ */

    switch ( policy )
    {
      case PRO_TERMINATETASKISR:
        /* [OS243]: If the reaction is to forcibly terminate the Task/Category 2
            OsIsr and no Task or OsIsr can be associated with the error,
            the running OS-Application is forcibly terminated by the Operating
            System. */
        current_nesting = EE_hal_get_IRQ_nesting_level();
        current_task    = EE_stk_queryfirst();
        if ( current_nesting == 0U ) {
          /* [OS108]: If the Operating System module forcibly terminates a task,
              it terminates the task, releases all allocated OSEK resources and
              calls EnableAllInterrupts()/ ResumeOSInterrupts() /
              ResumeAllInterrupts() if the Task called DisableAllInterrupts() /
              SuspendOSInterrupts() / SuspendAllInterrupts() before without the
              corresponding EnableAllInterrupts() / ResumeOSInterrupts() /
              ResumeAllInterrupts() call. */
            (void)EE_oo_release_all_resources(current_task);
            (void)EE_as_release_all_spinlocks(current_task);
#ifndef __OO_NO_CHAINTASK__
            EE_th_terminate_nextask[current_task] = EE_NIL;
#endif /* __OO_NO_CHAINTASK__ */
          EE_hal_terminate_task(current_task);
        } else {
          EE_oo_ShutdownOS_internal(error);
        }
        break;

      /* [OS475]: If the ProtectionHook() returns PRO_IGNORE and the
          ProtectionHook() was not called with E_OS_PROTECTION_ARRIVAL then the
          Operating System module shall call ShutdownOS(). */
      case PRO_IGNORE:
      case PRO_TERMINATEAPPL:
      case PRO_TERMINATEAPPL_RESTART:
      case PRO_SHUTDOWN:
      default:
        /* ProtectionHook returned a wrong policy: shutdown the OS */
        EE_oo_ShutdownOS_internal(error);
        break;
    }
  }
}
#endif /* EE_AS_OSAPPLICATIONS__ */

