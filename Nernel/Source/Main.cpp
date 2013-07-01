#include "Kernel/MultiBoot.hpp"
#include "Kernel/KernelConsole.hpp"
#include "Kernel/Arch.hpp"

extern "C" void Main(UInt32 pMultiBootMagic, MultiBoot::MultiBootHeader* pMultiBootHeader)
{
    KernelConsole::Load();
    MultiBoot::Load(pMultiBootMagic, pMultiBootHeader);
    Arch::Load();

    KernelConsole::WriteLine("Hello World!");
    while (true);
}
