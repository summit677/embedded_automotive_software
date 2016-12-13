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

 /** @file      ee_as_rpc.c
  *  @brief     Implementation of Synchronous Remote Procedure Call (RPC) to
  *             execute kernel primitives in other cores. This shares some
  *             requirements with RN (Remote Notification) Kernel Extension, but
  *             unlike that, take advantage of the fact that Autosar
  *             Specification explicitly states synchronous behavior.
  *  @author    Errico Guidieri
  *  @date      2012
  */

#include "ee_internal.h"
#include "ee_api.h"

#ifdef EE_MASTER_CPU

/** @brief Flag that a core is serving a RPCs */
EE_BIT volatile EE_SHARED_UDATA EE_as_rpc_serving[EE_MAX_CPU];

/** @brief Flag used to signal that ShutdownAllCores have been called */
EE_BIT volatile EE_SHARED_UDATA EE_as_shutdown_all_cores_flag;

/** @brief Used to pass ShutdownAllCores error parameter to other cores */
StatusType volatile EE_SHARED_UDATA EE_as_shutdown_all_cores_error;

/** @brief Mask used in shutdown all cores procedure for synchronization */
EE_UREG volatile EE_SHARED_IDATA
  EE_as_shutdown_mask = ( (EE_UREG)1U << EE_MAX_CPU ) - 1U;

#ifdef __EE_MEMORY_PROTECTION__
/** @brief array used to crossing memory protection between cores when call a
    service with param2 as some kind of reference */
EE_as_rpc_outparam EE_SHARED_UDATA EE_as_rpc_out_param2[EE_MAX_CPU];
/** @brief array used to crossing memory protection between cores when call a
    service with param3 as some kind of reference */
EE_as_rpc_outparam EE_SHARED_UDATA EE_as_rpc_out_param3[EE_MAX_CPU];
#endif /* __EE_MEMORY_PROTECTION__ */

#endif /* EE_MASTER_CPU */

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* Put the following code in ee_kernel_text */
#define OS_START_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

static EE_UREG EE_as_get_service_index_and_limit(
  OSServiceIdType ServiceId, EE_os_param_id * limit_id_ref)
{
  EE_UREG service_index;
  switch( ServiceId )
  {
    case OSServiceId_ActivateTask:
    case OSServiceId_ChainTask:
    case OSServiceId_GetTaskState:
    case OSServiceId_SetEvent:
      service_index = 0U;
      if (limit_id_ref != NULL)
      {
        *limit_id_ref = (EE_os_param_id)EE_AS_RPC_TASKS_SIZE;
      }
      break;
#ifndef __OO_NO_ALARMS__
    case OSServiceId_GetAlarmBase:
    case OSServiceId_GetAlarm:
    case OSServiceId_SetRelAlarm:
    case OSServiceId_SetAbsAlarm:
    case OSServiceId_CancelAlarm:
      service_index = 1U;
      if (limit_id_ref != NULL)
      {
        *limit_id_ref = EE_AS_RPC_ALARMS_SIZE;
      }
      break;
    case OSServiceId_GetCounterValue:
    case OSServiceId_GetElapsedValue:
      service_index = 2U;
      if (limit_id_ref != NULL)
      {
        *limit_id_ref = EE_AS_RPC_COUNTERS_SIZE;
      }
      break;
#endif /* !__OO_NO_ALARMS__ */
#ifdef EE_AS_SCHEDULETABLES__
    case OSServiceId_StartScheduleTableRel:
    case OSServiceId_StartScheduleTableAbs:
    case OSServiceId_StopScheduleTable:
    case OSServiceId_GetScheduleTableStatus:
      service_index = 3U;
      if (limit_id_ref != NULL)
      {
        *limit_id_ref = EE_AS_RPC_SCHEDTABS_SIZE;
      }
      break;
#endif /* EE_AS_SCHEDULETABLES__ */
#ifdef EE_AS_OSAPPLICATIONS__
    case OSServiceId_GetApplicationState:
    case OSServiceId_TerminateApplication:
      service_index = 4U;
      if (limit_id_ref != NULL)
      {
        *limit_id_ref = EE_AS_RPC_OSAPPLS_SIZE;
      }
      break;
#endif /* EE_AS_OSAPPLICATIONS__ */
#ifdef EE_AS_IOC__
/*                  Inter OSApplication Communication (IOC)                   */
    case OSServiceId_IOCService:
      service_index = 5U;
      if (limit_id_ref != NULL)
      {
        *limit_id_ref = ((EE_os_param_id)-1); /* TODO EG: Check This better !!! */
      }
      break;
#endif /* EE_AS_IOC__ */
    default:
      /* Wrong Service ID return a Error Value */
      service_index = EE_UREG_MINUS1;
      if (limit_id_ref != NULL)
      {
        *limit_id_ref = 0U;
      }
      break;
  }

  return service_index;
}

