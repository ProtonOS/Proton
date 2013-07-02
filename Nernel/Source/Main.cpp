#include "Kernel/Arch.hpp"
#include "Kernel/Debug.hpp"
#include "Kernel/KernelConsole.hpp"
#include "Kernel/MultiBoot.hpp"
#include "Kernel/PMM.hpp"
#include "Kernel/SystemClock.hpp"

extern "C" void Main(UInt32 pMultiBootMagic, MultiBoot::MultiBootHeader * pMultiBootHeader)
{
    // Debugging Support
    KernelConsole::Load();

    // Preliminary Memory Management
    MultiBoot::Load(pMultiBootMagic, pMultiBootHeader);

    // Architecture Specific Support
    Arch::Load();

    // Debugging Support
    SystemClock::Load();

    // Complete Memory Management
    PMM::Load();
    // VirtualMemory::Load();

    // Preemptive MultiTasking Support
    // ThreadScheduler::Load();

    // MultiProcessor Support
    // SMP::Load();

    // Bootstrap Processor UserMode Boot
    // UserModeBoot()

    KernelConsole::WriteLine("Hello World!");
    while (true);
}
