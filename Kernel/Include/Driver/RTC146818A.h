#pragma once

#include <Core/Driver/RTC.h>

namespace Driver
{
    class RTC146818A : public Core::Driver::RTC
    {
    public:
        RTC146818A();
        bool OnRegister();
        void OnUnregister();

        uint8_t GetSecond();
        uint32_t GetSecondsSinceEpoch();

    private:
        static const uint16_t BaseIOPort    = 0x0070;
        static const uint16_t AddressIOPort = BaseIOPort + 0x00;
        static const uint16_t DataIOPort    = BaseIOPort + 0x01;

        bool mBinaryCodedDecimalMode;
        bool mMilitaryMode;

        RTC146818A(const RTC146818A&);
        void WaitForReady();
        uint8_t FromBCD(uint8_t value);
        uint8_t GetMinute();
        uint8_t GetHour();
        uint8_t GetDayOfWeek();
        uint8_t GetDayOfMonth();
        uint8_t GetMonth();
        uint8_t GetYear();
        uint8_t GetCentury();
        uint32_t GetTotalDays(uint16_t pYear, uint8_t pMonth, uint8_t pDay);
    };
}
