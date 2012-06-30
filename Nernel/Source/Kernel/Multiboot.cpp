#include "Multiboot.h"
#include <strings.h>

#define MULTIBOOT_MAGIC								0x2BADB002
#define MULTIBOOT_MEMORYMAP_TYPE_AVAILABLE			1
#define MULTIBOOT_MEMORYMAP_LOWER_MEMORY_SIZE		(1024 * 1024)

#define LOADEDMODULE_MAX							64
#define MEMORYBLOCK_MAX								64
#define RESERVEDMEMORYBLOCK_MAX						64


const char* Multiboot::sCommandLine = NULL;
Multiboot::LoadedModule Multiboot::sLoadedModules[LOADEDMODULE_MAX];
uint8_t Multiboot::sLoadedModuleCount = 0;
Multiboot::MemoryBlock Multiboot::sMemoryBlocks[MEMORYBLOCK_MAX];
uint8_t Multiboot::sMemoryBlockCount = 0;
uint8_t Multiboot::sCurrentMemoryBlock = 0;

Multiboot::LoadedModule::LoadedModule()
{
	mAddress = 0;
	mSize = 0;
	mIdentifier = 0;
	mReserved = 0;
}

void Multiboot::LoadedModule::Initialize(size_t pAddress, size_t pSize, const char* pIdentifier)
{
	mAddress = pAddress;
	mSize = pSize;
	mIdentifier = pIdentifier;
}

