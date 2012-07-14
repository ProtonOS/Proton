#include "GDT.h"

extern "C" {
void GDTUpdateRegister(void);
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
}
