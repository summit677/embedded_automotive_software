/**
 * \file RTC.c
 * \copyright Copyright (c) 2013 Infineon Technologies AG. All rights reserved.
 *
 *
 * 1.0
 * 2014-05-20
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
 *
 * \file
 * \brief RTC Driver
 *
 * The RTC Driver downstreams the values.
 * The RTC makes a serialization and shift the bits out to deserialization device.
 * The RTC does not provide a upstream of status information.
 */

#include "tc29xb/Ifx_reg.h"
#include <string.h>
#include "system.h"
#include "system_tc29x.h"
#include "RTC.h"

static uint8_t _RTC_running = RTC_STATUS_UNINITED;

/* forward declarations */
void createDateFromData(uint32_t *data, TIME *pActTime, uint8_t time);


/*!
 * \brief   Initialization of the RTC Driver.
 *
 * ...
 */
int32_t rtc_init(void)
{
	if (_RTC_running != RTC_STATUS_UNINITED)
		return 1;
	SYSTEM_DisableProtectionExt (0);
	I2C0_CLC.U = 0x0;       						// load clock control register
	while ((I2C0_CLC.U & 0x00000002) == 2);       // wait until module is enabled
	P15_PDR0.U = 0x00B00000;						// we set our SDA (P15.5) to TTL level because the RTC drives with 2,6V and speed grade 4 why it is driven hardly	
	SYSTEM_EnableProtectionExt (0);
	// we set the pins here
	P15_IOCR0.B.PC4 = 0x16; 						// SCL0
	P15_IOCR0.B.PC5 = 0x16; 						// SDA0
	I2C0_CLC1.U = 0x100;       					// load clock control register
	while ((I2C0_CLC1.U & 0x00000002) == 2);      // wait until module is enabled
	I2C0_GPCTL.U = 0x00000001; 					// use SCL0B/SDA0B
	// frequency must be less than 400kHz, we use 100kHz
	I2C0_FDIVCFG.U = 0x000103B0;

	// we are the master
	I2C0_ADDRCFG.U = 0x00380000;
	I2C0_FIFOCFG.U = 0x00030011;

	I2C0_RUNCTRL.U = 0x1; // modul run
	_RTC_running = RTC_STATUS_READY;
	return 0;
}

/*!
 * \brief   Starts the RTC Driver.
 *
 * ...
 */
int32_t rtc_start(void)
{
	if (_RTC_running != RTC_STATUS_READY)
		return 1;
	_RTC_running = RTC_STATUS_BUSY;

	return 0;
}

/*!
 * \brief   gets the actual time
 *
 * ...
 */
int32_t rtc_gettime(TIME *pActTime)
{
	uint32_t uiBytesReceived;
	uint32_t uiData[2];

	// send 2 bytes
	I2C0_TPSCTRL.B.TPS = 0x2;
	// send 0x6F, write flag and 0 as address
	I2C0_TXD.U = RTC_I2C_WRITE;
	I2C0_ICR.U = 0xF;
	// we wait until MRPS expired and we have the datas

	while (!I2C0_PIRQSS.B.TX_END);
	// reset the last single request (send byte)
	I2C0_ICR.U = 0xF;
	I2C0_PIRQSC.U = I2C0_PIRQSS.U;
	// send 1 byte
	I2C0_TPSCTRL.B.TPS = 0x1;
	// receive 8 bytes
	I2C0_MRPSCTRL.B.MRPS = 8;
	// send 0x6F and write 1
	I2C0_TXD.U = RTC_I2C_READ;
	// reset the last single request (from byte sended)
	I2C0_ICR.U = 0xF;
	// we wait until MRPS expired and we have the datas
	while (!I2C0_PIRQSS.B.TX_END);
	// we get our received count
	uiBytesReceived = I2C0_RPSSTAT.B.RPS;
	// we get the first 4 bytes
	uiData[0] = I2C0_RXD.U;
	// we get the second 4 bytes
	uiData[1] = I2C0_RXD.U;
	// we reset the burst request
	I2C0_ICR.U = 0xF;
	// we reset the TX_END and RX interrupt
	I2C0_PIRQSC.U = I2C0_PIRQSS.U;
	// if we don't read 8 bytes we stop and return an error
	if (uiBytesReceived != 8) return 1;

	createDateFromData(uiData, pActTime, 0x1);

	return 0;
}

