#include "Debug.hpp"
#include "KernelConsole.hpp"

extern "C" void Halt();
extern "C" void Panic(Core::String pString)
{
	KernelConsole::Clear(KernelConsole::CreateAttributes(KernelConsole::DarkBlack, KernelConsole::LightRed));
	Debug::WriteLine(pString);
	Halt();
}

namespace Debug
{
    void WriteString(Core::String pString)
    {
        KernelConsole::WriteString(pString);
    }

    void WriteLine(Core::String pString)
    {
        KernelConsole::WriteString(pString);
        KernelConsole::WriteCharacter('\n');
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