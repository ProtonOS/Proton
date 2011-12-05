#include <PortIO.h>
#include <Core/IDT.h>
#include <Core/RTC.h>

bool_t gRTC_BinaryCodedDecimalMode = TRUE;
bool_t gRTC_MilitaryMode = TRUE;

void RTC_Startup()
{
    RTC_WaitForReady();

    outb(RTC_AddressIOPort, 0x0B);
    uint8_t modes = inb(RTC_DataIOPort);
    if ((modes & 0x02) == 0x02) gRTC_MilitaryMode = TRUE;
    else gRTC_MilitaryMode = FALSE;
    if ((modes & 0x04) == 0x04) gRTC_BinaryCodedDecimalMode = FALSE;
    else gRTC_BinaryCodedDecimalMode = TRUE;
}

void RTC_Shutdown()
{
}

void RTC_WaitForReady()
{
    do { outb(RTC_AddressIOPort, 0x0A); }
    while ((inb(RTC_DataIOPort) & 0x80) != 0);
}

uint8_t RTC_FromBCD(uint8_t pValue) { return (((pValue >> 4) & 0x0F) * 10 + (pValue & 0x0F)); }

uint8_t RTC_GetSecond()
{
    RTC_WaitForReady();
    outb(RTC_AddressIOPort, 0x00);
    if (gRTC_BinaryCodedDecimalMode) return RTC_FromBCD(inb(RTC_DataIOPort));
    return inb(RTC_DataIOPort);
}

uint8_t RTC_GetMinute()
{
    RTC_WaitForReady();
    outb(RTC_AddressIOPort, 0x02);
    if (gRTC_BinaryCodedDecimalMode) return RTC_FromBCD(inb(RTC_DataIOPort));
    return inb(RTC_DataIOPort);
}

uint8_t RTC_GetHour()
{
    RTC_WaitForReady();
    outb(RTC_AddressIOPort, 0x04);
    if (gRTC_BinaryCodedDecimalMode)
    {
        if (gRTC_MilitaryMode) return RTC_FromBCD(inb(RTC_DataIOPort));
        uint8_t b = inb(RTC_DataIOPort);
        if ((b & 0x80) == 0x80) return RTC_FromBCD(b) + 12;
        if (RTC_FromBCD(b) == 12) return 0;
        return RTC_FromBCD(b);
    }
    else
    {
        if (gRTC_MilitaryMode) return inb(RTC_DataIOPort);
        uint8_t b = inb(RTC_DataIOPort);
        if ((b & 0x80) == 0x80) return b + 12;
        if (b == 12) return 0;
        return b;
    }
}

uint8_t RTC_GetDayOfWeek()
{
    RTC_WaitForReady();
    outb(RTC_AddressIOPort, 0x06);
    if (gRTC_BinaryCodedDecimalMode) return RTC_FromBCD(inb(RTC_DataIOPort));
    return inb(RTC_DataIOPort);
}

uint8_t RTC_GetDayOfMonth()
{
    RTC_WaitForReady();
    outb(RTC_AddressIOPort, 0x07);
    if (gRTC_BinaryCodedDecimalMode) return RTC_FromBCD(inb(RTC_DataIOPort));
    return inb(RTC_DataIOPort);
}

uint8_t RTC_GetMonth()
{
    RTC_WaitForReady();
    outb(RTC_AddressIOPort, 0x08);
    if (gRTC_BinaryCodedDecimalMode) return RTC_FromBCD(inb(RTC_DataIOPort));
    return inb(RTC_DataIOPort);
}

uint8_t RTC_GetYear()
{
    RTC_WaitForReady();
    outb(RTC_AddressIOPort, 0x09);
    if (gRTC_BinaryCodedDecimalMode) return RTC_FromBCD(inb(RTC_DataIOPort));
    return inb(RTC_DataIOPort);
}

uint8_t RTC_GetCentury()
{
    RTC_WaitForReady();
    outb(RTC_AddressIOPort, 0x32);
    if (gRTC_BinaryCodedDecimalMode) return RTC_FromBCD(inb(RTC_DataIOPort));
    return inb(RTC_DataIOPort);
}

#define MAX_MONTH_COUNT			12
uint8_t gDaysPerMonth[MAX_MONTH_COUNT + 1] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
uint8_t gLeapYearDaysPerMonth[MAX_MONTH_COUNT + 1] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

uint32_t RTC_GetTotalDays(uint16_t pYear,
                          uint8_t pMonth,
                          uint8_t pDay)
{
    uint32_t totalDays = 0;
    uint8_t month = 1;
    uint8_t* daysPerMonth = gDaysPerMonth;
    if (((pYear % 4 == 0) && (pYear % 100) != 0) || (pYear % 400) == 0) daysPerMonth = gLeapYearDaysPerMonth;
    while (month < pMonth) totalDays += daysPerMonth[month++];
    return ((pDay - 1) + totalDays + (365 * (pYear - 1)) + ((pYear - 1) / 4) - ((pYear - 1) / 100) + ((pYear - 1) / 400));
}

uint32_t RTC_GetSecondsSinceEpoch()
{
    uint32_t nowDays = RTC_GetTotalDays((RTC_GetCentury() * 100) + RTC_GetYear(), RTC_GetMonth(), RTC_GetDayOfMonth());
    uint32_t epochDays = RTC_GetTotalDays(1970, 1, 1);
    uint32_t secondsSinceEpoch = ((nowDays - epochDays) * 24 * 60 * 60) + (RTC_GetHour() * 60 * 60) + (RTC_GetMinute() * 60);
    return  secondsSinceEpoch + RTC_GetSecond();
}
