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
 * Author: 2002,2003 Paolo Gai, Bertozzi Francesco
 */


#ifndef __INCLUDE_ALARMS_ALARMS_H__
#define __INCLUDE_ALARMS_ALARMS_H__


/*************************************************************************
 Alarm Types
 *************************************************************************/


/* alarm id type (signed!) */
#ifndef EE_TYPEALARM
#define EE_TYPEALARM EE_SREG
#endif

/* If you change this definition, you must change the
 * EE_hal_read_timer, EE_hal_set_timer and
 * EE_hal_stop_timer too.
 * You are working on hardware alarm !
 */
#define EE_TYPECOUNTER EE_SREG

/* counter tick type */
#ifndef EE_TYPETICK
#define EE_TYPETICK EE_UREG
#endif

/* notification type */
#ifndef EE_TYPENOTIFY
#define EE_TYPENOTIFY EE_UINT8
#endif

/* Interrupt callout */
#ifndef EE_N_HARD_ALARMS
#define EE_N_HARD_ALARMS 255
#endif

#if (0 < EE_N_HARD_ALARMS)
void EE_altimer0 (void);
#endif

#if (1 < EE_N_HARD_ALARMS)
void EE_altimer1 (void);
#endif

#if (2 < EE_N_HARD_ALARMS)
void EE_altimer2 (void);
#endif

/*************************************************************************
 Variables defined by the application
 *************************************************************************/

typedef EE_TYPEALARM EE_counter_RAM_type; /* first alarm queued on the counter, initvalue: -1 */

/* these are the different types of alarm notifications... */
#define EE_ALARM_ACTION_TASK 0
#define EE_ALARM_ACTION_CALLBACK 1

/* initvalue: {a_valid_counter, a_valid_action, then you must put the correct
   parameters depending on the action } */
typedef struct {
  EE_TYPECOUNTER c;           /* the counter linked to the alarm */

  EE_TYPENOTIFY action;
  
  EE_TID TaskID;

  EE_VOID_CALLBACK f;
} EE_alarm_ROM_type;

/* initvalue: all zeroes --> no initialization! */
typedef struct {
  EE_TYPETICK cycle;          /* cycle for periodic alarms */
  EE_TYPETICK value;          /* expiration time (into a queue!) */
  EE_TYPEALARM next;          /* next alarm in the delta queue */
} EE_alarm_RAM_type;

/* this is the RAM part of a counter. 
   Initvalue = an array of {0,-1} elements */
extern EE_counter_RAM_type       EE_counter_RAM[];

/* this is the fixed part of the configuration of an alarm
   Initvalue= depends on how the alarm notification have to be configured */
extern const EE_alarm_ROM_type   EE_alarm_ROM[];

/* this part is the variable part of an alarm.
   Initvalue: an array of {0,0,0,-1} */
extern EE_alarm_RAM_type         EE_alarm_RAM[];

/***************************************************************************
 * Implementation defined functions
 ***************************************************************************/

/* This function notifies a tick to a counter. That is, the counter is
   incremented by 1.  It must be called into an ISR or into a
   thread notify that the event linked to the counter occurred. 

   The function will also implement the notification of expired alarms
   (calling an alarm callback or activating a thread). 

   The function is NOT atomic, and NO RESCHEDULING will take place
   after the execution of this function. To implement the
   rescheduling at task level, you can use the Schedule() function
   just after this notification.

   These functions can be called both into a thread and into an ISR.
*/
#ifndef __PRIVATE_TIMERALARM__
void EE_alarm_timer(EE_TYPECOUNTER c);
#endif

#ifndef __PRIVATE_INSERTALARM__
void EE_alarm_insert
      (EE_TYPEALARM AlarmID, EE_TYPETICK value);
#endif
       
#ifndef __PRIVATE_GETALARM__
void EE_alarm_get(EE_TYPEALARM AlarmID, EE_TYPETICK *Tick);
#endif

#ifndef __PRIVATE_SETRELALARM__
void EE_alarm_setrel(EE_TYPEALARM AlarmID, 
      EE_TYPETICK increment, EE_TYPETICK cycle);
#endif

#ifndef __PRIVATE_SETABSALARM__
void EE_alarm_setabs(EE_TYPEALARM AlarmID, 
      EE_TYPETICK start, EE_TYPETICK cycle);
#endif

#ifndef __PRIVATE_CANCELALARM__
void EE_alarm_cancel(EE_TYPEALARM AlarmID);
#endif

#endif
