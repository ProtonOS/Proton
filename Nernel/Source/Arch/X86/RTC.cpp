#include "PortIO.hpp"
#include "RTC.hpp"

bool RTC::BinaryCodedDecimalMode = true;
bool RTC::MilitaryMode = true;

UInt8 RTC::DaysPerMonth[RTC::MonthMax + 1] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
UInt8 RTC::LeapYearDaysPerMonth[RTC::MonthMax + 1] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void RTC::WaitForReady()
{
    do { outb(IOPortAddress, 0x0A); }
    while ((inb(IOPortData) & 0x80) != 0);
}

UInt8 RTC::FromBCD(UInt8 pValue) { return (((pValue >> 4) & 0x0F) * 10 + (pValue & 0x0F)); }

void RTC::Load()
{
    WaitForReady();

    outb(IOPortAddress, 0x0B);
    UInt8 modes = inb(IOPortData);
	MilitaryMode = (modes & 0x02) == 0x02;
	BinaryCodedDecimalMode = (modes & 0x04) != 0x04;
}

UInt8 RTC::GetSecond()
{
    WaitForReady();
    outb(IOPortAddress, 0x00);
	return BinaryCodedDecimalMode ? FromBCD(inb(IOPortData)) : inb(IOPortData);
}

UInt8 RTC::GetMinute()
{
    WaitForReady();
    outb(IOPortAddress, 0x02);
	return BinaryCodedDecimalMode ? FromBCD(inb(IOPortData)) : inb(IOPortData);
}

UInt8 RTC::GetHour()
{
    WaitForReady();
    outb(IOPortAddress, 0x04);
    if (MilitaryMode) return BinaryCodedDecimalMode ? FromBCD(inb(IOPortData)) : inb(IOPortData);
    UInt8 b = inb(IOPortData);
    if ((b & 0x80) == 0x80) return (BinaryCodedDecimalMode ? FromBCD(b) : b) + 12;
    if (BinaryCodedDecimalMode && FromBCD(b) == 12) return 0;
	if (!BinaryCodedDecimalMode && b == 12) return 0;
    return BinaryCodedDecimalMode ? FromBCD(b) : b;
}

UInt8 RTC::GetDayOfWeek()
{
    WaitForReady();
    outb(IOPortAddress, 0x06);
    return BinaryCodedDecimalMode ? FromBCD(inb(IOPortData)) : inb(IOPortData);
}

UInt8 RTC::GetDayOfMonth()
{
    WaitForReady();
    outb(IOPortAddress, 0x07);
    return BinaryCodedDecimalMode ? FromBCD(inb(IOPortData)) : inb(IOPortData);
}

UInt8 RTC::GetMonth()
{
    WaitForReady();
    outb(IOPortAddress, 0x08);
    return BinaryCodedDecimalMode ? FromBCD(inb(IOPortData)) : inb(IOPortData);
}

UInt8 RTC::GetYear()
{
    WaitForReady();
    outb(IOPortAddress, 0x09);
    return BinaryCodedDecimalMode ? FromBCD(inb(IOPortData)) : inb(IOPortData);
}

UInt8 RTC::GetCentury()
{
    WaitForReady();
    outb(IOPortAddress, 0x32);
    return BinaryCodedDecimalMode ? FromBCD(inb(IOPortData)) : inb(IOPortData);
}

UInt32 RTC::GetTotalDays(UInt16 pYear, UInt8 pMonth, UInt8 pDay)
{
    UInt32 totalDays = 0;
    UInt8 month = 1;
    UInt8* daysPerMonth = DaysPerMonth;
    if (((pYear % 4 == 0) && (pYear % 100) != 0) || (pYear % 400) == 0) daysPerMonth = LeapYearDaysPerMonth;
    while (month < pMonth) totalDays += daysPerMonth[month++];
    return ((pDay - 1) + totalDays + (365 * (pYear - 1)) + ((pYear - 1) / 4) - ((pYear - 1) / 100) + ((pYear - 1) / 400));
}

UInt32 RTC::GetSecondsSinceEpoch()
{
    UInt32 nowDays = GetTotalDays((GetCentury() * 100) + GetYear(), GetMonth(), GetDayOfMonth());
    UInt32 epochDays = GetTotalDays(1970, 1, 1);
    UInt32 secondsSinceEpoch = ((nowDays - epochDays) * 24 * 60 * 60) + (GetHour() * 60 * 60) + (GetMinute() * 60);
    return  secondsSinceEpoch + GetSecond();
}