void createDateFromData(uint32_t *data, TIME *pActTime, uint8_t time)
{
	pActTime->flags.B.hour_12 = (*(data+0) >> 22) & 0x1;

	pActTime->seconds = Bcd2Dec(*(data+0) & 0x7F);
	pActTime->minutes = Bcd2Dec(*(data+0)>>8 & 0x7F);

	if(pActTime->flags.B.hour_12 == 0x0)
	{
		pActTime->hours = Bcd2Dec(*(data+0)>>16 & 0x3F);
		pActTime->flags.B.pm_set = 0x0;
	}
	else
	{
		pActTime->hours = Bcd2Dec(*(data+0)>>16 & 0x1F);
		pActTime->flags.B.pm_set = (uint8_t)(*(data+0)>>21 & 0x1);
	}

	pActTime->day = Bcd2Dec((*(data+0)>>24) & 0x07);
	pActTime->date = Bcd2Dec((*(data+1)>>0) & 0x3F);
	pActTime->month = Bcd2Dec((*(data+1)>>8) & 0x1F);

	if (time)
	{
		pActTime->year = Bcd2Dec(*(data+1)>>16 & 0xFF);
		pActTime->flags.B.alarm0 = *(data+1)>>28 & 0x1;
		pActTime->flags.B.alarm1 = *(data+1)>>29 & 0x1;
	}

	pActTime->flags.B.leap_year = *(data+1)>>13 & 0x1;
	pActTime->flags.B.running = *(data+0)>>7 & 0x1;
}

/*!
 * \brief   gets the alarm time (0 or 1)
 *
 * ...
 */
int32_t rtc_getalarm(TIME *pActTime, uint8_t uiAlarmNumber)
{
	uint32_t uiData[2];
	uint32_t uiBytesReceived;

	if (uiAlarmNumber > 1) return 1;
	// send 2 bytes
	I2C0_TPSCTRL.B.TPS = 0x2;
	// send 0x6F, write flag and address
	if (uiAlarmNumber == ALARM_ID_0) I2C0_TXD.U = RTC_I2C_WRITE | (RTC_REG_ALARM0<<8);
	else I2C0_TXD.U = RTC_I2C_WRITE | (RTC_REG_ALARM1<<8);


	I2C0_ICR.U = 0xF;
	// we wait until MRPS expired and we have the datas
	while (!I2C0_PIRQSS.B.TX_END);
	// reset the last single request (send byte)
	I2C0_ICR.U = 0xF;
	I2C0_PIRQSC.U = I2C0_PIRQSS.U;
	// send 1 byte
	I2C0_TPSCTRL.B.TPS = 0x1;
	// receive 6 bytes
	I2C0_MRPSCTRL.B.MRPS = 6;
	// send 0x6F and write 1
	I2C0_TXD.U = RTC_I2C_READ;
	// reset the last single request (from byte sended)
	I2C0_ICR.U = 0xF;
	// we wait until MRPS expired and we have the datas
	while (!I2C0_PIRQSS.B.TX_END);
	// we get our received count
	uiBytesReceived = I2C0_RPSSTAT.B.RPS;
	// we get the first 4 bytes
	uiData[0] = I2C0_RXD.U;
	// we get the second 4 bytes
	uiData[1] = I2C0_RXD.U;
	// we reset the burst request
	I2C0_ICR.U = 0xF;
	// we reset the TX_END and RX interrupt
	I2C0_PIRQSC.U = I2C0_PIRQSS.U;
	// if we don't read 6 bytes we stop and return an error
	if (uiBytesReceived != 6) return 1;

	createDateFromData(uiData, pActTime, 0x0);

	pActTime->flags.B.alarm_match = (uiData[0] >> 28) & 0x7;

	return 0;
}

/*!
 * \brief   sets the actual time and flags
 *
 * ...
 */
