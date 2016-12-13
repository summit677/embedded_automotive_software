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

/*
 * Author: 2012,  Giuseppe Serano
 *         2014,  Christoph Kreuzberger
 */

#include "ee.h"
#include "ee_irq.h"

/* insert a stub for the functions not directly supported by __FP__ */
#ifdef __FP__
__INLINE__ void __ALWAYS_INLINE__ DisableAllInterrupts(void)
{
  EE_hal_disableIRQ();
}

__INLINE__ void __ALWAYS_INLINE__ EnableAllInterrupts(void)
{
  EE_hal_enableIRQ();
}

__INLINE__ void __ALWAYS_INLINE__ TerminateTask(void)
{
}

#define OSDEFAULTAPPMODE 1
__INLINE__ void __ALWAYS_INLINE__ StartOS(int i)
{
}
#endif

/* A few counters incremented at each event 
 * (alarm, button press or task activation...)
 */
volatile int timer_fired = 0;
volatile int button_fired = 0;
volatile int task1_fired = 0;
volatile int task2_fired = 0;

/* Let's remember the led status! 
 * Mutual exclusion on this variable is not included in the demo to make it
 * not too complicated; in general shared variables should be protected using
 * GetResource/ReleaseResource calls
 */
volatile EE_UINT8 led_status = 0;


/* Let's declare the tasks identifiers */
DeclareTask(Task1);
DeclareTask(Task2);


/* just a dummy delay */ 
#define ONEMILLION 1000
static void mydelay(void)
{
  int i,j;
  for (i = 0; i < ONEMILLION; i++)
    for (j = 0; j < ONEMILLION; j++)
    {
      ;	/* NOP */
    }
}


/* sets and resets a led configuration passed as parameter, leaving the other
 * bits unchanged
 * 
 * Note: led_blink is called both from Task1 and Task2. To avoid race 
 * conditions, we forced the atomicity of the led manipulation using IRQ
 * enabling/disabling. We did not use Resources in this case because the 
 * critical section is -really- small. An example of critical section using 
 * resources can be found in the osek_resource example.
 */
void led_blink(EE_UINT8 theled)
{
  DisableAllInterrupts();
  EE_LED_On(theled);
  EnableAllInterrupts();

  mydelay();

  DisableAllInterrupts();
  EE_LED_Off(theled);
  EnableAllInterrupts();
}


/*
 * Program the SysTick to raise interrupts
 */
void systick_program(void)
{
  EE_systick_set_period(	/* Set the SysTick period to (1ms @ 10MHz)   */
    MILLISECONDS_TO_TICKS(1, 10000000)
  );
  EE_systick_enable_int();	/* Enable SysTick Interrupts		      */
  EE_systick_start();		/* Start SysTick			      */

}


/* This is an ISR Type 2 which is attached to the SysTick peripheral IRQ pin
 * The ISR simply calls CounterTick to implement the timing reference
 */
ISR2(systick_handler)
  {
  EE_systick_clr_int_flg();
  static int timer_divisor = 0;

  timer_divisor++;
  if (timer_divisor == 4000) {
    timer_divisor = 0;
    timer_fired++;
    ActivateTask(Task1);
  }
}


/* Task1: just call the ChristmasTree */
TASK(Task1)
{
  task1_fired++;
  
  /* First half of the christmas tree */
  led_blink(0);
  led_blink(1);
  led_blink(2);
  
  /* CONFIGURATION 3 and 4: we put an additional Schedule() here! */
#ifdef MYSCHEDULE
  Schedule();
#endif

  /* Second half of the christmas tree */
  led_blink(3);
  led_blink(4);
  led_blink(5);
  
  TerminateTask();
}
#define LCD_DISPLAY_LINE_SIZE 20

/* Task2: Print the counters on the LCD Display */
TASK(Task2)
{
  static char	line1[LCD_DISPLAY_LINE_SIZE + 1] = "Tmr:000 Tsk1:000";
  static char	line2[LCD_DISPLAY_LINE_SIZE + 1] = "Btn:000 Tsk2:000";

  /* count the number of Task2 activations */
  task2_fired++;

  led_blink(6);


  /* prints a report
   * Note: after the first printf in main(), then only this task uses printf
   * In this way we avoid raceconditions in the usage of stdout.
   */
  line1[4] = (((timer_fired%1000)/100)+'0');
  line1[5] = (((timer_fired%100)/10)+'0');
  line1[6] = ((timer_fired%10)+'0');

  line1[13] = (((task1_fired%1000)/100)+'0');
  line1[14] = (((task1_fired%100)/10)+'0');
  line1[15] = ((task1_fired%10)+'0');

  DISPLAY_DisplayString(3, 0, line1);

  line2[4] = (((button_fired%1000)/100)+'0');
  line2[5] = (((button_fired%100)/10)+'0');
  line2[6] = ((button_fired%10)+'0');

  line2[13] = (((task2_fired%1000)/100)+'0');
  line2[14] = (((task2_fired%100)/10)+'0');
  line2[15] = ((task2_fired%10)+'0');

  DISPLAY_DisplayString(4, 0, line2);
  TerminateTask();
}


/*
 * Programs User Buttons to rais interrupts.
 */
void buttons_program(void)
{
  rtiSetPeriod(rtiCOMPARE3, 2000000);
  rtiEnableNotification(rtiNOTIFICATION_COMPARE3);
}

/*
 * User Buttons Interrupts Handler which activates Task2.
 */

ISR2(rtiCompare3Interrupt)
{
  /* Clear Interrupt Sources */
  rtiREG1->INTFLAG = 8U;
  /* count the number of button presses */
if (CheckSwitch1() || CheckSwitch2()){
  button_fired++;
  EE_LED_Toggle(5);
  
  ActivateTask(Task2); 
}
}


int main()
{ 
  /* MCU Initialization */
  EE_mcu_init();

  /* Display Initialization */
  DISPLAY_Clear(Black);
  DISPLAY_SetBackColor(Black);
  DISPLAY_SetTextColor(White);
  DISPLAY_DisplayString(0, 0, "Hardware: TI TMS570");
  DISPLAY_DisplayString(1, 0, "Software: Erika RTOS");
  DISPLAY_SetTextColor(Red);
  DISPLAY_DisplayString(9, 0, "by Chris Kreuzberger");

  /* Programs SysTick to raise interrupts */
  systick_program();

  /* Programs User Buttons to raise interrupts */
  buttons_program();

  mydelay();

  /* let's start the multiprogramming environment...*/
  StartOS(OSDEFAULTAPPMODE);

  /* now the background activities: in this case, we do nothing. */
  for (;;)
  {
    ;	/* NOP */
  }
  
  /* Warning: statement is unreachable */
  /* return 0; */

}
