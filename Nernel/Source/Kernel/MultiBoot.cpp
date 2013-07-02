#include "MultiBoot.hpp"

extern char __EOK;
extern char __SOK;

namespace MultiBoot
{
    constexpr UInt32 MagicNumber = 0x2BADB002;
    constexpr UInt32 MemoryMapAvailableType = 1;

    struct MultiBootModule
    {
    public:
        UInt32 Start;
        UInt32 End;
        UInt32 Identifier;
        UInt32 Reserved;
    };

    struct MultiBootMemoryMap
    {
    public:
        UInt32 Size;
        UInt32 AddressLower;
        UInt32 AddressUpper;
        UInt32 LengthLower;
        UInt32 LengthUpper;
        UInt32 Type;
    };


    struct MultiBootHeader
    {
    public:
        UInt32 Flags;
        UInt32 LowerMemory;
        UInt32 UpperMemory;
        UInt32 BootDevice;
        UInt32 CommandLine;
        UInt32 ModulesCount;
        Core::Ptr<MultiBootModule> Modules;
        UInt32 Symbolic0;
        UInt32 Symbolic1;
        UInt32 Symbolic2;
        UInt32 Symbolic3;
        UInt32 MemoryMapsSize;
        Core::Ptr<MultiBootMemoryMap> MemoryMaps;
        UInt32 DrivesLength;
        UInt32 DrivesAddress;
        UInt32 ConfigTable;
        UInt32 BootLoader;
        UInt32 APMTable;
        UInt32 VBEControlInfo;
        UInt32 VBEModeInfo;
        UInt16 VBEMode;
        UInt16 VBEInterfaceSegment;
        UInt16 VBEInterfaceOffset;
        UInt16 VBEInterfaceLength;
    };

	UInt TotalPhysicalMemory = 0;

    LoadedModule LoadedModules[LoadedModulesMax];
    UInt32 LoadedModulesCount = 0;

    ReservedMemoryBlock ReservedMemoryBlocks[ReservedMemoryBlocksMax];
    UInt32 ReservedMemoryBlocksCount = 0;

    AvailableMemoryBlock AvailableMemoryBlocks[AvailableMemoryBlocksMax];
    UInt32 AvailableMemoryBlocksCount = 0;

