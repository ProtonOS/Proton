#pragma once

namespace RTC
{
    constexpr UInt16 IOPortBase = 0x0070;
    constexpr UInt16 IOPortAddress = IOPortBase + 0x00;
    constexpr UInt16 IOPortData = IOPortBase + 0x01;

    constexpr UInt8 MonthMax = 12;

    void Load();

    void WaitForReady();

    UInt8 FromBCD(UInt8 pValue);

    UInt8 GetSecond();
    UInt8 GetMinute();
    UInt8 GetHour();
    UInt8 GetDayOfWeek();
    UInt8 GetDayOfMonth();
    UInt8 GetMonth();
    UInt8 GetYear();
    UInt8 GetCentury();
    UInt32 GetTotalDays(UInt16 pYear, UInt8 pMonth, UInt8 pDay);
    UInt32 GetSecondsSinceEpoch();

    extern bool BinaryCodedDecimalMode;
    extern bool MilitaryMode;

    extern UInt8 DaysPerMonth[];
    extern UInt8 LeapYearDaysPerMonth[];
};
