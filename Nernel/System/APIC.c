#include "APIC.h"
#include "Common.h"
#include "IDT.h"
#include "IDT_ExternalStubs.h"
#include "MSR.h"
#include "PIC.h"
#include "x86/PortIO.h"

#define APIC__MSR									0x1B
#define APIC__MSR_Enable							0x800

#define APIC__BaseAddress_Mask						0xFFFFF000
#define APIC__IRQRedirect_Max						24

#define APIC__IOAPIC__Register_Select				0x00
#define APIC__IOAPIC__Register_Window				0x10

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


size_t gAPIC_BaseAddress = 0;
size_t gAPIC_IOBaseAddress = 0xFEC00000;
uint32_t gAPIC_LegacyTimer_MaxHertz = 1193182;
uint16_t gAPIC_LegacyTimer_Hertz = 100;
uint16_t gAPIC_LegacyTimer_MillisecondsPerCycle = 0;
bool_t gAPIC_FrequencyTimerFinished = FALSE;
uint32_t gAPIC_Timer_Hertz = 100;
uint64_t gAPIC_CPUBusFrequency = 0;
uint32_t gAPIC_MillisecondsElapsed = 0;
uint32_t gAPIC_SecondsElapsed = 0;

void APIC_LegacyTimer(InterruptRegisters pRegisters)
{
	static uint32_t frequencyTimerCount = 0;
	if (!gAPIC_FrequencyTimerFinished)
	{
		++frequencyTimerCount;
		if (frequencyTimerCount == 1)
		{
			*(size_t*)(gAPIC_BaseAddress + APIC__Register__Timer__InitialCount) = 0xFFFFFFFF;
		}
		else if (frequencyTimerCount == (unsigned int)(gAPIC_LegacyTimer_Hertz + 1))
		{
			*(size_t*)(gAPIC_BaseAddress + APIC__Register__LVT__Timer) = APIC__Flags__Disable;
			gAPIC_CPUBusFrequency = (((0xFFFFFFFF - *(size_t*)(gAPIC_BaseAddress + APIC__Register__Timer__CurrentCount)) + 1) * 16);
			gAPIC_FrequencyTimerFinished = TRUE;
		}
	}
	gAPIC_MillisecondsElapsed += gAPIC_LegacyTimer_MillisecondsPerCycle;
	if (gAPIC_MillisecondsElapsed >= 1000)
	{
		++gAPIC_SecondsElapsed;
		gAPIC_MillisecondsElapsed -= 1000;

		//time_t tickTime = time(NULL);
		//Log_WriteLine(LOGLEVEL__Information, "Ticked @ %24.24s", ctime(&tickTime));
	}
}

void APIC_SchedulerTimer(InterruptRegisters pRegisters)
{
	*(size_t*)(gAPIC_BaseAddress + APIC__Register__EndOfInterrupt) = 0;
}

