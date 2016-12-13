/** @file	ee_board_common_led.h
 *
 *  @brief	Erika Enterprise - LED Driver - BOARD Common Header File.
 *
 *  <!--
 *    ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 *    Copyright (C) 2002-2013  Evidence Srl
 *
 *    This file is part of ERIKA Enterprise.
 *
 *    ERIKA Enterprise is free software; you can redistribute it
 *    and/or modify it under the terms of the GNU General Public License
 *    version 2 as published by the Free Software Foundation, 
 *    (with a special exception described below).
 *
 *    Linking this code statically or dynamically with other modules is
 *    making a combined work based on this code.  Thus, the terms and
 *    conditions of the GNU General Public License cover the whole
 *    combination.
 *
 *    As a special exception, the copyright holders of this library give you
 *    permission to link this code with independent modules to produce an
 *    executable, regardless of the license terms of these independent
 *    modules, and to copy and distribute the resulting executable under
 *    terms of your choice, provided that you also meet, for each linked
 *    independent module, the terms and conditions of the license of that
 *    module.  An independent module is a module which is not derived from
 *    or based on this library.  If you modify this code, you may extend
 *    this exception to your version of the code, but you are not
 *    obligated to do so.  If you do not wish to do so, delete this
 *    exception statement from your version.
 *
 *    ERIKA Enterprise is distributed in the hope that it will be
 *    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 *    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License version 2 for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    version 2 along with ERIKA Enterprise; if not, write to the
 *    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *    Boston, MA 02110-1301 USA.
 *  -->
 *
 *  The LED driver provides LED services for Clock and RAM initialization.
 *  In the LED configuration set, the LED specific settings for the Clock
 *  (i.e. PLL setting) and RAM (i.e. section base address and size) shall be
 *  configured.
 *
 *  The LED module shall provide a service to provide software triggering of a
 *  hardware reset.
 *
 *  @note	Only an authorized user shall be able to call this reset service
 *  		function.
 *
 *  The LED module shall provide services to get the reset reason of the last
 *  reset if the hardware supports such a feature.
 *
 *  @note	In an ECU, there are several sources which can cause a reset.
 *  		Depending on the reset reason, several application scenarios
 *  		might be necessary after reinitialization of the LED.
 * 
 *  @author	Riccardo Schiavi
 *  @author	Giuseppe Serano
 *  @version	0.1
 *  @date	2013
 */

#ifndef	__EE_BOARD_COMMON_LED_H__
#define	__EE_BOARD_COMMON_LED_H__

/** @brief	Numeric ID of a LED.
 *
 *  Numeric ID of a LED.
 *  Implementation specific.
 *  But not all values may be valid within this type.
 *  This type shall be chosen in order to have the most efficient implementation
 *  on a specific micro controller platform.
 *
 *  Parameters of type <tt>EE_TYPELED</tt> contain the numeric ID of a LED.
 *
 *  The mapping of the ID is implementation specific but not configurable and
 *  not all values may be valid within this type.
 *
 *  For parameter values of type <tt>EE_TYPELED</tt>, the Led's user shall use
 *  the symbolic names provided by the driver implementation.
 *
 *  The type shall be EE_UREG depending on uC platform.
 */
#ifndef	EE_TYPELED
#define	EE_TYPELED		EE_UREG
#endif

/** @brief	Invalid Configuration Flags.	*/
#define	E_LED_PARAM_FLAGS	0xFE

/** @brief	LED Configuration Flags.
 *
 *  Type for setting-up a LED Driver Configuration.
 */
#ifndef	EE_TYPELEDFLAGS
#define	EE_TYPELEDFLAGS	EE_UREG
#endif

/** @brief	LED Driver Default Configuration Flag. */
#define	EE_LED_FLAG_DEFAULT	0x00U

/** @brief	LED Driver Initialization.
 *  @param[in]	Led	Led to be Initialized.
 *  @param[in]	Flags	LED Configuration Flags.
 *  @return	Extended Status Only:
 *  	- E_OK:			LED Driver Initializion Success.
 *  	- E_NOT_OK:		LED Driver Already Initialized.
 *  	- E_LED_PARAM_FLAG:	LED Configuration Flags Invalid.
 *
 *  This service initializes the LED driver.
 *
 *  The function <tt>EE_Board_Init()</tt> shall initialize the LED module.
 *
 *  If Extended Status for the LED module is enabled: If the LED Driver has
 *  ALREADY been initialized, the function <tt>EE_Board_Init()</tt> shall return
 *  the development error <tt>E_NOT_OK</tt>.
 *
 *  If Extended Status for the LED module is enabled: If the parameter
 *  <tt>Flags</tt> is invalid (not within the range specified by
 *  implementation), the function <tt>EE_Board_Init()</tt> shall return the
 *  development error <tt>E_LED_PARAM_FLAGS</tt>.
 *
 *  A re-initialization of the LED Driver by executing the
 *  <tt>EE_Board_Init()</tt> function requires a de-initialization before by
 *  executing a <tt>EE_Board_DeInit</tt>.
 */
#ifdef	__BOARD_EXTENDED_STATUS__
EE_TYPERET
#else
void
#endif
EE_Led_Init(
  EE_TYPELED		Led,
  EE_TYPELEDFLAGS	Flags
);

/** @brief	LED Driver De-Initialization.
 *  @param[in]	Led	Led to be De-Initialized.
 *  @param[in]	Flags	LED De-initialization Configuration Flags.
 *  @return	Extended Status Only:
 *  	- E_OK:			LED Driver De-Initializion Success.
 *  	- E_NOT_OK:		LED Driver NOT Initialized.
 *  	- E_LED_PARAM_FLAG:	LED Driver De-Initialization Configuration
 *  				Flags Invalid.
 *
 *  This service de-initializes the LED Driver.
 *
 *  The function <tt>EE_Board_DeInit()</tt> shall deinitialize the Board Module.
 *  Values of registers which are not writeable are excluded. It's the
 *  responsibility of the hardware design that the state does not lead to
 *  undefined activities in the uC.
 *
 *  If Extended Status for the LED module is enabled: If the LED Driver is
 *  NOT initialized, the function <tt>EE_Board_DeInit()</tt> shall return the
 *  development error <tt>E_NOT_OK</tt>.
 *
 *  If Extended Status for the LED module is enabled: If the parameter
 *  <tt>Flags</tt> is invalid (not within the range specified by
 *  implementation), the function <tt>EE_Board_DeInit()</tt> shall return the
 *  development error <tt>E_LED_PARAM_FLAGS</tt>.
 */
#ifdef	__BOARD_EXTENDED_STATUS__
EE_TYPERET
#else
void
#endif
EE_Led_DeInit(
  EE_TYPELED		Led,
  EE_TYPELEDFLAGS	Flags
);

#endif	/* __EE_BOARD_COMMON_LED_H__ */