#ifdef __EE_MEMORY_PROTECTION__

static void EE_as_rpc_conf_call_params( EE_TYPEASRPC  volatile  * rpc_ref,
  OSServiceIdType ServiceId, EE_os_param_id param1_id, EE_os_param param2,
  EE_os_param param3 )
{
  rpc_ref->remote_procedure   = ServiceId;
  rpc_ref->param1.value_param = param1_id;

  switch ( ServiceId )
  {
    case OSServiceId_GetTaskState:
      rpc_ref->param2.task_state_ref = &EE_as_rpc_out_param2[EE_CURRENTCPU].
        task_state;
      rpc_ref->param3 = param3;
      break;
#ifndef __OO_NO_ALARMS__
    case OSServiceId_GetAlarmBase:
      rpc_ref->param2.alarm_base_ref = &EE_as_rpc_out_param2[EE_CURRENTCPU].
        alarm_base;
      rpc_ref->param3 = param3;
      break;
    case OSServiceId_GetAlarm:
      rpc_ref->param2.tick_ref = &EE_as_rpc_out_param2[EE_CURRENTCPU].tick;
      rpc_ref->param3 = param3;
      break;
    case OSServiceId_GetCounterValue:
      rpc_ref->param2.tick_ref = &EE_as_rpc_out_param2[EE_CURRENTCPU].tick;
      rpc_ref->param3 = param3;
      break;
    case OSServiceId_GetElapsedValue:
      rpc_ref->param2.tick_ref = &EE_as_rpc_out_param2[EE_CURRENTCPU].tick;
      rpc_ref->param3.tick_ref = &EE_as_rpc_out_param3[EE_CURRENTCPU].tick;
      break;
#endif /* !__OO_NO_ALARMS__ */
#ifdef EE_AS_SCHEDULETABLES__
    case OSServiceId_GetScheduleTableStatus:
      rpc_ref->param2.schedule_table_status_ref =
        &EE_as_rpc_out_param2[EE_CURRENTCPU].schedule_table_status;
      rpc_ref->param3 = param3;
      break;
#endif /* EE_AS_SCHEDULETABLES__ */
#ifdef EE_AS_OSAPPLICATIONS__
    case OSServiceId_GetApplicationState:
      rpc_ref->param2.application_state_ref =
        &EE_as_rpc_out_param2[EE_CURRENTCPU].application_state;
      rpc_ref->param3 = param3;
#endif /* EE_AS_OSAPPLICATIONS__ */
      break;
    default:
      rpc_ref->param2 = param2;
      rpc_ref->param3 = param3;
      break;
  }
}

