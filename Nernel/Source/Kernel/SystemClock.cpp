#include "SystemClock.hpp"

#ifdef X86
#include "../Arch/X86/PIT.hpp"
#include "../Arch/X86/RTC.hpp"
#endif

UInt32 SystemClock::StartupSecondsSinceEpoch = 0;

void SystemClock::Load()
{
    StartupSecondsSinceEpoch = RTC::GetSecondsSinceEpoch();
}

bool SystemClock::IsReady() { return StartupSecondsSinceEpoch != 0; }

UInt32 SystemClock::GetSecondsSinceEpoch() { return StartupSecondsSinceEpoch + PIT::SecondsSinceStartup; }

UInt16 SystemClock::GetMillisecondsSinceSecond() { return PIT::MillisecondsSinceSecond; }

UInt64 SystemClock::GetMillisecondsSinceStartup() { return PIT::MillisecondsSinceStartup; }
