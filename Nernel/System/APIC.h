#pragma once

#include "IDT.h"

void APIC_Startup();
void APIC_Shutdown();

extern void APIC_TimerExpired(InterruptRegisters pRegisters);
extern void APIC_TimerCycle(InterruptRegisters pRegisters);

extern uint32_t gAPIC_MillisecondsElapsed;
extern uint32_t gAPIC_SecondsElapsed;
