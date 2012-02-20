#include <Core/PIT.h>
#include <Core/RTC.h>
#include <Core/SystemClock.h>

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

uint32_t SystemClock_GetSecondsSinceEpoch() { return gSystemClock_StartupTime + PIT_GetSecondsElapsed(); }

uint16_t SystemClock_GetMilliseconds() { return PIT_GetMillisecondsElapsed(); }

uint64_t SystemClock_GetTicks() { return PIT_GetMillisecondsSinceStartup(); }