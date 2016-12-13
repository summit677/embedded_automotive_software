#define LEDCTL_LED_FIRST	0
#define LEDCTL_LED_COUNT 	4

#define LED_ON_TIME_DELTA	10000000

typedef enum myLEDS {
  EE__NO_LEDS = 0x00,
  EE_LED_0 = 0x01,
  EE_LED_1 = 0x02,
  EE_LED_2 = 0x04,
  EE_LED_3 = 0x08,
  EE__ALL_LEDS = 0x0F
} EE_tc2x7_led_id;


/** @brief Initialize LED port
 *  @return Number of LEDs
 */
__INLINE__ int LEDCTL_Init(void){	
	P13_OMR.U |= (EE__ALL_LEDS);
	P13_IOCR0.U = 0x80808080;
	return LEDCTL_LED_COUNT;
}

/** @brief Toggle one LED
 *  @param led The LED to be toggled 
 *	@see EE_tc2x7_led_id
 */
__INLINE__ void LEDCTL_Toggle(EE_tc2x7_led_id led){
  P13_OMR.U = (led << 16) + led; // PCLx=1; PSx=1 -> toggle pin (LEDs low active)
}

/** @brief Turns one LED off
 *  @param led The LED to be turned off 
 *	@see EE_tc2x7_led_id
 */
__INLINE__ void LEDCTL_Off(EE_tc2x7_led_id led){
  P13_OMR.U = led; // PCLx=0; PSx=1 -> set pin (LEDs low active)
}

/** @brief Turns one LED on
 *  @param led The LED to be turned on
 *	@see EE_tc2x7_led_id
 */
__INLINE__ void LEDCTL_On(EE_tc2x7_led_id led){
  P13_OMR.U = (led << 16); // PCLx=1; PSx=0 -> reset pin (LEDs low active) 
}