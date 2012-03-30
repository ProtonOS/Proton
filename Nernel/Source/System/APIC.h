#pragma once

#include "IDT.h"
#include "Thread.h"

#define APIC__LocalMSR								0x1B
#define APIC__MSR_Enable							0x800

#define APIC__Register__APIC_ID						0x020
#define APIC__Register__APIC_Version				0x030
#define APIC__Register__TaskPriority				0x080
#define APIC__Register__EndOfInterrupt				0x0B0
#define APIC__Register__LogicalDestination			0x0D0
#define APIC__Register__DestinationFormat			0x0E0
#define APIC__Register__SpuriousInterruptVector		0x0F0
#define APIC__Register__ErrorStatus					0x280
#define APIC__Register__InterruptCommandLow			0x300
#define APIC__Register__InterruptCommandHigh		0x310
// LVT = Local Vector Table
#define APIC__Register__LVT__Timer					0x320
#define APIC__Register__LVT__PerformanceCounter		0x340
#define APIC__Register__LVT__LocalInterrupt_0		0x350
#define APIC__Register__LVT__LocalInterrupt_1		0x360
#define APIC__Register__LVT__ErrorVector			0x370
#define APIC__Register__Timer__InitialCount			0x380
#define APIC__Register__Timer__CurrentCount			0x390
#define APIC__Register__Timer__Divisor				0x3E0
//#define APIC__Register_Last						0x38F

#define APIC__Flags__SoftwareEnable					0x100
#define APIC__Flags__CPUFocus						0x200
#define APIC__Flags__NonMaskableInterrupt			0x400
#define APIC__Flags__Disable						0x10000

#define APIC__Timer__Periodic						0x20000
#define APIC__Timer__BaseDivisor					0x100000

#define APIC__Timer__CycleHertz						100
#define APIC__Max									64

#define APIC__StackSize								0x100000


typedef struct _APIC APIC;

struct _APIC
{
	size_t BaseAddress;
	uint64_t BusFrequency;
	uint32_t Index;
	uint32_t TickCount;
	Thread* CurrentThread;
	uint8_t Stack[APIC__StackSize];
};

APIC* APIC_Create(uint32_t pMSR);
void APIC_Destroy(APIC* pAPIC);

extern APIC* gAPIC_Array[];
