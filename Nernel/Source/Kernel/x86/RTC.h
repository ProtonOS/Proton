#pragma once

namespace RTC
{
	void Startup();

	uint8_t GetSecond();
	uint8_t GetMinute();
	uint8_t GetHour();
	uint8_t GetDayOfWeek();
	uint8_t GetDayOfMonth();
	uint8_t GetMonth();
	uint8_t GetYear();
	uint8_t GetCentury();
	uint32_t GetTotalDays(uint16_t pYear, uint8_t pMonth, uint8_t pDay);
	uint32_t GetSecondsSinceEpoch();
};
