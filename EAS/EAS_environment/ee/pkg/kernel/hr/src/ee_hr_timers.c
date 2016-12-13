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
 * Author:2003 Paolo Gai
 * Edited for HR by Alessandro Biondi (2013)
 */

#include "ee_internal.h"

/*
 * The idea of this file is the following: 
 *
 * - on some architectures there is just the possibility to have two
 *   timers, one to export a timing reference (basically a free running
 *   timer) and one to export an interrupt source.
 *
 * - we need to export a way to treat this single interrupt source as
 *   a set of "virtual" timers, which will expire after some time.
 *
 * - we specify the functions which are called after an interrupt in a
 *   static way as consts
 *
 * - we program the single interrupt source as the
 *   union of the four timers
 *
 * - it must be possible for one IRQ function to reprogram its timer
 *   source without interfering with the others
 *
 * - although this can give some precision errors, timer expiration
 *   values are given as relative values. Every time we need to program
 *   the timer, the current time is taken again and an absolute value is
 *   computed. the absolute value is then stored in the data structure to 
 *   compute the timer expiration.
 */


/* this is the array of pointers to functions that are called by the
   multiplexer */

typedef void (*voidfpointer)(void);


const voidfpointer EE_hr_timer_functions[3] = 
{
  EE_hr_IRQ_budget,
  EE_hr_IRQ_recharging,
  EE_hr_IRQ_dlcheck
};

// This is in the BSS - everything initialized to 0
struct {
  EE_STIME t;
  int next;
  int used;
} EE_hr_timer_data[3];

int EE_hr_timer_first = -1;

void EE_hr_timer_reprogram(void)
{
  EE_STIME tmp;

  // reprogram the timer
  if (EE_hr_timer_first == -1) {
    // nobody is there
    EE_hal_stop_budget_timer();
  } else {
    // reprogram the timer
    tmp = EE_hr_timer_data[EE_hr_timer_first].t - EE_hal_gettime();
    if (tmp < EE_TIMER_MINCAPACITY) {
      EE_hal_set_budget_timer(EE_TIMER_MINCAPACITY);
    } else {
      EE_hal_set_budget_timer(tmp);
    }
  }
}


/* Set timer source "timer" to fire at time t */
void EE_hr_timer_set(int timer, EE_STIME t)
{
  register EE_STIME cur_time, abs_time;
  int p,q;

  // take the absolute time
  cur_time = EE_hal_gettime();
  abs_time = cur_time + t;

  // cancel the timer if it is running
  if (EE_hr_timer_data[timer].used) {
    // This part is similar to stop_timer, but without timer reprogramming
    p = -1;
    q = EE_hr_timer_first;
    
    while ( (q != -1) && (q != timer) ) {
      p = q;
      q = EE_hr_timer_data[q].next;
    }
    
    if (q == -1) {
      /* the timer is not there, something wrong? */
    } else {
      /* q == timer !!! */
      if ( p == -1 ) {
	// remove the first item in the timer queue
	EE_hr_timer_first = EE_hr_timer_data[EE_hr_timer_first].next;
      } else {
	// remove an item in the middle of the ready queue
	EE_hr_timer_data[p].next = EE_hr_timer_data[q].next;
      }
    }
  }

  // set the value inside the data structure
  EE_hr_timer_data[timer].t = abs_time;
  EE_hr_timer_data[timer].used = 1;

  // insert the timer in the queue
  p = -1;
  q = EE_hr_timer_first;
      
  while ((q != -1) && 
	 (EE_STIME)(abs_time - EE_hr_timer_data[q].t) >= 0
	 ) {
    p = q;
    q = EE_hr_timer_data[q].next;
  }
      
  if (p != -1)
    EE_hr_timer_data[p].next = timer;
  else {
    EE_hr_timer_first = timer;

    // reprogram the timer
    EE_hal_set_budget_timer(t);
  }
  
  EE_hr_timer_data[timer].next = q;
}

/* Stops timer source "timer" */
void EE_hr_timer_stop(int timer)
{
  int p;
  int q;
  
  p = -1;
  q = EE_hr_timer_first;
  
  while ( (q != -1) && (q != timer) ) {
    p = q;
    q = EE_hr_timer_data[q].next;
  }
  
  if (q == -1) {
    /* the timer is not there */
  } else {
    /* q == timer !!! */
    if ( p == -1 ) {
      // remove the first item in the timer queue
      EE_hr_timer_first = EE_hr_timer_data[EE_hr_timer_first].next;

      EE_hr_timer_reprogram();
    } else {
      // remove an item in the middle of the ready queue
      EE_hr_timer_data[p].next = EE_hr_timer_data[q].next;
    }
    EE_hr_timer_data[timer].used = 0;
  }

}

/* IRQ handler for the single IRQ */
void EE_hr_IRQ_timer_multiplexer(void)
{
  register EE_STIME curtime;
  register EE_FREG flag;
  register int tmp;
  
  flag = EE_hal_begin_nested_primitive();

  // read the current time
  curtime = EE_hal_gettime();

  // loop until there are expired things to do
  while (EE_hr_timer_first != -1 &&
	 (EE_STIME)(EE_hr_timer_data[EE_hr_timer_first].t - curtime) < EE_TIMER_MINCAPACITY) {
    // update the data structures
    EE_hr_timer_data[EE_hr_timer_first].used = 0;
    tmp = EE_hr_timer_first;
    EE_hr_timer_first = EE_hr_timer_data[EE_hr_timer_first].next;

    // call the function
    // note that this function may call EE_hr_set_timer and EE_hr_timer_stop
    EE_hr_timer_functions[tmp]();
  }

  EE_hr_timer_reprogram();

  EE_hal_end_nested_primitive(flag);
}


