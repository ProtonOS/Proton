#include <System/APIC.h>
#include <Common.h>
#include <System/PIT.h>
#include <System/x86/PortIO.h>

uint32_t gPIT_MaxHertz = 1193182;
uint16_t gPIT_CycleHertz = 100;
uint16_t gPIT_MillisecondsPerCycle = 0;
uint16_t gPIT_MillisecondsElapsed = 0;
uint32_t gPIT_SecondsElapsed = 0;
volatile APIC* gPIT_FrequencyTesting = NULL;
uint32_t gPIT_FrequencyCount = 0;

void PIT_Channel0(InterruptRegisters pRegisters)
{
	if (gPIT_FrequencyTesting)
	{
		++gPIT_FrequencyCount;
		if (gPIT_FrequencyCount == 20)//(unsigned int)(gPIT_CycleHertz + 1))
		{
			gPIT_FrequencyCount = 0;
			gPIT_FrequencyTesting = NULL;
		}
	}
	gPIT_MillisecondsElapsed += gPIT_MillisecondsPerCycle;
	if (gPIT_MillisecondsElapsed >= 1000)
	{
		++gPIT_SecondsElapsed;
		gPIT_MillisecondsElapsed -= 1000;

		//time_t tickTime = time(NULL);
		//Log_WriteLine(LOGLEVEL__Information, "Ticked @ %24.24s", ctime(&tickTime));
	}
}

void PIT_Startup()
{
	gPIT_MillisecondsPerCycle = 1000 / gPIT_CycleHertz;
	IDT_RegisterHandler(IDT__IRQ__RemappedBase + 0, &PIT_Channel0);

	uint32_t divisor = gPIT_MaxHertz / gPIT_CycleHertz;
	uint8_t low = divisor & 0xFF;
	uint8_t high = (divisor >> 8) & 0xFF;

    outb(0x43, 0x36);
    outb(0x40, low);
    outb(0x40, high);
}

void PIT_Shutdown()
{
}

void PIT_TestAPICFrequency(APIC* pAPIC)
{
	gPIT_FrequencyTesting = pAPIC;
}

bool_t PIT_TestingAPICFrequency()
{
	return gPIT_FrequencyTesting != NULL;
}