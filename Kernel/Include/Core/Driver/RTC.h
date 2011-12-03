#pragma once

#include <Core/Device.h>

namespace Core
{
    namespace Driver
    {
        class RTC : public Device
        {
        public:
            virtual bool OnRegister() = 0;
            virtual void OnUnregister() = 0;

            virtual uint8_t GetSecond() = 0;
            virtual uint32_t GetSecondsSinceEpoch() = 0;
        };
    }
}
