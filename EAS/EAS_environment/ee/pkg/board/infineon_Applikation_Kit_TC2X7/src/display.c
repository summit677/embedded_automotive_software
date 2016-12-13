/**
 * \file display.c
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

/*! \file display.c
 *   Simple display driver implementation for TC2X5 Application Kit
 *
 *
 *  \version
 *    04.02.2013  initial version derived from Infineon demo application
 *    21.02.2013  Adaption for TriCore2 Free Toolchain
 *    22.02.2013  DISPLAY_Buffer_Write function corrected
 *    13.03.2013  DISPLAY_Buffer_Putc function corrected
 *    14.03.2013  Adaption for new display type (Infineon)
 *
 */

#include "../inc/display.h"

static int qspi0_init (void);
static void tft_write_data (unsigned short uiValue);
static void tft_write_data_last (unsigned short uiValue, unsigned short last);
static void tft_write_command (unsigned char regaddr);
static void tft_write_command_data (unsigned char regaddr, unsigned short data);
static unsigned short tft_read_data (unsigned char regaddr);
static unsigned short tft_read_datas (unsigned char regaddr);
static unsigned short tft_get_data (void);
static void tft_terminate_access (void);
static void tft_display_home (unsigned long x, unsigned long y);
static void tft_init (void);
static void tft_clrscr (void);



#define FRAMEBUFFER_LEN	(TFT_XSIZE*TFT_YSIZE)

unsigned char frameBuffer[FRAMEBUFFER_LEN];// = {0};
unsigned short colorTable[COLOR_DEPTH*COLOR_DEPTH*COLOR_DEPTH + COLOR_CUSTOM_COLORS + COLOR_GRAYSCALES];// = {0};		//color depth + custom colors + graycolors
volatile uint8_t BackgroundColor = COL_BLACK;
volatile int defaultDispBufReady = 0;
short touch_x;
short touch_y;

#if EE_MAX_CPU == 1 || !defined(EE_BUILD_SINGLE_ELF)
volatile unsigned short tft_id;
#elif defined(EE_MASTER_CPU)
volatile unsigned short EE_SHARED_IDATA tft_id;
#else
extern volatile unsigned short EE_SHARED_IDATA tft_id;
#endif /* EE_MASTER_CPU */

//set BackgroundColor
void DISPLAY_BackgroundColor(uint8_t color){
  BackgroundColor = color;
}

//initializes the display buffer and the color table
void DISPLAY_Buffer_Init(void)
{
	DISPLAY_colorTable_StdInit();
	DISPLAY_Framebuffer_Init();
}

//Initialize frame buffer with color 1 of the colorTable
void DISPLAY_Framebuffer_Init(void)
{
	int i;
	for(i=0;i<(FRAMEBUFFER_LEN);i++){
		frameBuffer[i] = 1;
	}
}

void DISPLAY_colorTable_StdInit(void)
{
	int i, j, r, g, b, step;

	i = 0;
	step = 255 / (COLOR_DEPTH-1);

	for(r = 0; r < COLOR_DEPTH; r++){
		for(g = 0; g < COLOR_DEPTH; g++){
			for(b = 0; b < COLOR_DEPTH; b++){
				colorTable[i++] = RGB(b*step, g*step, r*step);
			}
		}
	}

	//Infineon Colors
	colorTable[i++] = RGB(227, 0, 55);
	colorTable[i++] = RGB(0, 94, 168);

	step = 255 / (COLOR_GRAYSCALES+1);

	//Grayscales
	for(j = 0; j < COLOR_GRAYSCALES; j++){
		colorTable[i+j] = RGB((j+1)*step, (j+1)*step, (j+1)*step);
	}

}

//Paint the whole Display Buffer onto the display
void writeDisplay(void)
{
	tft_display_home (0, 0);
	tft_write_command(0x2C);

	int i;
	for(i = 0; i < FRAMEBUFFER_LEN; i++){
		tft_write_data_last(colorTable[frameBuffer[i]], (i < FRAMEBUFFER_LEN-1)?0x00:0x01);
	}
}

//change the color of one pixel in the display buffer with the specified color
void writePixel(int x, int y, uint8_t color)
{
	frameBuffer[y*TFT_XSIZE+x] = color; //color;
}