void APIC_Startup()
{
	IDT_RegisterHandler(IDT__IRQ__RemappedBase + 0, &APIC_LegacyTimer);
	IDT_SetInterrupt(IDT__IRQ__RemappedBase + 16 + 0, (uint32_t)APIC_TimerExpired, IDT__Selector__DescriptorIndex, IDT__Type__Interrupt386_Gate32Bit | IDT__Type__Present);
	IDT_SetInterrupt(IDT__IRQ__RemappedBase + 16 + 7, (uint32_t)APIC_TimerCycle, IDT__Selector__DescriptorIndex, IDT__Type__Interrupt386_Gate32Bit | IDT__Type__Present);

	gAPIC_BaseAddress = MSR_Read(APIC__MSR) & APIC__BaseAddress_Mask;
	MSR_Write(APIC__MSR, gAPIC_BaseAddress | APIC__MSR_Enable);

	*(size_t*)(gAPIC_BaseAddress + APIC__Register__SpuriousInterruptVector) = (IDT__IRQ__RemappedBase + 16 + 7) | APIC__Flags__SoftwareEnable;
	*(size_t*)(gAPIC_BaseAddress + APIC__Register__LVT__Timer) = (IDT__IRQ__RemappedBase + 16 + 0);
	*(size_t*)(gAPIC_BaseAddress + APIC__Register__Timer__Divisor) = 0x03;

	gAPIC_LegacyTimer_MillisecondsPerCycle = 1000 / gAPIC_LegacyTimer_Hertz;
	uint32_t divisor = gAPIC_LegacyTimer_MaxHertz / gAPIC_LegacyTimer_Hertz;
	uint8_t low = divisor & 0xFF;
	uint8_t high = (divisor >> 8) & 0xFF;

    outb(0x43, 0x36);
    outb(0x40, low);
    outb(0x40, high);

	PIC_StartInterrupts();
	while (!gAPIC_FrequencyTimerFinished) ;

	IDT_SetInterrupt(0x30, (uint32_t)IDT_ISR30, IDT__Selector__DescriptorIndex, IDT__Type__Interrupt386_Gate32Bit | IDT__Type__Present);
	IDT_SetInterrupt(0x37, (uint32_t)IDT_ISR37, IDT__Selector__DescriptorIndex, IDT__Type__Interrupt386_Gate32Bit | IDT__Type__Present);

	printf("CPU Bus Frequency = %u MHz\n", (unsigned int)(gAPIC_CPUBusFrequency / 1000 / 1000));	

	divisor = gAPIC_CPUBusFrequency / 16 / gAPIC_Timer_Hertz;
	IDT_RegisterHandler(IDT__IRQ__RemappedBase + 16 + 0, &APIC_SchedulerTimer);

	*(size_t*)(gAPIC_BaseAddress + APIC__Register__Timer__InitialCount) = divisor < 16 ? 16 : divisor;
	*(size_t*)(gAPIC_BaseAddress + APIC__Register__LVT__Timer) = (IDT__IRQ__RemappedBase + 16 + 0) | APIC__Timer__Periodic;
	*(size_t*)(gAPIC_BaseAddress + APIC__Register__Timer__Divisor) = 0x03;

	/*
	*(size_t*)(gAPIC_IOBaseAddress + APIC__IOAPIC__Register_Select) = 0;
	uint32_t ioAPICID = *(size_t*)(gAPIC_IOBaseAddress + APIC__IOAPIC__Register_Window);
	*(size_t*)(gAPIC_IOBaseAddress + APIC__IOAPIC__Register_Select) = 1;
	uint32_t ioAPICVersion = *(size_t*)(gAPIC_IOBaseAddress + APIC__IOAPIC__Register_Window);

	printf("IOAPICID = 0x%x, IOAPICVersion = 0x%x\n", (unsigned int)ioAPICID, (unsigned int)ioAPICVersion);
	for (uint8_t irq = 0; irq < APIC__IRQRedirect_Max; ++irq)
	{
		*(size_t*)(gAPIC_IOBaseAddress + APIC__IOAPIC__Register_Select) = 0x10 + (irq * 2);
		uint32_t ioAPICLowerTableEntry = *(size_t*)(gAPIC_IOBaseAddress + APIC__IOAPIC__Register_Window);
		if (irq < 0x10)
		printf ("Rewriting IOApicEntry IRQ %u, 0x%x to 0x%x\n", (unsigned int)irq, (unsigned int)ioAPICLowerTableEntry, (unsigned int)(ioAPICLowerTableEntry & 0xFFFFFF00) | (IDT__IRQ__RemappedBase + irq));
		ioAPICLowerTableEntry = (ioAPICLowerTableEntry & 0xFFFFFF00) | (IDT__IRQ__RemappedBase + irq);
		*(size_t*)(gAPIC_IOBaseAddress + APIC__IOAPIC__Register_Select) = 0x10 + (irq * 2);
		*(size_t*)(gAPIC_IOBaseAddress + APIC__IOAPIC__Register_Window) = ioAPICLowerTableEntry;
	}
	*/
}

void APIC_Shutdown()
{
}
