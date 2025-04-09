#ifndef __RTC_H
#define	__RTC_H

#include "stm32f10x.h"

struct DATETABLE{
	int year;
	int month;
	int day;
	int week;
	int hour;
	int minute;
	int sec;
};


void RTC_Init(void);
uint8_t RTC_SetSec(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t sec);
uint8_t RTC_GetDate(struct DATETABLE* thisDate);

#endif
