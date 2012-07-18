#include "GDT.h"
#include "TSS.h"

extern "C" {
void* gGDTRegisterPointer = nullptr;
}

namespace GDT
{
	const uint8_t ACCESS_ACCESSED = 0x01;
	const uint8_t ACCESS_READWRITE = 0x02;
	const uint8_t ACCESS_DIRECTIONCONFORMING = 0x04;
	const uint8_t ACCESS_EXECUTABLE = 0x08;
	const uint8_t ACCESS_ONE = 0x10;
	const uint8_t ACCESS_RING0 = 0x00;
	const uint8_t ACCESS_RING1 = 0x20;
	const uint8_t ACCESS_RING2 = 0x40;
	const uint8_t ACCESS_RING3 = 0x60;
	const uint8_t ACCESS_PRESENT = 0x80;
	const uint8_t ACCESS_READWRITEONEPRESENT = ACCESS_READWRITE | ACCESS_ONE | ACCESS_PRESENT;

	const uint8_t FLAGS_SELECTOR32BIT = 0x40;
	const uint8_t FLAGS_GRANULARITY4KB = 0x80;
	const uint8_t FLAGS_SELECTOR32BITGRANULARITY4KB = FLAGS_SELECTOR32BIT | FLAGS_GRANULARITY4KB;

	Register sRegister;
	Descriptor* sDescriptors = nullptr;
	uint32_t sDescriptorCount = 0;
	TSSDescriptor* sTSSDescriptors = nullptr;
	uint32_t sTSSDescriptorCount = 0;

	void Startup()
	{
		sDescriptors = new Descriptor[5];
		sDescriptorCount = 5;

		sDescriptors[0].Set(0x00000000, 0x00000000, 0x00, 0x00);
		sDescriptors[1].Set(0x00000000, 0xFFFFFFFF, ACCESS_READWRITEONEPRESENT | ACCESS_EXECUTABLE, FLAGS_SELECTOR32BITGRANULARITY4KB);
		sDescriptors[2].Set(0x00000000, 0xFFFFFFFF, ACCESS_READWRITEONEPRESENT, FLAGS_SELECTOR32BITGRANULARITY4KB);
		sDescriptors[3].Set(0x00000000, 0xFFFFFFFF, ACCESS_READWRITEONEPRESENT | ACCESS_EXECUTABLE | ACCESS_RING3, FLAGS_SELECTOR32BITGRANULARITY4KB);
		sDescriptors[4].Set(0x00000000, 0xFFFFFFFF, ACCESS_READWRITEONEPRESENT | ACCESS_RING3, FLAGS_SELECTOR32BITGRANULARITY4KB);

	    sRegister.Limit = (sizeof(Descriptor) * sDescriptorCount) - 1;
		sRegister.Address = (size_t)&sDescriptors[0];

		gGDTRegisterPointer = &sRegister;
		GDTUpdateRegister();
	}

	void AllocateTSS(uint16_t pProcessorCount)
	{
		Descriptor* oldDescriptors = sDescriptors;

		sDescriptors = new Descriptor[5 + pProcessorCount];
		sDescriptorCount = 5 + pProcessorCount;

		sDescriptors[0].Set(0x00000000, 0x00000000, 0x00, 0x00);
		sDescriptors[1].Set(0x00000000, 0xFFFFFFFF, ACCESS_READWRITEONEPRESENT | ACCESS_EXECUTABLE, FLAGS_SELECTOR32BITGRANULARITY4KB);
		sDescriptors[2].Set(0x00000000, 0xFFFFFFFF, ACCESS_READWRITEONEPRESENT, FLAGS_SELECTOR32BITGRANULARITY4KB);
		sDescriptors[3].Set(0x00000000, 0xFFFFFFFF, ACCESS_READWRITEONEPRESENT | ACCESS_EXECUTABLE | ACCESS_RING3, FLAGS_SELECTOR32BITGRANULARITY4KB);
		sDescriptors[4].Set(0x00000000, 0xFFFFFFFF, ACCESS_READWRITEONEPRESENT | ACCESS_RING3, FLAGS_SELECTOR32BITGRANULARITY4KB);

	    sRegister.Limit = (sizeof(Descriptor) * sDescriptorCount) - 1;
		sRegister.Address = (size_t)&sDescriptors[0];

		sTSSDescriptors = new TSSDescriptor[pProcessorCount];
		sTSSDescriptorCount = pProcessorCount;

		for (uint32_t index = 0; index < sTSSDescriptorCount; ++index)
		{
			uint32_t baseAddress = reinterpret_cast<uint32_t>(&sTSSDescriptors[index]);
			sDescriptors[5 + index].Set(baseAddress, baseAddress + sizeof(TSSDescriptor), 0xE9, 0x00);
			sTSSDescriptors[index].ss0 = 0x10;

		}

		GDTUpdateRegister();

		delete [] oldDescriptors;
	}

	void SetTSSStackAndRegister(uint16_t pProcessorIndex, uint32_t pStackAddress)
	{
		sTSSDescriptors[pProcessorIndex].esp0 = pStackAddress;

		TSSUpdateRegister(((5 + pProcessorIndex) << 3) | 0x03);
	}

	void CopyForRealMode(uint32_t pGDTBaseAddress, uint32_t pGDTRegisterAddress)
	{
		memcpy((void*)pGDTBaseAddress, (void*)&sDescriptors[0], (unsigned int)(sDescriptorCount * sizeof(Descriptor)));
		memcpy((void*)pGDTRegisterAddress, (void*)&sRegister, sizeof(Register));
		((Register*)pGDTRegisterAddress)->Address = pGDTBaseAddress;
	}
}
