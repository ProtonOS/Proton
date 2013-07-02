#include "Kernel/Arch.hpp"
#include "Kernel/KernelConsole.hpp"
#include "Kernel/MultiBoot.hpp"
#include "Kernel/SystemClock.hpp"
#include "Kernel/Debug.hpp"

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
    // PhysicalMemory::Load();
    // VirtualMemory::Load();

    // Preemptive MultiTasking Support
    // ThreadScheduler::Load();

    // MultiProcessor Support
    // SMP::Load();

    // Bootstrap Processor UserMode Boot
    // UserModeBoot()

    KernelConsole::WriteLine("Hello World!");
    Debug::Write<Int64, 16>(-0x9001DEADBEEF);
    Debug::WriteFormat("\n%u %% %i %x %s", 273, -999, 0xface, "Wat");
    while (true);
}
