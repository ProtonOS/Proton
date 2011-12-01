#pragma once

#include <Core/Device.h>

namespace Core
{
    class COMPortLogger : public Device
    {
    public:
        COMPortLogger();
        bool OnRegister();
        void OnUnregister();

        void WriteByte(uint8_t pByte);

    private:
        static const uint16_t BaseIOPort        = 0x02F8;
        static const uint16_t DataIOPort        = BaseIOPort + 0x00;
        static const uint16_t InterruptIOPort   = BaseIOPort + 0x01;
        static const uint16_t FIFOIOPort        = BaseIOPort + 0x02;
        static const uint16_t LineDataIOPort    = BaseIOPort + 0x03;
        static const uint16_t ModemDataIOPort   = BaseIOPort + 0x04;
        static const uint16_t LineStatusIOPort  = BaseIOPort + 0x05;
        static const uint16_t ModemStatusIOPort = BaseIOPort + 0x06;
        static const uint16_t ScratchIOPort     = BaseIOPort + 0x07;

        static const uint32_t WriteAttempts = 1000;

        COMPortLogger(const COMPortLogger&);
        bool IsTransmitEmpty();
    };
}
