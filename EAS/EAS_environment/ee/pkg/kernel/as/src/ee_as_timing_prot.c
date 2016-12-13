/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2013 Evidence Srl
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

 /** @file      ee_as_timing_prot.c
  *  @brief     AUTOSAR timing protection implementation
  *  @author    Errico Guidieri
  *  @date      2013
  */

#include "ee_internal.h"

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA OS section: ee_kernel_data */
#define OS_START_SEC_VAR_DATA
/* The following code belong to ERIKA OS section: ee_kernel_text */
#define OS_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** Active Timing Protection Information set */
EE_as_tp_active_type EE_as_tp_active = { INVALID_TIMING_PROTECTION, NULL,
  NULL };

/** Be Aware that Reclamation Time Frame Budget Can Interrupt the Kernel
    because it updates only TP data structures */
#ifndef EE_NO_RECLAMATION_TIME_FRAMES

/** Shortcut to the Time Frames Reclaim Budget conf structure */
static const EE_as_tp_budget_conf_type * const
  reclamation_time_frame_budget_conf_ref = 
    &EE_as_tp_budget_confs[EE_RECLAMATION_TIME_FRAMES_BUDGET_ID];
/** Shortcut to the Time Frames Reclaim Budget data structure */
static EE_as_tp_budget_data_type * const
  reclamation_time_frame_budget_data_ref =
    &EE_as_tp_budget_data[EE_RECLAMATION_TIME_FRAMES_BUDGET_ID];
/** This hold the last update time for Time Frames Reclamation Budget */
static TickType reclamation_time_frame_budget_last_update = 0U;

static void EE_as_tp_update_reclamation_budget ( TickType now )
{
  const TickType delta = EE_hal_swfrt_eval_elapsed_time(now,
    reclamation_time_frame_budget_last_update);

  reclamation_time_frame_budget_last_update = now;

  /* Set a remaining budget of 0 in case (delta > real remaining budget) */
  if ( delta < reclamation_time_frame_budget_data_ref->remaining_budget ) {
    reclamation_time_frame_budget_data_ref->remaining_budget -= delta;
  } else {
    reclamation_time_frame_budget_data_ref->remaining_budget = 0U;
  }
}

static void EE_as_tp_RAM_first_expiring_against_reclamation (
  EE_as_tp_RAM_ref const tp_ram_ref )
{
  if ( EE_as_tp_budget_data[tp_ram_ref->first_expiring].
    remaining_budget > reclamation_time_frame_budget_data_ref->
      remaining_budget )
  {
    tp_ram_ref->first_expiring = EE_RECLAMATION_TIME_FRAMES_BUDGET_ID;
  }
}

static void EE_as_tp_time_frames_reclaim ( void )
{
  TickType const now = EE_hal_swfrt_get_current_time();
  TimingProtectionType tp_id;

  for ( tp_id = 0U; tp_id < EE_MAX_TP; ++tp_id )
  {
    const EE_as_tp_ROM_const_ref  tp_rom_ref = EE_as_tp_ROM_refs[tp_id];
    const EE_as_tp_RAM_ref        tp_ram_ref = EE_as_tp_RAM_refs[tp_id];

    if ( (tp_rom_ref != NULL) && (tp_ram_ref != NULL) )
    {
      const TickType  delta =
        EE_hal_swfrt_eval_elapsed_time(now, tp_ram_ref->interarrival_frame.
          frame_start);

      if ( delta >= tp_rom_ref->frame_duration ) {
        tp_ram_ref->interarrival_frame.active = EE_FALSE;
      }
    }
  }

  /* RESET the Time Frames Reclamation Budget and Update
     reclamation_time_frame_budget_last_update */
  reclamation_time_frame_budget_data_ref->remaining_budget =
    reclamation_time_frame_budget_conf_ref->budget_value;
  reclamation_time_frame_budget_last_update = now;
}

