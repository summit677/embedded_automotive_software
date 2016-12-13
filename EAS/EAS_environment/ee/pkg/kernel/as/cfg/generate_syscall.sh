#!/bin/sh
# ###*B*###
# ERIKA Enterprise - a tiny RTOS for small microcontrollers
# 
# Copyright (C) 2002-2011  Evidence Srl
# 
# This file is part of ERIKA Enterprise.
# 
# ERIKA Enterprise is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation, 
# (with a special exception described below).
# 
# Linking this code statically or dynamically with other modules is
# making a combined work based on this code.  Thus, the terms and
# conditions of the GNU General Public License cover the whole
# combination.
# 
# As a special exception, the copyright holders of this library give you
# permission to link this code with independent modules to produce an
# executable, regardless of the license terms of these independent
# modules, and to copy and distribute the resulting executable under
# terms of your choice, provided that you also meet, for each linked
# independent module, the terms and conditions of the license of that
# module.  An independent module is a module which is not derived from
# or based on this library.  If you modify this code, you may extend
# this exception to your version of the code, but you are not
# obligated to do so.  If you do not wish to do so, delete this
# exception statement from your version.
# 
# ERIKA Enterprise is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License version 2 for more details.
# 
# You should have received a copy of the GNU General Public License
# version 2 along with ERIKA Enterprise; if not, write to the
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301 USA.
# ###*E*###

# Author: 2011, Bernardo  Dal Seno

# This script generates all system call defintions to standard output,
# using templates specified by the definitions of the functions
# EE_SYSCALL_0, EE_SYSCALL_1...  Such functions are specific for each
# architecture, and are defined in scripts contained in pkg/cpu/XXX/cfg.
# One of those script must be sourced (i.e., executed with the shell command
# "source") before executing this script.

set -e

if [ "$(type -t EE_SYSCALL_0)" != "function" ]; then
    echo >&2 "No template definitions"
    echo >&2 "Please source the architecture-specific generate_syscall_XXX.sh"
    exit 2
fi

EE_SYSCALL_1 StatusType ActivateTask TaskType TaskID EE_ID_ActivateTask
EE_SYSCALL_0 StatusType TerminateTask EE_ID_TerminateTask
EE_SYSCALL_1 StatusType ChainTask TaskType TaskID EE_ID_ChainTask
EE_SYSCALL_0 StatusType Schedule EE_ID_Schedule
EE_SYSCALL_0 StatusType ForceSchedule EE_ID_ForceSchedule
EE_SYSCALL_1 StatusType GetTaskID TaskRefType TaskID EE_ID_GetTaskID
EE_SYSCALL_2 StatusType GetTaskState TaskType TaskID TaskStateRefType State EE_ID_GetTaskState
EE_SYSCALL_0 void EnableAllInterrupts EE_ID_EnableAllInterrupts
EE_SYSCALL_0 void DisableAllInterrupts EE_ID_DisableAllInterrupts
EE_SYSCALL_0 void ResumeAllInterrupts EE_ID_ResumeAllInterrupts
EE_SYSCALL_0 void SuspendAllInterrupts EE_ID_SuspendAllInterrupts
EE_SYSCALL_0 void ResumeOSInterrupts EE_ID_ResumeOSInterrupts
EE_SYSCALL_0 void SuspendOSInterrupts EE_ID_SuspendOSInterrupts
echo '#ifndef __OO_NO_RESOURCES__'
EE_SYSCALL_1 StatusType GetResource ResourceType ResID EE_ID_GetResource
EE_SYSCALL_1 StatusType ReleaseResource ResourceType ResID EE_ID_ReleaseResource
echo '#endif /* ! __OO_NO_RESOURCES__ */'
echo '#ifndef __OO_NO_EVENTS__'
EE_SYSCALL_2 StatusType SetEvent TaskType TaskID EventMaskType Mask EE_ID_SetEvent
EE_SYSCALL_1 StatusType ClearEvent EventMaskType Mask EE_ID_ClearEvent
EE_SYSCALL_2 StatusType GetEvent TaskType TaskID EventMaskRefType Event EE_ID_GetEvent
EE_SYSCALL_1 StatusType WaitEvent EventMaskType Mask EE_ID_WaitEvent
echo '#endif /* ! __OO_NO_EVENTS__ */'
echo '#ifndef __OO_NO_ALARMS__'
EE_SYSCALL_1 void CounterTick CounterType c EE_ID_counter_tick
EE_SYSCALL_2 StatusType GetAlarmBase AlarmType AlarmID AlarmBaseRefType Info EE_ID_GetAlarmBase
EE_SYSCALL_2 StatusType GetAlarm AlarmType AlarmID TickRefType Tick EE_ID_GetAlarm
EE_SYSCALL_3 StatusType SetRelAlarm AlarmType AlarmID TickType increment TickType cycle EE_ID_SetRelAlarm
EE_SYSCALL_3 StatusType SetAbsAlarm AlarmType AlarmID TickType start TickType cycle EE_ID_SetAbsAlarm
EE_SYSCALL_1 StatusType CancelAlarm AlarmType AlarmID EE_ID_CancelAlarm
echo '#endif /* ! __OO_NO_ALARMS__ */'
EE_SYSCALL_0 AppModeType GetActiveApplicationMode EE_ID_GetActiveApplicationMode
EE_SYSCALL_1 StatusType StartOS AppModeType Mode EE_ID_StartOS
EE_SYSCALL_1 void ShutdownOS StatusType Error EE_ID_ShutdownOS
echo '#ifdef __OO_SEM__'
EE_SYSCALL_1 StatusType WaitSem SemRefType Sem EE_ID_WaitSem
EE_SYSCALL_1 void WaitSem SemRefType Sem EE_ID_WaitSem
EE_SYSCALL_1 int TryWaitSem SemRefType Sem EE_ID_TryWaitSem
EE_SYSCALL_1 StatusType PostSem SemRefType Sem EE_ID_PostSem
EE_SYSCALL_1 int GetValueSem SemRefType Sem EE_ID_GetValueSem
echo '#endif /* __OO_SEM__ */'

#This should go into a different header
echo
echo '#ifdef __OO_ORTI_SERVICETRACE__'
echo '#if defined(RTDRUID_CONFIGURATOR_NUMBER) \'
echo ' && RTDRUID_CONFIGURATOR_NUMBER >= RTDRUID_CONFNUM_ORTI_SERVICE_API'
EE_SYSCALL_1 void EE_ORTI_ext_set_service EE_UINT8 srv EE_ID_ORTI_ext_set_service
echo '#else /* __EE_MEMORY_PROTECTION__ && RTDRUID_CONFNUM_ORTI_SERVICE_API */'
echo '__INLINE__ void EE_ORTI_ext_set_service(EE_UINT8 srv)'
echo '{'
echo '}'
echo '#endif /* else __EE_MEMORY_PROTECTION__ && RTDRUID_CONFNUM_ORTI_SERVICE_API */'
echo '#endif /* __OO_ORTI_SERVICETRACE__ */'
