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

 /** @file      ee_as_timing_prot.h
  *  @brief     Types, macros and function declaration for AUTOSAR
  *             timing protection
  *  @author    Errico Guidieri
  *  @date      2013
  */

#ifndef INCLUDE_EE_KERNEL_AS_TIMING_PROT__
#define INCLUDE_EE_KERNEL_AS_TIMING_PROT__
/* Declare something only if the timing protection feature is enabled */
#ifdef EE_TIMING_PROTECTION__

#ifndef EE_CPU_CLOCK
#error To handle Timing Protection is CPU clock frequency have to be configured!
#endif /* !EE_CPU_CLOCK */

/** @brief Macro that represent the MAX time allowed for TP */
#define EE_AS_TP_MAX_TIME EE_HAL_SWFRT_TIMER_DURATION

/** @brief Macro that represent no time (time equal to zero */
#define EE_OS_NO_TIME ((TickType)0U)

/** @brief Macro used to populate TP data structures, with saturation
           handling. */
#define EE_AS_TP_MICRO_TO_TICKS_SATURATED(X_US, SAT) \
  ((EE_MICRO_TO_CPU_TICKS(X_US) > (SAT))? (SAT): EE_MICRO_TO_CPU_TICKS(X_US))

/** @typedef Used as index to access timing protection informations */
typedef EE_UTID TimingProtectionType;
#define INVALID_TIMING_PROTECTION ((TimingProtectionType)-1)
#define EE_MAX_TP   ((EE_UTID)(EE_MAX_TASK + EE_MAX_ISR_ID))

#ifndef EE_NO_RECLAMATION_TIME_FRAMES
/** Macro that represent the Time Frames Reclamation Budget */
#define EE_RECLAMATION_TIME_FRAMES_BUDGET_ID (EE_MAX_TIMING_BUDGET - 1U)
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */

/** @typedef Used as index to access time budget information */
typedef EE_UTID BudgetType;
#define INVALID_BUDGET            ((BudgetType)-1)

/** @typedef This ENUM hold timing budget identifiers used to access data
    structures */
typedef enum {
  EE_EXECUTION_BUDGET,
  EE_RESOURCE_LOCK_BUDGET,
  EE_OS_INTERRUPT_LOCK_BUDGET,
  EE_ALL_INTERRUPT_LOCK_BUDGET,
#ifndef EE_NO_RECLAMATION_TIME_FRAMES
  /** Special Budget to reclaim expired time frames */
  EE_RECLAMATION_TIME_FRAMES_BUDGET,
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */
} EE_as_tp_budget_type;

/** @typedef This struct hold a time budget dynamic data */
typedef struct EE_as_tp_budget_data_type_ {
  TickType    remaining_budget;
  EE_TYPEBOOL active;
} EE_as_tp_budget_data_type;

/** @typedef This struct hold a time budget configuration data */
typedef struct EE_as_tp_budget_conf_type_ {
  EE_as_tp_budget_type  budget_type;
  TickType              budget_value;
} EE_as_tp_budget_conf_type;

/** @typedef This struct hold time frame data */
typedef struct EE_as_tp_time_frame_type {
  TickType    frame_start;
  EE_TYPEBOOL active;
} EE_as_tp_time_frame;

/** @typedef Pointer to an array that hold resource lock budgets.
    If configured for a TP is accessed with the ResourceID */
#ifndef __OO_NO_RESOURCES__
typedef BudgetType const (*ResourceLockBudgetsTypeRef)[EE_MAX_RESOURCE];
#endif /* !__OO_NO_RESOURCES__ */

/** @typedef This struct hold static timing protection configuration for a TASK
             or an ISR2 */
typedef struct EE_as_tp_ROM_type_ {
  /** Index of the first budget tied to this TP */
  BudgetType                  budgets_begin_index;
  /** Index of the end budget tied to this TP */
  BudgetType                  budgets_end_index;
  /** Index of the execution budget if configured */
  BudgetType                  execution_budget_index;
  /** Index of the OS ISR lock budget if configured */
  BudgetType                  os_isr_lock_budget_index;
  /** Index of the ALL ISR lock budget if configured */
  BudgetType                  all_isr_lock_budget_index;
#ifndef __OO_NO_RESOURCES__
  /** Reference to the array that hold the resource lock budgets */
  ResourceLockBudgetsTypeRef  resources_lock_budgets_ref;
#endif /* !__OO_NO_RESOURCES__ */
  /** Inter-arrival frame duration, a value of EE_OS_NO_TIME means
      no protection */
  TickType                    frame_duration;
} EE_as_tp_ROM_type;

