#include "Multiboot.h"
#include <strings.h>

#define MULTIBOOT_MAGIC								0x2BADB002
#define MULTIBOOT_MEMORYMAP_TYPE_AVAILABLE			1
#define MULTIBOOT_MEMORYMAP_LOWER_MEMORY_SIZE		(1024 * 1024)

#define LOADEDMODULE_MAX							64
#define MEMORYBLOCK_MAX								64
#define RESERVEDMEMORYBLOCK_MAX						64

extern char __SOK;
extern char __EOK;
extern char gStack;
extern size_t gStackSize;

namespace Multiboot {
    class Header
	{
	public:
		uint32_t Flags;
		uint32_t LowerMemory;
		uint32_t UpperMemory;
		uint32_t BootDevice;
		const char* CommandLine;
		uint32_t ModulesCount;
		Module* Modules;
		uint32_t Symbolic0;
		uint32_t Symbolic1;
		uint32_t Symbolic2;
		uint32_t Symbolic3;
		uint32_t MemoryMapsSize;
		MemoryMap* MemoryMaps;
		uint32_t DrivesLength;
		uint32_t DrivesAddress;
		uint32_t ConfigTable;
		uint32_t BootLoader;
		uint32_t APMTable;
		uint32_t VBEControlInfo;
		uint32_t VBEModeInfo;
		uint16_t VBEMode;
		uint16_t VBEInterfaceSegment;
		uint16_t VBEInterfaceOffset;
		uint16_t VBEInterfaceLength;
	};

	class Module
	{
	public:
		uint32_t Start;
		uint32_t End;
		const char* Identifier;
		uint32_t Reserved;
	};

	class MemoryMap
	{
	public:
		uint32_t Size;
		uint32_t AddressLower;
		uint32_t AddressUpper;
		uint32_t LengthLower;
		uint32_t LengthUpper;
		uint32_t Type;
	};

	class ReservedMemoryBlock
	{
	public:
		size_t Address;
		size_t Size;
	};

	class MemoryBlock
	{
	public:
		size_t Address;
		size_t Size;
		size_t Used;
		size_t Reserved;
	};
    const char* sCommandLine = nullptr;
    LoadedModule sLoadedModules[LOADEDMODULE_MAX];
    uint8_t sLoadedModuleCount = 0;
    MemoryBlock sMemoryBlocks[MEMORYBLOCK_MAX];
    uint8_t sMemoryBlockCount = 0;
    uint8_t sCurrentMemoryBlock = 0;
    void LoadedModule::Initialize(size_t pAddress, size_t pSize, const char* pIdentifier)
    {
	    mAddress = pAddress;
	    mSize = pSize;
	    mIdentifier = pIdentifier;
    }
    void Startup(uint32_t pMultibootMagic, Header* pMultibootHeader)
    {
        if (pMultibootMagic != MULTIBOOT_MAGIC) Halt();
        sCommandLine = pMultibootHeader->CommandLine;        
	    size_t kernelStart = reinterpret_cast<size_t>(&__SOK);
	    size_t kernelEnd = reinterpret_cast<size_t>(&__EOK);
	    size_t kernelSize = kernelEnd - kernelStart;
	    size_t stackStart = reinterpret_cast<size_t>(&gStack);
	    size_t stackSize = gStackSize;

	    ReservedMemoryBlock reservedMemoryBlocks[RESERVEDMEMORYBLOCK_MAX] = { { kernelStart, kernelSize }, { stackStart, stackSize } };
	    uint8_t reservedMemoryBlockCount = 2;

        Module* module = pMultibootHeader->Modules;
        uint32_t moduleCount = pMultibootHeader->ModulesCount;

	    for (uint32_t moduleIndex = 0; moduleIndex < moduleCount; ++moduleIndex, ++module)
	    {
		    sLoadedModules[sLoadedModuleCount].Initialize(module->Start, module->End - module->Start, module->Identifier);
            ++sLoadedModuleCount;

		    reservedMemoryBlocks[reservedMemoryBlockCount].Address = module->Start;
		    reservedMemoryBlocks[reservedMemoryBlockCount].Size = module->End - module->Start;
		    ++reservedMemoryBlockCount;
	    }

        MemoryMap* memoryMap = pMultibootHeader->MemoryMaps;
        uint32_t memoryMapCount = pMultibootHeader->MemoryMapsSize / sizeof(MemoryMap);
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

    void Shutdown()
    {
	    sCommandLine = nullptr;
	    sLoadedModuleCount = 0;
	    sMemoryBlockCount = 0;
    }

    LoadedModule* GetLoadedModule(const char* pFilename)
    {
        for (uint8_t index = 0; index < sLoadedModuleCount; ++index)
        {
            if (!strcasecmp(sLoadedModules[index].GetIdentifier(), pFilename)) return &sLoadedModules[index];
        }
	    return nullptr;
    }
    void* AdjustMemory(ptrdiff_t pAdjustment)
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
        return nullptr;
    }
}