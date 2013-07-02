#include "Debug.hpp"
#include "MultiBoot.hpp"
#include "PMM.hpp"

namespace PMM
{
	constexpr UInt8 BytesToMegaBytesShifts = 20;

	constexpr UInt8 PageSizeShifts = 12;
    constexpr UInt PageSize = (1 << PageSizeShifts);

	constexpr UInt8 StateAvailable = 0x00;
	constexpr UInt8 StateUnavailable = 0x01;
	constexpr UInt8 StatePartial = 0x02;
	constexpr UInt8 StateFull = 0x03;

    UInt TotalPhysicalMemoryMapped = 0;
    UInt8 TreeLevels = 0;
    Core::Ptr<UInt8> Tree = nullptr;

	UInt RelativeIndexToAbsoluteIndex(UInt8 pTreeLevel, UInt pRelativeIndex) { return ((1 << pTreeLevel) - 1) + pRelativeIndex; }

	void SetPageUnavailable(UInt pRelativeIndex)
	{
		UInt absoluteIndex = RelativeIndexToAbsoluteIndex(TreeLevels, pRelativeIndex);
		Tree[absoluteIndex] = StateUnavailable;
		for (UInt8 treeLevel = TreeLevels - 1; treeLevel > 0; --treeLevel)
		{
			pRelativeIndex >>= 1;
			absoluteIndex = RelativeIndexToAbsoluteIndex(treeLevel, pRelativeIndex);
			switch (Tree[absoluteIndex])
			{
			case StateAvailable:
				Tree[absoluteIndex] = StatePartial;
				break;
			case StateUnavailable:
				Panic("PMM (SetPageUnavailable): StateUnavailable not expected");
				break;
			case StatePartial:
				{
					UInt8 leftChild = Tree[RelativeIndexToAbsoluteIndex(treeLevel, pRelativeIndex << 1)];
					UInt8 rightChild = Tree[RelativeIndexToAbsoluteIndex(treeLevel, (pRelativeIndex << 1) + 1)];
					// If either child is Available or Partial, parent stays Partial
					if (leftChild == StateAvailable || rightChild == StateAvailable ||
						leftChild == StatePartial || rightChild == StatePartial) return;
					// If both children are Unavailable, parent becomes Unavailable
					if (leftChild == StateUnavailable && rightChild == StateUnavailable)
						Tree[absoluteIndex] = StateUnavailable;
					else // If both children are Full or one is Unavailable, parent becomes Full
						Tree[absoluteIndex] = StateFull;
					break;
				}
			case StateFull:
				Panic("PMM (SetPageUnavailable): StateFull not expected");
				break;
			default:
				Panic("PMM (SetPageUnavailable): State unknown");
				break;
			}
		}
	}

	void SetPageFull(UInt pRelativeIndex)
	{
		UInt absoluteIndex = RelativeIndexToAbsoluteIndex(TreeLevels, pRelativeIndex);
		Tree[absoluteIndex] = StateFull;
		for (UInt8 treeLevel = TreeLevels - 1; treeLevel > 0; --treeLevel)
		{
			pRelativeIndex >>= 1;
			absoluteIndex = RelativeIndexToAbsoluteIndex(treeLevel, pRelativeIndex);
			switch (Tree[absoluteIndex])
			{
			case StateAvailable:
				Tree[absoluteIndex] = StatePartial;
				break;
			case StateUnavailable:
				Panic("PMM (SetPageFull): StateUnavailable not expected");
				break;
			case StatePartial:
				{
					UInt8 leftChild = Tree[RelativeIndexToAbsoluteIndex(treeLevel, pRelativeIndex << 1)];
					UInt8 rightChild = Tree[RelativeIndexToAbsoluteIndex(treeLevel, (pRelativeIndex << 1) + 1)];
					// If either child is Available or Partial, parent stays Partial
					if (leftChild == StateAvailable || rightChild == StateAvailable ||
						leftChild == StatePartial || rightChild == StatePartial) return;
					// If both children are Unavailable, parent becomes Unavailable
					if (leftChild == StateUnavailable && rightChild == StateUnavailable)
						Tree[absoluteIndex] = StateUnavailable;
					else // If both children are Full or one is Unavailable, parent becomes Full
						Tree[absoluteIndex] = StateFull;
					break;
				}
			case StateFull:
				Panic("PMM (SetPageFull): StateFull not expected");
				break;
			default:
				Panic("PMM (SetPageFull): State unknown");
				break;
			}
		}
	}

