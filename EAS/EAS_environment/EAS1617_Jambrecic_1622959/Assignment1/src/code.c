#include "ee.h"
#include "ee_irq.h"



DeclareEvent(Event1);
DeclareEvent(Event_Mix);
//DeclareCounter(system_counter);

/* Let's declare the tasks identifiers */
DeclareTask(Task_Event);
DeclareTask(Task_Alarm);
DeclareTask(Task_Mix);



//dodano??
ISR2(button_isr){

	EE_tc2x7_button_irq_clear_request();

	SetEvent(Task_Event,Event1);
}


TASK(Task_Event){
	EE_tc2x7_led_id led0;
	led0=EE_LED_0;

	while (1){

		WaitEvent(Event1);
		LEDCTL_Toggle(led0);
		ClearEvent(Event1);
	}

	TerminateTask();

}

TASK(Task_Alarm)
{	EE_tc2x7_led_id led1;
	led1=EE_LED_1;

	//SetRelAlarm(Alarm,500,0);
	LEDCTL_Toggle(led1);
	TerminateTask();
}

TASK(Task_Mix)
{	EE_tc2x7_led_id led2;
	led2=EE_LED_2;
	while(1){

		WaitEvent(Event_Mix);
		LEDCTL_Toggle(led2);
		ClearEvent(Event_Mix);
	}

	TerminateTask();
}



int main( void )
{  int k ;

	k= LEDCTL_Init();
	EE_tc2x7_button_irq_init(2);
	/* let's start the multiprogramming environment...*/
	StartOS(OSDEFAULTAPPMODE);

	/* now the background activities... Only for FP Kernel */
	for (;;)
		;
	return 0;
}
