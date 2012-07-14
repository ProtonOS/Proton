#include "../PortIO.h"
#include "../../RTC.h"

namespace RTC
{
	const uint16_t IOPORT_BASE = 0x0070;
	const uint16_t IOPORT_ADDRESS = IOPORT_BASE + 0x00;
	const uint16_t IOPORT_DATA = IOPORT_BASE + 0x01;

	bool sBinaryCodedDecimalMode = true;
	bool sMilitaryMode = true;

	void WaitForReady()
	{
		do { outb(IOPORT_ADDRESS, 0x0A); }
		while ((inb(IOPORT_DATA) & 0x80) != 0);
	}

	uint8_t FromBCD(uint8_t pValue)
	{
		return (((pValue >> 4) & 0x0F) * 10 + (pValue & 0x0F));
	}

	void Startup()
	{
		WaitForReady();

		outb(IOPORT_ADDRESS, 0x0B);
		uint8_t modes = inb(IOPORT_DATA);
		sMilitaryMode = (modes & 0x02) == 0x02;
		sBinaryCodedDecimalMode = (modes & 0x04) == 0x00;
	}

	uint8_t GetSecond()
	{
		WaitForReady();
		outb(IOPORT_ADDRESS, 0x00);
		if (sBinaryCodedDecimalMode) return FromBCD(inb(IOPORT_DATA));
		return inb(IOPORT_DATA);
	}

	uint8_t GetMinute()
	{
		WaitForReady();
		outb(IOPORT_ADDRESS, 0x02);
		if (sBinaryCodedDecimalMode) return FromBCD(inb(IOPORT_DATA));
		return inb(IOPORT_DATA);
	}

	uint8_t GetHour()
	{
		WaitForReady();
		outb(IOPORT_ADDRESS, 0x04);
		if (sBinaryCodedDecimalMode)
		{
			if (sMilitaryMode) return FromBCD(inb(IOPORT_DATA));
			uint8_t b = inb(IOPORT_DATA);
			if ((b & 0x80) == 0x80) return FromBCD(b) + 12;
			if (FromBCD(b) == 12) return 0;
			return FromBCD(b);
		}
		else
		{
			if (sMilitaryMode) return inb(IOPORT_DATA);
			uint8_t b = inb(IOPORT_DATA);
			if ((b & 0x80) == 0x80) return b + 12;
			if (b == 12) return 0;
			return b;
		}
	}

	uint8_t GetDayOfWeek()
	{
		WaitForReady();
		outb(IOPORT_ADDRESS, 0x06);
		if (sBinaryCodedDecimalMode) return FromBCD(inb(IOPORT_DATA));
		return inb(IOPORT_DATA);
	}

	uint8_t GetDayOfMonth()
	{
		WaitForReady();
		outb(IOPORT_ADDRESS, 0x07);
		if (sBinaryCodedDecimalMode) return FromBCD(inb(IOPORT_DATA));
		return inb(IOPORT_DATA);
	}

	uint8_t GetMonth()
	{
		WaitForReady();
		outb(IOPORT_ADDRESS, 0x08);
		if (sBinaryCodedDecimalMode) return FromBCD(inb(IOPORT_DATA));
		return inb(IOPORT_DATA);
	}

	uint8_t GetYear()
	{
		WaitForReady();
		outb(IOPORT_ADDRESS, 0x09);
		if (sBinaryCodedDecimalMode) return FromBCD(inb(IOPORT_DATA));
		return inb(IOPORT_DATA);
	}

	uint8_t GetCentury()
	{
		WaitForReady();
		outb(IOPORT_ADDRESS, 0x32);
		if (sBinaryCodedDecimalMode) return FromBCD(inb(IOPORT_DATA));
		return inb(IOPORT_DATA);
	}

	const uint8_t MAX_MONTH_COUNT = 12;
	uint8_t sDaysPerMonth[MAX_MONTH_COUNT + 1] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	uint8_t sLeapYearDaysPerMonth[MAX_MONTH_COUNT + 1] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	uint32_t GetTotalDays(uint16_t pYear, uint8_t pMonth, uint8_t pDay)
	{
		uint32_t totalDays = 0;
		uint8_t month = 1;
		uint8_t* daysPerMonth = sDaysPerMonth;
		if (((pYear % 4 == 0) && (pYear % 100) != 0) || (pYear % 400) == 0) daysPerMonth = sLeapYearDaysPerMonth;
		while (month < pMonth) totalDays += daysPerMonth[month++];
		return ((pDay - 1) + totalDays + (365 * (pYear - 1)) + ((pYear - 1) / 4) - ((pYear - 1) / 100) + ((pYear - 1) / 400));
	}

	uint32_t GetSecondsSinceEpoch()
	{
		uint32_t nowDays = GetTotalDays((GetCentury() * 100) + GetYear(), GetMonth(), GetDayOfMonth());
		uint32_t epochDays = GetTotalDays(1970, 1, 1);
		uint32_t secondsSinceEpoch = ((nowDays - epochDays) * 24 * 60 * 60) + (GetHour() * 60 * 60) + (GetMinute() * 60);
		return  secondsSinceEpoch + GetSecond();
	}
}
