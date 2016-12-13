
#include <ee.h>
#include <ee_irq.h>

DeclareTask(TaskSlave2);

TASK(TaskSlave2)
{
	while(1){
		LEDCTL_On(EE_LED_2);
		delay_ms(100);	/* dummy work */
		LEDCTL_Off(EE_LED_2);
	}
	TerminateTask();
}


/*
 * MAIN TASK
 */
int main(void)
{
	StartOS(DONOTCARE);
}
