using System.Collections.Generic;
using System.Kernel;

namespace System
{
    public static unsafe class GC
    {
		internal const ulong HeapSize = 0x100000;
		internal const uint MinimumObjectSize = 0x40;
		internal const int ShiftsForMinimumObjectSize = 6;

        internal struct GCHeap
        {
			public HeapAllocator.PageOfHeaps* PageOfHeaps;
			public GCHeap* AllocatorPrev;
			public GCHeap* AllocatorNext;
			public ulong HeapSize;
			public byte* Heap;
			public ulong TreeSize;
			public uint* Tree;
            public uint TreeLevels;
			public GCHeap* Prev;
			public GCHeap* Next;
			public GCObject* AllocatedFirst;
			public GCObject* AllocatedLast;

			internal void SetBitInTree(byte pLevel, ulong pIndex, bool pValue)
			{
				ulong bitIndex = (((ulong)1 << pLevel) - 1) + pIndex;
				ulong dwordIndex = bitIndex >> 5;
				bitIndex &= 0x1F;
				if (pValue) Tree[dwordIndex] |= ((uint)1 << (byte)bitIndex);
				else Tree[dwordIndex] &= ~((uint)1 << (byte)bitIndex);
			}

			internal void SetBitsInTree(byte pLevel, ulong pIndex, ulong pBlockCount, bool pValue)
			{
				for (ulong index = 0; index < pBlockCount; ++index)
				{
					SetBitInTree(pLevel, pIndex + index, pValue);
					if (pBlockCount > 1) SetBitsInTree((byte)(pLevel - 1), (pIndex + index) >> 1, pBlockCount >> 1, pValue);
				}
			}

			internal bool IsSetInTree(byte pLevel, ulong pIndex)
			{
				ulong bitIndex = (((ulong)1 << pLevel) - 1) + pIndex;
				ulong dwordIndex = bitIndex >> 5;
				bitIndex &= 0x1F;
				return (Tree[dwordIndex] & ((uint)1 << (byte)bitIndex)) != 0;
			}

			internal bool IsBlockAvailableInTree(byte pLevel, ulong pIndex)
			{
				if (pLevel == TreeLevels) return true;
				if (IsSetInTree(pLevel, pIndex)) return false;
				if (IsBlockAvailableInTree((byte)(pLevel + 1), pIndex << 1) &&
					IsBlockAvailableInTree((byte)(pLevel + 1), (pIndex << 1) + 1)) return true;
				return false;
			}

			internal bool FindAvailableInTree(byte pLevel, ulong pIndex, ref ulong pSize, ref ulong pAddress)
			{
				if (TreeLevels == 1) return false;
				ulong nextBlockSize = ((ulong)1 << ((byte)TreeLevels - pLevel - 1)) << 2;
				if (!IsSetInTree(pLevel, pIndex))
				{
					if ((pLevel + 1) < TreeLevels && pSize <= nextBlockSize)
					{
						if (FindAvailableInTree((byte)(pLevel + 1), pIndex << 1, ref pSize, ref pAddress)) return true;
					}
					else if (IsBlockAvailableInTree(pLevel, pIndex))
					{
						pSize = ((ulong)1 << ((byte)TreeLevels - pLevel - 1)) << ShiftsForMinimumObjectSize;
						pAddress = (pIndex << ((byte)TreeLevels - pLevel - 1)) << ShiftsForMinimumObjectSize;
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
						pSize = ((ulong)1 << ((byte)TreeLevels - pLevel - 1)) << ShiftsForMinimumObjectSize;
						pAddress = ((pIndex + 1) << ((byte)TreeLevels - pLevel - 1)) << ShiftsForMinimumObjectSize;
						return true;
					}
				}
				return false;
			}

			public void LinkObjectToAllocated(GCObject* pObject)
			{
				if (AllocatedLast == null)
				{
					pObject->AllocatedPrev = null;
					pObject->AllocatedNext = null;
					AllocatedFirst = pObject;
					AllocatedLast = pObject;
				}
				else
				{
					AllocatedLast->AllocatedNext = pObject;
					pObject->AllocatedPrev = AllocatedLast;
					pObject->AllocatedNext = null;
					AllocatedLast = pObject;
				}
			}
			public void UnlinkObjectFromAllocated(GCObject* pObject)
			{
				if (pObject->AllocatedPrev == null) AllocatedFirst = pObject->AllocatedNext;
				else pObject->AllocatedPrev->AllocatedNext = pObject->AllocatedNext;
				if (pObject->AllocatedNext == null) AllocatedLast = pObject->AllocatedPrev;
				else pObject->AllocatedNext->AllocatedPrev = pObject->AllocatedPrev;
			}
		}

        internal enum GCObjectFlags : uint
        {
			None = 0,
            Marked = 1
        }

        internal struct GCObject
        {
			public GCHeap* Heap;
			public GCObject* AllocatedPrev;
			public GCObject* AllocatedNext;
			public Type.TypeData* TypeData;
            public GCObjectFlags Flags;
            public uint HeapSize;
            public uint DataSize;
        }

		private static GCHeap* HeapFirst = null;
		private static GCHeap* HeapLast = null;

		private static GCHeap* AllocateHeap(ulong pHeapSize, bool pSingleObject)
		{
			// TODO: Thread-safety
			GCHeap* heap = HeapAllocator.AllocateHeap(pHeapSize, pSingleObject);
			if (HeapLast == null)
			{
				heap->Prev = null;
				heap->Next = null;
				HeapFirst = heap;
				HeapLast = heap;
			}
			else
			{
				HeapLast->Next = heap;
				heap->Prev = HeapLast;
				heap->Next = null;
				HeapLast = heap;
			}
			return heap;
		}

