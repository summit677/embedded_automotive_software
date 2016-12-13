/**
 * \file
 * \brief RTC Driver include file
 *
 * The RTC Driver downstreams the values.
 * The RTC makes a serialization and shift the bits out to deserialization device.
 * The RTC does not provide a upstream of status information.
 */
#ifndef INCLUDE_EE_APPKIT_TC2X7_BOARD_RTC_H__
#define INCLUDE_EE_APPKIT_TC2X7_BOARD_RTC_H__

#include <stdint.h>

#define RTC_I2C_WRITE		0xDE
#define RTC_I2C_READ		0xDF

#define RTC_REG_TIME		0x00
#define RTC_REG_ALARM0		0x0A
#define RTC_REG_ALARM1		0x11

#define RTC_STATUS_UNINITED	0x00
#define RTC_STATUS_READY	0x01
#define RTC_STATUS_BUSY		0x02

#define ALARM_ID_0			0x00
#define ALARM_ID_1			0x01

#define ALARM_OFF			0x00
#define ALARM_ON			0x01

#define ALARM_MATCH_SECONDS	0x00
#define ALARM_MATCH_MINUTES	0x01
#define ALARM_MATCH_HOURS	0x02
#define	ALARM_MATCH_DAY		0x03
#define	ALARM_MATCH_DATE	0x04
#define ALARM_MATCH_EXACT	0x07

#define ALARM_POL_LOW		0x00
#define ALARM_POL_HIGH		0x01

#define TIME_FORMAT_24H		0x00
#define TIME_FORMAT_12H		0x01

#define	TIME_AM				0x00
#define TIME_PM				0x01

// this is for our time from the RTC (BCD coded)
typedef volatile struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
	volatile union
	{
		uint16_t U;
		struct
		{
			uint16_t running :1;		// when set then the rtc is running or should be run
			uint16_t hour_12 :1;		// when set we use 12 hours time, when clear the 24 hours
			uint16_t pm_set  :1;		// when set it is pm, only when we use 12 hours time (hour_12 is set)
			uint16_t leap_year :1;		// when set then we have a leap year, read only
			uint16_t alarm0  :1;		// when set then alarm 0 is active
			uint16_t alarm1  :1;		// when set then alarm 1 is active
			uint16_t alarm_match :3;	// defines the alarm match
			uint16_t reserved :7;		// reserved bits
		} B;
	} flags;
} TIME;

static inline uint8_t Bcd2Dec(uint8_t bcd)
{
	return ((bcd >> 4)*10+(bcd & 0x0F));
}

static inline uint8_t Dec2Bcd(uint8_t dec)
{
	return (((dec/10)<<4)+(dec%10));
}

int32_t rtc_init(void);
int32_t rtc_start(void);
int8_t  rtc_status(void);
int32_t rtc_gettime(TIME *pActTime);
int32_t rtc_getalarm(TIME *pActTime, uint8_t uiAlarmNumber);
int32_t rtc_settime(TIME *pActTime);
int32_t rtc_setalarm(TIME *pAlarmTime, uint8_t uiAlarmNumber, uint8_t uiAlarmMatch, uint8_t uiAlarmPolarity);
int32_t rtc_alarm_onoff(uint8_t uiAlarmNumber, uint8_t uiOn);
int32_t rtc_reset_alarmflag(uint8_t uiAlarmNumber);
void rtc_create_time(TIME* time, uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t day, uint8_t date, uint8_t month, uint8_t year, uint8_t time_format, uint8_t ampm);
void rtc_calcSecMinData(uint32_t *data, TIME *pActTime, int ls);
void rtc_calcHoursMonth(uint32_t *data, TIME *pActTime);
void rtc_calcYearControl(uint32_t *data, TIME *pActTime);
void getDayString(TIME* time, char* buf);
void getAlarmMatchString(TIME* time, char* buf);

#endif /* INCLUDE_EE_APPKIT_TC2X7_BOARD_RTC_H__ */
