#include "ee.h"
#include "ee_irq.h"




/* Let's declare the tasks identifiers */
DeclareTask(Task_Event);
DeclareTask(Task_Alarm);
DeclareTask(Task_Mix);
DeclareResource(resource1);
DeclareResource(resource2);
DeclareEvent(Event1);

TASK(Task_Alarm1)/* lower priority task */
{	LEDCTL_Toggle(EE_LED_0);
	GetResource(resource1);

	SetEvent(Task_Alarm2,Event1);

	GetResource(resource2);
	ReleaseResource(resource2);
	ReleaseResource(resource1);



	TerminateTask();
}

TASK(Task_Alarm2)/*higher priority task */

{	LEDCTL_Toggle(EE_LED_1);

	GetResource(resource2);
	WaitEvent(Event1);
	ClearEvent(Event1);


	GetResource(resource1);

	ReleaseResource(resource1);
	ReleaseResource(resource2);



	TerminateTask();
}




int main( void )
{	LEDCTL_Init();
	/* let's start the multiprogramming environment...*/
	StartOS(OSDEFAULTAPPMODE);

	/* now the background activities... Only for FP Kernel */
	for (;;)
		;
	return 0;
}
