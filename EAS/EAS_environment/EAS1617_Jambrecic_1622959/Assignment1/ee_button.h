/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2011 Steve Langstaff
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation,
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

/*
 * Author: 2014,  Christoph Kreuzberger (for TC2X5)
 * Modified: 2015 Fabian Mauroner
 */
 
 


#ifndef INCLUDE_EE_APPKIT_TC2X7_BOARD_BUTTON_H__
#define INCLUDE_EE_APPKIT_TC2X7_BOARD_BUTTON_H__

#include "mcu/infineon_common_tc2Yx/inc/ee_tc2Yx_mcu.h"


/*
   The param intvec is used to configure the level of interrupt vector that
   will serve the request.
*/

__INLINE__ void __ALWAYS_INLINE__ EE_tc2x7_button_irq_init( EE_TYPEISR2PRIO intvec )
{
  /*  On reset all IOCR (input/output configuration register) configure port
      as input connected to pull-up, that is what we need, so we wont change
      it, but if the internal pull-up will have problem with button pull-up
      I have just to enable the following line */
  /* P20_IOCR0.PC0 = EE_TC29X_INPUT_TRISTATE; */

  /* P20.0 is input selection 0 */
  SCU_EICR3.B.EXIS0 = 0U;

  /*  The button will shortcut the pin with GND when pressed ->
      we will use the falling edge to generate the interrupt */
	
  SCU_EICR3.B.FEN0      = 0;
  SCU_EICR3.B.REN0      = 1;
  SCU_EICR3.B.EIEN0     = 0;
  SCU_EICR3.B.LDEN0		= 0;
  SCU_EICR3.B.INP0 		= 0;
  
  SCU_IGCR0.B.IPEN06 	= 1;
  SCU_IGCR0.B.IGP0 		= 2;
  SCU_IGCR0.B.GEEN0 	= 1;
	
	

  /* I will use OGU0 (Output Gate-in Unit 0) -> SRC_SCUERU0
   *  [0..7] SRPN = INTERRUPT_NR
   *  [10] Service Request enable
   *  [11..12] Type Of Service (means which CPU will handle it)
   */
  SRC_SCUERU0.U = EE_TC2YX_SRN_TYPE_OF_SERVICE(EE_CURRENTCPU) |
    EE_TC2YX_SRN_ENABLE | EE_TC2YX_SRN_PRIORITY(intvec);
}

__INLINE__ void __ALWAYS_INLINE__ EE_tc2x7_button_irq_clear_request( void )
{
  /* Clear the request */
  SCU_FMR.B.FC6       = 1U;
}

__INLINE__ EE_BIT __ALWAYS_INLINE__ EE_tc2x7_read_button( void )
{
  return P20_IN.B.P0 == 0;
}



#endif /* INCLUDE_EE_APPKIT_TC2X7_BOARD_BUTTON_H__ */