    void Load(UInt32 pMultiBootMagic, Core::Ptr<MultiBootHeader> pMultiBootHeader)
    {
        //extern char __EOK;
        //extern char __SOK;
        //UInt kernelStart = (UInt)&__SOK;
        //UInt kernelEnd = (UInt)&__EOK;
        //UInt kernelSize = kernelEnd - kernelStart;

        Core::Ptr<MultiBootModule> multiBootModule = pMultiBootHeader->Modules;
        ReservedMemoryBlocks[0].Address = 0;
		ReservedMemoryBlocks[0].Size = (UInt)&__EOK;
        ReservedMemoryBlocksCount = 1;

        for (UInt32 multiBootModuleIndex = 0; multiBootModuleIndex < pMultiBootHeader->ModulesCount; ++multiBootModuleIndex, ++multiBootModule) {
            LoadedModules[LoadedModulesCount].Address = multiBootModule->Start;
            LoadedModules[LoadedModulesCount].Size = multiBootModule->End - multiBootModule->Start;
            UInt32 multiBootModuleIdentifierIndex = 0;
            Core::Ptr<char> multiBootModuleIdentifier = reinterpret_cast<char *>(multiBootModule->Identifier);
            while (multiBootModuleIdentifier[multiBootModuleIdentifierIndex] && multiBootModuleIdentifierIndex < (LoadedModuleIdentifierMax - 1)) {
                LoadedModules[LoadedModulesCount].Identifier[multiBootModuleIdentifierIndex] = multiBootModuleIdentifier[multiBootModuleIdentifierIndex];
                ++multiBootModuleIdentifierIndex;
            }
            LoadedModules[LoadedModulesCount].Identifier[multiBootModuleIdentifierIndex] = '\0';

            ReservedMemoryBlocks[ReservedMemoryBlocksCount].Address = LoadedModules[LoadedModulesCount].Address;
            ReservedMemoryBlocks[ReservedMemoryBlocksCount].Size = LoadedModules[LoadedModulesCount].Size;

            ++LoadedModulesCount;
            ++ReservedMemoryBlocksCount;
        }

        Core::Ptr<MultiBootMemoryMap> multiBootMemoryMap = pMultiBootHeader->MemoryMaps;
        UInt32 multiBootMemoryMapsCount = pMultiBootHeader->MemoryMapsSize / sizeof(MultiBootMemoryMap);

        for (UInt32 multiBootMemoryMapIndex = 0; multiBootMemoryMapIndex < multiBootMemoryMapsCount; ++multiBootMemoryMapIndex, ++multiBootMemoryMap) {
			UInt address = multiBootMemoryMap->AddressLower;
			UInt size = multiBootMemoryMap->LengthLower;
#ifdef IS64BIT
                address |= multiBootMemoryMap->AddressUpper << 32;
                size |= multiBootMemoryMap->LengthUpper << 32;
#endif
            if (multiBootMemoryMap->Type == MemoryMapAvailableType && address >= reinterpret_cast<UInt>(&__SOK)) {
                AvailableMemoryBlocks[AvailableMemoryBlocksCount].Address = address;
                AvailableMemoryBlocks[AvailableMemoryBlocksCount].Size = size;
                AvailableMemoryBlocks[AvailableMemoryBlocksCount].Used = 0;
                ++AvailableMemoryBlocksCount;
            }
			if ((address + size) >= TotalPhysicalMemory) TotalPhysicalMemory = address + size;
        }

        Core::Ptr<ReservedMemoryBlock> reservedMemoryBlock = ReservedMemoryBlocks;
        for (UInt32 reservedMemoryBlockIndex = 0; reservedMemoryBlockIndex < ReservedMemoryBlocksCount; ++reservedMemoryBlockIndex, ++reservedMemoryBlock) {
            Core::Ptr<AvailableMemoryBlock> availableMemoryBlock = AvailableMemoryBlocks;
            for (UInt32 availableMemoryBlockIndex = 0; availableMemoryBlockIndex < AvailableMemoryBlocksCount; ++availableMemoryBlockIndex, ++availableMemoryBlock) {
                if (reservedMemoryBlock->Address + reservedMemoryBlock->Size > availableMemoryBlock->Address &&
                    reservedMemoryBlock->Address < availableMemoryBlock->Address + availableMemoryBlock->Size) {
                        if (reservedMemoryBlock->Address > availableMemoryBlock->Address &&
                            reservedMemoryBlock->Address + reservedMemoryBlock->Size >= availableMemoryBlock->Address + availableMemoryBlock->Size) {
                                // Space left at start, but not at end
                                availableMemoryBlock->Size = reservedMemoryBlock->Address - availableMemoryBlock->Address;
                        } else if (reservedMemoryBlock->Address <= availableMemoryBlock->Address &&
                            reservedMemoryBlock->Address + reservedMemoryBlock->Size < availableMemoryBlock->Address + availableMemoryBlock->Size) {
                                // Space left at end, but not at start
                                availableMemoryBlock->Size = (availableMemoryBlock->Address + availableMemoryBlock->Size) - (reservedMemoryBlock->Address + reservedMemoryBlock->Size);
                                availableMemoryBlock->Address = reservedMemoryBlock->Address + reservedMemoryBlock->Size;
                        } else if (reservedMemoryBlock->Address > availableMemoryBlock->Address &&
                            reservedMemoryBlock->Address + reservedMemoryBlock->Size < availableMemoryBlock->Address + availableMemoryBlock->Size) {
                                // Space left at both start and end
                                if (AvailableMemoryBlocksCount == AvailableMemoryBlocksMax) {
                                    //Panic("Insufficient memory blocks to process available memory");
                                    while (true);
                                }

                                for (UInt32 copyBlockIndex = AvailableMemoryBlocksCount; copyBlockIndex > availableMemoryBlockIndex; --copyBlockIndex) {
                                    AvailableMemoryBlocks[copyBlockIndex] = AvailableMemoryBlocks[copyBlockIndex - 1];
                                }
                                ++AvailableMemoryBlocksCount;

                                AvailableMemoryBlocks[availableMemoryBlockIndex + 1].Size = (availableMemoryBlock->Address + availableMemoryBlock->Size) - (reservedMemoryBlock->Address + reservedMemoryBlock->Size);
                                AvailableMemoryBlocks[availableMemoryBlockIndex + 1].Address = reservedMemoryBlock->Address + reservedMemoryBlock->Size;
                                availableMemoryBlock->Size = reservedMemoryBlock->Address - availableMemoryBlock->Address;

                        } else {
                            // No space, used the whole darn block!
                            for (UInt32 copyBlockIndex = availableMemoryBlockIndex + 1; copyBlockIndex < AvailableMemoryBlocksCount; ++copyBlockIndex) {
                                AvailableMemoryBlocks[copyBlockIndex - 1] = AvailableMemoryBlocks[copyBlockIndex];
                            }
                            --AvailableMemoryBlocksCount;
                        }
                }
            }
        }
        if (AvailableMemoryBlocksCount == 0) {
            //Panic("There is no memory blocks available");
            while (true);
        }
    }
}