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

 /** @file  ee_tc_irq_internal.h
  *  @brief Header with definition for ISR2 Wrappers generation
  *  @author Errico Guidieri
  *  @date 2012
  */
#ifndef INCLUDE_EE_TC_IRQ_INTERNAL_H__
#define INCLUDE_EE_TC_IRQ_INTERNAL_H__

/* I need cpu internals for multistack global structures */
#include "cpu/tricore/inc/ee_tc_internal.h"
/* I need Kernel inclusion for Common Context and Service Protection */
#include "ee.h"
/* I need Kernel inclusion for IRQ callbacks */
#include "ee_irq.h"
/* Plus I need Common End-ISR Scheduler Implementation */
#include "cpu/common/inc/ee_irqstub.h"
/* Plus I need TriCore IRQ handling defines */
#include "cpu/tricore/inc/ee_tc_irq.h"

/* Labels for Kernel Tracing. It has been used an utility function to generate
   only one copy of Tracing Labels.
   Enabled when ORTI is enabled and ISR are handled by ERIKA's intvec */
#if defined(__OO_ORTI_SERVICETRACE__) && ((!defined(EE_ERIKA_ISR_HANDLING_OFF))\
  || defined(EE_MM_OPT))
/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define API_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

void __NEVER_INLINE__ EE_tc_isr2_call_handler( EE_tc_ISR_handler f )
{
  /* Call The ISR User Handler */
  if ( f != NULL ) {
    f();
  }
}

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
#define API_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#else /* __OO_ORTI_SERVICETRACE__ &&
  (!EE_ERIKA_ISR_HANDLING_OFF || EE_MM_OPT) */
__INLINE__ void __ALWAYS_INLINE__ EE_tc_isr2_call_handler( EE_tc_ISR_handler f )
{
  /* Call The ISR User Handler */
  if ( f != NULL ) {
    f();
  }
}
#endif /* __OO_ORTI_SERVICETRACE__ &&
    (!EE_ERIKA_ISR_HANDLING_OFF || EE_MM_OPT) */

#ifdef EE_AS_OSAPPLICATIONS__

/* ISR2 support have to be configured when OS Applications are used */
#if defined(EE_MAX_ISR_ID) && (EE_MAX_ISR_ID > 0)

/* Include Kernel Data Structures Declarations + Change Protection Set Code */
#include "cpu/tricore/inc/ee_tc_mem_prot_internal.h"

/*
 * With memory protection ISR2 are somehow tricky: we need to execute them in
 * trusted/untrusted mode according to the OS application they belong to, and
 * we must be able to terminate them if necessary. We use one ISR2 stack
 * per application and we maintain a stack of ISR2 descriptors to track
 * the active ISR2s. A complete implementation would use two stacks per
 * application: one for trusted and one for untrusted mode.
 */