static void EE_as_rpc_get_inout_params ( OSServiceIdType ServiceId,
  EE_os_param * param2_ref, EE_os_param * param3_ref )
{
  switch ( ServiceId )
  {
    case OSServiceId_GetTaskState:
      *(param2_ref->task_state_ref) = EE_as_rpc_out_param2[EE_CURRENTCPU].
        task_state;
      break;
#ifndef __OO_NO_ALARMS__
    case OSServiceId_GetAlarmBase:
      *(param2_ref->alarm_base_ref) = EE_as_rpc_out_param2[EE_CURRENTCPU].
        alarm_base;
      break;
    case OSServiceId_GetAlarm:
      *(param2_ref->tick_ref) = EE_as_rpc_out_param2[EE_CURRENTCPU].tick;
      break;
    case OSServiceId_GetCounterValue:
      *(param2_ref->tick_ref) = EE_as_rpc_out_param2[EE_CURRENTCPU].tick;
      break;
    case OSServiceId_GetElapsedValue:
      *(param2_ref->tick_ref) = EE_as_rpc_out_param2[EE_CURRENTCPU].tick;
      *(param3_ref->tick_ref) = EE_as_rpc_out_param3[EE_CURRENTCPU].tick;
      break;
#endif /* !__OO_NO_ALARMS__ */
#ifdef EE_AS_SCHEDULETABLES__
    case OSServiceId_GetScheduleTableStatus:
      *(param2_ref->schedule_table_status_ref) =
        EE_as_rpc_out_param2[EE_CURRENTCPU].schedule_table_status;
      break;
#endif /* EE_AS_SCHEDULETABLES__ */
#ifdef EE_AS_OSAPPLICATIONS__
    case OSServiceId_GetApplicationState:
      *(param2_ref->application_state_ref) =
        EE_as_rpc_out_param2[EE_CURRENTCPU].application_state;
      break;
#endif /* EE_AS_OSAPPLICATIONS__ */
    default:
      break;
  }
}

#else /* __EE_MEMORY_PROTECTION__ */

__INLINE__ void __ALWAYS_INLINE__ EE_as_rpc_conf_call_params(
  EE_TYPEASRPC  volatile  * rpc_ref, OSServiceIdType ServiceId,
    EE_os_param_id param1_id, EE_os_param param2, EE_os_param param3 )
{
  if (rpc_ref != NULL)
  {
    rpc_ref->remote_procedure   = ServiceId;
    rpc_ref->param1.value_param = param1_id;
    rpc_ref->param2             = param2;
    rpc_ref->param3             = param3;
  }
}
/* Nothing to do in case of no memory protection */
#define EE_as_rpc_get_inout_params(ServiceId, param2_ref, param3_ref) ((void)0)
#endif /* __EE_MEMORY_PROTECTION__ */

/* Called Inside a Primitive: no need to handle Interrupts */
StatusType EE_as_rpc( OSServiceIdType ServiceId, EE_os_param param1,
  EE_os_param param2, EE_os_param param3 )
{
  /* Uninitialized locals */
  register  EE_TYPEASRPC  volatile  * rpc_ref;
  register  EE_TYPEASREMOTEIDCONSTREF r_id_ref;
  register  StatusType                ret_value;
  register  CoreIdType                rp_cpu;
  EE_os_param_id                      limit_id = INVALID_OBJECTID;
  /* Initialized locals */
  register  EE_UREG const service_index =
    EE_as_get_service_index_and_limit(ServiceId, &limit_id);

  /* [OS589]: All functions that are not allowed to operate cross core shall
      return E_OS_CORE in extended status if called with parameters that require
      a cross core operation. (BSW4080013) */
  if ( (service_index == EE_UREG_MINUS1) ||
       (EE_as_rpc_services_table[service_index] == NULL) )
  {
    /* This condition correspond to a kernel bug: check the configuration */
    ret_value = E_OS_CORE;
  } else if ( param1.value_param > limit_id ) {
    /* Check if object id of the service is valid */
    /* This condition correspond to an error in service caller site */
    ret_value = E_OS_ID;
  } else {

    /* Get the Remote Id Structure */
    r_id_ref = &EE_as_rpc_services_table[service_index][param1.value_param];
    rp_cpu = r_id_ref->core_id;

    /* Check if the requiring core is different from actual core */
    if ( rp_cpu == EE_CURRENTCPU )
    {
      /* This condition correspond to an error in service caller site */
      ret_value = E_OS_ID;
    } else
#ifdef EE_SERVICE_PROTECTION__
    /* Check for service access right */
    if ( (r_id_ref->core0_index != EE_UREG_MINUS1) &&
      ((EE_as_rpc_remote_access_rules[r_id_ref->core0_index + EE_CURRENTCPU] &
        EE_APP_TO_MASK(EE_as_active_app)) == 0) )
    {
      ret_value = E_OS_ACCESS;
    } else
#ifdef EE_AS_OSAPPLICATIONS__
    if ( (ServiceId == OSServiceId_TerminateApplication) &&
      (EE_as_Application_ROM[EE_as_active_app].Mode == EE_MEMPROT_USR_MODE) )
    {
      ret_value = E_OS_ACCESS;
    } else
#endif /* EE_AS_OSAPPLICATIONS__ */
#endif /* EE_SERVICE_PROTECTION__ */
    {
      /* Select the Right RPC RAM Structure to be populated */
      rpc_ref = &EE_as_rpc_RAM[EE_CURRENTCPU];

      /* Configure RPC parameters */
      EE_as_rpc_conf_call_params(rpc_ref, ServiceId, r_id_ref->param_id, param2,
        param3);

      {
        /* Initialized locals */
        /* Select the Right Spinlock Where Synchronize on */
        register  EE_TYPESPIN spinlock_id = EE_as_core_spinlocks[rp_cpu];

        /* Enter RPC Critical Section */
        EE_hal_spin_in(spinlock_id);
        /* Configure an Active RPC and, if needed, signal it */
        rpc_ref->serving_core = rp_cpu;
        if ( EE_as_rpc_serving[rp_cpu] == 0U )
        {
          /* In any case we shall assure that EE_hal_IRQ_interprocessor can
             handle multiple sequential calls */
          EE_hal_IRQ_interprocessor((EE_UINT8)rp_cpu);
        }
        /* Exit RPC Critical Section */
        EE_hal_spin_out(spinlock_id);
      }

      while ( rpc_ref->serving_core == rp_cpu ) {
        ; /* Wait that remote call Terminate */
      }

      /* Move in-out parameters in Call domain in case of memory protection */
      EE_as_rpc_get_inout_params(ServiceId, &param2, &param3);
      ret_value = rpc_ref->error;
    }
  }
  return ret_value;
}

