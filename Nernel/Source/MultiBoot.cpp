#include "Core/Types.hpp"
#include "MultiBoot.hpp"

extern char __EOK;
extern char __SOK;

namespace MultiBoot {
    constexpr uint32 MagicNumber = 0x2BADB002;
    constexpr uint32 MemoryMapAvailableType = 1;
    constexpr uint32 ReservedMemoryBlocksMax = 64;
    constexpr uint32 LoadedModulesMax = 64;
    constexpr uint32 LoadedModuleIdentifierMax = 128;
    constexpr uint32 AvailableMemoryBlocksMax = 64;

    struct MultiBootModule {
    public:
        uint32 Start;
        uint32 End;
        Core::Ptr<char> Identifier;
        uint32 Reserved;
    };

    struct MultiBootMemoryMap {
    public:
        uint32 Size;
        uint32 AddressLower;
        uint32 AddressUpper;
        uint32 LengthLower;
        uint32 LengthUpper;
        uint32 Type;
    };


    struct ReservedMemoryBlock {
    public:
        uintptr Address;
        uintptr Size;
    };

    struct MultiBootHeader {
    public:
        uint32 Flags;
        uint32 LowerMemory;
        uint32 UpperMemory;
        uint32 BootDevice;
        uint32 CommandLine;
        uint32 ModulesCount;
        Core::Ptr<MultiBootModule> Modules;
        uint32 Symbolic0;
        uint32 Symbolic1;
        uint32 Symbolic2;
        uint32 Symbolic3;
        uint32 MemoryMapsSize;
        Core::Ptr<MultiBootMemoryMap> MemoryMaps;
        uint32 DrivesLength;
        uint32 DrivesAddress;
        uint32 ConfigTable;
        uint32 BootLoader;
        uint32 APMTable;
        uint32 VBEControlInfo;
        uint32 VBEModeInfo;
        uint16 VBEMode;
        uint16 VBEInterfaceSegment;
        uint16 VBEInterfaceOffset;
        uint16 VBEInterfaceLength;
    };
    struct LoadedModule {
    public:
        uintptr Address;
        uintptr Size;
        char Identifier[LoadedModuleIdentifierMax];
    };

    struct AvailableMemoryBlock {
    public:
        uintptr Address;
        uintptr Size;
        uintptr Used;
    };


    LoadedModule LoadedModules[LoadedModulesMax];
    uint32 LoadedModulesCount = 0;

    AvailableMemoryBlock AvailableMemoryBlocks[AvailableMemoryBlocksMax];
    uint32 AvailableMemoryBlocksCount = 0;

    void Load(uint32 pMultiBootMagic, Core::Ptr<MultiBootHeader> pMultiBootHeader) {
        //extern char __EOK;
        //extern char __SOK;
        //uintptr kernelStart = (uintptr)&__SOK;
        //uintptr kernelEnd = (uintptr)&__EOK;
        //uintptr kernelSize = kernelEnd - kernelStart;

        Core::Ptr<MultiBootModule> multiBootModule = pMultiBootHeader->Modules;
        ReservedMemoryBlock reservedMemoryBlocks[ReservedMemoryBlocksMax] = {{ 0, (uintptr)&__EOK }};
        uint32 reservedMemoryBlocksCount = 1;

        for (uint32 multiBootModuleIndex = 0; multiBootModuleIndex < pMultiBootHeader->ModulesCount; ++multiBootModuleIndex, ++multiBootModule) {
            LoadedModules[LoadedModulesCount].Address = multiBootModule->Start;
            LoadedModules[LoadedModulesCount].Size = multiBootModule->End - multiBootModule->Start;
            uint32 multiBootModuleIdentifierIndex = 0;
            while (multiBootModule->Identifier[multiBootModuleIdentifierIndex] && multiBootModuleIdentifierIndex < (LoadedModuleIdentifierMax - 1)) {
                LoadedModules[LoadedModulesCount].Identifier[multiBootModuleIdentifierIndex] = multiBootModule->Identifier[multiBootModuleIdentifierIndex];
                ++multiBootModuleIdentifierIndex;
            }
            LoadedModules[LoadedModulesCount].Identifier[multiBootModuleIdentifierIndex] = '\0';

            reservedMemoryBlocks[reservedMemoryBlocksCount].Address = LoadedModules[LoadedModulesCount].Address;
            reservedMemoryBlocks[reservedMemoryBlocksCount].Size = LoadedModules[LoadedModulesCount].Size;

            ++LoadedModulesCount;
            ++reservedMemoryBlocksCount;
        }

        Core::Ptr<MultiBootMemoryMap> multiBootMemoryMap = pMultiBootHeader->MemoryMaps;
        uint32 multiBootMemoryMapsCount = pMultiBootHeader->MemoryMapsSize / sizeof(MultiBootMemoryMap);

        for (uint32 multiBootMemoryMapIndex = 0; multiBootMemoryMapIndex < multiBootMemoryMapsCount; ++multiBootMemoryMapIndex, ++multiBootMemoryMap) {
            if (multiBootMemoryMap->Type == MemoryMapAvailableType && multiBootMemoryMap->AddressLower >= (uintptr)&__SOK) {
                AvailableMemoryBlocks[AvailableMemoryBlocksCount].Address = multiBootMemoryMap->AddressLower;
                AvailableMemoryBlocks[AvailableMemoryBlocksCount].Size = multiBootMemoryMap->LengthLower;
                AvailableMemoryBlocks[AvailableMemoryBlocksCount].Used = 0;
                ++AvailableMemoryBlocksCount;
            }
        }

        Core::Ptr<ReservedMemoryBlock> reservedMemoryBlock = reservedMemoryBlocks;
        for (uint32 reservedMemoryBlockIndex = 0; reservedMemoryBlockIndex < reservedMemoryBlocksCount; ++reservedMemoryBlockIndex, ++reservedMemoryBlock) {
            Core::Ptr<AvailableMemoryBlock> availableMemoryBlock = AvailableMemoryBlocks;
            for (uint32 availableMemoryBlockIndex = 0; availableMemoryBlockIndex < AvailableMemoryBlocksCount; ++availableMemoryBlockIndex, ++availableMemoryBlock) {
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

                                       for (uint32 copyBlockIndex = AvailableMemoryBlocksCount; copyBlockIndex > availableMemoryBlockIndex; --copyBlockIndex) {
                                           AvailableMemoryBlocks[copyBlockIndex] = AvailableMemoryBlocks[copyBlockIndex - 1];
                                       }
                                       ++AvailableMemoryBlocksCount;

                                       AvailableMemoryBlocks[availableMemoryBlockIndex + 1].Size = (availableMemoryBlock->Address + availableMemoryBlock->Size) - (reservedMemoryBlock->Address + reservedMemoryBlock->Size);
                                       AvailableMemoryBlocks[availableMemoryBlockIndex + 1].Address = reservedMemoryBlock->Address + reservedMemoryBlock->Size;
                                       availableMemoryBlock->Size = reservedMemoryBlock->Address - availableMemoryBlock->Address;

                        } else {
                            // No space, used the whole darn block!
                            for (uint32 copyBlockIndex = availableMemoryBlockIndex + 1; copyBlockIndex < AvailableMemoryBlocksCount; ++copyBlockIndex) {
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