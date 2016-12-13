#include <ee.h>
#include <ee_irq.h>


DeclareTask(TaskMaster);


TASK(TaskMaster)
{
	while(1){
		GetSpinlock (SpinlockScreen);
		graph_fillRectangle(20, 20, 40, 40, COL_BLUE);


		GetSpinlock (SpinlockLed);
		LEDCTL_Toggle(EE_LED_0);
		ReleaseSpinlock (SpinlockLed);


		printScrn();
		ReleaseSpinlock (SpinlockScreen);
	}
	TerminateTask();
}



/*
 * MAIN TASK
 */
int main(void)
{
	LEDCTL_Init();
	DISPLAY_Init();
	DISPLAY_BackgroundColor(COL_WHITE);
	clearDisplayBuffer();
	printScrn();
	
	StatusType start_core_status; /* return values will be ignored */
	StartCore(OS_CORE_ID_1, &start_core_status);

	StartCore(OS_CORE_ID_2, &start_core_status);

	StartOS(DONOTCARE);
}

