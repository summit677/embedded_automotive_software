#include <ee.h>
#include <ee_irq.h>
#include <stdio.h>

DeclareTask(TaskSlave1);

TASK(TaskSlave1)
{
	while(1){
		GetSpinlock (SpinlockLed);
		LEDCTL_On(EE_LED_1);
		ReleaseSpinlock (SpinlockLed);

		GetSpinlock (SpinlockScreen);
		graph_fillRectangle(20, 20, 40, 40, COL_YELLOW);
		printScrn();
		ReleaseSpinlock (SpinlockScreen);

		GetSpinlock (SpinlockLed);
		LEDCTL_Off(EE_LED_1);
		ReleaseSpinlock (SpinlockLed);
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
