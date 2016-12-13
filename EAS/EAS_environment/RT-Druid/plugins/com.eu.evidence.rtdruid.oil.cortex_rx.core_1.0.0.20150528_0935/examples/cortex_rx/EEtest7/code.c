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



/*
 * A minimal EE demo for BCC2 mode testing. Task1 is a non-preemptive task
 * and Task2 can have pending activations.
 * Author: 2011  Giuseppe Serano
 *         2014  Christoph Kreuzberger
 * Based on examples/s12xs/porting_examples/monostack/EEtest7
 */


#include "ee.h"
#include "ee_irq.h"
#include "test/assert/inc/ee_assert.h"

//#define TRUE 1

/* Assertions */
enum EE_ASSERTIONS {
  EE_ASSERT_FIN = 0,
  EE_ASSERT_INIT,
  EE_ASSERT_TASK2_FIRED,
  EE_ASSERT_TIMER_FIRED,
  EE_ASSERT_TASK1_FIRED,
  EE_ASSERT_TASK2_ACTIVATIONS,
  EE_ASSERT_DIM
};
EE_TYPEASSERTVALUE EE_assertions[EE_ASSERT_DIM];

/* Final result */
volatile EE_TYPEASSERTVALUE result;

/* Counters */
volatile int timer_fired = 0;
volatile int timer_divisor = 0;
volatile int task1_fired = 0;
volatile int task2_fired = 0;
volatile int counter = 0;

/*
 * SysTick ISR2
 * systick_handler = rtiCompare2Interrupt
 */
ISR2(systick_handler)
{
  EE_systick_clr_int_flg();
  EE_std_enableIRQ_nested();
  timer_divisor++;
  if (timer_divisor == 2000)
  {
    timer_divisor = 0;
    timer_fired++;
    EE_assert(EE_ASSERT_TIMER_FIRED, timer_fired == 1, EE_ASSERT_TASK2_FIRED);
    ActivateTask(Task1);
    EE_systick_stop();
  }
}

/*
 * TASK 1
 */
TASK(Task1)
{
    task1_fired++;
    EE_assert(EE_ASSERT_TASK1_FIRED, task1_fired == 1, EE_ASSERT_TIMER_FIRED);
    ActivateTask(Task2);
    //TerminateTask();
}

/*
 * TASK 2
 */
TASK(Task2)
{
    task2_fired++;
    if (task2_fired == 1)
    {
      ActivateTask(Task2);
      ActivateTask(Task2);
      ActivateTask(Task2);
      ActivateTask(Task2);
      ActivateTask(Task2);
      ActivateTask(Task2);
      EE_assert(EE_ASSERT_TASK2_FIRED, task2_fired == 1, EE_ASSERT_INIT);
    }
    else
      TerminateTask();
}

/*
 * MAIN TASK
 */
int main(void)
{

  /*Initializes Erika related stuffs*/
  EE_system_init(); 

  EE_systick_set_period(10000);
  EE_systick_enable_int();
  EE_systick_start();

  EE_assert(EE_ASSERT_INIT, TRUE, EE_ASSERT_NIL);

  StartOS(OSDEFAULTAPPMODE);

  ActivateTask(Task2);

  while(task2_fired<5);
  EE_assert(
    EE_ASSERT_TASK2_ACTIVATIONS, task2_fired == 5, EE_ASSERT_TASK1_FIRED
  );
  EE_assert_range(EE_ASSERT_FIN, EE_ASSERT_INIT, EE_ASSERT_TASK2_ACTIVATIONS);
  result = EE_assert_last();
  counter++;

  /* Forever loop: background activities (if any) should go here */
  for (;result == 1;)
  {
    while (counter % 100000) counter++;
    EE_LED_Toggle(0);
    counter++;
  }

}
