#pragma once

#include <Core/InterruptRegisters.h>

namespace Core
{
    class InterruptHandler
    {
    public:
        virtual void OnInterrupt(Core::InterruptRegisters pRegisters) = 0;
    };
}
