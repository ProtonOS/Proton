#pragma once

void RTC_Startup();
void RTC_Shutdown();

void RTC_WaitForReady();
uint8_t RTC_FromBCD(uint8_t pValue);
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