/** @typedef This struct hold timing protection actual data for a TASK or an
             ISR2 */
typedef struct EE_as_tp_RAM_type_ {
  /** Inter-arrival frame protection data */
  EE_as_tp_time_frame interarrival_frame;
  /** Time of the last budgets update */
  TickType            last_update;
  /** Index of the first expiring budget */
  BudgetType          first_expiring;
} EE_as_tp_RAM_type;

/** @typedef TP ROM reference types */
typedef EE_as_tp_ROM_type const *EE_as_tp_ROM_const_ref;
/** @typedef TP RAM reference types */
typedef EE_as_tp_RAM_type *EE_as_tp_RAM_ref;

/** @typedef This struct hold the information related to the first expiring
      budget active. Active TP have to be initialized to
      INVALID_TIMING_PROTECTION, the other fields could be left
      uninitialized. */
typedef struct EE_as_tp_active_type_ {
  /** Index of the active timing protection. Used as shortcut index.
      Otherwise I should always discriminate between current TASK or ISR2 */
  TimingProtectionType      active_tp;
  /** Active timing protection RAM reference */
  EE_as_tp_RAM_ref          active_tp_RAM_ref;
  /** Active timing protection ROM reference */
  EE_as_tp_ROM_const_ref    active_tp_ROM_ref;
} EE_as_tp_active_type;

/** @var Array of timing protection configuration structures indexed with
    TASK_IDs and EE_MAX_TASK + EE_ISR2_IDs */
extern const EE_as_tp_ROM_const_ref
  EE_as_tp_ROM_refs[/*EE_MAX_TASK + EE_MAX_ISR2*/];

/** @var Array of timing protection data structures indexed with TASK_IDs and
    EE_MAX_TASK + EE_ISR2_IDs */
extern const EE_as_tp_RAM_ref EE_as_tp_RAM_refs[/*EE_MAX_TASK + EE_MAX_ISR2*/];

 /** @var Array with all the expiring budgets protection data */
extern const EE_as_tp_budget_conf_type
  EE_as_tp_budget_confs[/*EE_MAX_TIMING_BUDGET*/];

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* The following variables belong to ERIKA OS section: ee_kernel_data */
#define OS_START_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** @var Active Timing Protection Information set */
extern EE_as_tp_active_type EE_as_tp_active;
/** @var Array with all the expiring budgets protection data */
extern EE_as_tp_budget_data_type EE_as_tp_budget_data[/*EE_MAX_TIMING_BUDGET*/];

#ifdef EE_SUPPORT_MEMMAP_H
#define OS_STOP_SEC_VAR_DATA
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

/** Utilities Macros to convert TASK/ISR2 ID's in TP ID's */
#define EE_AS_TP_ID_FROM_TASK(task_id)  ((TimingProtectionType)task_id)
#define EE_AS_TP_ID_FROM_ISR2(isr2_id)  (EE_AS_TP_ID_FROM_TASK(EE_MAX_TASK) +\
  isr2_id)

/** Set active timing protection data structures informations */
__INLINE__ void __ALWAYS_INLINE__ EE_as_tp_active_set ( TimingProtectionType
  tp_id )
{
  /* If EE_as_tp_RAM_refs[tp_id] is NULL it MUST be NULL
     EE_as_tp_ROM_refs[tp_id]  too!!! */
  if ( (tp_id != INVALID_TIMING_PROTECTION) &&
    (EE_as_tp_RAM_refs[tp_id] != NULL) )
  {
    EE_as_tp_active.active_tp         = tp_id;
    EE_as_tp_active.active_tp_RAM_ref = EE_as_tp_RAM_refs[tp_id];
    EE_as_tp_active.active_tp_ROM_ref = EE_as_tp_ROM_refs[tp_id];
  } else {
    EE_as_tp_active.active_tp         = INVALID_TIMING_PROTECTION;
  }
}