__INLINE__ void __ALWAYS_INLINE__ EE_TC_CHANGE_STACK_POINTER
  EE_tc_isr2_wrapper_body( EE_tc_ISR_handler f )
{
  /* In TriCore Architecture the vector table is ordered by priority so ISR
     priority is her ID. After an ISR is taken, her priority value (i.e. her id)
     is automatically loaded, by the hardware, in ICR.CCPN field. */
  register ISRType const isr2_id = (ISRType)EE_tc_get_ICR().bits.CCPN;
  /* Interrupted OS-Application */
  register ApplicationType const app_from = EE_as_active_app;
  /* Get NEW ISR2 Stack Data Structures. The following Data Structures is used
     to save SP and OS-Application for an interrupted ISR2/TASK.
     It is accessed via EE_IRQ_nesting_level (not incremented). */
  register EE_as_ISR_RAM_type * const isr_stack_ptr = 
    &EE_as_ISR_stack[EE_hal_get_IRQ_nesting_level()];

  /* The ISR2 ROM is accessed via isr_id */
  register EE_as_ISR_ROM_type const * const isr_rom_ptr =
    &EE_as_ISR_ROM[isr2_id];
  /* ISR2's OS-Application ID (the OS-Application where to switch to) */
  register ApplicationType const app_to = isr_rom_ptr->ApplID;
  /* OS-Application to switch to informations */
  register EE_as_Application_RAM_type * const app_RAM_ptr =
    &EE_as_Application_RAM[app_to];
  register EE_as_Application_ROM_type const * const app_ROM_ptr =
    &EE_as_Application_ROM[app_to];
  /* Interrupted TOS structure pointer */
  register struct EE_TC_TOS * p_interrupted_tos;
  /* Interrupted Stack */
  register EE_ADDR interrupted_sp;

  /* Pause and Update the active TP */
  EE_as_tp_active_pause_and_update_budgets();

  /* [OS287]: Check if the OS-Application to witch current ISR belong is
      Terminated or Restarting, if yes return immediately. */
  /* [OS563]: The OperatingSystem shall not schedule any other Tasks which
      belong to the same OS-Application as the non-trusted caller of the
      service. Also interrupts of Category 2 which belong to the same
      OS-Application shall be disabled during the execution of the service. */
  /* XXX: The following is not a real ISR2 disabling but is the best I can do */
  /* [SWS_Os_00470]: The Operating System module shall limit the inter-arrival
      time of Category 2 ISRs to one per OsIsrTimeFrame. (SRS_Os_11008) */
  /* [SWS_Os_00471]: The Operating System module shall measure the start of an
      OsIsrTimeFrame from the point at which it recognises the interrupt
      (i.e. in the Operating System interrupt wrapper). (SRS_Os_11008) */
  /* [SWS_Os_00048]: If Category 2 interrupt occurs before the end of the
      OsIsrTimeFrame then the Operating System module SHALL NOT not execute the
      user provided ISR AND shall call the ProtectionHook() with
      E_OS_PROTECTION_ARRIVAL. (SRS_Os_11008) */
  if (  (app_RAM_ptr->ApplState == APPLICATION_ACCESSIBLE) &&
        ((app_RAM_ptr->TrustedFunctionCallsCounter == 0U) ||
          (app_ROM_ptr->Mode == EE_MEMPROT_TRUST_MODE)) &&
        EE_as_tp_handle_interarrival(EE_AS_TP_ID_FROM_ISR2(isr2_id))  )
  {
    /* Switch ORTI ISR2 value */
    EE_ORTI_set_runningisr2(f);

    /* Get the interrupted SP */
    interrupted_sp = EE_tc_get_prev_stack();

    /* Get witch TOS have been interrupted */
    if ( EE_IRQ_nesting_level > 0U ) {
      p_interrupted_tos = &EE_tc_system_tos[EE_as_Application_ROM[app_from].
        ISRTOS];
      /* Save the previous ISR TOS */
      p_interrupted_tos->ram_tos = interrupted_sp;
    } else {
      p_interrupted_tos = &EE_tc_system_tos[EE_tc_active_tos];
    }

    /* Increment nesting level here, with IRQ disabled and after ISR
       Stack Access */
    EE_increment_IRQ_nesting_level();
    /* Save interrupted "Top Of Stack" (TASK or ISR2) to be restored. */
    isr_stack_ptr->TerminationTOS   = p_interrupted_tos->ram_tos;
    /* Save Interrupted OS-Application */
    isr_stack_ptr->Interrupted_App  = app_from;
    /* Save the ID of this ISR to be retrieved by GetISRID() Kernel Function. It's
       used even to flag if an ISR need to be Terminated (not yet supported). */
    isr_stack_ptr->ISR_ID           = isr2_id;

    /* Save Info for TerminateISR: I need to save a context to switch back to and
       there's only explicit lower context saving instruction. I would need a
       CALL to save upper context otherwise: problematic. */
    EE_tc_svlcx();
    /* PCXI Saving */
    isr_stack_ptr->ISR_Terminate_data = EE_tc_get_pcxi();

  /* Set the right execution context */
#ifdef EE_SERVICE_PROTECTION__
    if ( app_to == KERNEL_OSAPPLICATION ) {
      EE_as_set_execution_context( Kernel_Context );
    } else {
      EE_as_set_execution_context( ISR2_Context );
    }
#endif /* EE_SERVICE_PROTECTION__ */

    /* Start the timing protection for the new ISR2 */
    EE_as_tp_active_start_for_ISR2(isr2_id);

    if ( (EE_IRQ_nesting_level == 1U) || (app_from != app_to) ) {
      /* Switch on NEW ISR2 User Stack */
      EE_tc_set_SP(EE_tc_system_tos[app_ROM_ptr->ISRTOS].ram_tos);
      EE_as_active_app = app_to;

      /* Monitor actual stack after ISR2 data structures initialization: In this
         way I can terminate the ISR in case of overflow */
      EE_as_check_and_handle_stack_overflow(app_to,app_ROM_ptr->ISRTOS);

      /* Return in User Stack + Set protection domain active */
      EE_tc_set_psw_user_stack();

      /* Reset Protection Domain */
      EE_tc_set_os_app_prot_set_from_appid(app_to);
    } else {
      /* Set the stack back, set back the active application and re-enable
         User-1 Mode (if needed) */
      EE_tc_set_SP(interrupted_sp);

      /* Monitor actual stack after ISR2 data structures initialization: In this
         way I can terminate the ISR in case of overflow */
      EE_as_check_and_handle_stack_overflow(app_to, app_ROM_ptr->ISRTOS);

      /* Set protection domain active (Trusted or Untrusted) + return in
         User Stack */
      EE_UREG temp_psw = (((EE_tc_get_psw() & EE_TC_PSW_PRS_IO_CLEAN_MASK) &
        EE_TC_PSW_IS_CLEAN_MASK) | EE_as_Application_ROM[app_to].Mode) |
        EE_TC_PSW_APP_TO_PRS(app_to);

      /* Here possible User-1 mode will be re-enabled */
      EE_tc_set_psw(temp_psw);
    }

    /* Enable IRQ if nesting  is allowed (It works even with memory protection,
       because User Tasks run in User-1 mode that has ISR handling enabled) */
    EE_std_enableIRQ_nested();

    /* Call The ISR User Handler */
    EE_tc_isr2_call_handler(f);

#ifndef __EE_MEMORY_PROTECTION__
    /* Disable IRQ if nesting is allowed. Note: if nesting is not allowed,
       the IRQs are already disabled (It works even with memory protection,
       because User Tasks run in User-1 mode that has ISR handling enabled). */
    EE_std_disableIRQ_nested();
    /* Stop active TP */
    EE_as_tp_active_stop();
    /* Handle ISR2 Rollback and Termination + Scheduling. */
    EE_as_TerminateISR2();
#else /* !__EE_MEMORY_PROTECTION__ */
    /* Handle ISR2 Rollback and Termination + Scheduling. */
    EE_tc_syscall( EE_ID_TerminateISR2 );
#endif /* !__EE_MEMORY_PROTECTION__ */
  } else {
    /* Restart old TP's first expiring budget */
    EE_as_tp_active_set_from_id_with_restart(EE_as_tp_active.active_tp);
  }
}
#else /* EE_MAX_ISR_ID > 0 */
#error You cannot use ISR2 with OSApplications without configuring them!
#endif /* EE_MAX_ISR_ID > 0 */