//change the color of one pixel in the display buffer with the specified color
//initializes the displayBuffer if not already done
void setPx(int x, int y, uint8_t color){
	if (x<0 || x>=320) return;
	if (y<0 || y>=240) return;
	if(!defaultDispBufReady)
	{
		DISPLAY_Buffer_Init();
		defaultDispBufReady = 1;
	}
	writePixel(x, y, color);
}

//draw the contents of the display buffer onto the display
void printScrn(void){
	if(!defaultDispBufReady)
	{
		DISPLAY_Buffer_Init();
		defaultDispBufReady=1;
	}
	writeDisplay();
}

//draws a character at the specified location
void drawChar(char ch,int x0, int y0, int color, TFont font)
{

	int x, y, j, bytesPerLine;
	unsigned char letter, mask;
	char * bitmap_ptr;

	bitmap_ptr = font.font_bitmap;
	bytesPerLine = font.bytes_per_line;

	for (y = 0; y < font.ysize ; y++)
	{

		for(j = 0; j < bytesPerLine; j++){

			mask = 0x80;
			for (x = 0; x < 8; x++)       //each character has 8 x values
			{

				letter = *(bitmap_ptr + (((ch+1) * font.ysize * bytesPerLine) + j - y * bytesPerLine));
				if ((letter & mask) != 0)
				{
					setPx(((bytesPerLine-j-1)*8) + x + x0 - (font.bytes_per_line * 8 - font.xsize), y+y0,(unsigned char) color);
				}
				// CK
				else
				  {
					setPx(((bytesPerLine-j-1)*8) + x + x0 - (font.bytes_per_line * 8 - font.xsize), y+y0,(unsigned char) BackgroundColor);

				  }

				mask = mask >> 1;

			}

		}
	}
}

//draws a String into the frameBuffer at the specified location
void drawString(int x, int y, int count, char* chars, int color, TFont font, int centerx, int centery){
	int i;
	int x0,y0;

	// one line only
	if(centerx == 1){
		x = x - (count * font.xsize) / 2;
	}

	// one line only
	if(centery == 1){
		y = y - font.ysize / 2;
	}

	x0 = x;
	y0 = y;

	for(i=0;i<count;i++){
		if(y0 > (TFT_YSIZE-font.ysize)){
			break;
		}
		if(chars[i] == '\n'){
			y0 -= font.ysize;
			x0 = x;
		}else{
			drawChar(chars[i], x0, y0, color, font);
			x0 += font.xsize;
			if(x0 > TFT_XSIZE-font.xsize){
				x0 = x;
				y0 -= font.ysize;
			}
		}
	}
}

void drawBinary(unsigned char in, int x, int y, int color, TFont font){

	unsigned char val = in;

	int i = 0;
		for(i = 0; i < 8; i++){

			unsigned char mask = val & 0x1;

			if(mask == 0x1){
				drawString(x-10*i, y, 1, 	"1", color, font, 0, 0);
			} else {
				drawString(x-10*i, y, 1, 	"0", color, font, 0, 0);
			}

			val = val >> 1;

		}

}

//Initialize the qspi driver and tft drivers
void DISPLAY_Init(void)
{
	/* we enable the background light Application Kit */
	P20_OUT.B.P13 = 1;
	P20_IOCR12.B.PC13 = 0x10;

	qspi0_init(); //initializes qspi driver
	tft_init(); //initialzes tft driver
}

//fill the frameBuffer with BGCOLOR
void DISPLAY_Clear(void)
{
	tft_clrscr();
}