#ifndef EE_NO_RECLAMATION_TIME_FRAMES
__INLINE__ void __ALWAYS_INLINE__
  EE_as_tp_active_minimum_against_reclamation ( BudgetType minimum_budget_index,
    TickType actual_min )
{
  if ( actual_min <=
    EE_as_tp_budget_data[EE_RECLAMATION_TIME_FRAMES_BUDGET_ID].
      remaining_budget )
  {
    EE_as_tp_active.active_tp_RAM_ref->first_expiring = minimum_budget_index;
  } else {
    EE_as_tp_active.active_tp_RAM_ref->first_expiring =
      EE_RECLAMATION_TIME_FRAMES_BUDGET_ID;
  }
}
#else /* !EE_NO_RECLAMATION_TIME_FRAMES */
__INLINE__ void __ALWAYS_INLINE__
  EE_as_tp_active_minimum_against_reclamation ( BudgetType minimum_budget_index,
    TickType actual_min )
{
  EE_as_tp_active.active_tp_RAM_ref->first_expiring = minimum_budget_index;
}
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */

/* Search and set as first expiring, the minimum between active budgets of the
   active TP */
__INLINE__ void __ALWAYS_INLINE__ EE_as_tp_active_eval_first_expiring ( void )
{
  BudgetType  b_index;
  BudgetType  minimum_budget_index = INVALID_BUDGET;
  TickType    actual_min = ((TickType)-1);

  for ( b_index = EE_as_tp_active.active_tp_ROM_ref->budgets_begin_index;
        b_index <= EE_as_tp_active.active_tp_ROM_ref->budgets_end_index;
        ++b_index )
  {
    /* Get the budget data reference indexed */
    EE_as_tp_budget_data_type * b_data_ref = &EE_as_tp_budget_data[b_index];
    /* If the indexed budget is active... */
    if ( b_data_ref->active != EE_FALSE ) {
      /* Check if it is NOT the first budget that we found active... */
      if ( minimum_budget_index != INVALID_BUDGET ) {
        /* Check if it is smaller than the actual minimum */
        if ( actual_min > b_data_ref->remaining_budget ) {
          minimum_budget_index = b_index;
          actual_min = b_data_ref->remaining_budget;
        }
      } else {
        /* If this is the first budget found active it is the actual
           minimum too. */
        minimum_budget_index = b_index;
        actual_min = b_data_ref->remaining_budget;
      }
    }
  }

  if ( minimum_budget_index != INVALID_BUDGET ) {
    EE_as_tp_active_minimum_against_reclamation(minimum_budget_index,
      actual_min);
  }
#ifndef EE_NO_RECLAMATION_TIME_FRAMES
  else {
    EE_as_tp_active.active_tp_RAM_ref->first_expiring =
      EE_RECLAMATION_TIME_FRAMES_BUDGET_ID;
  }
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */
}

/** This function deactivate an active budget and reset it */
__INLINE__ void __ALWAYS_INLINE__ EE_as_tp_stop_budget (
  EE_as_tp_RAM_ref const tp_ram_ref, BudgetType budget_id )
{
  EE_as_tp_budget_data_type * const time_budget_ref =
    &EE_as_tp_budget_data[budget_id];

  time_budget_ref->remaining_budget = EE_as_tp_budget_confs[budget_id].
    budget_value;

  time_budget_ref->active           = EE_FALSE;
}

/** Reset timing protection budgets */
__INLINE__ void __ALWAYS_INLINE__ EE_as_tp_reset_budgets ( TimingProtectionType
 tp_id )
{
  BudgetType b_index;
  for ( b_index = EE_as_tp_ROM_refs[tp_id]->budgets_begin_index;
        b_index <= EE_as_tp_ROM_refs[tp_id]->budgets_end_index;
        ++b_index )
  {
    EE_as_tp_stop_budget(EE_as_tp_RAM_refs[tp_id], b_index);
  }
}