int32_t rtc_settime(TIME *pActTime)
{
	uint32_t uiData;

	// send 10 bytes, 0x6E, 0x00 as address and 8 byte data
	I2C0_TPSCTRL.B.TPS = 0x2+8;

	// send device address, reg address,
	// seconds, minutes. Activate running flag
	// and send to fifo
	uiData = RTC_I2C_WRITE | 0x0;
	rtc_calcSecMinData(&uiData, pActTime, 16);
	if (pActTime->flags.B.running) uiData |= 0x00800000;
	I2C0_TXD.U = uiData;

	uiData = 0x00;
	rtc_calcHoursMonth(&uiData, pActTime);
	I2C0_TXD.U = uiData;

	uiData = 0x0;
	rtc_calcYearControl(&uiData, pActTime);

	// send data to fifo
	I2C0_TXD.U = uiData;
	// we wait until MRPS expired and we have the data
	while (!I2C0_PIRQSS.B.TX_END);
	// we reset the burst request
	I2C0_ICR.U = 0xF;
	// we reset the TX_END and RX interrupt
	I2C0_PIRQSC.U = I2C0_PIRQSS.U;

	return 0;
}

void rtc_calcSecMinData(uint32_t *data, TIME *pActTime, int ls)
{
	*(data) |= (Dec2Bcd(pActTime->seconds) & 0x7F) << (ls+0);
	*(data) |= (Dec2Bcd(pActTime->minutes) & 0x7F) << (ls+8);
}

void rtc_calcHoursMonth(uint32_t *data, TIME *pActTime)
{
	*(data) |= (Dec2Bcd(pActTime->hours) & 0x3F) << 0;
	*(data) |= ((pActTime->day) & 0x7) << 8;
	*(data) |= (Dec2Bcd(pActTime->date) & 0x3F) << 16;
	*(data) |= (Dec2Bcd(pActTime->month) & 0x1F) << 24;

	// we set the hour flags
	if (pActTime->flags.B.hour_12 == 0x1)
	{
		*(data) |= 0x1 << 6;
	    if (pActTime->flags.B.pm_set == 0x1)
	    {
	    	*(data) |= 0x1 << 5;
	    }
	    else
	    {
	    	*(data) &= ~(0x1 << 5);
	    }
	}
	else
	{
		*(data) &= ~(0x1 << 6);
	}
}

void rtc_calcYearControl(uint32_t *data, TIME *pActTime)
{
	*(data) |= (Dec2Bcd(pActTime->year) & 0xFF) << 0;

	// we set our alarm0 flag
	if (pActTime->flags.B.alarm0) *(data) |= 0x00001000;
	// we set our alarm0 flag
	if (pActTime->flags.B.alarm1) *(data) |= 0x00002000;
	// we set the OUT bit to 1
	*(data) |= 0x00008000;
}

/*!
 * \brief   sets an alarm time (0 or 1)
 *
 * ...
 */
int32_t rtc_setalarm(TIME *pAlarmTime, uint8_t uiAlarmNumber, uint8_t uiAlarmMatch, uint8_t uiAlarmPolarity)
{
	uint32_t uiData;

	if (uiAlarmNumber > 1) return 1;

	// send 10 bytes, 0x6E, 0 as address and datas
	I2C0_TPSCTRL.B.TPS = 0x2+6;

	// we prepare the datas whith address 0xa,
	if (uiAlarmNumber == ALARM_ID_0)
	{
		uiData = RTC_I2C_WRITE | (RTC_REG_ALARM0 << 8);
	}
	else
	{
		uiData = RTC_I2C_WRITE | (RTC_REG_ALARM1 << 8);
	}

	rtc_calcSecMinData(&uiData, pAlarmTime, 16);
	I2C0_TXD.U = uiData;

	uiData = 0x0;
	rtc_calcHoursMonth(&uiData, pAlarmTime);

	// we match on all values, clear the alarm flag
	uiData |= (uiAlarmMatch<<12);

	// alarm polarity can only be set on alarm0, alarm1 uses the same as alarm0
	if (uiAlarmNumber == ALARM_ID_0)
	{
		uiData |= (uiAlarmPolarity << 15);
	}

	// send data to fifo
	I2C0_TXD.U = uiData;
	// we wait until MRPS expired and we have the datas
	while (!I2C0_PIRQSS.B.TX_END);
	// we reset the burst request
	I2C0_ICR.U = 0xF;
	// we reset the TX_END and RX interrupt
	I2C0_PIRQSC.U = I2C0_PIRQSS.U;

	return 0;
}