void EE_as_tp_active_start_idle ( void ) {
  TickType const now = EE_hal_swfrt_get_current_time();
  EE_as_tp_active_set(INVALID_TIMING_PROTECTION);

  EE_as_tp_update_reclamation_budget(now);

  if ( reclamation_time_frame_budget_data_ref->
    remaining_budget == 0U )
  {
    /* Handle here the Time Frames Reclamation Budget */
    EE_as_tp_time_frames_reclaim();
  }

  /* Just Re-enable Time Frame Reclamation Budget */
  EE_hal_tp_set_expiration(reclamation_time_frame_budget_data_ref->
    remaining_budget);
}

#else /* !EE_NO_RECLAMATION_TIME_FRAMES */
#define EE_as_tp_update_reclamation_budget(now)                     ((void)0)
#define EE_as_tp_RAM_first_expiring_against_reclamation(tp_ram_ref) ((void)0)
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */

/** Update Active Budgets: It return the active budget reference */
static EE_as_tp_budget_data_type * EE_as_tp_active_update_budgets (
  TickType now, BudgetType * first_expiring_index_ref )
{
  /* Update budgets */
  BudgetType  b_index;

  EE_as_tp_budget_data_type *     first_expiring_budget_ref;
  EE_as_tp_RAM_type * const       tp_ram = EE_as_tp_active.active_tp_RAM_ref;
  EE_as_tp_ROM_type const * const tp_rom = EE_as_tp_active.active_tp_ROM_ref;
  TickType const                  delta  = EE_hal_swfrt_eval_elapsed_time(now,
    tp_ram->last_update);

  tp_ram->last_update = now;

  /* Prepare return values */
  (*first_expiring_index_ref)  = tp_ram->first_expiring;
  first_expiring_budget_ref =
    &EE_as_tp_budget_data[(*first_expiring_index_ref)];

  /* Always Update the Reclaim Time Frames budget */
  EE_as_tp_update_reclamation_budget(now);

  for ( b_index = tp_rom->budgets_begin_index;
        b_index <= tp_rom->budgets_end_index; ++b_index )
  {
    if ( EE_as_tp_budget_data[b_index].active != EE_FALSE )
    {
      if ( delta < EE_as_tp_budget_data[b_index].remaining_budget ) {
        EE_as_tp_budget_data[b_index].remaining_budget -= delta;
      } else {
        EE_as_tp_budget_data[b_index].remaining_budget = 0U;
      }
    }
  }

  return first_expiring_budget_ref;
}

static void EE_as_tp_active_handle_budget_expired ( EE_as_tp_budget_type
  expired )
{
  switch ( expired ) {
    case  EE_EXECUTION_BUDGET:
      EE_as_call_protection_error(EE_as_active_app,
        E_OS_PROTECTION_TIME);
      break;
    case  EE_RESOURCE_LOCK_BUDGET:
    case  EE_OS_INTERRUPT_LOCK_BUDGET:
    case  EE_ALL_INTERRUPT_LOCK_BUDGET:
      EE_as_call_protection_error(EE_as_active_app,
        E_OS_PROTECTION_LOCKED);
      break;
#ifndef EE_NO_RECLAMATION_TIME_FRAMES
    case EE_RECLAMATION_TIME_FRAMES_BUDGET:
      /* Reclaim the already expired Time Frames */
      EE_as_tp_time_frames_reclaim();
      break;
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */
    default:
      /* THIS CANNOT NEVER HAPPENS (Signal the biggest error, just in case) */
      EE_as_call_protection_error(EE_as_active_app,
        E_OS_PROTECTION_EXCEPTION);
      break;
  }
}

