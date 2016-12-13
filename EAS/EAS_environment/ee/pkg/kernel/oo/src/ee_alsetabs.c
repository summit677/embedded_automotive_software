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
 * CVS: $Id: ee_alsetabs.c,v 1.1 2005/07/16 12:23:42 pj Exp $
 */

#include "ee_internal.h"

/* SetAbsAlarm
   - This function occupies the AlarmID setting an alarm when the counter 
     will reach the absolute value passed as parameter.
   - the Alarm must not be already in use

   returns the relative value in ticks before the alarm expires
   - it returns E_OK if all OK
     E_OS_STATE if the Alarm is already used
     E_OS_ID if the alarm is invalid (only Extended status)
     E_OS_VALUE if the parameters are incorrect
*/

#ifndef __PRIVATE_SETABSALARM__
StatusType EE_oo_SetAbsAlarm(AlarmType AlarmID, 
    TickType start, TickType cycle)
{
  /* Error Value */
  register StatusType ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_SETABSALARM);

  EE_as_monitoring_the_stack();

#ifdef EE_SERVICE_PROTECTION__
  /* [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /* [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      “invalid value” of  the service. (BSW11009, BSW11013) */
  /* SetAbsAlarm is callable by Task and ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

#ifdef EE_AS_RPC__
  if ( EE_AS_ID_REMOTE(AlarmID) )
  {
    EE_os_param unmarked_alarm_id, as_start, as_cycle;
    unmarked_alarm_id.value_param = EE_AS_UNMARK_REMOTE_ID(AlarmID);
    as_start.value_param = start;
    as_cycle.value_param = cycle;
    /* forward the request to another CPU in synchronous way */
    ev = EE_as_rpc(OSServiceId_SetAbsAlarm, unmarked_alarm_id,
      as_start, as_cycle);
  } else {
#endif /* EE_AS_RPC__ */

/* If local alarm are not defined cut everything else */
#if defined(EE_MAX_ALARM) && (EE_MAX_ALARM > 0U)

#if ( defined(EE_AS_OSAPPLICATIONS__) && defined(EE_SERVICE_PROTECTION__) )
    if ( AlarmID >= EE_MAX_ALARM ) {
      ev = E_OS_ID;
    } else if ( EE_ALARM_ACCESS_ERR(AlarmID, EE_as_active_app) ) {
      ev = E_OS_ACCESS;
    } else
#elif defined(__OO_EXTENDED_STATUS__)
    if ( AlarmID >= EE_MAX_ALARM ) {
      ev = E_OS_ID;
    } else
#endif /* EE_AS_OSAPPLICATIONS__ || E_SERVICE_PROTECTION__ ||
__OO_EXTENDED_STATUS__ */
#ifdef __OO_EXTENDED_STATUS__
    if ( (start > EE_counter_ROM[EE_oo_counter_object_ROM[AlarmID].c].
          maxallowedvalue) || ((cycle != 0U) &&
      ((cycle < EE_counter_ROM[EE_oo_counter_object_ROM[AlarmID].c].mincycle) ||
        (cycle > EE_counter_ROM[EE_oo_counter_object_ROM[AlarmID].c].
          maxallowedvalue))) )
    {
      ev = E_OS_VALUE;
    } else
#endif /* __OO_EXTENDED_STATUS__ */

    if ( EE_oo_counter_object_RAM[AlarmID].used ) {
      ev = E_OS_STATE;
    } else {
      EE_oo_handle_abs_counter_object_insertion(AlarmID, start, cycle);
      ev = E_OK;
    }
#else /* EE_MAX_ALARM > 0U */
    {
      ev = E_OS_ID;
    }
#endif /* EE_MAX_ALARM > 0U */
#ifdef EE_AS_RPC__
  }
#endif /* EE_AS_RPC__ */
  if ( ev != E_OK ) {
    EE_ORTI_set_lasterror(ev);
    EE_oo_notify_error_SetAbsAlarm(AlarmID, start, cycle, ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_SETABSALARM);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}
#endif /* !__PRIVATE_SETABSALARM__ */
