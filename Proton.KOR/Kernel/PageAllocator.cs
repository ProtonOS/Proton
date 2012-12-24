using System;
using System.Collections.Generic;

namespace System.Kernel
{
	internal unsafe static class PageAllocator
	{
		internal const ulong MinimumTotalMemory = 0x4000000;
		internal const uint MinimumPageSize = 0x1000;
		internal const int ShiftsForMinimumPageSize = 12;

		private static uint* Tree = null;
		private static byte TreeLevels = 0;

		private static void Panic() { while (true) ; }

		internal static void Initialize(MultibootHeader* pMultibootHeader)
		{
			int memoryMapCount = (int)(pMultibootHeader->MemoryMapsSize / sizeof(MultibootMemoryMap));
			ulong lastMemoryMapAddress = pMultibootHeader->MemoryMaps[memoryMapCount - 1].AddressLower | ((ulong)pMultibootHeader->MemoryMaps[memoryMapCount - 1].AddressUpper << 32);
			ulong lastMemoryMapLength = pMultibootHeader->MemoryMaps[memoryMapCount - 1].LengthLower | ((ulong)pMultibootHeader->MemoryMaps[memoryMapCount - 1].LengthUpper << 32);
			ulong totalMemory = lastMemoryMapAddress + lastMemoryMapLength;
			if (totalMemory < MinimumTotalMemory) Panic();
			ulong totalMemoryHighestBit = 0;
			int totalMemoryHighestBitShiftOff = 0;
			for (int bit = 63; bit >= 0; --bit)
			{
				++totalMemoryHighestBitShiftOff;
				if ((totalMemory & ((ulong)1 << bit)) != 0)
				{
					totalMemoryHighestBit = (ulong)1 << bit;
					break;
				}
			}
			if ((totalMemory << totalMemoryHighestBitShiftOff) != 0) totalMemoryHighestBit <<= 1;
			int shiftsForTotalMemory = 64 - totalMemoryHighestBitShiftOff;
			TreeLevels = (byte)((shiftsForTotalMemory - ShiftsForMinimumPageSize) + 1);
			ulong bytesRequiredForTree = ((ulong)1 << TreeLevels) >> 3;
			ulong treeStartAddress = pMultibootHeader->Modules[pMultibootHeader->ModulesCount - 1].End;
			if ((treeStartAddress & 0x03) != 0) treeStartAddress += 4 - (treeStartAddress & 0x03);
			Tree = (uint*)treeStartAddress;
			ulong treeElementCount = bytesRequiredForTree >> 2;
			for (ulong index = 0; index < treeElementCount; ++index) Tree[index] = 0;

			SetBitsInTree((byte)(TreeLevels - 1), 0, (treeStartAddress + (treeElementCount << 2)) >> ShiftsForMinimumPageSize, true);

			MultibootMemoryMap* memoryMap = null;
			ulong memoryMapAddress = 0;
			ulong memoryMapLength = 0;
			for (int memoryMapIndex = 0; memoryMapIndex < memoryMapCount; ++memoryMapIndex)
			{
				memoryMap = &pMultibootHeader->MemoryMaps[memoryMapIndex];
				if (memoryMap->Type != 1)
				{
					memoryMapAddress = memoryMap->AddressLower | ((ulong)memoryMap->AddressUpper << 32);
					memoryMapLength = memoryMap->LengthLower | ((ulong)memoryMap->LengthUpper << 32);
					SetBitsInTree((byte)(TreeLevels - 1), memoryMapAddress >> ShiftsForMinimumPageSize, memoryMapLength >> ShiftsForMinimumPageSize, true);
				}
			}

			ulong lengthOfOutOfBoundsMemory = totalMemoryHighestBit - totalMemory;
			if (lengthOfOutOfBoundsMemory > 0) SetBitsInTree((byte)(TreeLevels - 1), totalMemory >> ShiftsForMinimumPageSize, lengthOfOutOfBoundsMemory >> ShiftsForMinimumPageSize, true);
		}