    void Load()
    {
        Debug::WriteFormat("TotalPhysicalMemory: %uMB\n", MultiBoot::TotalPhysicalMemory >> BytesToMegaBytesShifts);
        TotalPhysicalMemoryMapped = Core::RoundUpToPowerOfTwo(MultiBoot::TotalPhysicalMemory);
        Debug::WriteFormat("TotalPhysicalMemoryMapped: %uMB\n", TotalPhysicalMemoryMapped >> BytesToMegaBytesShifts);

        UInt tempTotalPhysicalMemoryMapped = TotalPhysicalMemoryMapped;
        while (tempTotalPhysicalMemoryMapped >= PageSize) {
            TreeLevels++;
            tempTotalPhysicalMemoryMapped >>= 1;
        }
        // Wastes one byte, but it would be wasted anyway
        UInt totalTreeSize = 1 << TreeLevels;
		UInt treeAddress = 0;

        for (UInt32 availableBlockIndex = 0; availableBlockIndex < MultiBoot::AvailableMemoryBlocksCount; ++availableBlockIndex) {
            if (MultiBoot::AvailableMemoryBlocks[availableBlockIndex].Size >= totalTreeSize) {
				treeAddress = MultiBoot::AvailableMemoryBlocks[availableBlockIndex].Address;
                break;
            }
        }
		if (treeAddress == 0) Panic("PMM (Load): Insufficient contiguous space for allocation tree");
        Tree = Core::Ptr<UInt8>(treeAddress);

		Core::Memory::MemClear(Tree, totalTreeSize);

		if (MultiBoot::TotalPhysicalMemory != TotalPhysicalMemoryMapped)
		{
			UInt unavailablePagesStart = MultiBoot::TotalPhysicalMemory >> PageSizeShifts;
			UInt unavailablePagesCount = (TotalPhysicalMemoryMapped - MultiBoot::TotalPhysicalMemory) >> PageSizeShifts;
			for (UInt unavailablePagesOffset = 0; unavailablePagesOffset < unavailablePagesCount; ++unavailablePagesOffset)
				SetPageUnavailable(unavailablePagesStart + unavailablePagesOffset);
		}

		for (UInt32 reservedMemoryBlockIndex = 0; reservedMemoryBlockIndex < MultiBoot::ReservedMemoryBlocksCount; ++reservedMemoryBlockIndex) {
			UInt reservedPagesStart = MultiBoot::ReservedMemoryBlocks[reservedMemoryBlockIndex].Address >> PageSizeShifts;
			UInt reservedPagesCount = MultiBoot::ReservedMemoryBlocks[reservedMemoryBlockIndex].Size >> PageSizeShifts;
			if (MultiBoot::ReservedMemoryBlocks[reservedMemoryBlockIndex].Size & ((1 << PageSizeShifts) - 1)) ++reservedPagesCount;
			for (UInt reservedPagesOffset = 0; reservedPagesOffset < reservedPagesCount; ++reservedPagesOffset)
				SetPageFull(reservedPagesStart + reservedPagesOffset);
		}

		UInt reservedTreePagesStart = treeAddress >> PageSizeShifts;
		UInt reservedTreePagesCount = totalTreeSize >> PageSizeShifts;
		if (totalTreeSize & ((1 << PageSizeShifts) - 1)) ++reservedTreePagesCount;
		for (UInt reservedTreePagesOffset = 0; reservedTreePagesOffset < reservedTreePagesCount; ++reservedTreePagesOffset)
			SetPageFull(reservedTreePagesStart + reservedTreePagesOffset);
    }
};
