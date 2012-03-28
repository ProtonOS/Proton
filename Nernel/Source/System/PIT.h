#pragma once

#include "APIC.h"
#include "IDT.h"

void PIT_Startup();
void PIT_Shutdown();

void PIT_TestAPICFrequency(APIC* pAPIC);

extern uint32_t gPIT_MillisecondsElapsed;
extern uint32_t gPIT_SecondsElapsed;