/* Get the budget index to the TP represented by tp_rom_ref */
static BudgetType EE_as_tp_active_get_budget ( EE_as_tp_ROM_const_ref const
  tp_rom_ref, EE_as_tp_budget_type budget_type, EE_UTID ref_object_id )
{
  BudgetType                  budget_index;

  switch ( budget_type ) {
    case  EE_EXECUTION_BUDGET:
      budget_index = tp_rom_ref->execution_budget_index;
      break;
#ifndef __OO_NO_RESOURCES__
    case  EE_RESOURCE_LOCK_BUDGET:
      if ( tp_rom_ref->resources_lock_budgets_ref != NULL ) {
        budget_index = (*tp_rom_ref->resources_lock_budgets_ref)[ref_object_id];
      } else {
        budget_index = INVALID_BUDGET;
      }
      break;
#endif /* __OO_NO_RESOURCES__ */
    case  EE_OS_INTERRUPT_LOCK_BUDGET:
      budget_index = tp_rom_ref->os_isr_lock_budget_index;
      break;
    case  EE_ALL_INTERRUPT_LOCK_BUDGET:
      budget_index = tp_rom_ref->all_isr_lock_budget_index;
      break;
#ifndef EE_NO_RECLAMATION_TIME_FRAMES
    case EE_RECLAMATION_TIME_FRAMES_BUDGET:
      /* budget_index = EE_RECLAMATION_TIME_FRAMES_BUDGET_ID; */
      /* break; */
      /* XXX: EE_RECLAMATION_TIME_FRAME_BUDGET is an invalid budget where
              this function is called */
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */
    default:
      /* THIS CANNOT NEVER HAPPENS */
      budget_index = INVALID_BUDGET;
      break;
  }

  return budget_index;
}

/** Set active Timing Protection from TASK Id, saving "now" in "last update" */
void EE_as_tp_active_set_from_TASK ( TaskType task_id )
{
  EE_as_tp_active_set(EE_AS_TP_ID_FROM_TASK(task_id));

  if ( EE_as_tp_active.active_tp != INVALID_TIMING_PROTECTION )
  {
    /* Frame reclamation check against first expiring */
    EE_as_tp_RAM_first_expiring_against_reclamation(EE_as_tp_active.
      active_tp_RAM_ref);

    EE_as_tp_active.active_tp_RAM_ref->last_update =
      EE_hal_swfrt_get_current_time();
  }
}

/** Set active timing protection from TP Id, saving "now" in "last update" */
void EE_as_tp_active_set_from_id_with_restart ( TimingProtectionType tp_id )
{
  EE_as_tp_RAM_type * tp_ram_ref;

  EE_as_tp_active_set(tp_id);

  if ( EE_as_tp_active.active_tp != INVALID_TIMING_PROTECTION )
  {
    tp_ram_ref              = EE_as_tp_active.active_tp_RAM_ref;
    tp_ram_ref->last_update = EE_hal_swfrt_get_current_time();

    /* Frame reclamation check against first expiring */
    EE_as_tp_RAM_first_expiring_against_reclamation(tp_ram_ref);

    /* Restart the first expiring budget */
    EE_hal_tp_set_expiration( EE_as_tp_budget_data[tp_ram_ref->first_expiring].
      remaining_budget );
  }
#ifndef EE_NO_RECLAMATION_TIME_FRAMES
  else {
    /* Just restart the Time Frames Reclamation Budget */
    EE_hal_tp_set_expiration(reclamation_time_frame_budget_data_ref->
      remaining_budget);
  }
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */
}

/** Start timing protection for the TASK that is going to be started or
    released from a wait */
