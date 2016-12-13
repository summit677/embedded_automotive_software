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
 * Author: 2002-2004 Paolo Gai
 * CVS: $Id: ee_startos.c,v 1.5 2006/12/03 22:07:50 pj Exp $
 */

#include "ee_internal.h"

/*
 * ORTI variables
 */
#if defined(RTDRUID_CONFIGURATOR_NUMBER) \
 && (RTDRUID_CONFIGURATOR_NUMBER >= RTDRUID_CONFNUM_NO_ORTI_VARS)

#ifdef __OO_ORTI_SERVICETRACE__
/* This shall belong to ee_api_bss the section mapping is done inside header
   file to be portable to DIAB compiler */
volatile EE_UINT8 EE_ORTI_servicetrace;
#endif /* __OO_ORTI_SERVICETRACE__ */

#ifdef __OO_ORTI_LASTERROR__
StatusType EE_ORTI_lasterror;
#endif /* __OO_ORTI_LASTERROR__ */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* Put the following code in ee_kernel_text */
#define OS_START_SEC_CODE
/* Put the following variables in ee_kernel_bss, needed for EE_oo_start_os */
#define OS_START_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#ifdef __OO_ORTI_PRIORITY__
EE_TYPEPRIO EE_ORTI_th_priority[EE_MAX_TASK];
#endif /* __OO_ORTI_PRIORITY__ */

#endif /* RTDRUID_CONFIGURATOR_NUMBER */

#ifdef __OO_ORTI_RUNNINGISR2__
volatile EE_ORTI_runningisr2_type EE_ORTI_runningisr2;
#endif /* __OO_ORTI_RUNNINGISR2__ */

/* StartOS

  - called to start the operating system in a specific Application
    mode
  - it does not need to return to the caller
*/
#ifndef __PRIVATE_STARTOS__

#ifdef __OO_AUTOSTART_TASK__
/*
 * MISRA NOTE: This function is a workaround to provide
 * the capability to access task id array as a real array
 * although it is declared as pointer.
 * This measure prevents from misra error:
 * "pointer arithmetic other than array indexing used".
 * Furthermore, in order to prevent from the following error:
 * Possible use of null pointer 'task_id_vec' of type 'const EE_TID *'
 * a check on task_id_vec has been added
 */
static EE_TID compute_task_tid(const EE_TID task_id_vec[], EE_UREG t)
{
  EE_TID res = EE_NIL;

  if ( task_id_vec != NULL ) {
    res = task_id_vec[t];
  }

  return res;
}

static void EE_oo_autostart_tasks(AppModeType Mode)
{
  register EE_UREG n, t;
  n = EE_oo_autostart_task_data[Mode].n;
  for ( t = 0U; t < n; t++ ) {
    /* Insert the TID in Ready Queue */
    EE_TID tid = compute_task_tid(EE_oo_autostart_task_data[Mode].task, t);
    EE_oo_task_in_ready_queue(tid);
  }
}
#else /* __OO_AUTOSTART_TASK__ */
#define EE_oo_autostart_tasks(Mode)     ((void)0)
#endif /* __OO_AUTOSTART_TASK__ */

#ifdef __OO_AUTOSTART_ALARM__
/*
 * MISRA NOTE: This function is a workaround to provide
 * the capability to access alarm id array as a real array
 * although it is declared as pointer.
 * This measure prevents from Misra error:
 * "pointer arithmetic other than array indexing used"
 * please note that this function is very similiar to
 * compute_task_tid but the first argument is a vector
 * and Misra does not allow the cast from EE_TYPEALARM*
 * to EE_TID* and viceversa. Therefore the two function,
 * although similar need to be kept separated.
 * Furthermore, in order to prevent from the following error:
 * Possible use of null pointer 'alarm_id_vec' of type 'const EE_TID *'
 * a check on alarm_id_vec has been added
 */
