#include "Core/Types.hpp"
#include "MultiBoot.hpp"

extern char __EOK;
extern char __SOK;

namespace MultiBoot {
    constexpr UInt32 MagicNumber = 0x2BADB002;
    constexpr UInt32 MemoryMapAvailableType = 1;
    constexpr UInt32 ReservedMemoryBlocksMax = 64;
    constexpr UInt32 LoadedModulesMax = 64;
    constexpr UInt32 LoadedModuleIdentifierMax = 128;
    constexpr UInt32 AvailableMemoryBlocksMax = 64;

    struct MultiBootModule {
    public:
        UInt32 Start;
        UInt32 End;
        UInt32 Identifier;
        UInt32 Reserved;
    };

    struct MultiBootMemoryMap {
    public:
        UInt32 Size;
        UInt32 AddressLower;
        UInt32 AddressUpper;
        UInt32 LengthLower;
        UInt32 LengthUpper;
        UInt32 Type;
    };


    struct ReservedMemoryBlock {
    public:
        UInt Address;
        UInt Size;
    };

    struct MultiBootHeader {
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
    struct LoadedModule {
    public:
        UInt Address;
        UInt Size;
        char Identifier[LoadedModuleIdentifierMax];
    };

    struct AvailableMemoryBlock {
    public:
        UInt Address;
        UInt Size;
        UInt Used;
    };


    LoadedModule LoadedModules[LoadedModulesMax];
    UInt32 LoadedModulesCount = 0;

    AvailableMemoryBlock AvailableMemoryBlocks[AvailableMemoryBlocksMax];
    UInt32 AvailableMemoryBlocksCount = 0;

    void Load(UInt32 pMultiBootMagic, Core::Ptr<MultiBootHeader> pMultiBootHeader) {
        //extern char __EOK;
        //extern char __SOK;
        //UInt kernelStart = (UInt)&__SOK;
        //UInt kernelEnd = (UInt)&__EOK;
        //UInt kernelSize = kernelEnd - kernelStart;

        Core::Ptr<MultiBootModule> multiBootModule = pMultiBootHeader->Modules;
        ReservedMemoryBlock reservedMemoryBlocks[ReservedMemoryBlocksMax] = {{ 0, (UInt)&__EOK }};
        UInt32 reservedMemoryBlocksCount = 1;

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

            reservedMemoryBlocks[reservedMemoryBlocksCount].Address = LoadedModules[LoadedModulesCount].Address;
            reservedMemoryBlocks[reservedMemoryBlocksCount].Size = LoadedModules[LoadedModulesCount].Size;

            ++LoadedModulesCount;
            ++reservedMemoryBlocksCount;
        }

        Core::Ptr<MultiBootMemoryMap> multiBootMemoryMap = pMultiBootHeader->MemoryMaps;
        UInt32 multiBootMemoryMapsCount = pMultiBootHeader->MemoryMapsSize / sizeof(MultiBootMemoryMap);

        for (UInt32 multiBootMemoryMapIndex = 0; multiBootMemoryMapIndex < multiBootMemoryMapsCount; ++multiBootMemoryMapIndex, ++multiBootMemoryMap) {
            if (multiBootMemoryMap->Type == MemoryMapAvailableType && multiBootMemoryMap->AddressLower >= (UInt)&__SOK) {
                AvailableMemoryBlocks[AvailableMemoryBlocksCount].Address = multiBootMemoryMap->AddressLower;
                AvailableMemoryBlocks[AvailableMemoryBlocksCount].Size = multiBootMemoryMap->LengthLower;
                AvailableMemoryBlocks[AvailableMemoryBlocksCount].Used = 0;
                ++AvailableMemoryBlocksCount;
            }
        }

        Core::Ptr<ReservedMemoryBlock> reservedMemoryBlock = reservedMemoryBlocks;
        for (UInt32 reservedMemoryBlockIndex = 0; reservedMemoryBlockIndex < reservedMemoryBlocksCount; ++reservedMemoryBlockIndex, ++reservedMemoryBlock) {
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