		private static void SetBitInTree(byte pLevel, ulong pIndex, bool pValue)
		{
			ulong bitIndex = (((ulong)1 << pLevel) - 1) + pIndex;
			ulong dwordIndex = bitIndex >> 5;
			bitIndex &= 0x1F;
			if (pValue) Tree[dwordIndex] |= ((uint)1 << (byte)bitIndex);
			else Tree[dwordIndex] &= ~((uint)1 << (byte)bitIndex);
		}

		private static void SetBitsInTree(byte pLevel, ulong pIndex, ulong pBlockCount, bool pValue)
		{
			for (ulong index = 0; index < pBlockCount; ++index)
			{
				SetBitInTree(pLevel, pIndex + index, pValue);
				if (pBlockCount > 1) SetBitsInTree((byte)(pLevel - 1), (pIndex + index) >> 1, pBlockCount >> 1, pValue);
			}
		}

		private static bool IsSetInTree(byte pLevel, ulong pIndex)
		{
			ulong bitIndex = (((ulong)1 << pLevel) - 1) + pIndex;
			ulong dwordIndex = bitIndex >> 5;
			bitIndex &= 0x1F;
			return (Tree[dwordIndex] & ((uint)1 << (byte)bitIndex)) != 0;
		}

		private static bool IsBlockAvailableInTree(byte pLevel, ulong pIndex)
		{
			if (pLevel == TreeLevels) return true;
			if (IsSetInTree(pLevel, pIndex)) return false;
			if (IsBlockAvailableInTree((byte)(pLevel + 1), pIndex << 1) &&
				IsBlockAvailableInTree((byte)(pLevel + 1), (pIndex << 1) + 1)) return true;
			return false;
		}

		private static bool FindAvailableInTree(byte pLevel, ulong pIndex, ref ulong pSize, ref ulong pAddress)
		{
			ulong nextBlockSize = ((ulong)1 << (TreeLevels - pLevel - 1)) << 2;
			if (!IsSetInTree(pLevel, pIndex))
			{
				if ((pLevel + 1) < TreeLevels && pSize <= nextBlockSize)
				{
					if (FindAvailableInTree((byte)(pLevel + 1), pIndex << 1, ref pSize, ref pAddress)) return true;
				}
				else if (IsBlockAvailableInTree(pLevel, pIndex))
				{
					pSize = ((ulong)1 << (TreeLevels - pLevel - 1)) << ShiftsForMinimumPageSize;
					pAddress = (pIndex << (TreeLevels - pLevel - 1)) << ShiftsForMinimumPageSize;
					return true;
				}
			}
			if (!IsSetInTree(pLevel, pIndex + 1))
			{
				if ((pLevel + 1) < TreeLevels && pSize <= nextBlockSize)
				{
					if (FindAvailableInTree((byte)(pLevel + 1), (pIndex << 1) + 1, ref pSize, ref pAddress)) return true;
				}
				else if (IsBlockAvailableInTree(pLevel, pIndex + 1))
				{
					pSize = ((ulong)1 << (TreeLevels - pLevel - 1)) << ShiftsForMinimumPageSize;
					pAddress = ((pIndex + 1) << (TreeLevels - pLevel - 1)) << ShiftsForMinimumPageSize;
					return true;
				}
			}
			return false;
		}
		
		internal static ulong Allocate(ref ulong pSize)
		{
			// TODO: Make thread-safe
			if (pSize < MinimumPageSize) pSize = MinimumPageSize;
			ulong pages = pSize >> ShiftsForMinimumPageSize;
			if ((pSize & (((ulong)1 << ShiftsForMinimumPageSize) - 1)) != 0) ++pages;
			pSize = MinimumPageSize * pages;
			ulong address = 0;
			if (!FindAvailableInTree(0, 0, ref pSize, ref address)) Panic();
			SetBitsInTree((byte)(TreeLevels - 1), address >> ShiftsForMinimumPageSize, pSize >> ShiftsForMinimumPageSize, true);
			return address;
		}

		internal static void Deallocate(ulong pAddress, ulong pSize)
		{
			// TODO: Make thread-safe
			if ((pSize & (((ulong)1 << ShiftsForMinimumPageSize) - 1)) != 0) Panic();
			SetBitsInTree((byte)(TreeLevels - 1), pAddress >> ShiftsForMinimumPageSize, pSize >> ShiftsForMinimumPageSize, false);
		}
	}
}