void EE_as_tp_active_start_on_TASK_stacking ( TaskType task_id )
{
  BudgetType                  execution_budget_index;

  EE_as_tp_active_set_from_TASK(task_id);

  if ( EE_as_tp_active.active_tp != INVALID_TIMING_PROTECTION )
  {
    /* On Activation/Release the only budget going active is EXECUTIONBUDGET */
    execution_budget_index = EE_as_tp_active.active_tp_ROM_ref->
      execution_budget_index;
    if ( execution_budget_index != INVALID_BUDGET )
    {
      EE_as_tp_RAM_ref const  tp_ram_ref = EE_as_tp_active.active_tp_RAM_ref;
      EE_as_tp_budget_data_type * const execution_budget_ref =
        &EE_as_tp_budget_data[execution_budget_index];

      /* Update TP RAM and execution budget */
      tp_ram_ref->first_expiring    = execution_budget_index;
      execution_budget_ref->active  = EE_TRUE;

      /* Frame reclamation check against first expiring */
      EE_as_tp_RAM_first_expiring_against_reclamation(tp_ram_ref);

      /* Update the last update */
      tp_ram_ref->last_update = EE_hal_swfrt_get_current_time();
      /* Restart the first expiring budget */
      EE_hal_tp_set_expiration(EE_as_tp_budget_data[tp_ram_ref->first_expiring].
        remaining_budget);
    }
  }
#ifndef EE_NO_RECLAMATION_TIME_FRAMES
  else {
    /* Just restart the Time Frames Reclamation Budget */
    EE_hal_tp_set_expiration(reclamation_time_frame_budget_data_ref->
      remaining_budget);
  }
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */
}

/** Start timing protection for a new ISR2 */
void EE_as_tp_active_start_for_ISR2 ( ISRType isr2_id )
{
  BudgetType                  execution_budget_index;

  /* Activate the TP for the ISR2 */
  EE_as_tp_active_set(EE_AS_TP_ID_FROM_ISR2(isr2_id));

  if ( EE_as_tp_active.active_tp != INVALID_TIMING_PROTECTION ) {
    /* At the beginning of a ISR2 the only budget going active is
       EXECUTIONBUDGET. */
    execution_budget_index = EE_as_tp_active.active_tp_ROM_ref->
      execution_budget_index;

    if ( execution_budget_index != INVALID_BUDGET )
    {
      EE_as_tp_RAM_ref const  tp_ram_ref = EE_as_tp_active.active_tp_RAM_ref;
      EE_as_tp_budget_data_type * const execution_budget_ref =
        &EE_as_tp_budget_data[execution_budget_index];

      /* Update TP RAM and execution budget */
      tp_ram_ref->first_expiring    = execution_budget_index;
      execution_budget_ref->active  = EE_TRUE;

      /* Frame reclamation check against first expiring */
      EE_as_tp_RAM_first_expiring_against_reclamation(tp_ram_ref);

      /* Update the last update */
      tp_ram_ref->last_update = EE_hal_swfrt_get_current_time();
      /* Restart the first expiring budget */
      EE_hal_tp_set_expiration(EE_as_tp_budget_data[tp_ram_ref->first_expiring].
        remaining_budget);
    }
  }
#ifndef EE_NO_RECLAMATION_TIME_FRAMES
  else {
    /* Just restart the Time Frames Reclamation Budget */
    EE_hal_tp_set_expiration(reclamation_time_frame_budget_data_ref->
      remaining_budget);
  }
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */
}

/** Stop the active timing protection without updates. Used where we are
    sure that current TP won't be restarted. Instead the Time Frame Reclamation
    Budget is enabled if needed */
void EE_as_tp_active_stop ( void ) {
  EE_hal_tp_stop();
  EE_as_tp_active_set(INVALID_TIMING_PROTECTION);
#ifndef EE_NO_RECLAMATION_TIME_FRAMES
  /* Just Re-enable Time Frame Reclamation Budget */
  EE_hal_tp_set_expiration(reclamation_time_frame_budget_data_ref->
    remaining_budget);
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */
}

void EE_as_tp_active_pause_and_update_budgets_impl ( void )
{
  /* Not used */
  BudgetType  b_index;
  (void)EE_as_tp_active_update_budgets(EE_hal_tp_get_current_time_and_pause(),
    &b_index);
}

