#pragma once

void RealTimeClock_Startup();
void RealTimeClock_Shutdown();

void RealTimeClock_WaitForReady();
uint8_t RealTimeClock_FromBCD(uint8_t pValue);
uint8_t RealTimeClock_GetSecond();
uint8_t RealTimeClock_GetMinute();
uint8_t RealTimeClock_GetHour();
uint8_t RealTimeClock_GetDayOfWeek();
uint8_t RealTimeClock_GetDayOfMonth();
uint8_t RealTimeClock_GetMonth();
uint8_t RealTimeClock_GetYear();
uint8_t RealTimeClock_GetCentury();
uint32_t RealTimeClock_GetTotalDays(uint16_t pYear, uint8_t pMonth, uint8_t pDay);
uint32_t RealTimeClock_GetSecondsSinceEpoch();