void TOUCH_read(void)
{
	unsigned char tx_buffer[5] = {0x90, 0x00, 0xD0, 0x00, 0x00};
	unsigned char rx_buffer[5];

	/* the display function doesn't perform a read after writing a data packed for performance reasons*/
	while (QSPI0_STATUS.B.RXFIFOLEVEL)
		(void)QSPI0_RXEXIT.U;

	while (QSPI0_STATUS.B.TXFIFOLEVEL > 1);			/* send the first 2 bytes */
	QSPI0_BACONENTRY.U = 0x90278400 | (7 << 23);
	QSPI0_DATAENTRY0.U = tx_buffer[0];
	QSPI0_DATAENTRY0.U = tx_buffer[1];
	while (QSPI0_STATUS.B.RXFIFOLEVEL != 4);
	rx_buffer[0] = QSPI0_RXEXIT.U;
	(void)QSPI0_RXEXIT.U;	// ignore status
	rx_buffer[1] = QSPI0_RXEXIT.U;
	(void)QSPI0_RXEXIT.U;	// ignore status

	while (QSPI0_STATUS.B.TXFIFOLEVEL > 2);
	QSPI0_DATAENTRY0.U = tx_buffer[2];
	QSPI0_DATAENTRY0.U = tx_buffer[3];
	while (QSPI0_STATUS.B.RXFIFOLEVEL != 4);
	rx_buffer[2] = QSPI0_RXEXIT.U;
	(void)QSPI0_RXEXIT.U;	// ignore status
	rx_buffer[3] = QSPI0_RXEXIT.U;
	(void)QSPI0_RXEXIT.U;	// ignore status

	while (QSPI0_STATUS.B.TXFIFOLEVEL > 2);
	QSPI0_BACONENTRY.U = 0x90278401 | (7 << 23);	/* send the last bytes */
	QSPI0_DATAENTRY0.U = tx_buffer[4];
	while (QSPI0_STATUS.B.RXFIFOLEVEL != 2);
	rx_buffer[4] = QSPI0_RXEXIT.U;
	(void)QSPI0_RXEXIT.U;	// ignore status

	short x = ((rx_buffer[1]<<8) | (rx_buffer[2])) >> 3;
	short y = ((rx_buffer[3]<<8) | (rx_buffer[4])) >> 3;

	touch_x = ((float)x - XMIN_TOUCH) / (XMAX_TOUCH - XMIN_TOUCH) * (float)TFT_XSIZE;
	touch_y = ((float)y - YMIN_TOUCH) / (YMAX_TOUCH - YMIN_TOUCH) * (float)TFT_YSIZE;
}

short TOUCH_x(void)
{
	return (P20_IN.B.P0 == 1) ? -1 : touch_x;
}

short TOUCH_y(void)
{
	return (P20_IN.B.P0 == 1) ? -1 : touch_y;
}

static int qspi0_init(void)
{
  EE_tc2Yx_endinit_set(EE_TC_ENDINIT_DISABLE);
  QSPI0_CLC.U = 0x0;            // load clock control register
  (void)QSPI0_CLC.U;
  P20_PDR0.U = 0x0;
  P20_PDR1.U = 0x00080000;      // we set our MRST0 to TTL level because the CPLD drives with 3,3V
  EE_tc2Yx_endinit_set(EE_TC_ENDINIT_ENABLE);
  P20_IOCR0.B.PC3 = 0x13;       // chip select touch (SLSO09)
  P20_IOCR4.B.PC6 = 0x13;       // chip select cpld (SLSO08) 
  P20_IOCR8.B.PC11 = 0x13;		// SCLK0					
  P20_IOCR12.B.PC14 = 0x13;		// MTSR0
  QSPI0_GLOBALCON.U = 0x00003E00;
  QSPI0_GLOBALCON1.U = (0x4 << 20) | 0x0000fe7f;
  QSPI0_SSOC.U = 0x03000000;
  QSPI0_ECON0.U = 0x0240;       // 20ns SCLK TFT
  QSPI0_ECON1.U = 0x0558;       // 500ns SCLK
  QSPI0_ECON7.U = 0x0540;       // we use this channel to release endless mode of CPLD
  QSPI0_GLOBALCON.B.EN = 1;     // Run request, nothing is ongoing no check
  return (0);
}

static void tft_write_data(unsigned short uiValue)
{
  // we wait until we have 2 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 2);
  QSPI0_BACONENTRY.U = 0x80200400 | (15 << 23);
  QSPI0_DATAENTRY0.U = uiValue;
}

static void tft_write_data_last(unsigned short uiValue, unsigned short last)
{
  // we wait until we have 2 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 2);
  QSPI0_BACONENTRY.U = 0x80200400 | (15 << 23) | last;
  QSPI0_DATAENTRY0.U = uiValue;
}

static void tft_write_command(unsigned char regaddr)
{
  // we wait until we have 2 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 2);
  QSPI0_BACONENTRY.U = 0x80200400 | (9 << 23);
  QSPI0_DATAENTRY0.U = (0x1 << 8) | regaddr;
}