void Multiboot::Startup(uint32_t pMultibootMagic, void* pMultibootHeader)
{
    if (pMultibootMagic != MULTIBOOT_MAGIC) Halt();
	Header* header = reinterpret_cast<Header*>(pMultibootHeader);
    sCommandLine = header->CommandLine;

    extern char __SOK;
    extern char __EOK;
	extern char gStack;
	extern size_t gStackSize;
	size_t kernelStart = (size_t)&__SOK;
	size_t kernelEnd = (size_t)&__EOK;
	size_t kernelSize = kernelEnd - kernelStart;
	size_t stackStart = (size_t)&gStack;
	size_t stackSize = gStackSize;

	ReservedMemoryBlock reservedMemoryBlocks[RESERVEDMEMORYBLOCK_MAX] = { { kernelStart, kernelSize }, { stackStart, stackSize } };
	uint8_t reservedMemoryBlockCount = 2;

    Module* module = header->Modules;
    uint32_t moduleCount = header->ModulesCount;

	for (uint32_t moduleIndex = 0; moduleIndex < moduleCount; ++moduleIndex, ++module)
	{
		sLoadedModules[sLoadedModuleCount].Initialize(module->Start, module->End - module->Start, module->Identifier);
        ++sLoadedModuleCount;

		reservedMemoryBlocks[reservedMemoryBlockCount].Address = module->Start;
		reservedMemoryBlocks[reservedMemoryBlockCount].Size = module->End - module->Start;
		++reservedMemoryBlockCount;
	}

    MemoryMap* memoryMap = header->MemoryMaps;
    uint32_t memoryMapCount = header->MemoryMapsSize / sizeof(MemoryMap);
	sMemoryBlockCount = 0;
	for (uint32_t memoryMapIndex = 0; memoryMapIndex < memoryMapCount; ++memoryMapIndex, ++memoryMap)
	{
		if (memoryMap->Type == MULTIBOOT_MEMORYMAP_TYPE_AVAILABLE && memoryMap->AddressLower >= MULTIBOOT_MEMORYMAP_LOWER_MEMORY_SIZE)
		{
			sMemoryBlocks[sMemoryBlockCount].Address = memoryMap->AddressLower;
			sMemoryBlocks[sMemoryBlockCount].Size = memoryMap->LengthLower;
			++sMemoryBlockCount;
		}
	}

	ReservedMemoryBlock* reservedMemoryBlock = reservedMemoryBlocks;
	for (uint32_t reservedMemoryBlockIndex = 0; reservedMemoryBlockIndex < reservedMemoryBlockCount; ++reservedMemoryBlockIndex, ++reservedMemoryBlock)
	{
		MemoryBlock* memoryBlock = sMemoryBlocks;
		for (uint32_t memoryBlockIndex = 0; memoryBlockIndex < sMemoryBlockCount; ++memoryBlockIndex, ++memoryBlock)
		{
			if ((reservedMemoryBlock->Address >= memoryBlock->Address && reservedMemoryBlock->Address < (memoryBlock->Address + memoryBlock->Size)) ||
				((reservedMemoryBlock->Address + reservedMemoryBlock->Size) > memoryBlock->Address && (reservedMemoryBlock->Address + reservedMemoryBlock->Size) < (memoryBlock->Address + memoryBlock->Size)))
			{
				if (reservedMemoryBlock->Address == memoryBlock->Address)
				{
					memoryBlock->Address = memoryBlock->Address + reservedMemoryBlock->Size;
					memoryBlock->Size = memoryBlock->Size - reservedMemoryBlock->Size;
					if (memoryBlock->Size == 0)
					{
						for (uint32_t copyBlockIndex = memoryBlockIndex + 1; copyBlockIndex < sMemoryBlockCount; ++copyBlockIndex)
						{
							sMemoryBlocks[copyBlockIndex - 1] = sMemoryBlocks[copyBlockIndex];
						}
						--sMemoryBlockCount;
						break;
					}
				}
				else if ((reservedMemoryBlock->Address + reservedMemoryBlock->Size) == (memoryBlock->Address + memoryBlock->Size))
				{
					memoryBlock->Size = memoryBlock->Size - reservedMemoryBlock->Size;
					break;
				}
				else if (sMemoryBlockCount == MEMORYBLOCK_MAX)
				{
					Panic("Insufficient memory blocks to process available memory");
				}
				else
				{
					++sMemoryBlockCount;
					for (uint32_t copyBlockIndex = sMemoryBlockCount - 1; copyBlockIndex > memoryBlockIndex; --copyBlockIndex)
					{
						sMemoryBlocks[copyBlockIndex] = sMemoryBlocks[copyBlockIndex - 1];
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
	if (sMemoryBlockCount == 0)
	{
		Panic("There is no memory blocks available");
	}
}

void Multiboot::Shutdown()
{
	sCommandLine = NULL;
	sLoadedModuleCount = 0;
	sMemoryBlockCount = 0;
}

Multiboot::LoadedModule* Multiboot::GetLoadedModule(const char* pFilename)
{
    for (uint8_t index = 0; index < sLoadedModuleCount; ++index)
    {
        if (!strcasecmp(sLoadedModules[index].GetIdentifier(), pFilename)) return &sLoadedModules[index];
    }
	return NULL;
}

void* Multiboot::AdjustMemory(ptrdiff_t pAdjustment)
{
	MemoryBlock* memoryBlock = &sMemoryBlocks[sCurrentMemoryBlock];
	if (pAdjustment >= 0)
	{
        uint32_t adjustment = (uint32_t)pAdjustment;
		uint8_t originalIndex = sCurrentMemoryBlock;
		while ((memoryBlock->Size - memoryBlock->Used) < adjustment &&
               sCurrentMemoryBlock < sMemoryBlockCount)
		{
			++sCurrentMemoryBlock;
		    if (sCurrentMemoryBlock >= sMemoryBlockCount)
		    {
			    sCurrentMemoryBlock = originalIndex;
			    Panic("Insufficient memory for allocator break");
		    }
			memoryBlock = &sMemoryBlocks[sCurrentMemoryBlock];
		}
		memoryBlock->Used += adjustment;
		return (void*)(memoryBlock->Address + (memoryBlock->Used - adjustment));
	}
	memoryBlock->Used += pAdjustment;
	if (memoryBlock->Used == 0 && sCurrentMemoryBlock > 0) --sCurrentMemoryBlock;
    return NULL;
}
