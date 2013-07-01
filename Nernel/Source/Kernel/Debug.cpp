#include "Debug.hpp"
#include "KernelConsole.hpp"

void Debug::WriteString(const char* pString)
{
	KernelConsole::WriteString(pString, 0);
}

void Debug::WriteUInt64(UInt64 pValue, UInt8 pBase)
{
}
