#include "IDT.h"
#include "GDT.h"
#include "PortIO.h"
#include "Processor.h"
#include "SMP.h"
#include "ThreadScheduler.h"
#include "TSS.h"

extern "C" {
void Started();
void SMPInit();
void SMPInitEnd();
void GDTSwitchToUserMode();

uint8_t* gSMPTempStack = nullptr;
uint8_t* gSMPTempStackBottom = nullptr;
uint8_t* gSMPTempStackTop = nullptr;
uint16_t gSMPStartedProcessors = 0;

void SMPEnteredUserMode()
{
	uint32_t taskRegister = TSSGetRegister();
	uint16_t processorIndex = (taskRegister - 0x28) >> 3;
	Processor* processor = Processor::sProcessors[processorIndex];
	printf("Processor %u Ready, %uMHz Bus\n", processorIndex, (unsigned int)(processor->BusFrequency / 1000 / 1000));
	++gSMPStartedProcessors;
	if (processor->Index == 0) ThreadScheduler::Startup((uint32_t)&Started, 0x100000);
	while (true) ;
}

uint32_t gEnteredUserModeAddress = (uint32_t)&SMPEnteredUserMode;

void SMPStarted()
{
	IDTUpdateRegister();
	new Processor();
	GDTSwitchToUserMode();
	while (true) ;
}
}

namespace SMP {
	class FPS
	{
	public:
		char Signature[4];
		uint32_t ConfigurationBaseAddress;
		uint8_t Length;
		uint8_t SpecificationRevision;
		uint8_t Checksum;
		uint8_t FeatureInformation1;
		uint8_t FeatureInformation2;
		uint8_t Reserved[3];
	};

	class Config
	{
	public:
		static const uint8_t ENTRYTYPE_CPU = 0;
		static const uint8_t ENTRYTYPE_BUS = 1;
		static const uint8_t ENTRYTYPE_IOAPIC = 2;
		static const uint8_t ENTRYTYPE_IOINTERRUPTASSIGNMENT = 3;
		static const uint8_t ENTRYTYPE_LOCALINTERRUPTASSIGNMENT = 4;

		char Signature[4];
		uint16_t Length;
		uint8_t SpecificationRevision;
		uint8_t Checksum;
		char OEMID[8];
		char ProductID[12];
		uint32_t OEMBaseAddress;
		uint16_t OEMLength;
		uint16_t EntryCount;
		uint32_t LocalAPICBaseAddress;
		uint16_t ExtendedLength;
		uint8_t ExtendedChecksum;
		uint8_t Reserved;
	};

	class CPU
	{
	public:
		static const uint8_t FLAGS_AVAILABLE = (1 << 0);
		static const uint8_t FLAGS_BOOTSTRAP = (1 << 1);

		uint8_t EntryType;
		uint8_t LocalAPICID;
		uint8_t LocalAPICVersion;
		uint8_t Flags;
		uint32_t Signature;
		uint32_t FeatureFlags;
		uint32_t Reserved[2];
	};

	FPS* LocateFPS()
	{
		FPS* fps = nullptr;
		for (char* ptr = 0; (unsigned int)ptr < (1024 * 1024); ptr += 16)
		{
			if (*ptr == '_' && *(ptr + 1) == 'M' && *(ptr + 2) == 'P' && *(ptr + 3) == '_')
			{
				fps = reinterpret_cast<FPS*>(ptr);
				break;
			}
		}
		return fps;
	}

	void Startup()
	{
		FPS* fps = LocateFPS();
		uint8_t* configEntries = reinterpret_cast<uint8_t*>(fps->ConfigurationBaseAddress + sizeof(Config));
		uint8_t* configEntryIterator = configEntries;
		uint16_t cpuCount = 0;
		while (*configEntryIterator == Config::ENTRYTYPE_CPU)
		{
			++cpuCount;
			configEntryIterator += sizeof(CPU);
		}
		GDT::AllocateTSS(cpuCount);
		Processor::AllocateProcessors(cpuCount);

		Processor* bootstrapProcessor = new Processor();

		gSMPTempStack = new uint8_t[0x4000 * cpuCount];
		gSMPTempStackBottom = gSMPTempStack + (0x4000 * cpuCount);
		gSMPTempStackTop = gSMPTempStackBottom;

		uint32_t realModeEntryPoint = 0x8000;
		uint32_t realModeGDTBaseAddress = 0x9000;
		uint32_t realModeGDTRegisterAddress = 0xA000;
		memcpy(reinterpret_cast<void*>(realModeEntryPoint), reinterpret_cast<void*>(&SMPInit), static_cast<size_t>(reinterpret_cast<uint8_t*>(&SMPInitEnd) - reinterpret_cast<uint8_t*>(&SMPInit)));
		GDT::CopyForRealMode(realModeGDTBaseAddress, realModeGDTRegisterAddress);
		outb(0x70, 0x0F);
		outb(0x71, 0x0A);
		uint32_t previousWarmBootOffset = *reinterpret_cast<uint32_t*>(0x467);
		*reinterpret_cast<uint32_t*>(0x467) = realModeEntryPoint;

		configEntryIterator = configEntries;
		while (*configEntryIterator == Config::ENTRYTYPE_CPU)
		{
			CPU* cpu = reinterpret_cast<CPU*>(configEntryIterator);
			if (!(cpu->Flags & CPU::FLAGS_BOOTSTRAP))
			{
				uint16_t startedProcessors = gSMPStartedProcessors;

				uint32_t icrHigh = (uint32_t)(cpu->LocalAPICID << 24);
				uint32_t icrLow = (1 << 8) | (1 << 10) | (1 << 14);
				bootstrapProcessor->SendInterruptCommand(icrLow, icrHigh);

				icrHigh = (uint32_t)(cpu->LocalAPICID << 24);
				icrLow = ((realModeEntryPoint >> 12) & 0xFF) | (1 << 9) | (1 << 10) | (1 << 14);
				bootstrapProcessor->SendInterruptCommand(icrLow, icrHigh);

				if (gSMPStartedProcessors == startedProcessors)
				{
					icrHigh = (uint32_t)(cpu->LocalAPICID << 24);
					icrLow = ((realModeEntryPoint >> 12) & 0xFF) | (1 << 9) | (1 << 10) | (1 << 14);
					bootstrapProcessor->SendInterruptCommand(icrLow, icrHigh);
				}

				while (gSMPStartedProcessors == startedProcessors) ;
			}
			configEntryIterator += sizeof(CPU);
		}
		outb(0x70, 0x0F);
		outb(0x71, 0x00);
		*reinterpret_cast<uint32_t*>(0x467) = previousWarmBootOffset;

		GDTSwitchToUserMode();
	}
}
