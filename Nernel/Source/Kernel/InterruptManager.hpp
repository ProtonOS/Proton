#pragma once

#ifdef X86
#include "../Arch/X86/IDT.hpp"
#endif

namespace InterruptManager
{
    void RegisterHandler(UInt8 pInterrupt, IDT::InterruptHandler pHandler);
    IDT::InterruptHandler GetHandler(UInt8 pInterrupt);
}