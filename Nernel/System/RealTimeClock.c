#include "InterruptDescriptorTable.h"
#include "RealTimeClock.h"
#include "x86/PortIO.h"

#define REALTIMECLOCK_BASEIOPORT		0x0070
#define REALTIMECLOCK_ADDRESSIOPORT		REALTIMECLOCK_BASEIOPORT + 0x00
#define REALTIMECLOCK_DATAIOPORT		REALTIMECLOCK_BASEIOPORT + 0x01

bool_t gRealTimeClock_BinaryCodedDecimalMode = TRUE;
bool_t gRealTimeClock_MilitaryMode = TRUE;

void RealTimeClock_Startup()
{
    RealTimeClock_WaitForReady();

    outb(REALTIMECLOCK_ADDRESSIOPORT, 0x0B);
    uint8_t modes = inb(REALTIMECLOCK_DATAIOPORT);
    if ((modes & 0x02) == 0x02) gRealTimeClock_MilitaryMode = TRUE;
    else gRealTimeClock_MilitaryMode = FALSE;
    if ((modes & 0x04) == 0x04) gRealTimeClock_BinaryCodedDecimalMode = FALSE;
    else gRealTimeClock_BinaryCodedDecimalMode = TRUE;
}

void RealTimeClock_Shutdown()
{
}

void RealTimeClock_WaitForReady()
{
    do { outb(REALTIMECLOCK_ADDRESSIOPORT, 0x0A); }
    while ((inb(REALTIMECLOCK_DATAIOPORT) & 0x80) != 0);
}

uint8_t RealTimeClock_FromBCD(uint8_t pValue) { return (((pValue >> 4) & 0x0F) * 10 + (pValue & 0x0F)); }

uint8_t RealTimeClock_GetSecond()
{
    RealTimeClock_WaitForReady();
    outb(REALTIMECLOCK_ADDRESSIOPORT, 0x00);
    if (gRealTimeClock_BinaryCodedDecimalMode) return RealTimeClock_FromBCD(inb(REALTIMECLOCK_DATAIOPORT));
    return inb(REALTIMECLOCK_DATAIOPORT);
}

uint8_t RTC_GetMinute()
{
    RealTimeClock_WaitForReady();
    outb(REALTIMECLOCK_ADDRESSIOPORT, 0x02);
    if (gRealTimeClock_BinaryCodedDecimalMode) return RealTimeClock_FromBCD(inb(REALTIMECLOCK_DATAIOPORT));
    return inb(REALTIMECLOCK_DATAIOPORT);
}

uint8_t RealTimeClock_GetHour()
{
    RealTimeClock_WaitForReady();
    outb(REALTIMECLOCK_ADDRESSIOPORT, 0x04);
    if (gRealTimeClock_BinaryCodedDecimalMode)
    {
        if (gRealTimeClock_MilitaryMode) return RealTimeClock_FromBCD(inb(REALTIMECLOCK_DATAIOPORT));
        uint8_t b = inb(REALTIMECLOCK_DATAIOPORT);
        if ((b & 0x80) == 0x80) return RealTimeClock_FromBCD(b) + 12;
        if (RealTimeClock_FromBCD(b) == 12) return 0;
        return RealTimeClock_FromBCD(b);
    }
    else
    {
        if (gRealTimeClock_MilitaryMode) return inb(REALTIMECLOCK_DATAIOPORT);
        uint8_t b = inb(REALTIMECLOCK_DATAIOPORT);
        if ((b & 0x80) == 0x80) return b + 12;
        if (b == 12) return 0;
        return b;
    }
}

uint8_t RealTimeClock_GetDayOfWeek()
{
    RealTimeClock_WaitForReady();
    outb(REALTIMECLOCK_ADDRESSIOPORT, 0x06);
    if (gRealTimeClock_BinaryCodedDecimalMode) return RealTimeClock_FromBCD(inb(REALTIMECLOCK_DATAIOPORT));
    return inb(REALTIMECLOCK_DATAIOPORT);
}

uint8_t RealTimeClock_GetDayOfMonth()
{
    RealTimeClock_WaitForReady();
    outb(REALTIMECLOCK_ADDRESSIOPORT, 0x07);
    if (gRealTimeClock_BinaryCodedDecimalMode) return RealTimeClock_FromBCD(inb(REALTIMECLOCK_DATAIOPORT));
    return inb(REALTIMECLOCK_DATAIOPORT);
}

uint8_t RealTimeClock_GetMonth()
{
    RealTimeClock_WaitForReady();
    outb(REALTIMECLOCK_ADDRESSIOPORT, 0x08);
    if (gRealTimeClock_BinaryCodedDecimalMode) return RealTimeClock_FromBCD(inb(REALTIMECLOCK_DATAIOPORT));
    return inb(REALTIMECLOCK_DATAIOPORT);
}

uint8_t RealTimeClock_GetYear()
{
    RealTimeClock_WaitForReady();
    outb(REALTIMECLOCK_ADDRESSIOPORT, 0x09);
    if (gRealTimeClock_BinaryCodedDecimalMode) return RealTimeClock_FromBCD(inb(REALTIMECLOCK_DATAIOPORT));
    return inb(REALTIMECLOCK_DATAIOPORT);
}

uint8_t RealTimeClock_GetCentury()
{
    RealTimeClock_WaitForReady();
    outb(REALTIMECLOCK_ADDRESSIOPORT, 0x32);
    if (gRealTimeClock_BinaryCodedDecimalMode) return RealTimeClock_FromBCD(inb(REALTIMECLOCK_DATAIOPORT));
    return inb(REALTIMECLOCK_DATAIOPORT);
}

#define MAX_MONTH_COUNT			12
uint8_t gDaysPerMonth[MAX_MONTH_COUNT + 1] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
uint8_t gLeapYearDaysPerMonth[MAX_MONTH_COUNT + 1] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

uint32_t RealTimeClock_GetTotalDays(uint16_t pYear, uint8_t pMonth, uint8_t pDay)
{
    uint32_t totalDays = 0;
    uint8_t month = 1;
    uint8_t* daysPerMonth = gDaysPerMonth;
    if (((pYear % 4 == 0) && (pYear % 100) != 0) || (pYear % 400) == 0) daysPerMonth = gLeapYearDaysPerMonth;
    while (month < pMonth) totalDays += daysPerMonth[month++];
    return ((pDay - 1) + totalDays + (365 * (pYear - 1)) + ((pYear - 1) / 4) - ((pYear - 1) / 100) + ((pYear - 1) / 400));
}

uint32_t RealTimeClock_GetSecondsSinceEpoch()
{
    uint32_t nowDays = RealTimeClock_GetTotalDays((RealTimeClock_GetCentury() * 100) + RealTimeClock_GetYear(), RealTimeClock_GetMonth(), RealTimeClock_GetDayOfMonth());
    uint32_t epochDays = RealTimeClock_GetTotalDays(1970, 1, 1);
    uint32_t secondsSinceEpoch = ((nowDays - epochDays) * 24 * 60 * 60) + (RealTimeClock_GetHour() * 60 * 60) + (RTC_GetMinute() * 60);
    return  secondsSinceEpoch + RealTimeClock_GetSecond();
}
