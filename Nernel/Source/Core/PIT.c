#include <stdio.h>
#include <time.h>

#include <PortIO.h>
#include <Core/IDT.h>
#include <Core/PIT.h>
#include <Core/SystemClock.h>

uint16_t gPIT_Hertz = 0;
uint16_t gPIT_Cycle = 0;
uint32_t gPIT_MillisecondsPerCycle = 0;
uint32_t gPIT_MillisecondsElapsedSinceStartup = 0;
uint32_t gPIT_MillisecondsElapsed = 0;
uint32_t gPIT_SecondsElapsed = 0;

void PIT_Startup(uint16_t pHertz)
{
    gPIT_Hertz = pHertz;
    gPIT_MillisecondsPerCycle = 1000 / gPIT_Hertz;

    uint16_t divisor = PIT_MaxHertz / gPIT_Hertz;
    uint8_t low = divisor & 0xFF;
    uint8_t high = (divisor >> 8) & 0xFF;

    outb(PIT_CommandIOPort, PIT_Counter0InitializeCommand);
    outb(PIT_Counter0IOPort, low);
    outb(PIT_Counter0IOPort, high);

    IDT_RegisterHandler(IDT_RemappedIRQBase + PIT_BaseIRQ, &PIT_Interrupt);
}

void PIT_Shutdown()
{
    //IDT_UnregisterHandler(IDT_RemappedIRQBase + BaseIRQ);
}

void PIT_Interrupt(InterruptRegisters pRegisters)
{
    if (SystemClock_IsReady())
    {
        ++gPIT_Cycle;
		gPIT_MillisecondsElapsedSinceStartup += gPIT_MillisecondsPerCycle;
        gPIT_MillisecondsElapsed += gPIT_MillisecondsPerCycle;
        if (gPIT_Cycle >= gPIT_Hertz)
        {
            gPIT_Cycle = 0;
            gPIT_MillisecondsElapsed = 0;
            ++gPIT_SecondsElapsed;

	        //time_t t = time(NULL);
	        //printf("Time = %s", ctime(&t));
        }
    }
}

uint64_t PIT_GetMillisecondsSinceStartup() { return gPIT_MillisecondsElapsedSinceStartup; }

uint16_t PIT_GetMillisecondsElapsed() { return gPIT_MillisecondsElapsed; }

uint32_t PIT_GetSecondsElapsed() { return gPIT_SecondsElapsed; }
