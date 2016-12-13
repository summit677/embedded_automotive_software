#include <ee.h>
#include <ee_irq.h>
#include <stdio.h>

DeclareTask(TaskSlave1);

TASK(TaskSlave1)
{
	while(1){
		LEDCTL_On(EE_LED_1);
		graph_fillRectangle(20, 20, 40, 40, COL_YELLOW);
		printScrn();
		LEDCTL_Off(EE_LED_1);
	}

	TerminateTask();
}


/*
 * MAIN TASK
 */
int main(void)
{
	DISPLAY_BackgroundColor(COL_WHITE);
	clearDisplayBuffer();

	StartOS(DONOTCARE);
}
