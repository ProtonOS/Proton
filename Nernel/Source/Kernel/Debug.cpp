#include "Debug.hpp"
#include "KernelConsole.hpp"

void Debug::WriteString(const char* pString)
{
    KernelConsole::WriteString(pString, 0);
}
