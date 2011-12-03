#include <PortIO.h>
#include <Core/IDT.h>
#include <Driver/RTC146818A.h>

Driver::RTC146818A::RTC146818A()
    : mBinaryCodedDecimalMode(false),
      mMilitaryMode(false)
{
}

bool Driver::RTC146818A::OnRegister()
{
    if (!IsIOPortAvailable(AddressIOPort) ||
        !IsIOPortAvailable(DataIOPort)) return false;

    ClaimIOPort(AddressIOPort);
    ClaimIOPort(DataIOPort);

    WaitForReady();

    outb(AddressIOPort, 0x0B);
    uint8_t modes = inb(DataIOPort);
    if ((modes & 0x02) == 0x02) mMilitaryMode = true;
    else mMilitaryMode = false;
    if ((modes & 0x04) == 0x04) mBinaryCodedDecimalMode = false;
    else mBinaryCodedDecimalMode = true;

    return true;
}

void Driver::RTC146818A::OnUnregister()
{
    ReleaseIOPort(AddressIOPort);
    ReleaseIOPort(DataIOPort);
}

void Driver::RTC146818A::WaitForReady()
{
    do { outb(AddressIOPort, 0x0A); }
    while ((inb(DataIOPort) & 0x80) != 0);
}

uint8_t Driver::RTC146818A::FromBCD(uint8_t pValue) { return (((pValue >> 4) & 0x0F) * 10 + (pValue & 0x0F)); }

uint8_t Driver::RTC146818A::GetSecond()
{
    WaitForReady();
    outb(AddressIOPort, 0x00);
    if (mBinaryCodedDecimalMode) return FromBCD(inb(DataIOPort));
    return inb(DataIOPort);
}

uint8_t Driver::RTC146818A::GetMinute()
{
    WaitForReady();
    outb(AddressIOPort, 0x02);
    if (mBinaryCodedDecimalMode) return FromBCD(inb(DataIOPort));
    return inb(DataIOPort);
}

uint8_t Driver::RTC146818A::GetHour()
{
    WaitForReady();
    outb(AddressIOPort, 0x04);
    if (mBinaryCodedDecimalMode)
    {
        if (mMilitaryMode) return FromBCD(inb(DataIOPort));
        uint8_t b = inb(DataIOPort);
        if ((b & 0x80) == 0x80) return FromBCD(b) + 12;
        if (FromBCD(b) == 12) return 0;
        return FromBCD(b);
    }
    else
    {
        if (mMilitaryMode) return inb(DataIOPort);
        uint8_t b = inb(DataIOPort);
        if ((b & 0x80) == 0x80) return b + 12;
        if (b == 12) return 0;
        return b;
    }
}

uint8_t Driver::RTC146818A::GetDayOfWeek()
{
    WaitForReady();
    outb(AddressIOPort, 0x06);
    if (mBinaryCodedDecimalMode) return FromBCD(inb(DataIOPort));
    return inb(DataIOPort);
}

uint8_t Driver::RTC146818A::GetDayOfMonth()
{
    WaitForReady();
    outb(AddressIOPort, 0x07);
    if (mBinaryCodedDecimalMode) return FromBCD(inb(DataIOPort));
    return inb(DataIOPort);
}

uint8_t Driver::RTC146818A::GetMonth()
{
    WaitForReady();
    outb(AddressIOPort, 0x08);
    if (mBinaryCodedDecimalMode) return FromBCD(inb(DataIOPort));
    return inb(DataIOPort);
}

uint8_t Driver::RTC146818A::GetYear()
{
    WaitForReady();
    outb(AddressIOPort, 0x09);
    if (mBinaryCodedDecimalMode) return FromBCD(inb(DataIOPort));
    return inb(DataIOPort);
}

uint8_t Driver::RTC146818A::GetCentury()
{
    WaitForReady();
    outb(AddressIOPort, 0x32);
    if (mBinaryCodedDecimalMode) return FromBCD(inb(DataIOPort));
    return inb(DataIOPort);
}

#define MAX_MONTH_COUNT			12
uint8_t gDaysPerMonth[MAX_MONTH_COUNT + 1] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
uint8_t gLeapYearDaysPerMonth[MAX_MONTH_COUNT + 1] = { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

uint32_t Driver::RTC146818A::GetTotalDays(uint16_t pYear,
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

uint32_t Driver::RTC146818A::GetSecondsSinceEpoch()
{
    uint32_t nowDays = GetTotalDays((GetCentury() * 100) + GetYear(), GetMonth(), GetDayOfMonth());
    uint32_t epochDays = GetTotalDays(1970, 1, 1);
    return ((nowDays - epochDays) * 24 * 60 * 60) + (GetHour() * 60 * 60) + (GetMinute() * 60) + GetSecond() + 1;
}
