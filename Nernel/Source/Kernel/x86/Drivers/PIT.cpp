#include "../IDT.h"
#include "../PortIO.h"
#include "../../PIT.h"

namespace PIT
{
	uint32_t sMaxHertz = 1193182;
	uint16_t sCycleHertz = 100;
	uint16_t sMillisecondsPerCycle = 0;
	uint32_t sMillisecondsElapsed = 0;
	uint32_t sSecondsElapsed = 0;

	void Channel0(IDT::InterruptRegisters pRegisters)
	{
		sMillisecondsElapsed += sMillisecondsPerCycle;
		if (sMillisecondsElapsed >= 1000)
		{
			++sSecondsElapsed;
			sMillisecondsElapsed -= 1000;

			time_t tickTime = time(NULL);
			printf("Ticked @ %24.24s\n", ctime(&tickTime));
		}
	}

	void Startup()
	{
		sMillisecondsPerCycle = 1000 / sCycleHertz;
		IDT::RegisterCallback(IDT::IRQ_BASE + 0, Channel0);

		uint32_t divisor = sMaxHertz / sCycleHertz;
		uint8_t low = divisor & 0xFF;
		uint8_t high = (divisor >> 8) & 0xFF;

		outb(0x43, 0x36);
		outb(0x40, low);
		outb(0x40, high);
	}

	uint32_t GetSecondsElapsed()
	{
		return sSecondsElapsed;
	}

	uint32_t GetMillisecondsElapsed()
	{
		return sMillisecondsElapsed;
	}
}
