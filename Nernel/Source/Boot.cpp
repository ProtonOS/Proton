#include "Core/Types.hpp"

const uint32 gBootStackSize = 0x100000;
uint8 gBootStack[gBootStackSize];
const uint8* gBootStackTop = gBootStack + gBootStackSize;
const uint8* gBootStackBottom = gBootStack;

const uint32 gTempStackSize = 0x100000;
uint8 gTempStack[gTempStackSize];
const uint8* gTempStackTop = gTempStack + gTempStackSize;
const uint8* gTempStackBottom = gTempStack;
uint8* gTempStackCursor = gTempStack + gTempStackSize;

extern "C" void ProtectedModeStartup()
{
}

extern "C" void UserModeStartup()
{
}
