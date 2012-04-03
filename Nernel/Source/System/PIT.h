#pragma once

#include "APIC.h"
#include "IDT.h"

void PIT_Startup();
void PIT_Shutdown();

void PIT_TestAPICFrequency(APIC* pAPIC);
bool_t PIT_TestingAPICFrequency();

extern uint16_t gPIT_CycleHertz;
extern uint32_t gPIT_MillisecondsElapsed;
extern uint32_t gPIT_SecondsElapsed;
