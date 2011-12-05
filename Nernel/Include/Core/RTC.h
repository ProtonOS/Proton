#pragma once

#include <types.h>

#define RTC_BaseIOPort      0x0070
#define RTC_AddressIOPort   RTC_BaseIOPort + 0x00
#define RTC_DataIOPort      RTC_BaseIOPort + 0x01

void RTC_Startup();
void RTC_Shutdown();

void RTC_WaitForReady();
uint8_t RTC_FromBCD(uint8_t value);
uint8_t RTC_GetSecond();
uint8_t RTC_GetMinute();
uint8_t RTC_GetHour();
uint8_t RTC_GetDayOfWeek();
uint8_t RTC_GetDayOfMonth();
uint8_t RTC_GetMonth();
uint8_t RTC_GetYear();
uint8_t RTC_GetCentury();
uint32_t RTC_GetTotalDays(uint16_t pYear, uint8_t pMonth, uint8_t pDay);
uint32_t RTC_GetSecondsSinceEpoch();

extern bool_t gRTC_BinaryCodedDecimalMode;
extern bool_t gRTC_MilitaryMode;
