#include "Kernel/Arch.hpp"
#include "Kernel/KernelConsole.hpp"
#include "Kernel/MultiBoot.hpp"
#include "Kernel/SystemClock.hpp"

extern "C" void Main(UInt32 pMultiBootMagic, MultiBoot::MultiBootHeader* pMultiBootHeader)
{
    KernelConsole::Load();
    MultiBoot::Load(pMultiBootMagic, pMultiBootHeader);
    Arch::Load();
	SystemClock::Load();

    KernelConsole::WriteLine("Hello World!");
    while (true);
}
