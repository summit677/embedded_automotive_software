/**
 * \file graph.h
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

#ifndef INCLUDE_EE_APPKIT_TC2X7_BOARD_GRAPH_H__
#define INCLUDE_EE_APPKIT_TC2X7_BOARD_GRAPH_H__

#include <stdlib.h>
#include <math.h>
#include "display.h"

void graph_line(int x1, int y1, int x2, int y2, int width, int color);
void graph_drawZPoint(float z, float range);
void graph_drawXYPoint(float x, float y, float range);
int graph_pointOnAxis(float v, float range);
void graph_hLine(int x, int y, int length, int width, int color);
void graph_vLine(int x, int y, int length, int width, int color);
void graph_squarePoint(int x, int y, int width, int color);
void graph_rasterCircle(int x0, int y0, int radius, int color);
void graph_fillCircle (int ix, int iy, int radius, int color);
void graph_drawRectangle(int x, int y, int width, int height, int color, int line_width);
void graph_fillRectangle(int x, int y, int width, int height, int color);
void graph_drawColorMap(int entries);

#endif /* INCLUDE_EE_APPKIT_TC2X7_BOARD_GRAPH_H__ */