static void tft_write_command_data(unsigned char regaddr, unsigned short data)
{
  // we wait until we have 2 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 2);
  QSPI0_BACONENTRY.U = 0x80200401 | (31 << 23);
  QSPI0_DATAENTRY0.U = (regaddr << 22) | (data << 6);
}

static unsigned short tft_read_data(unsigned char regaddr)
{
  volatile unsigned long data[4];
  // we wait until we have 4 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 0);
  QSPI0_BACONENTRY.U = 0x80200400 | (15 << 23);
  QSPI0_DATAENTRY0.U = (0x1 << 15) | (regaddr << 6);
  QSPI0_BACONENTRY.U = 0x80200401 | (25 << 23);
  QSPI0_DATAENTRY0.U = 0;
  while (QSPI0_STATUS.B.RXFIFOLEVEL != 4);
  data[0] = QSPI0_RXEXIT.U;
  data[1] = QSPI0_RXEXIT.U;
  data[2] = QSPI0_RXEXIT.U;
  data[3] = QSPI0_RXEXIT.U;
  return (unsigned short) data[2];
}

static unsigned short tft_read_datas(unsigned char regaddr)
{
  volatile unsigned long data[4];
  // we wait until we have 4 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 0);
  QSPI0_BACONENTRY.U = 0x80200400 | (15 << 23);
  QSPI0_DATAENTRY0.U = (0x3 << 14) | (regaddr << 6);
  QSPI0_BACONENTRY.U = 0x80200400 | (25 << 23);
  QSPI0_DATAENTRY0.U = 0;
  while (QSPI0_STATUS.B.RXFIFOLEVEL != 4);
  data[0] = QSPI0_RXEXIT.U;
  data[1] = QSPI0_RXEXIT.U;
  data[2] = QSPI0_RXEXIT.U;
  data[3] = QSPI0_RXEXIT.U;
  return (unsigned short) data[2];
}

static unsigned short tft_get_data(void)
{
  volatile unsigned long data[2];
  // we wait until we have 4 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 0);
  QSPI0_BACONENTRY.U = 0x80200400 | (15 << 23);
  QSPI0_DATAENTRY0.U = 0;
  while (QSPI0_STATUS.B.RXFIFOLEVEL != 2);
  data[0] = QSPI0_RXEXIT.U;
  data[1] = QSPI0_RXEXIT.U;
  return (unsigned short) data[0];
}

static void tft_terminate_access(void)
{
  // we wait until we have 2 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 2);
  // this will write our last data
  QSPI0_BACONENTRY.U = 0x80200401 | (15 << 23);
  QSPI0_DATAENTRY0.U = 0;
  // we wait until we have 2 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 2);
  // this will terminate our endless transfer
  QSPI0_BACONENTRY.U = 0xF0200401 | (8 << 23);
  QSPI0_DATAENTRY0.U = 0;
}

static void tft_display_home(unsigned long x, unsigned long y)
{
  if (tft_id == 0x9341)
  {
	tft_write_command (0x2A); // Column Address Set, we change only the start (2 Parameters)
    tft_write_data ((unsigned short)(x>>8));
    tft_write_data ((unsigned short)x);
	tft_terminate_access();
	tft_write_command (0x2B); // Page Address Set, we change only the start (2 Parameters)
    tft_write_data ((unsigned short)(y>>8));
    tft_write_data ((unsigned short)y);
	tft_terminate_access();
  }
  else
  {
	if (tft_id == 0x47)
    {
      tft_write_command_data (0x0002, (unsigned short)(x>>8));
      tft_write_command_data (0x0003, (unsigned short)x); //Column Start
      tft_write_command_data (0x0006, (unsigned short)(y>>8));
      tft_write_command_data (0x0007, (unsigned short)y); //Row Start
    }
    else
    {
      tft_write_command_data (0x0020, (unsigned short)y);
      tft_write_command_data (0x0021, (unsigned short)x);
      tft_write_command_data (0x0050, 0x00);
      tft_write_command_data (0x0051, 239);
      tft_write_command_data (0x0052, 0x00);
      tft_write_command_data (0x0053, 319);
	}
  }
}