void EE_as_tp_active_update_budgets_and_restart_impl ( void )
{
  EE_as_tp_budget_data_type * first_expiring_budget_ref;
  BudgetType                  first_expiring_index;

  first_expiring_budget_ref = 
    EE_as_tp_active_update_budgets(EE_hal_swfrt_get_current_time(),
      &first_expiring_index);

  /* This function is the TP's CRITICAL SECTION end it have to handle
     Trusted Function Call TP Error Delaying. */
  /* [SWS_Os_00565]: When CallTrustedFunction() is called and the caller of
      CallTrustedFunction() is supervised with timing protection, the Operating
      System shall delay any timing protection errors until the return of
      CallTrustedFunction(). */
  /* [SWS_Os_00564]: If such a violation is detected inside a nested call
      sequence of CallTrustedFunction() of a task, the delay shall last until
      the return of the last CallTrustedFunction(). */
  if ( !EE_as_active_app_is_inside_trusted_function_call() )
  {
    if ( first_expiring_budget_ref->remaining_budget > 0U ) {
      /* Restart the timing protection */
      EE_hal_tp_set_expiration(first_expiring_budget_ref->remaining_budget);
    } else {
      /* Handle Budget Expiration */
      EE_as_tp_active_handle_budget_expired(
        EE_as_tp_budget_confs[first_expiring_index].budget_type);
      /* The following code it's executed only if first expiring was
         EE_RECLAMATION_TIME_FRAMES_BUDGET_ID */
      EE_as_tp_active_eval_first_expiring();
    }
  }
}

void EE_as_tp_active_budget_expired ( void ) {
  if ( EE_as_tp_active.active_tp != INVALID_TIMING_PROTECTION ) {
    EE_as_tp_active_handle_budget_expired( EE_as_tp_budget_confs[
      EE_as_tp_active.active_tp_RAM_ref->first_expiring].budget_type );
  } else {
#ifndef EE_NO_RECLAMATION_TIME_FRAMES
  /* EE_as_tp_active_handle_budget_expired(EE_RECLAMATION_TIME_FRAMES_BUDGET);
   Better the following */
  /* Handle Time Frames Reclamation Budget... */
  EE_as_tp_time_frames_reclaim();
  /* ...And Re-enable it */
  EE_hal_tp_set_expiration(reclamation_time_frame_budget_data_ref->
    remaining_budget);
#else /* !EE_NO_RECLAMATION_TIME_FRAMES */
  /* KERNEL BUG Condition! Signal the worst condition */
  EE_as_call_protection_error(EE_as_active_app, E_OS_PROTECTION_EXCEPTION);
#endif  /* !EE_NO_RECLAMATION_TIME_FRAMES */
  }
}

/** This function update actual active budgets, active a new budget for
    the active TP, after that reassess the first expiring budget */
void EE_as_tp_active_activate_budget_impl ( EE_as_tp_budget_type budget_type,
  EE_UTID ref_object_id, EE_TYPEBOOL start_first )
{
  EE_as_tp_budget_data_type * first_expiring_budget_ref;
  /* Not Used: just to feed EE_as_tp_active_update_budgets... */
  BudgetType                  b_index_first_expiring;

  /* Index and Reference to the budget to be activated */
  BudgetType                  time_budget_index;
  EE_as_tp_budget_data_type * time_budget_ref;

  /* Get TP {RAM, ROM} references */
  EE_as_tp_RAM_type * const tp_ram_ref = EE_as_tp_active.active_tp_RAM_ref;
  EE_as_tp_ROM_type   const * const tp_rom_ref =
    EE_as_tp_active.active_tp_ROM_ref;

  /* Check if the budget to be activated is configured */
  time_budget_index = EE_as_tp_active_get_budget( tp_rom_ref, budget_type,
    ref_object_id );

  /* If the budget is really configured: handle the update + activation */
  if ( time_budget_index != INVALID_BUDGET )
  {
    /* Update active budgets and get information for the first expiring
       budget */
    first_expiring_budget_ref = EE_as_tp_active_update_budgets(
      EE_hal_swfrt_get_current_time(), &b_index_first_expiring);

    /* Get the budget data structure */
    time_budget_ref = &EE_as_tp_budget_data[time_budget_index];

    /* Activate the new one */
    time_budget_ref->active = EE_TRUE;

    /* Eventually change the first expiring budget */
    if ( time_budget_ref->remaining_budget < first_expiring_budget_ref->
      remaining_budget )
    {
      tp_ram_ref->first_expiring = time_budget_index;
      if ( start_first ) {
        EE_hal_tp_set_expiration(time_budget_ref->remaining_budget);
      }
    } else {
      if ( start_first ) {
        EE_hal_tp_set_expiration(first_expiring_budget_ref->remaining_budget);
      }
    }
  }
}

