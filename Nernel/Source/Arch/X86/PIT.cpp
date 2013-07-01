#include "../../Kernel/InterruptManager.hpp"
#include "../../Kernel/KernelConsole.hpp"

#include "PIC.hpp"
#include "PIT.hpp"
#include "PortIO.hpp"

UInt16 PIT::MillisecondsSinceSecond = 0;
UInt64 PIT::SecondsSinceStartup = 0;

extern "C" void PITChannel0(InterruptRegisters pRegisters)
{
	PIT::MillisecondsSinceSecond += PIT::MillisecondsPerCycle;
	if (PIT::MillisecondsSinceSecond >= 1000)
	{
		++PIT::SecondsSinceStartup;
		PIT::MillisecondsSinceSecond -= 1000;
	}
}

void PIT::Load()
{
	InterruptManager::RegisterHandler(PIC::IRQBaseISR + 0, &PITChannel0);

	UInt32 divisor = MaxHertz / CycleHertz;
	UInt8 low = divisor & 0xFF;
	UInt8 high = (divisor >> 8) & 0xFF;

    outb(IOPortMode, ModeBinary | ModeRateGenerator | ModeLowByteThenHighByte | ModeChannel0);
    outb(IOPortChannel0Data, low);
    outb(IOPortChannel0Data, high);
}
