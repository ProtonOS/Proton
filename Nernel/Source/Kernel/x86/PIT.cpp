#include "IDT.h"
#include "PIT.h"
#include "PortIO.h"
#include "Processor.h"

namespace PIT
{
	const uint32_t BUS_SAMPLES = 10;

	uint32_t sMaxHertz = 1193182;
	uint16_t sCycleHertz = 100;
	uint16_t sMillisecondsPerCycle = 0;
	uint32_t sMillisecondsElapsed = 0;
	uint32_t sSecondsElapsed = 0;
	bool sCalculatingProcessorBus = false;
	uint32_t sCalculatingProcessorBusCycles = 0;

	void Channel0(IDT::InterruptRegisters pRegisters)
	{
		if (sCalculatingProcessorBus)
		{
			++sCalculatingProcessorBusCycles;
			if (sCalculatingProcessorBusCycles == BUS_SAMPLES) sCalculatingProcessorBus = false;
		}
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

	uint32_t CalculateProcessorBus(Processor* pProcessor)
	{
		sCalculatingProcessorBusCycles = 0;
		sCalculatingProcessorBus = true;
		*reinterpret_cast<uint32_t*>(pProcessor->BaseAddress + Processor::LAPIC_REGISTER_TIMER_INITIALCOUNT) = 0xFFFFFFFF;
		while (sCalculatingProcessorBus) ;
		return (((0xFFFFFFFF - *reinterpret_cast<uint32_t*>(pProcessor->BaseAddress + Processor::LAPIC_REGISTER_TIMER_CURRENTCOUNT)) + 1) * 16) * (sCycleHertz / BUS_SAMPLES);
	}
}
