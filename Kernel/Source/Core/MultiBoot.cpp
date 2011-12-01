#include <Core/MultiBoot.h>

const char* Core::MultiBoot::sCommandLine = nullptr;
Core::MultiBoot::MemoryBlockArray Core::MultiBoot::sMemoryBlocks;
uint8_t Core::MultiBoot::sMemoryBlockCount = 0;

extern "C" {
extern char __BOF;
extern char __EOF;
}

bool Core::MultiBoot::Startup(uint32_t pMultiBootMagic, void * pMultiBootData)
{
    if (pMultiBootMagic != Magic) return false;
    Header& header = *reinterpret_cast<Header*>(pMultiBootData);
    sCommandLine = reinterpret_cast<const char*>(header.CommandLine);
    sMemoryBlocks.fill(MemoryBlock());

	uint32_t kernelHead = (uint32_t)&__BOF;
	uint32_t kernelTail = (uint32_t)&__EOF;
	uint32_t kernelSize = kernelTail - kernelHead;

    MemoryMap& memoryMap = *reinterpret_cast<MemoryMap*>(header.MemoryMapsAddress);
    uint32_t memoryMapSize = memoryMap.Size + 4;
    uint32_t memoryMapCount = header.MemoryMapsLength / memoryMapSize;

	for (uint32_t memoryMapIndex = 0;
         memoryMapIndex < memoryMapCount;
         ++memoryMapIndex, memoryMap = *reinterpret_cast<MemoryMap*>(header.MemoryMapsAddress + (memoryMapIndex * memoryMapSize)))
	{
		if (memoryMap.Type == MemoryMapAvailable)
		{
			uint32_t address = memoryMap.AddressLower;
			uint32_t length = memoryMap.LengthLower;
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
    return sMemoryBlockCount > 0;
}

void Core::MultiBoot::Shutdown()
{
    sMemoryBlockCount = 0;
    sCommandLine = nullptr;
}

const char* Core::MultiBoot::GetCommandLine() { return sCommandLine; }

Core::MultiBoot::MemoryBlock& Core::MultiBoot::GetMemoryBlock(uint8_t pIndex) { return sMemoryBlocks[pIndex]; }

uint8_t Core::MultiBoot::GetMemoryBlockCount() { return sMemoryBlockCount; }