#else /* EE_AS_OSAPPLICATIONS__ */

#if defined(__OO_ORTI_RUNNINGISR2__) || defined(EE_TIMING_PROTECTION__)
#ifndef EE_TIMING_PROTECTION__
/* Only ORTI local */
#define EE_generate_locals()  \
  register EE_ORTI_runningisr2_type orti_old
#elif (!defined(__OO_ORTI_RUNNINGISR2__))
/* Only Timing Protection locals */
#define EE_generate_locals()                  \
  register TimingProtectionType     old_tp;   \
  register ISRType                  isr2_id
#else /* !EE_TIMING_PROTECTION__ || !__OO_ORTI_RUNNINGISR2__ */
/* Both locals */
#define EE_generate_locals()                  \
  register EE_ORTI_runningisr2_type orti_old; \
  register TimingProtectionType     old_tp;   \
  register ISRType                  isr2_id
#endif /* !EE_TIMING_PROTECTION__ */
#else /* __OO_ORTI_RUNNINGISR2__ || EE_TIMING_PROTECTION__ */
/* No locals */
#define EE_generate_locals()  ((void)0)
#endif /*__OO_ORTI_RUNNINGISR2__ || EE_TIMING_PROTECTION__ */

#ifdef __OO_ORTI_RUNNINGISR2__
/* Keep the old ORTI and switch to new one */
#define EE_ORTI_running_isr2_begin(f)         \
  do {                                        \
    orti_old = EE_ORTI_get_runningisr2();     \
    EE_ORTI_set_runningisr2(f);               \
  } while ( 0 )
/* Restore old ORTI */
#define EE_ORTI_running_isr2_end()  EE_ORTI_set_runningisr2(orti_old)
#else /* __OO_ORTI_RUNNINGISR2__ */
#define EE_ORTI_running_isr2_begin(f) ((void)0)
#define EE_ORTI_running_isr2_end()    ((void)0)
#endif /* __OO_ORTI_RUNNINGISR2__ */