__INLINE__ void __ALWAYS_INLINE__ EE_as_tp_stop_interarrival_frame (
  TimingProtectionType tp_id )
{
  const EE_as_tp_RAM_ref tp_ram_ref = EE_as_tp_RAM_refs[tp_id];
  /* If the TASK/ISR2 has TP... */
  if ( tp_ram_ref != NULL ) {
    /*... Stop inter-arrival frame for it. */
    tp_ram_ref->interarrival_frame.active = EE_FALSE;
  }
}

/** Check inter-arrival error in case of Activation/Release from Wait Event.
    If not error happens restart the frame. return EE_TRUE if the
    Activation/Release have to be performed */
EE_TYPEBOOL EE_as_tp_handle_interarrival ( TimingProtectionType tp_id );

/** Reset active timing protection budgets. (If a TP is really active) */
__INLINE__ void __ALWAYS_INLINE__ EE_as_tp_active_reset_budgets ( void )
{
  if ( EE_as_tp_active.active_tp != INVALID_TIMING_PROTECTION )
  {
    BudgetType b_index;
    for ( b_index = EE_as_tp_active.active_tp_ROM_ref->budgets_begin_index;
          b_index <= EE_as_tp_active.active_tp_ROM_ref->budgets_end_index;
          ++b_index )
    {
      EE_as_tp_stop_budget(EE_as_tp_active.active_tp_RAM_ref, b_index);
    }
  }
}

/** Set active timing protection from TASK Id, saving "now" in "last update".
    Used when the TASK is restored from a preemption */
void EE_as_tp_active_set_from_TASK ( TaskType task_id );

/** Set active timing protection and restart it, saving "now" in
    "last update". Used when the ISR2 is restored from a preemption */
void EE_as_tp_active_set_from_id_with_restart ( TimingProtectionType tp_id );

/** Start timing protection for the TASK that is going to be started or
    released from a wait */
void EE_as_tp_active_start_on_TASK_stacking ( TaskType task_id );

/** Start timing protection for a new ISR2 */
void EE_as_tp_active_start_for_ISR2 ( ISRType isr2_id );

/** Stop the active timing protection without updates. Used where we are
    sure that current TP won't be restarted. Instead the Time Frame Reclamation
    Budget is enabled if needed */
void EE_as_tp_active_stop ( void );

#ifndef EE_NO_RECLAMATION_TIME_FRAMES
/** Active the Time Frames Reclamation Budget when the application goes in
    IDLE */
void EE_as_tp_active_start_idle ( void );
#else /* !EE_NO_RECLAMATION_TIME_FRAMES */
#define EE_as_tp_active_start_idle()  ((void)0)
#endif /* !EE_NO_RECLAMATION_TIME_FRAMES */

/* Implementation functions. Those TP functions are broken in two part to safe
   a function call, that can be not cheap in some architectures, if it's not
   really needed */
void EE_as_tp_active_pause_and_update_budgets_impl ( void );

void EE_as_tp_active_update_budgets_and_restart_impl ( void );

void EE_as_tp_active_activate_budget_impl ( EE_as_tp_budget_type budget_type,
  EE_UTID ref_object_id, EE_TYPEBOOL start_first );

void EE_as_tp_active_stop_budget_impl ( EE_as_tp_budget_type budget_type,
  EE_UTID ref_object_id, EE_TYPEBOOL start_first );

/** Called inside OS services primitives to update budget and "pause" the
    protection. We need to do this because some timing protection implementation
    could rely in hardware that can "interrupt" kernel and as we have to call
    the ProtectionHook, in case of budget expiration, that "terminate stuff"
    (TASKs/ISRs or Applications) we have to assure kernel consistency.

    N.B: another budget update is done at the end of the primitive so no ticks
         are given for free to a TASK/ISR2 budget */
__INLINE__ void __ALWAYS_INLINE__
  EE_as_tp_active_pause_and_update_budgets ( void )
{
  if ( EE_as_tp_active.active_tp != INVALID_TIMING_PROTECTION ) {
    EE_as_tp_active_pause_and_update_budgets_impl();
  }
}

/** Update the budgets plus restart the protection. Handle budget expiration too
    if it happens */
