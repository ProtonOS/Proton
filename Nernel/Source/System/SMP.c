#include <System/GDT.h>
#include <System/PIC.h>
#include <System/SMP.h>
#include <System/x86/PortIO.h>

size_t gSMP_FPSAddress = 0;
volatile uint8_t gSMP_StartedProcessors = 0;

void SMP_LogicalProcessorStartup()
{
	gSMP_StartedProcessors = 1;
	//printf("WOOOOOOOOOOOOOOOOOOT\n");
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
		SMP_FPS* fps = (SMP_FPS*)gSMP_FPSAddress;
		SMP_ConfigHeader* configHeader = (SMP_ConfigHeader*)fps->ConfigurationBaseAddress;
		printf("MP Table @ 0x%x, Size = %u, Revision = %u, Config @ 0x%x\n", (unsigned int)fps, (unsigned int)fps->Length, (unsigned int)fps->SpecificationRevision, (unsigned int)configHeader);
		printf("MP Config Entry Count = %u\n", (unsigned int)configHeader->EntryCount);
		uint8_t* configEntries = (uint8_t*)(fps->ConfigurationBaseAddress + sizeof(SMP_ConfigHeader));
		uint8_t* configEntryIterator = configEntries;

		uint32_t realModeEntryPoint = 0x8000;
		uint32_t realModeGDTBaseAddress = 0x9000;
		memcpy((void*)realModeEntryPoint, &GDT_LogicalProcessorInit, (unsigned int)&GDT_LogicalProcessorInitEnd - (unsigned int)&GDT_LogicalProcessorInit);
		memcpy((void*)realModeGDTBaseAddress, &gGDT_Descriptors[0], GDT__Descriptors_Max * sizeof(GDTDescriptor));
		printf("GDT_LogicalProcessorInit @ 0x%x, copied to 0x%x, Size = 0x%x\n", (unsigned int)&GDT_LogicalProcessorInit, (unsigned int)realModeEntryPoint, (unsigned int)&GDT_LogicalProcessorInitEnd - (unsigned int)&GDT_LogicalProcessorInit);
		outb(0x70, 0x0F);
		outb(0x71, 0x0A);
		uint32_t previousWarmBootOffset = *((volatile unsigned int *)0x467);
		*((volatile unsigned int *)0x467) = 0;
		*((volatile unsigned int *)0x469) = realModeEntryPoint >> 4;//((realModeEntryPoint & 0xFF000) << 12);
		while (*configEntryIterator == SMP__ConfigEntry__EntryType_Processor)
		{
			SMP_Processor* processor = (SMP_Processor*)configEntryIterator;
			if (!(processor->Flags & SMP__Processor__Flags_Bootstrap))
			{
				printf("MP Processor: LocalAPICID = %u, LocalAPICVersion = %u, Flags = 0x%x\n", (unsigned int)processor->LocalAPICID, (unsigned int)processor->LocalAPICVersion, (unsigned int)processor->Flags);
				printf("Signature = 0x%x, FeatureFlags = 0x%x\n", (unsigned int)processor->Signature, (unsigned int)processor->FeatureFlags);

				*(size_t*)(pBootstrapAPIC->BaseAddress + APIC__Register__ErrorStatus) = 0;

				*(size_t*)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandHigh) = (uint32_t)processor->LocalAPICID << 24;
				*(size_t*)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandLow) = 0xC500;
				for (uint64_t delay = 0; delay < 10000000; ++delay);

				*(size_t*)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandHigh) = (uint32_t)processor->LocalAPICID << 24;
				*(size_t*)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandLow) = 0x8500;
				for (uint64_t delay = 0; delay < 10000000; ++delay);

				*(size_t*)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandHigh) = (uint32_t)processor->LocalAPICID << 24;
				*(size_t*)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandLow) = 0x0600 | ((realModeEntryPoint >> 12) & 0xFF);
				for (uint64_t delay = 0; delay < 10000000; ++delay);

				if (!gSMP_StartedProcessors)
				{
					*(size_t*)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandHigh) = (uint32_t)processor->LocalAPICID << 24;
					*(size_t*)(pBootstrapAPIC->BaseAddress + APIC__Register__InterruptCommandLow) = 0x0600 | ((realModeEntryPoint >> 12) & 0xFF);
					for (uint64_t delay = 0; delay < 10000000; ++delay);
				}

				printf("Waiting for processor to startup...\n");
				while (!gSMP_StartedProcessors) IOWAIT();
				*(size_t*)(pBootstrapAPIC->BaseAddress + APIC__Register__ErrorStatus) = 0;
				printf("Should be booted!\n");
			}
			configEntryIterator += sizeof(SMP_Processor);
		}
		outb(0x70, 0x0F);
		outb(0x71, 0x00);
		*((volatile unsigned int *)0x467) = previousWarmBootOffset;
	}
}

void SMP_Shutdown()
{
}
