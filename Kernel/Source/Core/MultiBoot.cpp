#include <Core/MultiBoot.h>

using namespace Core;

MultiBoot::Header* MultiBoot::sHeader = nullptr;
MultiBoot::MemoryBlockArray MultiBoot::sMemoryBlocks;
uint8_t MultiBoot::sMemoryBlockCount = 0;

extern "C" {
extern char __BOF;
extern char __EOF;
}

bool MultiBoot::Initialize(uint32_t pMultiBootMagic, void * pMultiBootData)
{
    if (pMultiBootMagic != Magic) return false;
    sHeader = reinterpret_cast<Header*>(pMultiBootData);
    sMemoryBlocks.fill(MemoryBlock());

	uint32_t kernelHead = (uint32_t)&__BOF;
	uint32_t kernelTail = (uint32_t)&__EOF;
	uint32_t kernelSize = kernelTail - kernelHead;

    MemoryMap* memoryMap = reinterpret_cast<MemoryMap*>(sHeader->mmap_addr);
    uint32_t memoryMapSize = memoryMap->size + 4;
    uint32_t memoryMapCount = sHeader->mmap_length / memoryMapSize;

	for (uint32_t memoryMapIndex = 0;
         memoryMapIndex < memoryMapCount;
         ++memoryMapIndex, memoryMap = reinterpret_cast<MemoryMap*>(sHeader->mmap_addr + (memoryMapIndex * memoryMapSize)))
	{
		if (memoryMap->type == MemoryMapAvailable)
		{
			uint32_t address = memoryMap->addr0;
			uint32_t length = memoryMap->length0;
			if (address + length <= MemoryMapConventional) continue;
			if (kernelHead >= address && kernelTail <= (address + length))
			{
				address += kernelSize;
				length -= kernelSize;
			}
			sMemoryBlocks[sMemoryBlockCount].Address = address;
			sMemoryBlocks[sMemoryBlockCount].Length = length;
			sMemoryBlocks[sMemoryBlockCount].Used = 0;
			++sMemoryBlockCount;
		}
	}
    return true;
}

