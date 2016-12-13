/**
 * \file TLF35584.h
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
 *
 * \defgroup IfxLld_Demo_TLF35584_SrcDoc_Main Demo Source
 * \ingroup IfxLld_Demo_TLF35584_SrcDoc
 * \defgroup IfxLld_Demo_TLF35584_SrcDoc_Main_Interrupt Interrupts
 * \ingroup IfxLld_Demo_TLF35584_SrcDoc_Main
 */

#ifndef TLF35584_H
#define TLF35584_H 1

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include <ee.h>

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

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
void IfxTLF35584_init(void);
void IfxTLF35584_unprotect_register(void);
void IfxTLF35584_protect_register(void);
void IfxTLF35584_disable_window_watchdog(void);
void IfxTLF35584_disable_err_pin_monitor(void);
void IfxTLF35584_goto_normal_state(void);
void IfxTLF35584_goto_standby_state(void);
EE_UINT32 IfxTLF35584_write(EE_UINT32 send_data);

#endif  // TLF35584_H
