#pragma once

#include <Core/Device.h>

namespace Core
{
    namespace Driver
    {
        class PIC : public Device
        {
        public:
            virtual bool OnRegister() = 0;
            virtual void OnUnregister() = 0;

            virtual void StartInterrupts() = 0;
            virtual void StopInterrupts() = 0;
            virtual void ResetInterrupts(bool pSlave) = 0;
        };
    }
}
