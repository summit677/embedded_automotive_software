/**
 * \file 
 * \brief
 *
 * \version V0.1
 * \copyright Copyright (c) 2014 Infineon Technologies AG. All rights reserved.
 *
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "TLF35584.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/


EE_UINT32 TLFStep; // this is the value of DEVCTRL Register which should be different between A and B-Step

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
static void qspi2_init(void);

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/
/** \brief TLF (QSPI) initialization
 *
 * This function initializes Qspix in master mode.
 */
void IfxTLF35584_init(void)
{
    

	qspi2_init();
   
    /* first we check for A or B-step of TLF */
    IfxTLF35584_write(0x1C01);  /* read address 0xE */
    TLFStep = (IfxTLF35584_write(0x0000)>>1) & 0xFF;  /* get back the value */
    /* TLFStep now contains 0xE8/0x08 for A-step and 0x09 for B-Step */
}

void IfxTLF35584_unprotect_register(void)
{
    IfxTLF35584_write(0x8756);  /* PROTCFG = 0xAB */
    IfxTLF35584_write(0x87DE);  /* PROTCFG = 0xEF */
    IfxTLF35584_write(0x86AD);  /* PROTCFG = 0x56 */
    IfxTLF35584_write(0x8625);  /* PROTCFG = 0x12 */
}

void IfxTLF35584_protect_register(void)
{
    IfxTLF35584_write(0x87BE);  /* PROTCFG = 0xDF */
    IfxTLF35584_write(0x8668);  /* PROTCFG = 0x34 */
    IfxTLF35584_write(0x877D);  /* PROTCFG = 0xBE */
    IfxTLF35584_write(0x8795);  /* PROTCFG = 0xCA */
}

void IfxTLF35584_disable_window_watchdog(void)
{
    /* Disable Window Watchdog */
    IfxTLF35584_write(0x8D27);  /* WDCFG0 = 0x93 */
}

void IfxTLF35584_disable_err_pin_monitor(void)
{
    /* disable ERR pin monitor */
    if (TLFStep & 0x1)
        IfxTLF35584_write(0x8811);  /* SYSPCFG1 = 0x00 */
    else
        IfxTLF35584_write(0x8811);  /* SYSPCFG0 = 0x08 */
}

void IfxTLF35584_goto_standby_state(void)
{
    if (TLFStep & 0x1)
        IfxTLF35584_write(0xABD9);  /* DEVCTRL = 0xEC */
    else
        IfxTLF35584_write(0x9DD9);  /* DEVCTRL = 0xEC */
}

void IfxTLF35584_goto_normal_state(void)
{
    /* Switch TLF to normal state */
    if (TLFStep & 0x1)
        IfxTLF35584_write(0xABD5);  /* DEVCTRL = 0xEA */
    else
        IfxTLF35584_write(0x9DD5);  /* DEVCTRL = 0xEA */
}


/** \brief Demo run API
 *
 * This function is called from main, background loop
 */
EE_UINT32 IfxTLF35584_write(EE_UINT32 send_data)
{
	EE_UINT32 read_value;
	
	//for(int i = 0; i < 20000; i++);
	
	// we wait until we have 2 entry free no fifo
	while (QSPI2_STATUS.B.TXFIFOLEVEL > 2);
	QSPI2_BACONENTRY.U = 0x10200401 | (15 << 23)
							| (0x7 << 16)
							| (0x1 << 1)
							| (0x7 << 4);	
	QSPI2_DATAENTRY1.U = send_data;
	
	
	while (QSPI2_STATUS.B.RXFIFOLEVEL != 2);	
	read_value = QSPI2_RXEXIT.U;
	(void)QSPI2_RXEXIT.U;
	return read_value;
}


static void qspi2_init(void)
{
  EE_tc2Yx_endinit_set(EE_TC_ENDINIT_DISABLE);
  QSPI2_CLC.U = 0x0;            // load clock control register
  (void)QSPI2_CLC.U;
  EE_tc2Yx_endinit_set(EE_TC_ENDINIT_ENABLE);
  
  P14_IOCR0.B.PC2 	= 0x13;       // chip select (SLSO21)
  P15_IOCR0.B.PC3 	= 0x13;		// SCLK2				
  P15_IOCR4.B.PC6 	= 0x13;		// MTSR2  
  QSPI2_PISEL.U = (0x1 << 0);	// MRST2B
  
  QSPI2_GLOBALCON.U = 0x00003E01;
  QSPI2_GLOBALCON1.U = (0x4 << 20) | 0x0000fe7f;
  QSPI2_SSOC.U = 0x00020000;

  
  QSPI2_ECON1.U = 0x0558;       // 500ns SCLK
  QSPI2_GLOBALCON.B.EN = 1;     // Run request, nothing is ongoing no check
}

