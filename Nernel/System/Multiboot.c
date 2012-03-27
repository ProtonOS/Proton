#include "Common.h"
#include "Multiboot.h"

#define MULTIBOOT_MAGIC								0x2BADB002
#define MULTIBOOT_MEMORYMAP_TYPE_AVAILABLE			1
#define MULTIBOOT_MEMORYMAP_LOWER_MEMORY_SIZE		(1024 * 1024)

#define LOADEDMODULE_MAX							64
#define MEMORYBLOCK_MAX								64
#define RESERVEDMEMORYBLOCK_MAX						64


const char* gMultiboot_CommandLine = NULL;
LoadedModule gLoadedModules[LOADEDMODULE_MAX];
uint8_t gLoadedModuleCount = 0;
MemoryBlock gMemoryBlocks[MEMORYBLOCK_MAX];
uint8_t gMemoryBlockCount = 0;

void Multiboot_Startup(uint32_t pMultibootMagic, MultibootHeader* pMultibootHeader)
{
    if (pMultibootMagic != MULTIBOOT_MAGIC) Halt();
    gMultiboot_CommandLine = (const char*)pMultibootHeader->CommandLine;

    extern char __SOK;
    extern char __EOK;
	extern char gStack;
	extern char gStackSize;
	size_t kernelStart = (size_t)&__SOK;
	size_t kernelEnd = (size_t)&__EOK;
	size_t kernelSize = kernelEnd - kernelStart;
	size_t stackStart = (size_t)&gStack;
	size_t stackSize = *(size_t*)&gStackSize;

	ReservedMemoryBlock reservedMemoryBlocks[RESERVEDMEMORYBLOCK_MAX] = { { kernelStart, kernelSize }, { stackStart, stackSize } };
	uint8_t reservedMemoryBlockCount = 2;

    MultibootModule* module = pMultibootHeader->Modules;
    uint32_t moduleCount = pMultibootHeader->ModulesCount;

	for (uint32_t moduleIndex = 0; moduleIndex < moduleCount; ++moduleIndex, ++module)
	{
        gLoadedModules[gLoadedModuleCount].Address = module->Start;
        gLoadedModules[gLoadedModuleCount].Size = module->End - module->Start;
        gLoadedModules[gLoadedModuleCount].Identifier = (const char*)module->Identifier;
        ++gLoadedModuleCount;

		reservedMemoryBlocks[reservedMemoryBlockCount].Address = module->Start;
		reservedMemoryBlocks[reservedMemoryBlockCount].Size = module->End - module->Start;
		++reservedMemoryBlockCount;
	}

    MultibootMemoryMap* memoryMap = pMultibootHeader->MemoryMaps;
    uint32_t memoryMapCount = pMultibootHeader->MemoryMapsSize / sizeof(MultibootMemoryMap);
	gMemoryBlockCount = 0;
	for (uint32_t memoryMapIndex = 0; memoryMapIndex < memoryMapCount; ++memoryMapIndex, ++memoryMap)
	{
		if (memoryMap->Type == MULTIBOOT_MEMORYMAP_TYPE_AVAILABLE && memoryMap->AddressLower >= MULTIBOOT_MEMORYMAP_LOWER_MEMORY_SIZE)
		{
			gMemoryBlocks[gMemoryBlockCount].Address = memoryMap->AddressLower;
			gMemoryBlocks[gMemoryBlockCount].Size = memoryMap->LengthLower;
			gMemoryBlocks[gMemoryBlockCount].Used = 0;
			gMemoryBlocks[gMemoryBlockCount].Reserved = 0;
			++gMemoryBlockCount;
		}
	}

	ReservedMemoryBlock* reservedMemoryBlock = reservedMemoryBlocks;
	for (uint32_t reservedMemoryBlockIndex = 0; reservedMemoryBlockIndex < reservedMemoryBlockCount; ++reservedMemoryBlockIndex, ++reservedMemoryBlock)
	{
		MemoryBlock* memoryBlock = gMemoryBlocks;
		for (uint32_t memoryBlockIndex = 0; memoryBlockIndex < gMemoryBlockCount; ++memoryBlockIndex, ++memoryBlock)
		{
			if (reservedMemoryBlock->Address >= memoryBlock->Address && (reservedMemoryBlock->Address + reservedMemoryBlock->Size) <= (memoryBlock->Address + memoryBlock->Size))
			{
				if (reservedMemoryBlock->Address == memoryBlock->Address)
				{
					memoryBlock->Address += reservedMemoryBlock->Size;
					memoryBlock->Size -= reservedMemoryBlock->Size;
					if (memoryBlock->Size == 0)
					{
						for (uint32_t copyBlockIndex = memoryBlockIndex + 1; copyBlockIndex < gMemoryBlockCount; ++copyBlockIndex)
						{
							gMemoryBlocks[copyBlockIndex - 1] = gMemoryBlocks[copyBlockIndex];
						}
						--gMemoryBlockCount;
						break;
					}
				}
				else if ((reservedMemoryBlock->Address + reservedMemoryBlock->Size) <= (memoryBlock->Address + memoryBlock->Size))
				{
					memoryBlock->Size -= reservedMemoryBlock->Size;
					break;
				}
				else if (gMemoryBlockCount == MEMORYBLOCK_MAX)
				{
					Panic("Insufficient memory blocks to process available memory");
				}
				else
				{
					++gMemoryBlockCount;
					for (uint32_t copyBlockIndex = gMemoryBlockCount - 1; copyBlockIndex > memoryBlockIndex; --copyBlockIndex)
					{
						gMemoryBlocks[copyBlockIndex] = gMemoryBlocks[copyBlockIndex - 1];
					}
					size_t originalSize = memoryBlock->Size;
					memoryBlock->Size = reservedMemoryBlock->Address - memoryBlock->Address;
					(memoryBlock + 1)->Address = reservedMemoryBlock->Address + reservedMemoryBlock->Size;
					(memoryBlock + 1)->Size = originalSize - (memoryBlock->Size + reservedMemoryBlock->Size);
					break;
				}
			}
		}
	}
	if (gMemoryBlockCount == 0)
	{
		Panic("There is no memory blocks available");
	}
}

void Multiboot_Shutdown()
{
	gMultiboot_CommandLine = NULL;
	gLoadedModuleCount = 0;
	gMemoryBlockCount = 0;
}
