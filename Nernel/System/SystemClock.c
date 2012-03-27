#include "APIC.h"
#include "RTC.h"
#include "SystemClock.h"

uint32_t gSystemClock_StartupTime = 0;

void SystemClock_Startup()
{
    gSystemClock_StartupTime = RTC_GetSecondsSinceEpoch();
}

void SystemClock_Shutdown()
{
    gSystemClock_StartupTime = 0;
}

bool_t SystemClock_IsReady() { return gSystemClock_StartupTime != 0; }

uint32_t SystemClock_GetSecondsSinceEpoch() { return gSystemClock_StartupTime + gAPIC_SecondsElapsed; }

uint16_t SystemClock_GetMilliseconds() { return gAPIC_MillisecondsElapsed; }

uint64_t SystemClock_GetTicks() { return gAPIC_MillisecondsElapsed + (gAPIC_SecondsElapsed * 1000); }
