/**
 * \file display.h
 * \copyright Copyright (c) 2013 Infineon Technologies AG. All rights reserved.
 *
 *
 * 1.0
 * 2013-12-16
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
 */

/*
 *   Simple display driver for TC2X7 Application Kit
 *
 *
 */

#ifndef INCLUDE_EE_APPKIT_TC2X7_BOARD_DISPLAY_H__
#define INCLUDE_EE_APPKIT_TC2X7_BOARD_DISPLAY_H__

#include "ee.h"
#include "mcu/infineon_common_tc2Yx/inc/ee_tc2Yx_endinit.h"

#include "util.h"
#include <stdio.h>
#include <string.h>
#include "font.h"
#include <stdint.h>


#define TFT_XSIZE			320		//!< x dimension of tft display
#define TFT_YSIZE			240		//!< y dimension of tft display

#define XMAX_TOUCH   3700.0f		//maybe a option is needed to trim the values
#define XMIN_TOUCH   240.0f
#define YMIN_TOUCH   300.0f
#define YMAX_TOUCH   3740.0f

#define RGB(r, g, b)		((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | (((b) & 0xF8) >> 3))
#define COLOR_DEPTH			4
#define COLOR_GRAYSCALES	9
#define COLOR_CUSTOM_COLORS 2

#define COL_BLACK			0
#define COL_WHITE			63
#define COL_RED				3
#define COL_LIGHT_RED		23
#define COL_GREEN			8
#define COL_LIGHT_GREEN		12
#define COL_YELLOW			15
#define COL_ORANGE			11
#define COL_BLUE			32
#define COL_DARK_BLUE		16
#define COL_LIGHT_BLUE		56
#define COL_INFINEON_RED	64
#define COL_INFINEON_BLUE	65

#define COL_GRAY_90			66
#define COL_GRAY_80			67
#define COL_GRAY_70			68
#define COL_GRAY_60			69
#define COL_GRAY_50			70
#define COL_GRAY_40			71
#define COL_GRAY_30			72
#define COL_GRAY_20			73
#define COL_GRAY_10			74

#define BGCOLOR		COL_WHITE

void setPx(int x, int y, uint8_t color); //for direct use
void printScrn(void); //for direct use
void drawChar(char ch,int x0, int y0, int color, TFont font); //for direct use
void drawString(int x, int y, int count, char* chars, int color, TFont font, int centerx, int centery); //for direct use

void writePixel(int x, int y, uint8_t color);
void writeDisplay (void);

void DISPLAY_Init(void);
void DISPLAY_Clear(void);
void DISPLAY_Buffer_Init(void);
void DISPLAY_Framebuffer_Init(void);
void DISPLAY_colorTable_StdInit(void);


void DISPLAY_BackgroundColor(uint8_t color);
void clearDisplayBuffer(void);

void TOUCH_read(void);
short TOUCH_x(void);
short TOUCH_y(void);

#endif /* INCLUDE_EE_APPKIT_TC2X7_BOARD_DISPLAY_H__ */
