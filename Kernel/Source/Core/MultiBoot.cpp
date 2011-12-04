#include <Core/MultiBoot.h>

const char* Core::MultiBoot::sCommandLine = nullptr;
Core::MultiBoot::LoadedModuleArray Core::MultiBoot::sLoadedModules;
uint8_t Core::MultiBoot::sLoadedModuleCount = 0;
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
    sLoadedModules.fill(LoadedModule());
    sMemoryBlocks.fill(MemoryBlock());

	uint32_t kernelHead = (uint32_t)&__BOF;
	uint32_t kernelTail = (uint32_t)&__EOF;
	uint32_t kernelLength = kernelTail - kernelHead;

    uint32_t reservedStart = kernelHead;
    uint32_t reservedEnd = kernelTail;
    uint32_t reservedLength = kernelLength;
    bool reservedMapped = false;

    Module& module = *reinterpret_cast<Module*>(header.ModulesAddress);
    uint32_t moduleCount = header.ModulesCount;
	for (uint32_t moduleIndex = 0;
         moduleIndex < moduleCount;
         ++moduleIndex, module = *reinterpret_cast<Module*>(header.ModulesAddress + (moduleIndex * sizeof(Module))))
    {
        sLoadedModules[sLoadedModuleCount].Address = module.Start;
        sLoadedModules[sLoadedModuleCount].Length = module.End - module.Start;
        sLoadedModules[sLoadedModuleCount].Identifier = reinterpret_cast<const char*>(module.Identifier);
        ++sLoadedModuleCount;

        if (module.Start < reservedStart) reservedStart = module.Start;
        if (module.End > reservedEnd) reservedEnd = module.End;
        reservedLength = reservedEnd - reservedStart;
    }

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

            if (!reservedMapped && reservedStart == address && reservedLength <= length)
            {
                address += reservedLength;
                length -= reservedLength;
                reservedMapped = true;
            }

            if (length > 0)
            {
			    sMemoryBlocks[sMemoryBlockCount].Address = address;
			    sMemoryBlocks[sMemoryBlockCount].Length = length;
			    sMemoryBlocks[sMemoryBlockCount].Used = 0;
			    ++sMemoryBlockCount;
            }
		}
	}
    return reservedMapped && sLoadedModuleCount > 0 && sMemoryBlockCount > 0;
}

void Core::MultiBoot::Shutdown()
{
    sLoadedModuleCount = 0;
    sMemoryBlockCount = 0;
    sCommandLine = nullptr;
}

const char* Core::MultiBoot::GetCommandLine() { return sCommandLine; }

Core::MultiBoot::LoadedModule& Core::MultiBoot::GetLoadedModule(uint8_t pIndex) { return sLoadedModules[pIndex]; }

uint8_t Core::MultiBoot::GetLoadedModuleCount() { return sLoadedModuleCount; }

Core::MultiBoot::MemoryBlock& Core::MultiBoot::GetMemoryBlock(uint8_t pIndex) { return sMemoryBlocks[pIndex]; }

uint8_t Core::MultiBoot::GetMemoryBlockCount() { return sMemoryBlockCount; }