static EE_TYPEALARM compute_alarm_id(const EE_TYPEALARM alarm_id_vec[],
  EE_UREG t)
{
  EE_TYPEALARM res = (EE_TYPEALARM)EE_NIL;

  if ( alarm_id_vec != NULL ) {
    res = alarm_id_vec[t];
  }

  return res;
}

static void  EE_oo_autostart_alarms(AppModeType Mode)
{
  register EE_UREG n, t;
  n = EE_oo_autostart_alarm_data[Mode].n;
  for ( t = 0U; t < n; t++ ) {
    EE_TYPEALARM alarm_temp = compute_alarm_id(
      EE_oo_autostart_alarm_data[Mode].alarm, t);
    EE_oo_handle_rel_counter_object_insertion(alarm_temp,
      EE_oo_autostart_alarm_increment[alarm_temp],
      EE_oo_autostart_alarm_cycle[alarm_temp]);
  }
}
#else   /* __OO_AUTOSTART_ALARM__ */
#define EE_oo_autostart_alarms(Mode)    ((void)0)
#endif  /* __OO_AUTOSTART_ALARM__ */

#ifdef EE_AS_AUTOSTART_SCHEDULETABLE__

static const EE_as_schedule_table_autostart_data *
  get_schedule_table_autostart_data ( const EE_as_schedule_table_autostart_data
    scheduletable_data[], EE_UREG t )
{
  const EE_as_schedule_table_autostart_data * res = NULL;

  if ( scheduletable_data != NULL ) {
    res = &scheduletable_data[t];
  }

  return res;
}

static void  EE_oo_autostart_schedule_tables ( AppModeType Mode )
{
  register EE_UREG n, t;
  n = EE_as_schedule_table_autostart[Mode].n;
  for ( t = 0U; t<n; t++ ) {
    const EE_as_schedule_table_autostart_data * sched_data_ref =
      get_schedule_table_autostart_data(EE_as_schedule_table_autostart[Mode].
        autostart_schedule_tables, t);
    if ( sched_data_ref != NULL ) {
      switch ( sched_data_ref->start_kind ) {
        case EE_ST_START_ABSOLUTE:
          EE_as_StartScheduleTableAbs(sched_data_ref->scheduletable_id,
            sched_data_ref->start_value);
          break;
        case EE_ST_START_RELATIVE:
          EE_as_StartScheduleTableRel(sched_data_ref->scheduletable_id,
            sched_data_ref->start_value);
          break;
        case EE_ST_START_SYNCHRON:
          /* XXX: Not implement yet */
          /* EE_as_StatusType StartScheduleTableSynchron(sched_data_ref->
              scheduletable_id) */
          break;
        default:
          /* THIS CANNOT HAPPENS, CONFIGURATION ERROR */
          break;
      }
    }
  }
}
#else /* EE_AS_AUTOSTART_SCHEDULETABLE__ */
#define EE_oo_autostart_schedule_tables(Mode)   ((void)0)
#endif /* EE_AS_AUTOSTART_SCHEDULETABLE__ */

#if defined(__OO_AUTOSTART_TASK__) || defined(__OO_AUTOSTART_ALARM__) ||\
  defined(EE_AS_AUTOSTART_SCHEDULETABLE__)
static void EE_oo_autostart_os ( AppModeType Mode )
{
  if ( Mode < EE_MAX_APPMODE ) {
    EE_oo_autostart_tasks(Mode);
    EE_oo_autostart_alarms(Mode);
    EE_oo_autostart_schedule_tables(Mode);
  }
}
#else /* __OO_AUTOSTART_TASK__ || __OO_AUTOSTART_ALARM__ ||\
  EE_AS_AUTOSTART_SCHEDULETABLE__ */
#define EE_oo_autostart_os(Mode)    ((void)0);
#endif /* __OO_AUTOSTART_TASK__ || __OO_AUTOSTART_ALARM__ ||\
  EE_AS_AUTOSTART_SCHEDULETABLE__ */