/*!
 * \brief   activate or deactivate an alarm (0 or 1)
 *
 * ...
 */
int32_t rtc_alarm_onoff(uint8_t uiAlarmNumber, uint8_t uiOn)
{
	uint32_t uiData;
	uint32_t uiBytesReceived;

	if (uiAlarmNumber > 1) return 1;

	// send 2 bytes
	I2C0_TPSCTRL.B.TPS = 0x2;
	// send 0x6F, write flag and 0x7 as address
	I2C0_TXD.U = (0x07 << 8) | RTC_I2C_WRITE;
	I2C0_ICR.U = 0xF;
	// we wait until MRPS expired and we have the data
	while (!I2C0_PIRQSS.B.TX_END);
	// reset the last single request (send byte)
	I2C0_ICR.U = 0xF;
	I2C0_PIRQSC.U = I2C0_PIRQSS.U;
	// send 1 byte
	I2C0_TPSCTRL.B.TPS = 0x1;
	// receive 1 bytes
	I2C0_MRPSCTRL.B.MRPS = 1;
	// send 0x6F and write 1
	I2C0_TXD.U = RTC_I2C_READ;
	// reset the last single request (from byte sended)
	I2C0_ICR.U = 0xF;
	// we wait until MRPS expired and we have the datas
	while (!I2C0_PIRQSS.B.TX_END);
	// we get our received count
	uiBytesReceived = I2C0_RPSSTAT.B.RPS;
	// we get the first 4 bytes
	uiData = I2C0_RXD.U;
	// we reset the burst request
	I2C0_ICR.U = 0xF;
	// we reset the TX_END and RX interrupt
	I2C0_PIRQSC.U = I2C0_PIRQSS.U;
	// if we don't read 1 byte we stop and return an error
	if (uiBytesReceived != 1) return 2;
	// now we set the selected alarm
	if (uiOn == ALARM_ON)
	{
		if (uiAlarmNumber == ALARM_ID_0) uiData |= 0x10;
		else  uiData |= 0x20;
	}
	else
	{
		if (uiAlarmNumber == ALARM_ID_0) uiData &= ~0x10;
		else  uiData &= ~0x20;
	}
	// we set always out to 1
	uiData |= 0x80;
	// we write this now
	uiData <<= 16;
	uiData |= (0x07<<8) | RTC_I2C_WRITE;
	// send 3 bytes
	I2C0_TPSCTRL.B.TPS = 0x3;
	// send 0x6F, write flag and 0x7 as address
	I2C0_TXD.U = uiData;
	I2C0_ICR.U = 0xF;
	// we wait until MRPS expired and we have the datas
	while (!I2C0_PIRQSS.B.TX_END);
	// reset the last single request (send byte)
	I2C0_ICR.U = 0xF;
	I2C0_PIRQSC.U = I2C0_PIRQSS.U;

	return 0;
}

/*!
 * \brief   reset the alarm flags (0 or 1)
 *
 * ...
 */
