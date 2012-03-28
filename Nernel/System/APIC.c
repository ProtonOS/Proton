#include "APIC.h"
#include "Common.h"
#include "IDT.h"
#include "IDT_ExternalStubs.h"
#include "MSR.h"
#include "PIC.h"
#include "PIT.h"
#include "ThreadScheduler.h"


#define APIC__BaseAddress_Mask						0xFFFFF000
#define APIC__IRQRedirect_Max						24

#define APIC__IOAPIC__Register_Select				0x00
#define APIC__IOAPIC__Register_Window				0x10

extern void APIC_CycleTimer(InterruptRegisters pRegisters);

APIC* gAPIC_Array[APIC__Max];
uint8_t gAPIC_Count = 0;


void APIC_FrequencyTimer(InterruptRegisters pRegisters)
{
	*(size_t*)(gAPIC_Array[(pRegisters.int_no - 128) >> 1]->BaseAddress + APIC__Register__EndOfInterrupt) = 0;
}

APIC* APIC_Create(uint32_t pMSR)
{
	APIC* apic = (APIC*)calloc(1, sizeof(APIC));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: APIC_Create @ 0x%x", (unsigned int)apic);
	gAPIC_Array[gAPIC_Count] = apic;
	apic->Index = gAPIC_Count++;
	apic->BaseAddress = MSR_Read(pMSR) & APIC__BaseAddress_Mask;
	MSR_Write(pMSR, apic->BaseAddress | APIC__MSR_Enable);

	IDT_RegisterHandler(128 + (apic->Index * 2) + 0, &APIC_FrequencyTimer);
	IDT_SetInterrupt(128 + (apic->Index * 2) + 1, (uint32_t)APIC_CycleTimer, IDT__Selector__DescriptorIndex, IDT__Type__Interrupt386_Gate32Bit | IDT__Type__Present);

	*(size_t*)(apic->BaseAddress + APIC__Register__SpuriousInterruptVector) = (128 + (apic->Index * 2) + 1) | APIC__Flags__SoftwareEnable;
	*(size_t*)(apic->BaseAddress + APIC__Register__LVT__Timer) = 128 + (apic->Index * 2) + 0;
	*(size_t*)(apic->BaseAddress + APIC__Register__Timer__Divisor) = 0x03;

	PIT_TestAPICFrequency(apic);
	while (!apic->BusFrequency) ;

	printf("Logical Processor %u Bus Frequency = %u MHz\n", (unsigned int)apic->Index, (unsigned int)(apic->BusFrequency / 1000 / 1000));	

	uint32_t divisor = apic->BusFrequency / 16 / APIC__Timer__CycleHertz;
	IDT_RegisterHandler(128 + (apic->Index * 2) + 0, &ThreadScheduler_Timer);

	*(size_t*)(apic->BaseAddress + APIC__Register__Timer__InitialCount) = divisor < 16 ? 16 : divisor;
	*(size_t*)(apic->BaseAddress + APIC__Register__LVT__Timer) = (128 + (apic->Index * 2) + 0) | APIC__Timer__Periodic;
	*(size_t*)(apic->BaseAddress + APIC__Register__Timer__Divisor) = 0x03;
}

void APIC_Destroy(APIC* pAPIC)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: APIC_Destroy @ 0x%x", (unsigned int)pAPIC);
	free(pAPIC);
}

void APIC_Startup()
{
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
