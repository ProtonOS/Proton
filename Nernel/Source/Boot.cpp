#include "Core/Core.hpp"

const UInt32 gBootStackSize = 0x100000;
UInt8 gBootStack[gBootStackSize];
Core::Ptr<const UInt8> gBootStackTop = gBootStack + gBootStackSize;
Core::Ptr<const UInt8> gBootStackBottom = gBootStack;

const UInt32 gTempStackSize = 0x100000;
UInt8 gTempStack[gTempStackSize];
Core::Ptr<const UInt8> gTempStackTop = gTempStack + gTempStackSize;
Core::Ptr<const UInt8> gTempStackBottom = gTempStack;
Core::Ptr<UInt8> gTempStackCursor = gTempStack + gTempStackSize;

extern "C" void ProtectedModeStartup()
{
}

extern "C" void UserModeStartup()
{
}
