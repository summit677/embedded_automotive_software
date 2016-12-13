/**
 * \file graph.c
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


#include "../inc/graph.h"

// #ifdef __TASKING__
// double round(double number)
// {
    // return (double)((number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5));
// }
// #endif


#define GRAPH_X_TRANS		-30
#define GRAPH_Y_TRANS		+10

#define GRAPH_POINT_COLOR	12
#define GRAPH_POINT_SIZE	8

#define GRAPH_Z_POS			280

#define AXIS_LENGTH			160
#define AXIS_COLOR			0
#define AXIS_WIDTH			2
#define AXIS_USAGE			0.9

#define Z_AXIS_COLOR		0
#define Z_AXIS_WIDTH		2
#define Z_AXIS_RASTER_COLOR	0
#define Z_AXIS_RASTER_WIDTH	1

#define Z_AXIS_RULER_LENGTH	30
#define Z_AXIS_RULER_WIDTH	8


void graph_line(int x1, int y1, int x2, int y2, int width, int color)
{
	if (x1 == x2)
	{
		if (y2 > y1)
		{
			graph_vLine(x1, y1, y2 - y1, width, color);
		}
		else
		{
			graph_vLine(x1, y2, y1 - y2, width, color);
		}
	}
	else if (y1 == y2)
	{
		if (x2 > x1)
		{
			graph_hLine(x1, y1, x2 - x1, width, color);
		}
		else
		{
			graph_hLine(x2, y1, x1 - x2, width, color);
		}
	}
	else
	{
		// only 1 or even widths are allowed
		if (width != 1 && width % 2 != 0)
		{
			width++;
		}

		float sl;
		int i, j, temp;


		if (abs(x1 - x2) > abs(y1 - y2))
		{
			if (x2 < x1 )
			{
				temp = x1;
				x1 = x2;
				x2 = temp;

				temp = y1;
				y1 = y2;
				y2 = temp;
			}

			sl = (float)(y2 - y1) / (float)(x2 - x1);

			for (i = x1; i < x2; i++)
			{
				for (j = 0; j < width; j++)
				{
					setPx(i, (int)round(((sl * (float)(i - x1)) + y1) + j - (width / 2)), (uint8_t) color);
				}
			}
		}
		else
		{
			if (y2 < y1)
			{
				temp = x1;
				x1 = x2;
				x2 = temp;

				temp = y1;
				y1 = y2;
				y2 = temp;
			}

			sl = (float)(x2 - x1) / (float)(y2 - y1);
			for (i = y1; i < y2; i++)
			{
				for (j = 0; j < width; j++)
				{
					setPx((int)round(((sl * (float)(i - y1)) + x1) + j - (width/2)), i, (uint8_t) color);
				}
			}
		}
	}
}

void graph_drawZPoint(float z, float range)
{
	int dz = graph_pointOnAxis(z, range);
	graph_fillCircle(GRAPH_Z_POS, TFT_YSIZE/2 + GRAPH_Y_TRANS + dz, Z_AXIS_RULER_WIDTH, GRAPH_POINT_COLOR);
}

void graph_drawXYPoint(float x, float y, float range)
{
	int dx = graph_pointOnAxis(x, range);
	int dy = graph_pointOnAxis(y, range);

	graph_fillCircle(TFT_XSIZE/2 + GRAPH_X_TRANS + dx, TFT_YSIZE/2 + GRAPH_Y_TRANS + dy, GRAPH_POINT_SIZE, GRAPH_POINT_COLOR);

}

int graph_pointOnAxis(float v, float range)
{
	return ((int)(v / range) * ((AXIS_LENGTH * AXIS_USAGE) / 2));
}

void graph_hLine(int x, int y, int length, int width, int color)
{
	// only 1 or even widths are allowed
	if (width != 1 && width % 2 != 0)
	{
		width++;
	}

	int i = 0;
	for (i = x; i < x+length; i++)
	{
		if (width == 1)
		{
			setPx(i, y, (uint8_t) color);
		}
		else
		{
			int j = 0;
			for (j = 0; j < width; j++)
			{
				setPx(i, y-(width/2)+j, (uint8_t) color);
			}
		}
	}
}

void graph_vLine(int x, int y, int length, int width, int color)
{
	// only 1 or even widths are allowed
	if (width != 1 && width % 2 != 0)
	{
		width++;
	}

	int i = 0;
	for (i = y; i < y+length; i++)
	{

		if (width == 1)
		{
			setPx(x, i, (uint8_t) color);
		}
		else
		{
			int j = 0;
			for (j = 0; j < width; j++)
			{
				setPx(x-(width/2)+j, i, (uint8_t) color);
			}

		}
	}
}

void graph_squarePoint(int x, int y, int width, int color)
{
	// only 1 or even widths are allowed
	if (width != 1 && width % 2 != 0)
	{
		width++;
	}

	if (width == 1)
	{
		setPx(x, y, (uint8_t) color);
	}

	int i = 0;
	int j = 0;

	for (i = 0; i < width; i++)
	{
		for (j = 0; j < width; j++)
		{
			setPx(x-(width/2)+i, y-(width/2)+j, (uint8_t) color);
		}
	}

}

void graph_rasterCircle(int x0, int y0, int radius, int color)
{
	int error = 1 - radius;
	int errorY = 1;
	int errorX = -2 * radius;
	int x = radius, y = 0;

	setPx(x0, y0 + radius, (uint8_t) color);
	setPx(x0, y0 - radius, (uint8_t) color);
	setPx(x0 + radius, y0, (uint8_t) color);
	setPx(x0 - radius, y0, (uint8_t) color);

	while (y < x)
	{
		if (error > 0) // >= 0 produces a slimmer circle. =0 produces the circle picture at radius 11 above
		{
			x--;
			errorX += 2;
			error += errorX;
		}
		y++;
		errorY += 2;
		error += errorY;
		setPx(x0 + x, y0 + y, (uint8_t) color);
		setPx(x0 - x, y0 + y, (uint8_t) color);
		setPx(x0 + x, y0 - y, (uint8_t) color);
		setPx(x0 - x, y0 - y, (uint8_t) color);
		setPx(x0 + y, y0 + x, (uint8_t) color);
		setPx(x0 - y, y0 + x, (uint8_t) color);
		setPx(x0 + y, y0 - x, (uint8_t) color);
		setPx(x0 - y, y0 - x, (uint8_t) color);
	}
}

void graph_fillCircle (int ix, int iy, int radius, int color)
{
	int x, y;

	for (y = -radius; y <= radius; y++)
		for (x = -radius; x <= radius; x++)
			if ((x * x) + (y * y) <= (radius * radius))
				setPx(ix+x, iy+y, (uint8_t) color);
}

void graph_drawRectangle(int x, int y, int width, int height, int color, int line_width)
{
	// only 1 or even widths are allowed
	if (line_width != 1 && line_width % 2 != 0)
	{
		line_width++;
	}

	graph_hLine(x, y, width, line_width, color);
	graph_hLine(x, y + height, width, line_width, color);

	graph_vLine(x, y-line_width/2, height + line_width, line_width, color);
	graph_vLine(x+width, y-line_width/2, height + line_width, line_width, color);
}

void graph_fillRectangle(int x, int y, int width, int height, int color)
{
	int i, j;
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			setPx(x + i, y + j, (uint8_t) color);
		}
	}

}

void graph_drawColorMap(int entries)
{
	int i, x = 5, y = 5, size = 20;
	for (i = 0; i < entries+1; i++)
	{
		graph_fillRectangle(x, y, size, size, i);

		x += size;
		if (x > size * 10)
		{
			y += size;
			x = 5;
		}

	}
}
