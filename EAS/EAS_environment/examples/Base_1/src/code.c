#include "ee.h"
#include "ee_irq.h"




/* Let's declare the tasks identifiers */
DeclareTask(Task_Event);
DeclareTask(Task_Alarm);
DeclareTask(Task_Mix);



TASK(Task_Event)
{
	TerminateTask();
}

TASK(Task_Alarm)
{
	TerminateTask();
}

TASK(Task_Mix)
{
	TerminateTask();
}



int main( void )
{
	/* let's start the multiprogramming environment...*/
	StartOS(OSDEFAULTAPPMODE);

	/* now the background activities... Only for FP Kernel */
	for (;;)
		;
	return 0;
}