static void tft_init(void)
{
  tft_id = tft_read_data (0x0);
  if (tft_id == 0x0)
  {
    // check for ILI9341 controller
	tft_read_datas (0xD3); // Memory Access Control
	// the first byte is x (ignor)
	if (tft_get_data() == 0x0)
	  if (tft_get_data() == 0x93)
	    if (tft_get_data() == 0x41)
		  tft_id = 0x9341;
	tft_terminate_access();
#if defined EE_APPKIT_TC2X5 || EE_APPKIT_TC2X7
	/* There is a problem with C step devices: read ID is wrong (0x200) */
	/* this is the correct LCD controller type */
	tft_id = 0x9341;
#endif /* EE_APPKIT_TC2X5 */
  }
  

  if (tft_id == 0x9341)
  {
    // this is an ILI9341 controller
    /************* Start Initial Sequence **********/
    tft_write_command (0xCF);
    tft_write_data (0x00);
    tft_write_data (0x83);
    tft_write_data (0X30);
	tft_terminate_access();

    tft_write_command (0xED);
    tft_write_data (0x64);
    tft_write_data (0x03);
    tft_write_data (0x12);
    tft_write_data (0x81);
	tft_terminate_access();

    tft_write_command (0xE8);
    tft_write_data (0x85);
    tft_write_data (0x00);
    tft_write_data (0x78);
	tft_terminate_access();

    tft_write_command (0xCB);
    tft_write_data (0x39);
    tft_write_data (0x2C);
    tft_write_data (0x00);
    tft_write_data (0x34);
    tft_write_data (0x02);
	tft_terminate_access();

    tft_write_command (0xF7);
    tft_write_data (0x20);
	tft_terminate_access();

    tft_write_command (0xEA);
    tft_write_data (0x00);
    tft_write_data (0x00);
	tft_terminate_access();

    tft_write_command (0xC0);    //Power control
    tft_write_data (0x19);       //VRH[5:0]
	tft_terminate_access();

    tft_write_command (0xC1);    //Power control
    tft_write_data (0x11);       //SAP[2:0];BT[3:0]
	tft_terminate_access();

    tft_write_command (0xC5);    //VCM control
    tft_write_data (0x3C);
    tft_write_data (0x3F);
	tft_terminate_access();

    tft_write_command (0xC7);    //VCM control2
    tft_write_data (0x90);
	tft_terminate_access();

    tft_write_command (0x36);    // Memory Access Control
    tft_write_data (0x28);       // MV=1; BGR=1
	tft_terminate_access();

	tft_write_command (0x3A);    // Pixel Format Set
    tft_write_data (0x55);       // 16 bit
	tft_terminate_access();

    tft_write_command (0xB1);
    tft_write_data (0x00);
    tft_write_data (0x17);
	tft_terminate_access();

    tft_write_command (0xB6);    // Display Function Control
    tft_write_data (0x0A);
    tft_write_data (0xA2);
	tft_terminate_access();

    tft_write_command (0xF6);
    tft_write_data (0x01);
    tft_write_data (0x30);
	tft_terminate_access();

    tft_write_command (0xF2);    // 3Gamma Function Disable
    tft_write_data (0x00);
	tft_terminate_access();

    tft_write_command (0x26);    //Gamma curve selected
    tft_write_data (0x01);
	tft_terminate_access();

    tft_write_command (0xE0);    //Set Gamma
    tft_write_data (0x0F);
    tft_write_data (0x26);
    tft_write_data (0x22);
    tft_write_data (0x0A);
    tft_write_data (0x10);
    tft_write_data (0x0A);
    tft_write_data (0x4C);
    tft_write_data (0xCA);
    tft_write_data (0x36);
    tft_write_data (0x00);
    tft_write_data (0x15);
    tft_write_data (0x00);
    tft_write_data (0x10);
    tft_write_data (0x10);
    tft_write_data (0x00);
	tft_terminate_access();

    tft_write_command (0xE1);    //Set Gamma
    tft_write_data (0x00);
    tft_write_data (0x19);
    tft_write_data (0x1B);
    tft_write_data (0x05);
    tft_write_data (0x0F);
    tft_write_data (0x05);
    tft_write_data (0x33);
    tft_write_data (0x35);
    tft_write_data (0x49);
    tft_write_data (0x0F);
    tft_write_data (0x1F);
    tft_write_data (0x0F);
    tft_write_data (0x3F);
    tft_write_data (0x3F);
    tft_write_data (0x0F);
	tft_terminate_access();

	tft_write_command (0x2A);    // Column Address Set
    tft_write_data (0x0);
    tft_write_data (0x0);
    tft_write_data (0x1);
    tft_write_data (0x3F);
	tft_terminate_access();

	tft_write_command (0x2B);    // Page Address Set
    tft_write_data (0x0);
    tft_write_data (0x0);
    tft_write_data (0x0);
    tft_write_data (0xEF);
	tft_terminate_access();

	tft_write_command (0x11);    //Exit Sleep
	tft_terminate_access();

	delay_ms(120);

	tft_write_command (0x29);    //Display on
	tft_terminate_access();
  }
  else
  {
    if (tft_id == 0x47)
    {
      // we have a display with HX8347
      //Power on Setting
      tft_write_command_data (0x0019, 0x0001); //OSC_EN='1', start Osc
      tft_write_command_data (0x001F, 0x0088);// GAS=1, VOMG=00, PON=0, DK=1, XDK=0, DVDH_TRI=0, STB=0
      delay_ms (5);
      tft_write_command_data (0x001F, 0x0080);// GAS=1, VOMG=00, PON=0, DK=0, XDK=0, DVDH_TRI=0, STB=0
      delay_ms (5);
      tft_write_command_data (0x001F, 0x0090);// GAS=1, VOMG=00, PON=1, DK=0, XDK=0, DVDH_TRI=0, STB=0
      delay_ms (5);
      tft_write_command_data (0x001F, 0x00D0);// GAS=1, VOMG=10, PON=1, DK=0, XDK=0, DDVDH_TRI=0, STB=0
      delay_ms (5);
      //262k/65k color selection
      tft_write_command_data (0x0017, 0x0005); //default 0x0006 262k color
      // 0x0005 65k color
      //SET PANEL
      tft_write_command_data (0x0016, 0x0028); //MV=1, BRG=1
      tft_write_command_data (0x0036, 0x0000); //SS_P, GS_P, REV_P, BGR_P
      //Display ON Setting
      tft_write_command_data (0x0028, 0x0038); //GON=1, DTE=1, D=1000
      delay_ms (40);
      tft_write_command_data (0x0028, 0x003C); //GON=1, DTE=1, D=1100
      //Set GRAM Area
      tft_write_command_data (0x0002, 0x0000);
      tft_write_command_data (0x0003, 0x0000); //Column Start
      tft_write_command_data (0x0004, 0x0001);
      tft_write_command_data (0x0005, 0x003F); //Column End
      tft_write_command_data (0x0006, 0x0000);
      tft_write_command_data (0x0007, 0x0000); //Row Start
      tft_write_command_data (0x0008, 0x0000);
      tft_write_command_data (0x0009, 0x00EF); //Row End
    }
    else
    {
	  // we expect that this is an ILI932x
      tft_write_command_data (0x00, 0x0001);
      tft_write_command_data (0x01, 0x0100);        // set SM bit (with reg.0x60.GS=1 for rotate the display), was 0000
      tft_write_command_data (0x02, 0x0700);        // set 1 line inversion
      tft_write_command_data (0x0003, 0x1038);
      tft_write_command_data (0x04, 0x0000);        // Resize register
      tft_write_command_data (0x08, 0x0202);        // set the back porch and front porch
      tft_write_command_data (0x09, 0x0000);        // set non-display area refresh cycle ISC[3:0]
      tft_write_command_data (0x0A, 0x0000);        // FMARK function
      //Power On sequence //
      tft_write_command_data (0x10, 0x0080);        // SAP, BT[3:0], AP, DSTB, SLP, STB
      tft_write_command_data (0x11, 0x0007);        // DC1[2:0], DC0[2:0], VC[2:0]
      tft_write_command_data (0x12, 0x0000);        // VREG1OUT voltage
      tft_write_command_data (0x13, 0x0000);        // VDV[4:0] for VCOM amplitude
      tft_write_command_data (0x07, 0x0001);        //

      delay_ms (2000);              // Dis-sint8ge capacitor power voltage

      //         tft_write_command_data(0x10, 0x1790); // SAP, BT[3:0], AP, DSTB, SLP, STB
      tft_write_command_data (0x10, 0x1290);        // SAP, BT[3:0], AP, DSTB, SLP, STB
      tft_write_command_data (0x11, 0x0227);        // DC1[2:0], DC0[2:0], VC[2:0]
      delay_ms (1000);              // Delay 50ms
      tft_write_command_data (0x12, 0x001C);        // Internal reference voltage= Vci;
      delay_ms (1000);              // Delay 50ms
      tft_write_command_data (0x13, 0x1600);        // Set VDV[4:0] for VCOM amplitude
      tft_write_command_data (0x29, 0x0012);        // Set VCM[5:0] for VCOMH
      tft_write_command_data (0x2B, 0x000D);        // Set Frame Rate
      delay_ms (500);               // Delay 50ms

      tft_write_command_data (0x0020, 0x0000);
      tft_write_command_data (0x0021, 0x0000);
      // ----------- Adjust the Gamma Curve ----------//
      tft_write_command_data (0x0030, 0x0007);
      tft_write_command_data (0x0031, 0x0707);
      tft_write_command_data (0x0032, 0x0107);
      tft_write_command_data (0x0035, 0x0206);
      tft_write_command_data (0x0036, 0x0408);
      tft_write_command_data (0x0037, 0x0006);
      tft_write_command_data (0x0038, 0x0000);
      tft_write_command_data (0x0039, 0x0207);
      tft_write_command_data (0x003C, 0x0504);
      tft_write_command_data (0x003D, 0x1501);
      //------------------ Set GRAM area ---------------//
      tft_write_command_data (0x0050, 0x0000);      // Horizontal GRAM Start Address
      tft_write_command_data (0x0051, 0x00EF);      // Horizontal GRAM End Address
      tft_write_command_data (0x0052, 0x0000);      // Vertical GRAM Start Address
      tft_write_command_data (0x0053, 0x013F);      // Vertical GRAM Start Address
      tft_write_command_data (0x0060, 0xA700);      // Gate Scan Line was 2700 (set GS=1, with reg.0x01.MS=1 for rotate the display)
      tft_write_command_data (0x0061, 0x0001);      // NDL,VLE, REV
      tft_write_command_data (0x006A, 0x0000);      // set scrolling line
      //-------------- Partial Display Control ---------//
      tft_write_command_data (0x80, 0x0000);
      tft_write_command_data (0x81, 0x0000);
      tft_write_command_data (0x82, 0x0000);
      tft_write_command_data (0x83, 0x0000);
      tft_write_command_data (0x84, 0x0000);
      tft_write_command_data (0x85, 0x0000);
      //-------------- Panel Control -------------------//
      tft_write_command_data (0x90, 0x0010);
      tft_write_command_data (0x92, 0x0000);
      tft_write_command_data (0x93, 0x0003);
      tft_write_command_data (0x95, 0x0110);
      tft_write_command_data (0x97, 0x0000);
      tft_write_command_data (0x98, 0x0000);
      tft_write_command_data (0x07, 0x0173);        // 262K color and display ON   0133
      delay_ms (1000);
    }
  }
}

