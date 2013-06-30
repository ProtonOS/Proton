#include "Core/Types.hpp"

const UInt32 gBootStackSize = 0x100000;
UInt8 gBootStack[gBootStackSize];
const UInt8* gBootStackTop = gBootStack + gBootStackSize;
const UInt8* gBootStackBottom = gBootStack;

const UInt32 gTempStackSize = 0x100000;
UInt8 gTempStack[gTempStackSize];
const UInt8* gTempStackTop = gTempStack + gTempStackSize;
const UInt8* gTempStackBottom = gTempStack;
UInt8* gTempStackCursor = gTempStack + gTempStackSize;

extern "C" void ProtectedModeStartup()
{
}

extern "C" void UserModeStartup()
{
}