int32_t rtc_reset_alarmflag(uint8_t uiAlarmNumber)
{
	uint32_t uiData;
	uint32_t uiBytesReceived;

	if (uiAlarmNumber > 1) return 1;

	// send 2 bytes
	I2C0_TPSCTRL.B.TPS = 0x2;
	// send 0x6F, write flag and 0xD or 0x14as address
	if (uiAlarmNumber) I2C0_TXD.U = 0x14DE;
	else I2C0_TXD.U = 0x0DDE;
	I2C0_ICR.U = 0xF;
	// we wait until MRPS expired and we have the datas
	while (!I2C0_PIRQSS.B.TX_END);
	// reset the last single request (send byte)
	I2C0_ICR.U = 0xF;
	I2C0_PIRQSC.U = I2C0_PIRQSS.U;
	// send 1 byte
	I2C0_TPSCTRL.B.TPS = 0x1;
	// receive 1 bytes
	I2C0_MRPSCTRL.B.MRPS = 1;
	// send 0x6F and write 1
	I2C0_TXD.U = RTC_I2C_READ;
	// reset the last single request (from byte sended)
	I2C0_ICR.U = 0xF;
	// we wait until MRPS expired and we have the datas
	while (!I2C0_PIRQSS.B.TX_END);
	// we get our received count
	uiBytesReceived = I2C0_RPSSTAT.B.RPS;
	// we get the first 4 bytes
	uiData = I2C0_RXD.U;
	// we reset the burst request
	I2C0_ICR.U = 0xF;
	// we reset the TX_END and RX interrupt
	I2C0_PIRQSC.U = I2C0_PIRQSS.U;
	// if we don't read 1 byte we stop and return an error
	if (uiBytesReceived != 1) return 2;
	if (uiData & 0x8)
	{
		// the alarm flag is set we clear the flag
		uiData &= ~0x8;
		// we write this now
		uiData <<= 16;
		if (uiAlarmNumber) uiData |= 0x14DE;
		else uiData |= 0x0DDE;

		// send 3 bytes
		I2C0_TPSCTRL.B.TPS = 0x3;
		// send 0x6F, write flag and 0x7 as address
		I2C0_TXD.U = uiData;
		I2C0_ICR.U = 0xF;
		// we wait until MRPS expired and we have the datas
		while (!I2C0_PIRQSS.B.TX_END);
		// reset the last single request (send byte)
		I2C0_ICR.U = 0xF;
		I2C0_PIRQSC.U = I2C0_PIRQSS.U;
	}

	return 0;
}

/*!
 * \brief   Returns the Status of the RTC Driver.
 *
 * ...
 */
int8_t rtc_status(void)
{
	return (int8_t)_RTC_running;
}

/*!
 * \brief   Interrupt after command frame.
 *
 * ...
 */
int32_t rtc_int_alarm(void)
{
	return 0;
}

void rtc_create_time(TIME* time, uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year, uint8_t time_format, uint8_t ampm)
{
	time->seconds = seconds;
	time->minutes = minutes;
	time->hours = hours;
	time->date = date;
	time->day = day;
	time->month = month;
	time->year = year;

	time->flags.B.pm_set = ampm;
	time->flags.B.hour_12 = time_format;
}

/*!
 * \brief   return the textual representation of the day field (1 = Monday)
 *
 * ...
 */
void getDayString(TIME *time, char *buf)
{
	if (time->day == 1)
	{
		strcpy(buf, "Monday");
	}
	else if (time->day == 2)
	{
		strcpy(buf, "Tuesday");
	}
	else if (time->day == 3)
	{
		strcpy(buf, "Wednesday");
	}
	else if (time->day == 4)
	{
		strcpy(buf, "Thursday");
	}
	else if (time->day == 5)
	{
		strcpy(buf, "Friday");
	}
	else if (time->day == 6)
	{
		strcpy(buf, "Saturday");
	}
	else
	{
		strcpy(buf, "Sunday");
	}
}

/*!
 * \brief   return the textual representation of the alarm match field
 *
 * ...
 */
void getAlarmMatchString(TIME *time, char *buf)
{
	if (time->flags.B.alarm_match == ALARM_MATCH_DATE)
	{
		strcpy(buf, "DT");
	}
	else if (time->flags.B.alarm_match == ALARM_MATCH_DAY)
	{
		strcpy(buf, "DY");
	}
	else if (time->flags.B.alarm_match == ALARM_MATCH_HOURS)
	{
		strcpy(buf, "HR");
	}
	else if (time->flags.B.alarm_match == ALARM_MATCH_MINUTES)
	{
		strcpy(buf, "MI");
	}
	else if (time->flags.B.alarm_match == ALARM_MATCH_SECONDS)
	{
		strcpy(buf, "SE");
	}
	else if (time->flags.B.alarm_match == ALARM_MATCH_EXACT)
	{
		strcpy(buf, "EX");
	}
}
