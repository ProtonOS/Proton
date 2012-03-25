#pragma once

void SystemClock_Startup();
void SystemClock_Shutdown();

bool_t SystemClock_IsReady();
uint32_t SystemClock_GetSecondsSinceEpoch();
uint16_t SystemClock_GetMilliseconds();
//uint64_t SystemClock_GetTicks();

extern uint32_t gSystemClock_StartupTime;