#ifdef EE_TIMING_PROTECTION__
#define EE_tp_active_pause_update_and_save()      \
  do {                                            \
    EE_as_tp_active_pause_and_update_budgets();   \
    isr2_id = (ISRType)EE_tc_get_ICR().bits.CCPN; \
    old_tp = EE_as_tp_active.active_tp;           \
  } while ( 0 )

#define EE_tp_active_old_restart()                    \
  do {                                                \
    EE_as_tp_active_set_from_id_with_restart(old_tp); \
  } while ( 0 )
#else /*  EE_TIMING_PROTECTION__ */
#define EE_tp_active_pause_update_and_save()  ((void)0)
#define EE_tp_active_old_restart()            ((void)0)
#endif /*  EE_TIMING_PROTECTION__ */

#if (!defined(__OO_BCC1__)) && (!defined(__OO_BCC2__)) && \
    (!defined(__OO_ECC1__)) && (!defined(__OO_ECC2__))
#define EE_as_set_execution_context(ctx)  ((void)0)
#endif /* !__OO_BCC1__ && !__OO_BCC2__ && !__OO_ECC1__ && !__OO_ECC2__ */

__INLINE__ void __ALWAYS_INLINE__ EE_tc_isr2_wrapper_body( EE_tc_ISR_handler f )
{
  /* This macro generate the local variables eventually needed */
  EE_generate_locals();
  /* Update interrupted TP and Pause it, plus save a local reference to
     restore it at the ISR2 end, in case of ISR2 nesting */
  EE_tp_active_pause_update_and_save();
  /* [SWS_Os_00470]: The Operating System module shall limit the inter-arrival
      time of Category 2 ISRs to one per OsIsrTimeFrame. (SRS_Os_11008) */
  /* [SWS_Os_00471]: The Operating System module shall measure the start of an
      OsIsrTimeFrame from the point at which it recognises the interrupt
      (i.e. in the Operating System interrupt wrapper). (SRS_Os_11008) */
  /* [SWS_Os_00048]: If Category 2 interrupt occurs before the end of the
      OsIsrTimeFrame then the Operating System module shall not execute the user
      provided ISR AND shall call the ProtectionHook() with
      E_OS_PROTECTION_ARRIVAL. (SRS_Os_11008) */
  /* Check the interarrival */
  if ( EE_as_tp_handle_interarrival(EE_AS_TP_ID_FROM_ISR2(isr2_id)) )
  {
    /* Keep the old ORTI ISR2 and switch to new one */
    EE_ORTI_running_isr2_begin(f);
    /* Increment nesting level here, with IRQ disabled */
    EE_increment_IRQ_nesting_level();
    /* Set the context execution at ISR2 */
    EE_as_set_execution_context( ISR2_Context );
    /* Start TP protection for this ISR2 */
    EE_as_tp_active_start_for_ISR2(isr2_id);
    /* Enable IRQ if nesting  is allowed */
    EE_std_enableIRQ_nested();

    /* Call The ISR User Handler */
    EE_tc_isr2_call_handler(f);

    /* Disable IRQ if nesting is allowed.
       Note: if nesting is not allowed, the IRQs are
       already disabled */
    EE_std_disableIRQ_nested();
    /* Stop active TP */
    EE_as_tp_active_stop();
    /* ISR2 instance clean-up as requested by AR, must be
       called  before EE_decrement_IRQ_nesting_level */
    EE_std_end_IRQ_post_stub();
    /* Decrement nesting level here, with IRQ disabled */
    EE_decrement_IRQ_nesting_level();
    /* Restore the old ORTI ISR2 value */
    EE_ORTI_running_isr2_end();
    /* If the ISR at the lowest level is ended, restore old
       SP, reset CCPN and call the scheduler. */
    /* Check for scheduling point */
    if ( EE_is_inside_ISR_call() == 0 ) {
      /* Set CCPN to unmask next IRQ, it would have been
         done by RFE but we are not returning yet */
      EE_tc_set_int_prio( (EE_TYPEISR2PRIO)EE_ISR_UNMASKED );
      /* Restore task stack pointer if needed */
      EE_tc_set_prev_stack_back();
      /* Call the scheduler */
      EE_std_after_IRQ_schedule();
    }
  }
  /* Restore the interrupted timing protection set and restart it */
  EE_tp_active_old_restart();
}
#endif /* EE_AS_OSAPPLICATIONS__ */

/* ISR2 Wrapper Function Body */
#define EE_ISR2_WRAPPER_BODY(f)  EE_tc_isr2_wrapper_body(f)

#endif /* INCLUDE_EE_TC_IRQ_INTERNAL_H__ */

