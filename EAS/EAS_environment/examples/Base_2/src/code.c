#include "ee.h"
#include "ee_irq.h"




/* Let's declare the tasks identifiers */
DeclareTask(Task1);
DeclareTask(task2);



volatile unsigned int clock;

TASK(Task1)
{
	const unsigned int hour = (clock / 3600) % 24;
	const unsigned int minutes = (clock % 3600) / 60;
	const unsigned int seconds = clock % 60;
	char time[9];

	LEDCTL_Toggle(EE_LED_1);
	sprintf(time, "%.2u:%.2u:%.2u", hour, minutes, seconds);

	drawString(20, 60, strlen(time), time, COL_BLACK, FONT_STANDARD,0,0 );
	printScrn();

	clock++;

	TerminateTask();
}


TASK(Task2)
{
	unsigned char color = 0;

	LEDCTL_Toggle(EE_LED_2);
	graph_fillRectangle(20, 20, 40, 40, color++);
	printScrn();

	TerminateTask();
}



int main( void )
{
	LEDCTL_Init();
	DISPLAY_Init();
	DISPLAY_BackgroundColor(COL_WHITE);
	clearDisplayBuffer();
	printScrn();

    /* let's start the multiprogramming environment...*/
    StartOS(OSDEFAULTAPPMODE);

    /* now the background activities... Only for FP Kernel */
    for (;;)
        ;
    return 0;
}
