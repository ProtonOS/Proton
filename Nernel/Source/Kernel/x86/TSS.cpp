#include "TSS.h"

namespace TSS
{
	Descriptor* sDescriptors = nullptr;
	uint32_t sDescriptorCount = 0;

	Descriptor* Add(uint8_t* pStack, size_t pStackSize)
	{
		Descriptor* descriptor = nullptr;
		if (!sDescriptors)
		{
			sDescriptors = new Descriptor[1];
			descriptor = &sDescriptors[0];
			sDescriptorCount = 1;
		}
		else
		{
			Descriptor* oldDescriptors = sDescriptors;
			sDescriptors = new Descriptor[sDescriptorCount + 1];
			for (uint32_t index = 0; index < sDescriptorCount; ++index)
			{
				sDescriptors[index].CopyFrom(&oldDescriptors[index]);
			}
			descriptor = &sDescriptors[sDescriptorCount];
			sDescriptorCount++;

			delete [] oldDescriptors;
		}
		descriptor->esp0 = (uint32_t)(pStack + pStackSize);
		descriptor->ss0 = 0x10;
		return descriptor;
	}

	Descriptor* Get(uint16_t pIndex)
	{
		return &sDescriptors[pIndex];
	}
}