__INLINE__ void __ALWAYS_INLINE__
  EE_as_tp_active_update_budgets_and_restart ( void )
{
  if ( EE_as_tp_active.active_tp != INVALID_TIMING_PROTECTION ) {
      EE_as_tp_active_update_budgets_and_restart_impl();
  }
}

/** This function update actual active budgets, active a new budget for
    the active TP, after that reassess the first expiring budget */
__INLINE__ void __ALWAYS_INLINE__
  EE_as_tp_active_activate_budget ( EE_as_tp_budget_type budget_type,
    EE_UTID ref_object_id, EE_TYPEBOOL start_first )
{
  if ( EE_as_tp_active.active_tp != INVALID_TIMING_PROTECTION ) {
    EE_as_tp_active_activate_budget_impl(budget_type, ref_object_id,
      start_first);
  }
}

/** Stop an active budget and if the budget stopped is the actual first
    expiring budget reassess the new first expiring */
__INLINE__ void __ALWAYS_INLINE__ EE_as_tp_active_stop_budget (
  EE_as_tp_budget_type budget_type, EE_UTID ref_object_id, EE_TYPEBOOL
    start_first )
{
  if ( EE_as_tp_active.active_tp != INVALID_TIMING_PROTECTION ) {
    EE_as_tp_active_stop_budget_impl(budget_type, ref_object_id, start_first);
  }
}

/* [OS064]: If a task's OsTaskExecutionBudget is reached then the Operating
    System module shall call the ProtectionHook() with E_OS_PROTECTION_TIME. */
/* [OS210]: If a Category 2 ISRs OsIsrExecutionBudget is reached then the
    Operating System module shall call the ProtectionHook() with
    E_OS_PROTECTION_TIME. */
/* [OS033]: If a Task/Category 2 ISR holds an OSEK Resource and exceeds the
    Os[Task|Isr]ResourceLockBudget, the Operating System module shall call the
    ProtectionHook() with E_OS_PROTECTION_LOCKED.
    (BSW11008, BSW11013, BSW11014) */
/* [OS037]:If a Task/Category 2 ISR disables interrupts
    (via Suspend/Disable|All/OS|Interrupts()) and exceeds the configured
    Os[Task|Isr][All|OS]InterruptLockBudget, the Operating System module shall
    call the ProtectionHook() with E_OS_PROTECTION_LOCKED.
    (BSW11008, BSW11013, BSW11014) */
void EE_as_tp_active_budget_expired ( void );

#else /* EE_TIMING_PROTECTION__ */
/* void placeholders */
#define EE_as_tp_active_set(tp_id)                                    ((void)0)
#define EE_as_tp_active_set_from_TASK(task_id)                        ((void)0)
#define EE_as_tp_active_set_from_id_with_restart(tp_id)               ((void)0)
#define EE_as_tp_active_start_on_TASK_stacking(task_id)               ((void)0)
#define EE_as_tp_active_start_for_ISR2(isr2_id)                       ((void)0)
#define EE_as_tp_active_stop()                                        ((void)0)
#define EE_as_tp_active_start_idle()                                  ((void)0)
#define EE_as_tp_active_reset_budgets()                               ((void)0)
#define EE_as_tp_active_pause_and_update_budgets()                    ((void)0)
#define EE_as_tp_active_update_budgets_and_restart()                  ((void)0)
#define EE_as_tp_active_activate_budget(b_type, obj_id, start_first)  ((void)0)
#define EE_as_tp_active_stop_budget(b_type, obj_id, start_first)      ((void)0)
#define EE_as_tp_active_budget_expired()                              ((void)0)
#define EE_as_tp_stop_budget(tp_ram_ref, budget_id)                   ((void)0)
#define EE_as_tp_reset_budgets(tp_id)                                 ((void)0)
#define EE_as_tp_stop_interarrival_frame(tp_id)                       ((void)0)
#define EE_as_tp_handle_interarrival(tp_id)                           EE_TRUE
#define EE_hal_tp_stop()                                              ((void)0)
#endif /* EE_TIMING_PROTECTION__ */

#endif /* INCLUDE_EE_KERNEL_AS_TIMING_PROT__ */

