extern "C" {
#include <stdio.h>
#include <time.h>
}
#include <PortIO.h>
#include <Core/DeviceManager.h>
#include <Core/IDT.h>
#include <Driver/PIT8254.h>

Driver::PIT8254::PIT8254(uint16_t pHertz)
    : mHertz(pHertz),
      mCycle(0),
      mMillisecondsPerCycle(1000 / pHertz),
      mMillisecondsElapsed(0),
      mSecondsElapsed(0)
{
}

bool Driver::PIT8254::OnRegister()
{
    if (!IsIOPortAvailable(Counter0IOPort) ||
        !IsIOPortAvailable(Counter1IOPort) ||
        !IsIOPortAvailable(Counter2IOPort) ||
        !IsIOPortAvailable(CommandIOPort)) return false;

    ClaimIOPort(Counter0IOPort);
    ClaimIOPort(Counter1IOPort);
    ClaimIOPort(Counter2IOPort);
    ClaimIOPort(CommandIOPort);

    uint16_t divisor = MaxHertz / mHertz;
    uint8_t low = divisor & 0xFF;
    uint8_t high = (divisor >> 8) & 0xFF;

    outb(CommandIOPort, Command::Counter0InitializeCommand);
    outb(Counter0IOPort, low);
    outb(Counter0IOPort, high);

    Core::IDT::RegisterHandler(Core::IDT::RemappedIRQBase + BaseIRQ, this);

    return true;
}

void Driver::PIT8254::OnUnregister()
{
    ReleaseIOPort(Counter0IOPort);
    ReleaseIOPort(Counter1IOPort);
    ReleaseIOPort(Counter2IOPort);
    ReleaseIOPort(CommandIOPort);
}

void Driver::PIT8254::OnInterrupt(Core::InterruptRegisters)
{
    if (Core::SystemClock::IsReady())
    {
        ++mCycle;
        mMillisecondsElapsed += mMillisecondsPerCycle;
        if (mCycle >= mHertz)
        {
            mCycle = 0;
            mMillisecondsElapsed = 0;
            ++mSecondsElapsed;

	        //time_t t = time(NULL);
	        //printf("Time = %s", ctime(&t));
        }
    }
}

uint16_t Driver::PIT8254::GetMillisecondsElapsed() { return mMillisecondsElapsed; }

uint32_t Driver::PIT8254::GetSecondsElapsed() { return mSecondsElapsed; }
