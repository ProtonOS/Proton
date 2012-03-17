#include <string.h>
#include <strings.h>

#include <Core/MultiBoot.h>

const char* gMultiBoot_CommandLine = NULL;
MultiBoot_LoadedModule gMultiBoot_LoadedModules[MultiBoot_MaxLoadedModules];
uint8_t gMultiBoot_LoadedModuleCount = 0;
MultiBoot_MemoryBlock gMultiBoot_MemoryBlocks[MultiBoot_MaxMemoryBlocks];
uint8_t gMultiBoot_MemoryBlockCount = 0;

bool_t MultiBoot_Startup(uint32_t pMultiBootMagic, void* pMultiBootData)
{
    if (pMultiBootMagic != MultiBoot_Magic) return FALSE;
    MultiBoot_Header* header = (MultiBoot_Header*)pMultiBootData;
    gMultiBoot_CommandLine = (const char*)header->CommandLine;

    extern char __BOF;
    extern char __EOF;

	uint32_t kernelHead = (uint32_t)&__BOF;
	uint32_t kernelTail = (uint32_t)&__EOF;
	uint32_t kernelLength = kernelTail - kernelHead;

    uint32_t reservedStart = kernelHead;
    uint32_t reservedEnd = kernelTail;
    uint32_t reservedLength = kernelLength;
    bool_t reservedMapped = FALSE;

    MultiBoot_Module* module = (MultiBoot_Module*)header->ModulesAddress;
    uint32_t moduleCount = header->ModulesCount;
	for (uint32_t moduleIndex = 0;
         moduleIndex < moduleCount;
         ++moduleIndex, ++module)
    {
        gMultiBoot_LoadedModules[gMultiBoot_LoadedModuleCount].Address = module->Start;
        gMultiBoot_LoadedModules[gMultiBoot_LoadedModuleCount].Length = module->End - module->Start;
        gMultiBoot_LoadedModules[gMultiBoot_LoadedModuleCount].Identifier = (const char*)module->Identifier;
        ++gMultiBoot_LoadedModuleCount;

        if (module->Start < reservedStart) reservedStart = module->Start;
        if (module->End > reservedEnd) reservedEnd = module->End;
        reservedLength = reservedEnd - reservedStart;
    }

    MultiBoot_MemoryMap* memoryMap = (MultiBoot_MemoryMap*)header->MemoryMapsAddress;
    uint32_t memoryMapSize = memoryMap->Size + 4;
    uint32_t memoryMapCount = header->MemoryMapsLength / memoryMapSize;

	for (uint32_t memoryMapIndex = 0;
         memoryMapIndex < memoryMapCount;
         ++memoryMapIndex, memoryMap = (MultiBoot_MemoryMap*)(header->MemoryMapsAddress + (memoryMapIndex * memoryMapSize)))
	{
		if (memoryMap->Type == MultiBoot_MemoryMapAvailable)
		{
			uint32_t address = memoryMap->AddressLower;
			uint32_t length = memoryMap->LengthLower;
			if (address + length <= MultiBoot_MemoryMapConventional) continue;

            if (!reservedMapped && reservedStart == address && reservedLength <= length)
            {
                address += reservedLength;
                length -= reservedLength;
                reservedMapped = TRUE;
            }

            if (length > 0)
            {
			    gMultiBoot_MemoryBlocks[gMultiBoot_MemoryBlockCount].Address = address;
			    gMultiBoot_MemoryBlocks[gMultiBoot_MemoryBlockCount].Length = length;
			    gMultiBoot_MemoryBlocks[gMultiBoot_MemoryBlockCount].Used = 0;
			    ++gMultiBoot_MemoryBlockCount;
            }
		}
	}
    return reservedMapped && gMultiBoot_LoadedModuleCount > 0 && gMultiBoot_MemoryBlockCount > 0;
}

void MultiBoot_Shutdown()
{
    gMultiBoot_LoadedModuleCount = 0;
    gMultiBoot_MemoryBlockCount = 0;
    gMultiBoot_CommandLine = NULL;
}

void* MultiBoot_GetModuleByFileName(const char* pFileName)
{
    for (uint8_t index = 0; index < gMultiBoot_LoadedModuleCount; ++index)
    {
        size_t length = strlen(gMultiBoot_LoadedModules[index].Identifier);
        const char* fileName = gMultiBoot_LoadedModules[index].Identifier + length;
        while (fileName != gMultiBoot_LoadedModules[index].Identifier && *fileName != '/') --fileName;
        if (*fileName == '/') ++fileName;

        if (!strcasecmp(fileName, pFileName))
        {
        	return (void*)gMultiBoot_LoadedModules[index].Address;
        }
    }
	return NULL;
}

MultiBoot_LoadedModule* MultiBoot_GetLoadedModuleByFileName(const char* pFileName)
{
    for (uint8_t index = 0; index < gMultiBoot_LoadedModuleCount; ++index)
    {
        if (!strcasecmp(gMultiBoot_LoadedModules[index].Identifier, pFileName)) return &gMultiBoot_LoadedModules[index];
    }
	return NULL;
}