static void tft_clrscr(void)
{
  int i, j;
  P33_OMR.U = 0x00400040;
  tft_display_home (0, 0);

  // we wait until we have 2 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 2);
  QSPI0_BACONENTRY.U = 0x80200400 | (9 << 23);
    if (tft_id == 0x9341)
	  QSPI0_DATAENTRY0.U = (0x1 << 8) | 0x2C; // register 0x2C on ILI9341
    else
	  QSPI0_DATAENTRY0.U = (0x1 << 8) | 0x22; // register 0x22
    for (i = 0; i < 240; i++)
    {
      for (j = 0; j < 320; j++)
        {
    	  tft_write_data (BackgroundColor);
        }
    }
  // we wait until we have 2 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 2);
  // this will write our last data
  QSPI0_BACONENTRY.U = 0x80200401 | (15 << 23);
  QSPI0_DATAENTRY0.U = 0;
  // we wait until we have 2 entrys free no fifo
  while (QSPI0_STATUS.B.TXFIFOLEVEL > 2);
  // this will terminate our endless transfer
  QSPI0_BACONENTRY.U = 0xF0200401 | (8 << 23);
  QSPI0_DATAENTRY0.U = 0;

}

void clearDisplayBuffer()
{
	int i = 0;
	int j = 0;

	for (i = 0; i < TFT_XSIZE; i++) {
		for (j = 0; j < TFT_YSIZE; j++) {
			setPx(i, j, BackgroundColor);
		}
	}

}

