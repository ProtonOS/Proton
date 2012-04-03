#include <System/GDT.h>
#include <System/MSR.h>
#include <System/PIC.h>
#include <System/SMP.h>
#include <System/SystemClock.h>
#include <System/x86/PortIO.h>

size_t gSMP_FPSAddress = 0;
volatile bool_t gSMP_StartingBootstrapProcessor = FALSE;
volatile uint8_t gSMP_StartedProcessors = 0;
volatile uint8_t volatile * gSMP_TempStack = NULL;
volatile uint8_t volatile * gSMP_TempStackBottom = NULL;
volatile uint8_t volatile * gSMP_TempStackTop = NULL;

void SMP_LogicalProcessorStartup()
{
	IDT_Update(&gIDT_Register);
	//PIC_StartInterrupts();
	APIC_Create();
	//printf("Switching AP to User Mode\n");
	GDT_SwitchToUserMode();
	while(TRUE) ;
}

void SMP_Startup(APIC* pBootstrapAPIC)
{
	// Crazy search algorithm, should be fixed later to check the 3 explicit regions
	for (char* ptr = 0; (unsigned int)ptr < (1024 * 1024); ptr += 16)
	{
		if (*ptr == '_' && *(ptr + 1) == 'M' && *(ptr + 2) == 'P' && *(ptr + 3) == '_')
		{
			gSMP_FPSAddress = (size_t)ptr;
			break;
		}
	}
	if (gSMP_FPSAddress)
	{
		gSMP_TempStack = calloc(1, 0x400000);
		gSMP_TempStackBottom = gSMP_TempStack + 0x400000;
		gSMP_TempStackTop = gSMP_TempStackBottom;
		SMP_FPS* fps = (SMP_FPS*)gSMP_FPSAddress;
		//SMP_ConfigHeader* configHeader = (SMP_ConfigHeader*)fps->ConfigurationBaseAddress;
		uint8_t* configEntries = (uint8_t*)(fps->ConfigurationBaseAddress + sizeof(SMP_ConfigHeader));
		uint8_t* configEntryIterator = configEntries;

		uint32_t realModeEntryPoint = 0x8000;
		uint32_t realModeGDTBaseAddress = 0x9000;
		uint32_t realModeGDTRegisterAddress = 0xA000;
		memcpy((void*)realModeEntryPoint, (void*)&GDT_LogicalProcessorInit, (unsigned int)&GDT_LogicalProcessorInitEnd - (unsigned int)&GDT_LogicalProcessorInit);
		memcpy((void*)realModeGDTBaseAddress, (void*)&gGDT_Descriptors[0], (unsigned int)(GDT__Descriptors_Max * sizeof(GDTDescriptor)));
		memcpy((void*)realModeGDTRegisterAddress, (void*)&gGDT_Register, sizeof(GDTRegister));
		((GDTRegister*)realModeGDTRegisterAddress)->Address = 0x9000;
		outb(0x70, 0x0F);
		outb(0x71, 0x0A);
		uint32_t previousWarmBootOffset = *((volatile unsigned int volatile *)0x467);
		*((volatile unsigned int volatile *)0x467) = realModeEntryPoint;
		while (*configEntryIterator == SMP__ConfigEntry__EntryType_Processor)
		{
			SMP_Processor* processor = (SMP_Processor*)configEntryIterator;
			if (!(processor->Flags & SMP__Processor__Flags_Bootstrap))
			{
				uint8_t startedProcessorCount = gSMP_StartedProcessors;
				//*(volatile size_t volatile *)(pBootstrapAPIC->BaseAddress + APIC__Register__ErrorStatus) = 0;

				uint32_t icrHigh = (uint32_t)(processor->LocalAPICID << 24);
				uint32_t icrLow = (1 << 8) | (1 << 10) | (1 << 14);
				*(volatile size_t volatile *)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandHigh) = icrHigh;
				*(volatile size_t volatile *)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandLow) = icrLow;
				while ((*(volatile size_t volatile *)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandLow) & (1 << 12)) != 0) IOWAIT();

				icrHigh = (uint32_t)(processor->LocalAPICID << 24);
				icrLow = ((realModeEntryPoint >> 12) & 0xFF) | (1 << 9) | (1 << 10) | (1 << 14);
				*(volatile size_t volatile *)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandHigh) = icrHigh;
				*(volatile size_t volatile *)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandLow) = icrLow;
				while ((*(volatile size_t volatile *)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandLow) & (1 << 12)) != 0) IOWAIT();

				if (gSMP_StartedProcessors == startedProcessorCount)
				{
					icrHigh = (uint32_t)(processor->LocalAPICID << 24);
					icrLow = ((realModeEntryPoint >> 12) & 0xFF) | (1 << 9) | (1 << 10) | (1 << 14);
					*(volatile size_t volatile *)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandHigh) = icrHigh;
					*(volatile size_t volatile *)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandLow) = icrLow;
					while ((*(volatile size_t volatile *)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandLow) & (1 << 12)) != 0) IOWAIT();
				}

				while (gSMP_StartedProcessors == startedProcessorCount) IOWAIT();
				//*(volatile size_t volatile *)(pBootstrapAPIC->BaseAddress + APIC__Register__ErrorStatus) = 0;
			}
			configEntryIterator += sizeof(SMP_Processor);
		}
		outb(0x70, 0x0F);
		outb(0x71, 0x00);
		*((volatile unsigned int *)0x467) = previousWarmBootOffset;
	}
	gSMP_StartingBootstrapProcessor = TRUE;
}

void SMP_Shutdown()
{
}
