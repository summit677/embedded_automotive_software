/*
 * font.h
 *
 *  Created on: Jan 30, 2014
 *      Author: Fusseneg
 */

#ifndef INCLUDE_EE_APPKIT_TC2X7_BOARD_FONT_H__
#define INCLUDE_EE_APPKIT_TC2X7_BOARD_FONT_H__

typedef struct font {

	int xsize;
	int ysize;
	int bytes_per_line;
	char * font_bitmap;

} TFont;


// Fonts
extern const TFont FONT_STANDARD;
extern const TFont FONT_CONSOLAS_LARGE;
extern const TFont FONT_CONSOLAS_SMALL;

#endif /* INCLUDE_EE_APPKIT_TC2X7_BOARD_FONT_H__ */
