#include "PrimitiveTypes.h"
#include "MultiBoot.h"

MultiBoot::LoadedModule MultiBoot::LoadedModules[MultiBoot::LoadedModulesMax];
uint32 MultiBoot::LoadedModulesCount = 0;

MultiBoot::AvailableMemoryBlock MultiBoot::AvailableMemoryBlocks[MultiBoot::AvailableMemoryBlocksMax];
uint32 MultiBoot::AvailableMemoryBlocksCount = 0;

void MultiBoot::Load(uint32 pMultiBootMagic, MultiBootHeader* pMultiBootHeader)
{
    extern char __EOK;
    extern char __SOK;
	//uintptr kernelStart = (uintptr)&__SOK;
	//uintptr kernelEnd = (uintptr)&__EOK;
	//uintptr kernelSize = kernelEnd - kernelStart;

    MultiBootModule* multiBootModule = pMultiBootHeader->Modules;
	ReservedMemoryBlock reservedMemoryBlocks[ReservedMemoryBlocksMax] = { { 0, (uintptr)&__EOK } };
	uint32 reservedMemoryBlocksCount = 1;

	for (uint32 multiBootModuleIndex = 0; multiBootModuleIndex < pMultiBootHeader->ModulesCount; ++multiBootModuleIndex, ++multiBootModule)
	{
		LoadedModules[LoadedModulesCount].Address = multiBootModule->Start;
		LoadedModules[LoadedModulesCount].Size = multiBootModule->End - multiBootModule->Start;
		uint32 multiBootModuleIdentifierIndex = 0;
		while (multiBootModule->Identifier[multiBootModuleIdentifierIndex] && multiBootModuleIdentifierIndex < (LoadedModuleIdentifierMax - 1))
		{
			LoadedModules[LoadedModulesCount].Identifier[multiBootModuleIdentifierIndex] = multiBootModule->Identifier[multiBootModuleIdentifierIndex];
			++multiBootModuleIdentifierIndex;
		}
		LoadedModules[LoadedModulesCount].Identifier[multiBootModuleIdentifierIndex] = '\0';

		reservedMemoryBlocks[reservedMemoryBlocksCount].Address = LoadedModules[LoadedModulesCount].Address;
		reservedMemoryBlocks[reservedMemoryBlocksCount].Size = LoadedModules[LoadedModulesCount].Size;

		++LoadedModulesCount;
		++reservedMemoryBlocksCount;
	}

    MultiBootMemoryMap* multiBootMemoryMap = pMultiBootHeader->MemoryMaps;
    uint32 multiBootMemoryMapsCount = pMultiBootHeader->MemoryMapsSize / sizeof(MultiBootMemoryMap);

	for (uint32 multiBootMemoryMapIndex = 0; multiBootMemoryMapIndex < multiBootMemoryMapsCount; ++multiBootMemoryMapIndex, ++multiBootMemoryMap)
	{
		if (multiBootMemoryMap->Type == MemoryMapAvailableType && multiBootMemoryMap->AddressLower >= (uintptr)&__SOK)
		{
			AvailableMemoryBlocks[AvailableMemoryBlocksCount].Address = multiBootMemoryMap->AddressLower;
			AvailableMemoryBlocks[AvailableMemoryBlocksCount].Size = multiBootMemoryMap->LengthLower;
			AvailableMemoryBlocks[AvailableMemoryBlocksCount].Used = 0;
			++AvailableMemoryBlocksCount;
		}
	}

	ReservedMemoryBlock* reservedMemoryBlock = reservedMemoryBlocks;
	for (uint32 reservedMemoryBlockIndex = 0; reservedMemoryBlockIndex < reservedMemoryBlocksCount; ++reservedMemoryBlockIndex, ++reservedMemoryBlock)
	{
		AvailableMemoryBlock* availableMemoryBlock = AvailableMemoryBlocks;
		for (uint32 availableMemoryBlockIndex = 0; availableMemoryBlockIndex < AvailableMemoryBlocksCount; ++availableMemoryBlockIndex, ++availableMemoryBlock)
		{
			if (reservedMemoryBlock->Address + reservedMemoryBlock->Size > availableMemoryBlock->Address &&
				reservedMemoryBlock->Address < availableMemoryBlock->Address + availableMemoryBlock->Size)
			{
				if (reservedMemoryBlock->Address > availableMemoryBlock->Address &&
					reservedMemoryBlock->Address + reservedMemoryBlock->Size >= availableMemoryBlock->Address + availableMemoryBlock->Size)
				{
					// Space left at start, but not at end
					availableMemoryBlock->Size = reservedMemoryBlock->Address - availableMemoryBlock->Address;
				}
				else if (reservedMemoryBlock->Address <= availableMemoryBlock->Address &&
						 reservedMemoryBlock->Address + reservedMemoryBlock->Size < availableMemoryBlock->Address + availableMemoryBlock->Size)
				{
					// Space left at end, but not at start
					availableMemoryBlock->Size = (availableMemoryBlock->Address + availableMemoryBlock->Size) - (reservedMemoryBlock->Address + reservedMemoryBlock->Size);
					availableMemoryBlock->Address = reservedMemoryBlock->Address + reservedMemoryBlock->Size;
				}
				else if (reservedMemoryBlock->Address > availableMemoryBlock->Address &&
						 reservedMemoryBlock->Address + reservedMemoryBlock->Size < availableMemoryBlock->Address + availableMemoryBlock->Size)
				{
					// Space left at both start and end
					if (AvailableMemoryBlocksCount == AvailableMemoryBlocksMax)
					{
						//Panic("Insufficient memory blocks to process available memory");
						while (true) ;
					}

					for (uint32 copyBlockIndex = AvailableMemoryBlocksCount; copyBlockIndex > availableMemoryBlockIndex; --copyBlockIndex)
					{
						AvailableMemoryBlocks[copyBlockIndex] = AvailableMemoryBlocks[copyBlockIndex - 1];
					}
					++AvailableMemoryBlocksCount;
						
					AvailableMemoryBlocks[availableMemoryBlockIndex + 1].Size = (availableMemoryBlock->Address + availableMemoryBlock->Size) - (reservedMemoryBlock->Address + reservedMemoryBlock->Size);
					AvailableMemoryBlocks[availableMemoryBlockIndex + 1].Address = reservedMemoryBlock->Address + reservedMemoryBlock->Size;
					availableMemoryBlock->Size = reservedMemoryBlock->Address - availableMemoryBlock->Address;
						
				}
				else
				{
					// No space, used the whole darn block!
					for (uint32 copyBlockIndex = availableMemoryBlockIndex + 1; copyBlockIndex < AvailableMemoryBlocksCount; ++copyBlockIndex)
					{
						AvailableMemoryBlocks[copyBlockIndex - 1] = AvailableMemoryBlocks[copyBlockIndex];
					}
					--AvailableMemoryBlocksCount;
				}
			}
		}
	}
	if (AvailableMemoryBlocksCount == 0)
	{
		//Panic("There is no memory blocks available");
		while (true) ;
	}
}