/** This function update actual active budgets, active a new budget for
    the active TP, after that reassess the first expiring budget */
void EE_as_tp_active_stop_budget_impl ( EE_as_tp_budget_type budget_type,
  EE_UTID ref_object_id, EE_TYPEBOOL start_first )
{
  /* Index of stopping budget */
  BudgetType                  time_budget_index;

  /* Get TP {RAM, ROM} references */
  EE_as_tp_RAM_type * const tp_ram_ref = EE_as_tp_active.active_tp_RAM_ref;
  EE_as_tp_ROM_type   const * const tp_rom_ref =
    EE_as_tp_active.active_tp_ROM_ref;

  /* Check if the budget to be activated is configured */
  time_budget_index = EE_as_tp_active_get_budget( tp_rom_ref, budget_type,
    ref_object_id );

  /* If the budget is really configured: handle the update + activation */
  if ( time_budget_index != INVALID_BUDGET )
  {
    /* Stop and reset the given budget */
    EE_as_tp_stop_budget(tp_ram_ref, time_budget_index);
    /* Check if a new first expiring budget have to be set and evaluate it */
    if ( time_budget_index == tp_ram_ref->first_expiring ) {
      EE_as_tp_active_eval_first_expiring();
    }

    if ( start_first ) {
      /* Restart the first expiring budget */
      EE_hal_tp_set_expiration(EE_as_tp_budget_data[tp_ram_ref->first_expiring].
        remaining_budget);
    }
  }
}

EE_TYPEBOOL EE_as_tp_handle_interarrival ( TimingProtectionType tp_id )
{
  TickType                      now;
  EE_TYPEBOOL                   interarrival_ok = EE_TRUE;
  const EE_as_tp_ROM_const_ref  tp_rom_ref      = EE_as_tp_ROM_refs[tp_id];
  const EE_as_tp_RAM_ref        tp_ram_ref      = EE_as_tp_RAM_refs[tp_id];

  /* If this tp_id is really exist... ( If EE_as_tp_RAM_refs[tp_id] is NULL it
     MUST be NULL EE_as_tp_ROM_refs[tp_id] too!!! )
     (I check both pointers to make MISRA Happy) */
  if ( (tp_rom_ref != NULL) && (tp_ram_ref != NULL) )
  {
    /* If the inter-arrival frame protection is configured for this TP... */
    if ( tp_rom_ref->frame_duration != EE_OS_NO_TIME )
    {
      now = EE_hal_swfrt_get_current_time();

      /* Activation/Release is allowed if the frame is expired
         OR if inter-arrival protection is disabled */
      if( (EE_hal_swfrt_eval_elapsed_time(now, tp_ram_ref->interarrival_frame.
            frame_start) > tp_rom_ref->frame_duration) ||
        (tp_ram_ref->interarrival_frame.active == EE_FALSE) )
      {
        /* Save the new frame start time */
        tp_ram_ref->interarrival_frame.frame_start  = now;
        /* Active inter-arrival protection */
        tp_ram_ref->interarrival_frame.active       = EE_TRUE;
      } else {
        /* Inter-arrival Error: Call the Protection(Hook) */
        EE_as_call_protection_error(EE_as_active_app, E_OS_PROTECTION_ARRIVAL);
        /* And tell to the Kernel to not perform Activation/Release */
        interarrival_ok = EE_FALSE;
      }
    }
  }
  return interarrival_ok;
}

#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_DATA
#define OS_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