#ifndef __OO_NO_ALARMS__
static StatusType EE_as_rpc_execute( OSServiceIdType ServiceId, EE_os_param
  param1, EE_os_param param2, EE_os_param param3 )
#else
static StatusType EE_as_rpc_execute( OSServiceIdType ServiceId, EE_os_param
  param1, EE_os_param param2 )
#endif /* __OO_NO_ALARMS__ */
{
  register StatusType ret_value;

#ifdef __OO_HAS_ERRORHOOK__
  /* The ErrorHook have to be called in requiring core so I set error flag to
     prevent the call inside the handling core */
  EE_ErrorHook_nested_flag = 1U;
#endif /* __OO_HAS_ERRORHOOK__ */

  switch( ServiceId )
  {
    case OSServiceId_ActivateTask:
    case OSServiceId_ChainTask:
      ret_value = EE_oo_ActivateTask((TaskType)param1.value_param);
      break;
    case OSServiceId_GetTaskState:
      ret_value = EE_oo_GetTaskState((TaskType)param1.value_param, param2.task_state_ref);
      break;
#if defined(__OO_ECC1__) || defined(__OO_ECC2__)
    case OSServiceId_SetEvent:
      ret_value = EE_oo_SetEvent((TaskType)param1.value_param, (EventMaskType)param2.
        value_param);
      break;
#endif /* __OO_ECC1__ || __OO_ECC2__ */
#ifndef __OO_NO_ALARMS__
    case OSServiceId_GetAlarmBase:
      ret_value = EE_oo_GetAlarmBase((AlarmType)param1.value_param,
        param2.alarm_base_ref);
      break;
    case OSServiceId_GetAlarm:
      ret_value = EE_oo_GetAlarm((AlarmType)param1.value_param,
        param2.tick_ref);
      break;
    case OSServiceId_SetRelAlarm:
      ret_value = EE_oo_SetRelAlarm((AlarmType)param1.value_param,
        (TickType)param2.value_param, (TickType)param3.value_param);
      break;
    case OSServiceId_SetAbsAlarm:
      ret_value = EE_oo_SetAbsAlarm((AlarmType)param1.value_param,
        (TickType)param2.value_param, (TickType)param3.value_param);
      break;
    case OSServiceId_CancelAlarm:
      ret_value = EE_oo_CancelAlarm((AlarmType)param1.value_param);
      break;
    case OSServiceId_GetCounterValue:
      ret_value = EE_oo_GetCounterValue((CounterType)param1.value_param,
        param2.tick_ref);
      break;
    case OSServiceId_GetElapsedValue:
      ret_value = EE_oo_GetElapsedValue((CounterType)param1.value_param,
        param2.tick_ref, param3.tick_ref);
      break;
#endif /* __OO_NO_ALARMS__ */
#ifdef EE_AS_SCHEDULETABLES__
    case OSServiceId_StartScheduleTableRel:
      ret_value = EE_as_StartScheduleTableRel((ScheduleTableType)param1.
        value_param,  (TickType)param2.value_param);
      break;
    case OSServiceId_StartScheduleTableAbs:
      ret_value = EE_as_StartScheduleTableAbs((ScheduleTableType)param1.
        value_param, (TickType)param2.value_param);
      break;
    case OSServiceId_StopScheduleTable:
      ret_value = EE_as_StopScheduleTable((ScheduleTableType)param1.
        value_param);
      break;
    case OSServiceId_GetScheduleTableStatus:
    ret_value = EE_as_GetScheduleTableStatus((ScheduleTableType)param1.
      value_param, param2.schedule_table_status_ref);
      break;
#endif /* EE_AS_SCHEDULETABLES__ */
#ifdef EE_AS_OSAPPLICATIONS__
    case OSServiceId_GetApplicationState:
      ret_value = EE_as_GetApplicationState((ApplicationType)param1.value_param,
        param2.application_state_ref );
      break;
    case OSServiceId_TerminateApplication:
      ret_value = EE_as_TerminateApplication((ApplicationType)param1.
        value_param, (RestartType)param2.value_param);
      break;
#endif /* EE_AS_OSAPPLICATIONS__ */
    default:
      /* Wrong Service ID: this should never happens at this point */
      ret_value = E_OS_CORE;
      break;
  }

#ifdef __OO_HAS_ERRORHOOK__
  /* Restore initial condition: reset ErrorHook flag */
  EE_ErrorHook_nested_flag = 0U;
#endif /* __OO_HAS_ERRORHOOK__ */

  /* return error status */
  return ret_value;
}

