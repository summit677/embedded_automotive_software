#include <ee.h>
#include <ee_irq.h>


DeclareEvent(event);

DeclareTask(TaskUi);




#define ADC_X 		20
#define ADC_Y 		20
#define ADC_WIDTH	50
#define ADC_LENGHT	255

#define ON_OFF_X 		20
#define ON_OFF_Y 		100
#define ON_OFF_WIDTH	50
#define ON_OFF_LENGHT	50

unsigned char EE_SHARED_IDATA adc = 0x80;
unsigned char EE_SHARED_IDATA on_off = 0;


TASK(TaskUi)
{
	static unsigned int on_off_timer = 0;

	TOUCH_read();
	short touch_x = TOUCH_x();
	short touch_y = TOUCH_y();

	unsigned int display_adc = -1;
	if(ADC_Y < touch_y && touch_y < ADC_Y + ADC_WIDTH){
		if(ADC_X < touch_x && touch_x < ADC_X + ADC_LENGHT)
			display_adc = touch_x - ADC_X;
		else if (touch_x < ADC_X)
			display_adc = 0;
		else if (ADC_X + ADC_WIDTH < touch_x)
			display_adc = 255;

		GetSpinlock(s);
		if(display_adc != -1)
			adc = display_adc;
												/* TODO: protect the adc variable */
		ReleaseSpinlock(s);

	}

	if(on_off_timer)
		on_off_timer--;

	if(!on_off_timer &&
			ON_OFF_Y < touch_y && touch_y < ON_OFF_Y + ON_OFF_WIDTH &&
			ON_OFF_X < touch_x && touch_x < ON_OFF_X + ON_OFF_LENGHT){

		GetSpinlock(s);
		on_off ^= 0x01;																/* TODO: protect the on_off variable */
		ReleaseSpinlock(s);

		SetEvent(TaskPin,event);														/* TODO: set the event for task TaskPin */

		SetRelAlarm(AlarmPinMonitor, 10, 0);	//1ms
		on_off_timer = 10;
	}


	graph_fillRectangle(ADC_X, 			ADC_Y, 	adc, 				ADC_WIDTH, COL_BLACK);
	graph_fillRectangle(ADC_X + adc,	ADC_Y, 	ADC_LENGHT - adc, 	ADC_WIDTH, COL_WHITE);

	char on_off_color = (on_off) ? COL_GREEN : COL_RED;
	graph_fillRectangle(ON_OFF_X, ON_OFF_Y, ON_OFF_LENGHT, ON_OFF_WIDTH, on_off_color);


	printScrn();

	TerminateTask();
}

void StartupHook( void )
{

}

void ShutdownHook( StatusType Error ) {
  /* I will not test shutdown procedure here */
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

	graph_drawRectangle(ON_OFF_X-1, ON_OFF_Y-1, ON_OFF_LENGHT+2, ON_OFF_WIDTH+2, COL_GRAY_50, 2);
	graph_drawRectangle(ADC_X-1, ADC_Y-1, ADC_LENGHT + 2, ADC_WIDTH + 2, COL_GRAY_50, 2);

	printScrn();


	StatusType start_core_status; /* return values will be ignored */
	StartCore(OS_CORE_ID_1, &start_core_status);

	StartCore(OS_CORE_ID_2, &start_core_status);

	StartOS(DONOTCARE);
}