#ifdef __OO_HAS_STARTUPHOOK__
static void EE_oo_call_StartupHook(void)
{
  StartupHook();
}
#else /*  __OO_HAS_STARTUPHOOK__ */
#define EE_oo_call_StartupHook()    ((void)0)
#endif /* __OO_HAS_STARTUPHOOK__ */

/** @brief Flag that the OS is started */
EE_UREG volatile EE_oo_started;

#if defined(__OO_STARTOS_OLD__)
/*
 * If __OO_STARTOS_OLD__ is defined, the StartOS() returns,
 * (this is the old behaviour before the Autosar compliance).
 */
#define EE_oo_start_os()    ((void)0)
#else /* __OO_STARTOS_OLD__ */
/*
 * If __OO_STARTOS_OLD__ is not defined the system behaves 
 * like Autosar requires: infinite loop (do not return).
 * This is the standard solution.
 */
static void EE_oo_start_os(void)
{
  /*
   * This assignment prevents from MISRA 14.2/FlexeLint 522:
   * lacks side-effects
   */
  EE_oo_started = 1U;

  for(;;) {
    ;
  }
}
#endif /* __OO_STARTOS_OLD__ && !__MSRP__ */

StatusType EE_oo_StartOS(AppModeType Mode)
{
#if defined(__MSRP__) && (!defined(EE_AS_MULTICORE_NO_SYNC))
  /* The following contains cores application modes */
  extern AppModeType volatile EE_as_os_application_mode[];
  /* Mask for Autosar cores started */
  extern EE_UREG volatile EE_as_core_mask;
  /* Index used to cycle on cores an OS-Applications */
  register  EE_UREG i;
  /* Hold the value of application mode to be checked */
  register  AppModeType mode_to_check = DONOTCARE;
#endif /* __MSRP__ && !EE_AS_MULTICORE_NO_SYNC */
  /* Ready Queue Head Index */
  register EE_TID     rq;
  /* Error Value */
  register StatusType ev = E_OK;

  EE_ORTI_set_service_in(EE_SERVICETRACE_STARTOS);

  /* Check if this is the first time that I call StartOS */
  if ( EE_oo_started != 0U ) {
    ev = E_OS_STATE;
  } else {
#ifdef __OO_CPU_HAS_STARTOS_ROUTINE__
    /* the CPU initialization can return an error; 0 if all ok */
    if ( EE_cpu_startos() ) {
      ev = E_OS_SYS_INIT;
    } else
#endif /* __OO_CPU_HAS_STARTOS_ROUTINE__ */
    {
      /* Primitive Lock Procedure */
      EE_hal_disableIRQ();
      /* Initialize ORTI variables, so the debugger can see their
         initial value */
      EE_ORTI_set_runningisr2((EE_ORTI_runningisr2_type)NULL);

      /* Multicore Startup */
#if defined(__MSRP__) && (!defined(EE_AS_MULTICORE_NO_SYNC))
      /* [OS609]: If StartOS is called with the AppMode "DONOTCARE" the
          application mode of the other core(s) (differing from "DONOTCARE")
          shall be used. (BSW4080006) */
      /* [OS610]: At least one core shall define an AppMode other than
          "DONOTCARE". (BSW4080006) */

      /* It is not allowed to call StartOS on Cores that are not started by
          StartCore AUTOSAR API.
          "In Multi-Core configurations, each slave core that is used by AUTOSAR
          MUST be activated before StartOS is entered on the core".
        XXX: The condition that a core enter in StartOS even if the core has not
          been started by a call to StartCore AUTOSAR API is
          "unspecified behavior" (See Paragraph 7.9.6 Cores which are not
          controlled by the AUTOSAR OS on AUSTOSAR specification
          (v 4.0 rev 3.0)).
          So I decide to handle it jumping in an endless loop. */
/* TODO: maybe we need a special macro to identify the master core */
#if defined(EE_CURRENTCPU) && (EE_CURRENTCPU != 0)
      if( ((EE_as_core_mask & ((EE_UREG)1U << EE_CURRENTCPU)) == 0U) )
      {
        /* Enter in an endless loop if it happened */
        EE_oo_start_os();
      }
#endif /* EE_CURRENTCPU != 0 */

      /* Set on current CPU position the actual application mode */
      EE_as_os_application_mode[EE_CURRENTCPU] = Mode;

      /* [OS580]: All cores that belong to the AUTOSAR system shall be
          synchronized within the StartOS before the global StartupHook is
          called. (BSW4080006) */
      /* [OS581]: The global StartupHook shall be called on all cores
          immediately after the first synchronization point. (BSW4080006) */

      /* Synchronize Cores in Startup. Extracted from AUSTOSAR 4.0 rev 3
          Specification paragraph 7.9.4 Multi-Core start-up concept:
          "This release of the AUTOSAR specification does not support timeouts
          during the synchronization phase. Cores that are activated with
          StartCore but do not call StartOS may cause the system to hang.
          It is in the responsibility of the integrator to avoid such
          behavior." */
      EE_hal_sync_barrier(&EE_startos_before_hook_barrier, &EE_as_core_mask);

      /* [OS608]: If more than one core calls StartOS with an AppMode other than
          "DONOTCARE", the AppModes shall be the same. StartOS shall check this
          at the first synchronization point. In case of violation, StartOS
          shall not start the scheduling, shall not call any StartupHooks,
          and shall enter an endless loop on every core. (BSW4080006) */

      for ( i = 0U; i < (EE_UREG)EE_MAX_CPU; ++i )
      {
        register AppModeType current_mode = EE_as_os_application_mode[i];
        if ( current_mode != DONOTCARE )
        {
          if ( mode_to_check == DONOTCARE ) {
              mode_to_check = current_mode;
          } else if ( mode_to_check != current_mode ) {
            /* Error condition specified by OS608 requirement: enter in an
                endless loop */
            EE_oo_start_os();
          } else {
            /* Empty else statement to comply with MISRA 14.10 */
          }
        }
      }

      if ( mode_to_check != DONOTCARE ) {
        /* Set mode for this core as mode_to_check */
        Mode = mode_to_check;
      } else {
        /* XXX: It is not specified how to handle the condition that no cores
           defines an AppMode different from DONOTCARE. I choose to handle it
           using OSDEFAULTAPPMODE */
        Mode = OSDEFAULTAPPMODE;
      }
#endif /* __MSRP__ && !EE_AS_MULTICORE_NO_SYNC */

      /* Set EE_ApplicationMode for this core */
      EE_ApplicationMode = Mode;

      /* Set the StartOS flag after all error checks and before return to user
         code in StartupHook */
      EE_oo_started = 1U;

#if defined(__OO_HAS_STARTUPHOOK__) || defined(__OO_AUTOSTART_TASK__) || \
  defined(__OO_AUTOSTART_ALARM__) || defined(EE_AS_AUTOSTART_SCHEDULETABLE__)

      /* Set the context execution at StartupHook */
      EE_as_set_execution_context( StartupHook_Context );
      EE_oo_call_StartupHook();
      /* Set the context execution at Kernel */
      EE_as_set_execution_context( Kernel_Context );

      EE_oo_autostart_os(Mode);
#endif /* __OO_HAS_STARTUPHOOK__ || __OO_AUTOSTART_TASK__ ||
          __OO_AUTOSTART_ALARM__ || EE_AS_AUTOSTART_SCHEDULETABLE__ */

#ifdef EE_AS_OSAPPLICATIONS__
      /* [OS582]: The OS-Application-specific StartupHooks shall be called
          after the global StartupHook but only on the cores to which the
          OS-Application is bound. (BSW4080006, BSW4080008) */
      {
        register  ApplicationType i;
        /* Set the context execution at StartupHook Context */
        EE_as_set_execution_context( StartupHook_Context );
        /* ApplID 0 is always reserved by the Kernel */
        for ( i = 1U; i < EE_MAX_APP; ++i ) {
          register EE_HOOKTYPE startup_hook = EE_as_Application_startuphook[i];
          if ( startup_hook != 0U ) {
            /* [OS226] The Operating System module shall execute an
               application-specific startup hook with the access rights of the
               associated OS-Application. */
            EE_hal_call_app_hook(startup_hook, i);
          }
        }
        /* Set the context execution at Kernel */
        EE_as_set_execution_context( Kernel_Context );
      }
#endif /* EE_AS_OSAPPLICATIONS__ */

#if defined(__MSRP__) && (!defined(EE_AS_MULTICORE_NO_SYNC))
      /* [OS579]: All cores that belong to the AUTOSAR system shall be
          synchronized within the StartOS function before the scheduling is
          started and after the global StartupHook is called.
          (BSW4080001, BSW4080006) */
      EE_hal_sync_barrier(&EE_startos_before_scheduling_barrier,
        &EE_as_core_mask);
#endif /* __MSRP__ && !EE_AS_MULTICORE_NO_SYNC */

      /* Check if there is a preemption.
          This code is optimized for this case, but for code readability we
          could have been used EE_oo_preemption_point too. */
      rq = EE_rq_queryfirst();
      if ( rq != EE_NIL ) {
        /* Get the internal resource */
        EE_sys_ceiling |= EE_th_dispatch_prio[rq];
        /* Put the task in running state */
        EE_th_status[rq] = RUNNING;

        EE_ORTI_set_th_eq_dispatch_prio(rq);

        /* [SWS_Os_00469]: The Operating System module shall start an
            OsTaskTimeFrame when a task is activated successfully.
            (SRS_Os_11008) */
        /* Enable the inter-arrival for the stacking TASK */
        (void)EE_as_tp_handle_interarrival(EE_AS_TP_ID_FROM_TASK(rq));

        /* "Press TP start for the first time" for this new activation of the
            TASK */
        EE_as_tp_active_start_on_TASK_stacking(rq);

        /* Since we are into the StartOS, the task was NOT previously on
          the stack... (we do not have to check the was stacked field)
          So the code is equal for basic and extended task
          (all classes: BCC1, BCC2, ECC1, ECC2 are equal here)
          Look at EE_oo_run_next_task in ee_internal.h to see the usual
          differences. */
        EE_hal_ready2stacked(EE_rq2stk_exchange());
      } else {
        /* Prepare to go in IDLE Loop */
        EE_as_set_execution_context(Idle_Context);
        EE_as_tp_active_start_idle();
      }

      EE_ORTI_set_service_out(EE_SERVICETRACE_STARTOS);
      /* In Case of StartOS first call that return (this means:
        __OO_STARTOS_OLD__ enabled) I have to enable IRQ. */
      /* In any case enable IRQ before endless IDLE loop */
      EE_hal_enableIRQ();
      /* If __OO_STARTOS_OLD__ is defined -> return, otherwise endless cycle. */
      EE_oo_start_os();
    }
  }

  if ( ev != E_OK ) {
    register EE_FREG flag = EE_hal_begin_nested_primitive();
    EE_ORTI_set_lasterror(ev);
    EE_oo_notify_error_StartOS(Mode, ev);
    EE_ORTI_set_service_out(EE_SERVICETRACE_STARTOS);
    EE_hal_end_nested_primitive(flag);
  }

  return ev;
}

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* Put the following code in ee_kernel_text */
#define OS_STOP_SEC_CODE
/* Put the following variables in ee_kernel_bss */
#define OS_STOP_SEC_VAR_NOINIT
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* __PRIVATE_STARTOS__ */