#define EE_AS_RPC_INCREMENT_AND_HANDLE_WRAPAROUND(v) \
  ((v) = ((v) == (EE_UREG)(EE_MAX_CPU - 1))? 0U: ((v) + 1U))

void EE_as_rpc_handler( void ) {
  /* Uninitialized locals */
  register  EE_UREG                   prev_requiring_core_index;
  /* Constants */
  register  EE_FREG     const flag        = EE_hal_begin_nested_primitive();
  register  EE_TYPESPIN const spinlock_id = EE_as_core_spinlocks[EE_CURRENTCPU];
  /* Initialized locals */
  register  EE_TYPEASRPC  volatile  * rpc_ref = 0U;
  register  EE_UREG                   requiring_core_index =
    (EE_UREG)EE_CURRENTCPU;

  register  EE_BIT                    to_shutdown = 0U;
#if defined(EE_AS_IOC__) && defined(EE_AS_IOC_HAS_CALLBACKS__)
  register  EE_BIT                    to_IOC= 0U;
#endif /* EE_AS_IOC__ && EE_AS_IOC_HAS_CALLBACKS__ */

  /* Enter RPC critical section */
  EE_hal_spin_in(spinlock_id);

  /* If the ShutdownAllCores have not been called search the RPC to be served */
  if ( EE_as_shutdown_all_cores_flag != 0U )
  {
    /* Start shutdown procedure */
    to_shutdown = 1U;
  }
#if defined(EE_AS_IOC__) && defined(EE_AS_IOC_HAS_CALLBACKS__)
  else if ( EE_as_rpc_IOC[EE_CURRENTCPU] != 0U)
  {
    /* Start IOC procedure */
    to_IOC = 1U;
    /* Reset IOC Flag */
    EE_as_rpc_IOC[EE_CURRENTCPU] = 0U;
  }
#endif /* EE_AS_IOC__ && EE_AS_IOC_HAS_CALLBACKS__ */
  else
  {
    /* Declare to other CPUs that this core started serving RPCs */
    EE_as_rpc_serving[EE_CURRENTCPU] = 1U;

    /* Search the requiring core */
    for ( requiring_core_index = 0U; requiring_core_index < (EE_UREG)EE_MAX_CPU;
          ++requiring_core_index )
    {
      /*
       * Perform the following operation only for
       * index values other then current cpu id.
       */
      if (requiring_core_index != (EE_UREG)EE_CURRENTCPU) {

        /* Select the RPC Structure */
        rpc_ref = &EE_as_rpc_RAM[requiring_core_index];

        if ( rpc_ref->serving_core == EE_CURRENTCPU ) {
          break;
        }
      }
    }
  }

  /* Exit RPC critical section */
  EE_hal_spin_out(spinlock_id);

  do {

    /* If ShutdownAllCores have been called just shut this core down */
    if ( to_shutdown != 0U )
    {
      EE_oo_ShutdownOS_internal( EE_as_shutdown_all_cores_error );
    }
#if defined(EE_AS_IOC__) && defined(EE_AS_IOC_HAS_CALLBACKS__)
    else if ( to_IOC != 0U )
    {
      EE_as_IOC_callback_sequence();
    }
#endif /* EE_AS_IOC__ && EE_AS_IOC_HAS_CALLBACKS__ */
    else
    {
      if ( rpc_ref != NULL ) {
        /* Otherwise call the service */
#ifndef __OO_NO_ALARMS__
        rpc_ref->error = EE_as_rpc_execute( rpc_ref->remote_procedure,
          rpc_ref->param1, rpc_ref->param2, rpc_ref->param3 );
#else
        rpc_ref->error = EE_as_rpc_execute( rpc_ref->remote_procedure,
          rpc_ref->param1, rpc_ref->param2);
#endif /* __OO_NO_ALARMS__ */
      }
    }

    /* Enter RPC critical section */
    EE_hal_spin_in(spinlock_id);

#if defined(EE_AS_IOC__) && defined(EE_AS_IOC_HAS_CALLBACKS__)
    /*  If the just served service is OSServiceId_IOCService skip reset:
        this call type is not synchronous so it could incorrectly free
        core for following remote calls */
    if ( to_IOC == 0U )
    {
      if ( rpc_ref != NULL ) {
        /* Reset actual RPC */
        rpc_ref->serving_core = INVALID_CORE_ID;
      }
    } else {
      /* Reset IOC flag */
      to_IOC = 0U;
    }
#else  /* EE_AS_IOC__ && EE_AS_IOC_HAS_CALLBACKS__ */

    if ( rpc_ref != NULL ) {
      /* Reset actual RPC */
      rpc_ref->serving_core = INVALID_CORE_ID;
    }
#endif /* EE_AS_IOC__ && EE_AS_IOC_HAS_CALLBACKS__ */

    /* If the ShutdownAllCores have been called: Start shutdown procedure */
    if ( EE_as_shutdown_all_cores_flag != 0U )
    {
      to_shutdown = 1U;
    }
#if defined(EE_AS_IOC__) && defined(EE_AS_IOC_HAS_CALLBACKS__)
    else if ( EE_as_rpc_IOC[EE_CURRENTCPU] != 0U )
    {
      /* Start IOC procedure */
      to_IOC= 1U;
      /* Reset IOC Flag */
      EE_as_rpc_IOC[EE_CURRENTCPU] = 0U;
    }
#endif /* EE_AS_IOC__ && EE_AS_IOC_HAS_CALLBACKS__ */
    else
    {
      /*  Save previous core index value to check request list empty condition
          and (post)increment requiring_core_index. On start-up and increment
          handle wrap around. This has been made to avoid starvation on cores
          that could happen if all the time I restart from index 0 */
      prev_requiring_core_index = requiring_core_index;
      EE_AS_RPC_INCREMENT_AND_HANDLE_WRAPAROUND(requiring_core_index);
      while ( requiring_core_index != prev_requiring_core_index )
      {
        if ( requiring_core_index != (EE_UREG)EE_CURRENTCPU ) {
          /* Select the RPC Structure */
          rpc_ref = &EE_as_rpc_RAM[requiring_core_index];

          if ( rpc_ref->serving_core == EE_CURRENTCPU ) {
            break;
          }
        }

        EE_AS_RPC_INCREMENT_AND_HANDLE_WRAPAROUND(requiring_core_index);
      }

      /* If no other RPC requiring cores have been found reset serving flag */
      if ( requiring_core_index == prev_requiring_core_index )
      {
        EE_as_rpc_serving[EE_CURRENTCPU] = 0U;
        /* Acknowledge current IIRQ */
        EE_hal_IRQ_interprocessor_served((EE_UINT8)EE_CURRENTCPU);
      }
    }

    /* Exit RPC critical section */
    EE_hal_spin_out(spinlock_id);
  } while (  EE_as_rpc_serving[EE_CURRENTCPU] != 0U );

  EE_hal_end_nested_primitive(flag);
}

