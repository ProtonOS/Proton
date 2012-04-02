#include <System/APIC.h>
#include <Common.h>
#include <System/GDT.h>
#include <System/IDT.h>
#include <System/IDT_ExternalStubs.h>
#include <System/MSR.h>
#include <System/PIC.h>
#include <System/PIT.h>
#include <System/ThreadScheduler.h>
#include <System/x86/PortIO.h>

#define APIC__LocalMSR								0x1B
#define APIC__MSR_Enable							0x800

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

APIC* APIC_Create()
{
	APIC* apic = (APIC*)calloc(1, sizeof(APIC));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: APIC_Create @ 0x%x", (unsigned int)apic);
	gAPIC_Array[gAPIC_Count] = apic;
	apic->Index = gAPIC_Count++;
	apic->SchedulerInterrupt = 128 + (apic->Index * 2) + 0;
	apic->CycleInterrupt = 128 + (apic->Index * 2) + 1;
	apic->BaseAddress = MSR_Read(APIC__LocalMSR) & APIC__BaseAddress_Mask;
	apic->CurrentThread = NULL;
	apic->TickCount = 0;
	MSR_Write(APIC__LocalMSR, apic->BaseAddress | APIC__MSR_Enable);

	IDT_RegisterHandler(apic->SchedulerInterrupt, &APIC_FrequencyTimer);
	IDT_SetInterrupt(apic->CycleInterrupt, (uint32_t)APIC_CycleTimer, IDT__Selector__DescriptorIndex, IDT__Type__Interrupt386_Gate32Bit | IDT__Type__Present);

	*(size_t*)(apic->BaseAddress + APIC__Register__SpuriousInterruptVector) = apic->CycleInterrupt | APIC__Flags__SoftwareEnable;
	*(size_t*)(apic->BaseAddress + APIC__Register__LVT__Timer) = apic->SchedulerInterrupt;
	*(size_t*)(apic->BaseAddress + APIC__Register__Timer__Divisor) = 0x03;

	PIC_StartInterrupts();
	PIT_TestAPICFrequency(apic);
	while (!apic->BusFrequency) ;
	PIC_StopInterrupts();

	printf("Logical Processor %u Bus Frequency = %u MHz\n", (unsigned int)apic->Index, (unsigned int)(apic->BusFrequency / 1000 / 1000));	

	uint32_t divisor = apic->BusFrequency / 16 / APIC__Timer__CycleHertz;
	IDT_RegisterHandler(apic->SchedulerInterrupt, &ThreadScheduler_Timer);

	*(size_t*)(apic->BaseAddress + APIC__Register__Timer__InitialCount) = divisor < 16 ? 16 : divisor;
	*(size_t*)(apic->BaseAddress + APIC__Register__LVT__Timer) = apic->SchedulerInterrupt | APIC__Timer__Periodic;
	*(size_t*)(apic->BaseAddress + APIC__Register__Timer__Divisor) = 0x03;

	GDT_AssignTSS(apic->Index, (uint32_t)((&apic->Stack[0]) + APIC__StackSize));
	TSS_Update(0x2B + (apic->Index * 0x08));
	return apic;
}

void APIC_Destroy(APIC* pAPIC)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: APIC_Destroy @ 0x%x", (unsigned int)pAPIC);
	free(pAPIC);
}

void APIC_WaitForICRReady(APIC* pAPIC)
{
	while ((*(size_t*)(pAPIC->BaseAddress + APIC__Register__InterruptCommandLow) & (1 << 12))) IOWAIT();
}
