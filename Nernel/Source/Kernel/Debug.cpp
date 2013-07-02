#include "Debug.hpp"
#include "KernelConsole.hpp"

namespace Debug
{
    void WriteString(Core::String pString)
    {
        KernelConsole::WriteString(pString);
    }

    void WriteFormat(Core::String pString)
    {
        FindFormat<'%'>(pString);
        UInt start = 0;
        for (UInt i = 0; i < pString.Length(); ++i) {
            if (pString[i] == '%') {
                WriteString(pString.SubString(start, ++i));
                start = i + 1;
            }
        }
        WriteString(pString.SubString(start, pString.Length()));
    }
}