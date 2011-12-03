#pragma once

#include <Core/Device.h>
#include <Core/InterruptHandler.h>

namespace Core
{
    namespace Driver
    {
        class PIT : public Device, public InterruptHandler
        {
        public:
            virtual bool OnRegister() = 0;
            virtual void OnUnregister() = 0;

            virtual void OnInterrupt(Core::InterruptRegisters pRegisters) = 0;
            virtual uint16_t GetMillisecondsElapsed() = 0;
            virtual uint32_t GetSecondsElapsed() = 0;
        };
    }
}
