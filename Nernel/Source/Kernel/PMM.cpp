#include "Debug.hpp"
#include "MultiBoot.hpp"
#include "PMM.hpp"

namespace PMM
{
    constexpr UInt PageSize = 4096;

    UInt TotalPhysicalMemoryMapped = 0;
    UInt8 TreeLevels = 0;
    Core::Ptr<UInt8> Tree = nullptr;

    void Load()
    {
        Debug::WriteFormat("TotalPhysicalMemory: %uMB\n", MultiBoot::TotalPhysicalMemory >> 20);

        TotalPhysicalMemoryMapped = Core::RoundUpToPowerOfTwo(MultiBoot::TotalPhysicalMemory);
        UInt8 tempTreeLevels = 0;
        UInt tempTotalPhysicalMemoryMapped = TotalPhysicalMemoryMapped;
        while (tempTotalPhysicalMemoryMapped >= PageSize) {
            tempTreeLevels++;
            tempTotalPhysicalMemoryMapped >>= 1;
        }
        // Wastes one byte, but it would be wasted anyway
        UInt totalTreeSize = 1 << tempTreeLevels;

        for (UInt32 availableBlockIndex = 0; availableBlockIndex < MultiBoot::AvailableMemoryBlocksCount; ++availableBlockIndex) {
            if (MultiBoot::AvailableMemoryBlocks[availableBlockIndex].Size >= totalTreeSize) {
                Tree = Core::Ptr<UInt8>(MultiBoot::AvailableMemoryBlocks[availableBlockIndex].Address);
                break;
            }
        }
    }
};