#ifndef __PRIVATE_SHUTDOWNOS__
void EE_as_ShutdownAllCores( StatusType Error )
{
  register EE_SREG i;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in( EE_SERVICETRACE_SHUTDOWNALLCORES );
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
  /* ShutdownAllCores is callable by Task and ISR2, ErrorHook and StartupHook */
  if ( EE_oo_check_disableint_error() ) {
    ; /* Nothing to do, just get that this an Error */
  } else if ( (EE_as_execution_context > ErrorHook_Context) &&
    (EE_as_execution_context != StartupHook_Context) )
  {
    ; /* Nothing to do, just get that this an Error */
  } else
#endif /* EE_SERVICE_PROTECTION__ */
#ifdef EE_AS_OSAPPLICATIONS__
  /* [OS716]: If ShutdownAllCores is called from non trusted code the call
      shall be ignored. (BSW4080007) */
  if ( EE_as_Application_ROM[EE_as_active_app].Mode != EE_MEMPROT_TRUST_MODE ) {
    ; /* Nothing to do, just get that this an Error */
  } else
#endif /* EE_AS_OSAPPLICATIONS__ */
  {
    /* Acquire all cores spinlocks to signal that shutdown procedure is started.
       Spinlock are assigned to various core ain reverse order, but it's
       just a convention so I can acquire them in direct order */
    for ( i = 0; i < EE_MAX_CPU; ++i ) {
      EE_hal_spin_in( EE_as_core_spinlocks[i] );
    }

    /* If the procedure have been already started (by another core), just shut
       this core down, after have released all spinlocks */
    if ( EE_as_shutdown_all_cores_flag != 0U ) {
      /* Release all cores spinlocks */
      for ( i = 0; i < EE_MAX_CPU; ++i ) {
        EE_hal_spin_out( EE_as_core_spinlocks[i] );
      }
      /* This won't never return */
      EE_oo_ShutdownOS_internal( EE_as_shutdown_all_cores_error );
    }

    /* Save the Error parameter to be used in all other cores */
    EE_as_shutdown_all_cores_error = Error;

    /* set ShutdownAllCores global flag */
    EE_as_shutdown_all_cores_flag = 1U;

    /* Eventually signal cores with an IIRQ */
    for ( i = 0; i < EE_MAX_CPU; ++i ) {
      /* Perform the following operation only for
       * index values other then current cpu id. */
      if ( i != EE_CURRENTCPU ) {
        if ( EE_as_rpc_serving[i] == 0U ) {
          /* In any case we shall assure that EE_hal_IRQ_interprocessor can
             handle multiple sequential calls */
          EE_hal_IRQ_interprocessor((EE_UINT8)i);
        }
      }
    }

    /* Release all cores spinlocks */
    for ( i = 0; i < EE_MAX_CPU; ++i )
    {
      EE_hal_spin_out( EE_as_core_spinlocks[i] );
    }
    /* After signaling the shutdown all cores status: shut this core down:
       This won't never return */
    EE_oo_ShutdownOS_internal( Error );
  }

  EE_ORTI_set_service_out( EE_SERVICETRACE_SHUTDOWNALLCORES );

  EE_OS_EXIT_CRITICAL_SECTION();
  return;
}

/* If MemMap.h support is enabled (i.e. because memory protection): use it */
#ifdef EE_SUPPORT_MEMMAP_H
/* Put the following code in ee_kernel_text */
#define OS_STOP_SEC_CODE
#include "MemMap.h"
#endif /* EE_SUPPORT_MEMMAP_H */

#endif /* !__PRIVATE_SHUTDOWNOS__ */

