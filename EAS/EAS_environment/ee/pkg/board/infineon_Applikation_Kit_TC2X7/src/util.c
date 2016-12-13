/*
 * util.c
 *
 *  Created on: Feb 3, 2014
 *      Author: Fusseneg
 */
#include "../inc/util.h"

void delay_us(unsigned long time)
{
	unsigned long i;
	for (i = 0; i < time; i += 1)
	{
		for(int j=0; j < 200; j++);
	}
}

void delay_ms(unsigned long time)
{
unsigned long i;
	for (i = 0; i < time; i += 1)
	{
		for(int j=0; j < 200000; j++);
	}
}
