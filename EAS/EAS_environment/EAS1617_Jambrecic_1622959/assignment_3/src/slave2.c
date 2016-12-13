
#include <ee.h>
#include <ee_irq.h>

DeclareTask(TaskSlave2);

TASK(TaskSlave2)
{
	while(1){
		GetSpinlock (SpinlockLed);
		LEDCTL_On(EE_LED_2);
		ReleaseSpinlock (SpinlockLed);
		delay_ms(100);	/* dummy work */
		GetSpinlock (SpinlockLed);
		LEDCTL_Off(EE_LED_2);
		ReleaseSpinlock (SpinlockLed);
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
