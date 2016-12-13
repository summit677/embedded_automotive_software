
#include <ee.h>
#include <ee_irq.h>

DeclareTask(TaskMonitorPwm);
DeclareTask(TaskMonitorPin);


static int getPinValue(){ return P14_IN.B.P7; }
static int getPwmValue(){ return P33_IN.B.P10;}




extern unsigned char EE_SHARED_IDATA adc;
extern unsigned char EE_SHARED_IDATA on_off;


TASK(TaskPwmM)
{
	int pwm_ok;
	static unsigned char pwm_counter = 0;
	static unsigned char pwm_value = 0;



	if(pwm_counter < pwm_value)	pwm_ok = (getPwmValue() == 1) ? 1 : 0;
	else						pwm_ok = (getPwmValue() == 0) ? 1 : 0;


	if(++pwm_counter == 255){
		pwm_counter = 0;
		pwm_value = on_off ? adc : 0;							/* TODO: protect the adc and pwm_value variable */
	}


	if(pwm_ok)			LEDCTL_Off(EE_LED_0);
	else				LEDCTL_On(EE_LED_0);

	/* DEBUG */
	if(getPwmValue())	LEDCTL_On(EE_LED_2);
	else				LEDCTL_Off(EE_LED_2);
	/* ------ */

	TerminateTask();
}


TASK(TaskPinM)
{

	int pin_ok = (getPinValue() == on_off);						/* TODO: protect the adc and pwm_value variable */


	if(pin_ok)		LEDCTL_Off(EE_LED_1);
	else			LEDCTL_On(EE_LED_1);

	// DEBUG
	if(getPinValue())	LEDCTL_On(EE_LED_3);
	else				LEDCTL_Off(EE_LED_3);
	// ------

	TerminateTask();
}


/*
 * MAIN TASK
 */
int main(void)
{

	StartOS(DONOTCARE);
}