		private static void DeallocateHeap(GCHeap* pHeap)
		{
			// TODO: Thread-safety
			if (pHeap->Prev == null) HeapFirst = pHeap->Next;
			else pHeap->Prev->Next = pHeap->Next;
			if (pHeap->Next == null) HeapLast = pHeap->Prev;
			else pHeap->Next->Prev = pHeap->Prev;
			HeapAllocator.DeallocateHeap(pHeap);
		}

		private static GCObject* Allocate(uint pDataSize)
		{
			// TODO: Thread-safety
			ulong heapSize = (ulong)sizeof(GCObject) + pDataSize;
			ulong address = 0;
			bool largeObject = heapSize >= HeapSize;
			GCObject* obj = null;

			GCHeap* heap = null;
			if (!largeObject)
			{
				heap = HeapLast;
				while (heap != null)
				{
					if (heap->FindAvailableInTree(0, 0, ref heapSize, ref address)) break;
					heap = heap->Prev;
				}
			}
			if (heap == null)
			{
				ulong actualHeapSize = largeObject ? heapSize : HeapSize;
				heap = AllocateHeap(actualHeapSize, largeObject);
				if (largeObject) address = (ulong)heap->Heap;
				else heap->FindAvailableInTree(0, 0, ref heapSize, ref address);
			}
			if (!largeObject) heap->SetBitsInTree((byte)(heap->TreeLevels - 1), (address - (ulong)heap->Heap) >> ShiftsForMinimumObjectSize, heapSize >> ShiftsForMinimumObjectSize, true);

			obj = (GCObject*)address;
			obj->Heap = heap;
			heap->LinkObjectToAllocated(obj);
			obj->TypeData = null;
			obj->Flags = GCObjectFlags.None;
			obj->HeapSize = (uint)heapSize;
			obj->DataSize = pDataSize;
			return obj;
		}

		internal static object AllocateObject(Type.TypeData* pType)
		{
			GCObject* obj = Allocate(pType->Size);
			obj->TypeData = pType;
			return object.Internal_PointerToReference((void*)((ulong)obj + (ulong)sizeof(GCObject)));
		}

		//internal static string AllocateStringFromASCII(sbyte* pString, uint pLength)
		//{
		//    GCObject* obj = Allocate(sizeof(int) + (pLength << 1));
			
		//}


		//internal static object Allocate(uint objectSize)
		//{
		//    GCObject* gco = null;
		//    uint requiredSize = (uint)(sizeof(GCObject) + objectSize);
		//    if (!sHeapsReady)
		//    {
		//        if (requiredSize <= SmallHeapSize)
		//        {
		//            if (requiredSize > (SmallHeapSize - sReservedSmallHeapUsed)) while (true) ;
		//            gco = (GCObject*)(ReservedSmallHeapPointer + sReservedSmallHeapUsed);
		//            sReservedSmallHeapUsed += requiredSize;
		//        }
		//        else if (requiredSize <= LargeHeapSize)
		//        {
		//            if (requiredSize > (LargeHeapSize - sReservedLargeHeapUsed)) while (true) ;
		//            gco = (GCObject*)(ReservedLargeHeapPointer + sReservedLargeHeapUsed);
		//            sReservedLargeHeapUsed += requiredSize;
		//        }
		//        else while (true) ;
		//    }
		//    gco->TypeData = typeof(object).GetTypeDataPointer();
		//    gco->Flags = GCObjectFlags.Allocated;
		//    gco->AllocatedSize = requiredSize;
		//    gco->ActualSize = objectSize;
		//    sReservedObjectPointers[sReservedObjectCount++] = new IntPtr((void*)((byte*)gco + sizeof(GCObject)));
		//    return object.Internal_PointerToReference((void*)((byte*)gco + sizeof(GCObject)));
		//}

		//internal static object AllocateObject(Type.TypeData* typeData)
		//{
		//    // TODO: Atomic_AquireLock(&sBusy)
		//    object obj = Allocate(typeData->Size);
		//    GCObject* gco = (GCObject*)((byte*)obj.Internal_ReferenceToPointer() - sizeof(GCObject));
		//    gco->TypeData = typeData;
		//    // TODO: Atomic_ReleaseLock(&sBusy)
		//    return obj;
		//}

		//internal static string AllocateString(uint pLength)
		//{
		//    // TODO: Atomic_AquireLock(&sBusy)
		//    object obj = Allocate((uint)(sizeof(int) + pLength));
		//    GCObject* gco = (GCObject*)((byte*)obj.Internal_ReferenceToPointer() - sizeof(GCObject));
		//    gco->TypeData = typeof(string).GetTypeDataPointer();
		//    // TODO: Atomic_ReleaseLock(&sBusy)
		//    return (string)obj;
		//}

		//internal static Array AllocateArray(Type.TypeData* typeData, uint count)
		//{
		//    uint elementSize = typeData->ArrayElementType->Size;
		//    if (!(new RuntimeType(new RuntimeTypeHandle(new IntPtr((void*)typeData->ArrayElementType))).IsValueType)) elementSize = (uint)sizeof(IntPtr);
		//    // TODO: Atomic_AquireLock(&sBusy)
		//    object obj = Allocate((uint)(sizeof(int) + (elementSize * count)));
		//    GCObject* gco = (GCObject*)((byte*)obj.Internal_ReferenceToPointer() - sizeof(GCObject));
		//    gco->TypeData = typeData;
		//    // TODO: Atomic_ReleaseLock(&sBusy)
		//    return (Array)obj;
		//}
    }
}
