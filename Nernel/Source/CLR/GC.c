#include <System/Atomics.h>
#include <System/ThreadScheduler.h>
#include <CLR/GC.h>
#include <CLR/JIT.h>

Thread* GetCurrentThread();

GC* GC_Create(AppDomain* pDomain)
{
	GC* gc = (GC*)calloc(1, sizeof(GC));
	gc->Domain = pDomain;
	gc->SmallGeneration0HeapCount = 1;
	gc->SmallGeneration1HeapCount = 1;
	gc->SmallGeneration2HeapCount = 1;
	gc->LargeHeapCount = 1;
	gc->SmallGeneration0Heaps = (GCHeap**)calloc(1, sizeof(GCHeap*));
	gc->SmallGeneration1Heaps = (GCHeap**)calloc(1, sizeof(GCHeap*));
	gc->SmallGeneration2Heaps = (GCHeap**)calloc(1, sizeof(GCHeap*));
	gc->LargeHeaps = (GCHeap**)calloc(1, sizeof(GCHeap*));
	gc->SmallGeneration0Heaps[0] = GCHeap_Create(GCHeap__SmallHeap_Size, GCHeap__SmallHeap_InitialPoolSize, TRUE);
	gc->SmallGeneration1Heaps[0] = GCHeap_Create(GCHeap__SmallHeap_Size, GCHeap__SmallHeap_InitialPoolSize, TRUE);
	gc->SmallGeneration2Heaps[0] = GCHeap_Create(GCHeap__SmallHeap_Size, GCHeap__SmallHeap_InitialPoolSize, TRUE);
	gc->LargeHeaps[0] = GCHeap_Create(GCHeap__LargeHeap_Size, GCHeap__LargeHeap_InitialPoolSize, TRUE);

	return gc;
}

void GC_Destroy(GC* pGC)
{
    for (uint32_t index = 0; index < pGC->LargeHeapCount; ++index)
        GCHeap_Destroy(pGC->LargeHeaps[index]);
    for (uint32_t index = 0; index < pGC->SmallGeneration2HeapCount; ++index)
        GCHeap_Destroy(pGC->SmallGeneration2Heaps[index]);
    for (uint32_t index = 0; index < pGC->SmallGeneration1HeapCount; ++index)
        GCHeap_Destroy(pGC->SmallGeneration1Heaps[index]);
    for (uint32_t index = 0; index < pGC->SmallGeneration0HeapCount; ++index)
        GCHeap_Destroy(pGC->SmallGeneration0Heaps[index]);
    free (pGC->LargeHeaps);
    free (pGC->SmallGeneration2Heaps);
    free (pGC->SmallGeneration1Heaps);
    free (pGC->SmallGeneration0Heaps);
    free(pGC);
}

GCHeap* GCHeap_Create(uint32_t pHeapSize, uint32_t pInitialPoolSize, bool_t pUseTreeAllocation)
{
    GCHeap* heap = (GCHeap*)calloc(1, sizeof(GCHeap));
	heap->InitialPoolSize = pInitialPoolSize;
	for (uint32_t index = 0; index < pInitialPoolSize; ++index)
	{
		GCObject* object = (GCObject*)calloc(1, sizeof(GCObject));
		object->NextObject = heap->FreeObjectList;
		heap->FreeObjectList = object;
	}
	if (pUseTreeAllocation)
	{
		uint32_t bitsRequired = 1;
		pInitialPoolSize >>= 1;
		heap->AllocationTreeLevels = 1;
		while (pInitialPoolSize >= 8)
		{
			bitsRequired <<= 1;
			bitsRequired |= 1;
			pInitialPoolSize >>= 1;
			++heap->AllocationTreeLevels;
		}
		uint32_t bytesRequired = (bitsRequired + (8 - (bitsRequired & 0x0F))) >> 3;
		uint32_t dwordsRequired = bytesRequired >> 2;
		if (!dwordsRequired) dwordsRequired = 1;
		heap->AllocationTree = (uint32_t*)calloc(1, sizeof(uint32_t) * dwordsRequired);
	}
    heap->Size = pHeapSize;
    heap->Available = heap->Size;
    heap->Bottom = (uint8_t*)calloc(1, heap->Size);
    return heap;
}

void GCHeap_Destroy(GCHeap* pGCHeap)
{
	for (GCObject* object = pGCHeap->AllocatedObjectList, *objectNext = NULL; object; object = objectNext)
    {
		objectNext = object->NextObject;
        free (object);
    }
	for (GCObject* object = pGCHeap->FreeObjectList, *objectNext = NULL; object; object = objectNext)
    {
		objectNext = object->NextObject;
        free (object);
    }
	free(pGCHeap->AllocationTree);
    free(pGCHeap->Bottom);
    free(pGCHeap);
}

bool_t GCHeap_IsAllocatedInTree(uint32_t* pAllocationTree, uint32_t pLevel, uint32_t pIndex)
{
	uint32_t bitIndex = ((1 << pLevel) - 1) + pIndex;
	uint32_t dwordIndex = bitIndex >> 5;
	bitIndex &= 0x1F;
	return (pAllocationTree[dwordIndex] & (1 << bitIndex)) != 0;
}

void GCHeap_SetBitInTree(uint32_t* pAllocationTree, uint32_t pLevel, uint32_t pIndex, bool_t pAllocated)
{
	uint32_t bitIndex = ((1 << pLevel) - 1) + pIndex;
	uint32_t dwordIndex = bitIndex >> 5;
	bitIndex &= 0x1F;
	if (pAllocated) pAllocationTree[dwordIndex] |= (1 << bitIndex);
	else pAllocationTree[dwordIndex] &= ~(1 << bitIndex);
}

int32_t GCHeap_FindAvailableInTree(uint32_t* pAllocationTree, uint32_t pLevel, uint32_t pMaxLevels, uint32_t pIndex, uint32_t pSize, uint32_t* pAllocatedSize)
{
	if (pLevel >= pMaxLevels) return -1;
	uint32_t nextBlockSize = ((pMaxLevels - (pLevel + 1)) << 3);
	int32_t result = -1;
	if (!GCHeap_IsAllocatedInTree(pAllocationTree, pLevel, pIndex))
	{
		if (pSize <= nextBlockSize)
		{
			result = GCHeap_FindAvailableInTree(pAllocationTree, pLevel + 1, pMaxLevels, pIndex, pSize, pAllocatedSize);
			if (result >= 0) return result;
		}
		else
		{
			*pAllocatedSize = ((pMaxLevels - pLevel) << 3);
			return pIndex;
		}
	}

	if (!GCHeap_IsAllocatedInTree(pAllocationTree, pLevel, pIndex + 1))
	{
		if (pSize <= nextBlockSize)
		{
			result = GCHeap_FindAvailableInTree(pAllocationTree, pLevel + 1, pMaxLevels, pIndex + 1, pSize, pAllocatedSize);
		}
		else
		{
			*pAllocatedSize = ((pMaxLevels - pLevel) << 3);
			return pIndex;
		}
	}
	return result;
}

void GCHeap_SetBitsInTree(uint32_t* pAllocationTree, uint32_t pLevel, uint32_t pIndex, uint32_t pBlockCount, bool_t pAllocated)
{
	for (uint32_t index = 0; index < pBlockCount; ++index)
	{
		GCHeap_SetBitInTree(pAllocationTree, pLevel, pIndex + index, pAllocated);
		if (pBlockCount > 1) GCHeap_SetBitsInTree(pAllocationTree, pLevel - 1, (pIndex + index) >> 1, pBlockCount >> 1, pAllocated);
	}
}

GCObject* GCHeap_Allocate(GCHeap*** pGCHeaps, uint32_t* pGCHeapCount, uint32_t pHeapSize, uint32_t pSize, bool_t pUseTreeAllocation)
{
    GCObject* object = NULL;
    GCHeap* heap = NULL;
	uint32_t requiredSize = pSize + sizeof(GCObject*);
    for (uint32_t index = 0; !object && index < *pGCHeapCount; ++index)
    {
        heap = *pGCHeaps[index];
        if (heap->Available >= requiredSize)
        {
			uint32_t allocatedSize = 0;
			int32_t index = GCHeap_FindAvailableInTree(heap->AllocationTree, 0, heap->AllocationTreeLevels, 0, requiredSize, &allocatedSize);
			if (index < 0) continue;
			GCHeap_SetBitsInTree(heap->AllocationTree, heap->AllocationTreeLevels - 1, index, allocatedSize >> 3, TRUE);
			uint8_t* data = heap->Bottom + (index << 3);
			if (heap->FreeObjectList)
			{
				object = heap->FreeObjectList;
				heap->FreeObjectList = heap->FreeObjectList->NextObject;
			}
			else object = (GCObject*)calloc(1, sizeof(GCObject));
			object->NextObject = heap->AllocatedObjectList;
			heap->AllocatedObjectList = object;
			
            object->Flags = GCObjectFlags_Allocated;
			object->AllocatedSize = allocatedSize;
            object->ActualSize = pSize;
            object->Data = data + sizeof(GCObject*);
            object->Heap = heap;
            heap->Available -= allocatedSize;
            heap->Allocated += allocatedSize;
        }
    }
    if (!object)
    {
        *pGCHeapCount = *pGCHeapCount + 1;
        *pGCHeaps = (GCHeap**)realloc(*pGCHeaps, sizeof(GCHeap*) * (*pGCHeapCount));
		
        *pGCHeaps[*pGCHeapCount - 1] = GCHeap_Create(pHeapSize, (*pGCHeaps[0])->InitialPoolSize, pUseTreeAllocation);
        heap = *pGCHeaps[*pGCHeapCount - 1];
        heap->Size = pHeapSize;
        heap->Available = pHeapSize;
        heap->Bottom = (uint8_t*)calloc(1, pHeapSize);

		uint32_t allocatedSize = 0;
		int32_t index = GCHeap_FindAvailableInTree(heap->AllocationTree, 0, heap->AllocationTreeLevels, 0, requiredSize, &allocatedSize);
		GCHeap_SetBitsInTree(heap->AllocationTree, heap->AllocationTreeLevels - 1, index, allocatedSize >> 3, TRUE);
		uint8_t* data = heap->Bottom + (index << 3);
		object = heap->FreeObjectList;
		heap->FreeObjectList = heap->FreeObjectList->NextObject;
		object->NextObject = heap->AllocatedObjectList;
		heap->AllocatedObjectList = object;

        object->Flags = GCObjectFlags_Allocated;
		object->AllocatedSize = allocatedSize;
        object->ActualSize = pSize;
        object->Data = data + sizeof(GCObject*);
        object->Heap = heap;
        heap->Available -= allocatedSize;
        heap->Allocated += allocatedSize;
    }
	if (!object) Panic("Whoa, how did this happen?!");
	memset(object->Data, 0x00, pSize);
	*(GCObject**)((size_t)object->Data - sizeof(GCObject*)) = object;
    return object;
}

void GC_AllocateObject(AppDomain* pDomain, uint8_t* pStackStream, IRType* pType, uint32_t pSize, void** pAllocatedObject)
{
	if (pSize >= 0x7FFFFFFF) Panic("GC_AllocateObject pSize >= 0x7FFFFFFF");
	GC* gc = pDomain->GarbageCollector;
	Atomic_AquireLock(&gc->Busy);
    GCObject* object = NULL;
    if (pSize <= GCHeap__SmallHeap_Size - sizeof(GCObject*))
        object = GCHeap_Allocate(&gc->SmallGeneration0Heaps, &gc->SmallGeneration0HeapCount, GCHeap__SmallHeap_Size, pSize, TRUE);
    else if (pSize <= GCHeap__LargeHeap_Size - sizeof(GCObject*))
        object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, GCHeap__LargeHeap_Size, pSize, TRUE);
    else object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, pSize + sizeof(GCObject*), pSize, FALSE);
	object->Type = pType;
	*pAllocatedObject = object->Data;
	Atomic_ReleaseLock(&gc->Busy);
}

void GC_AllocateStringFromASCII(AppDomain* pDomain, uint8_t* pStackStream, int8_t* pString, uint32_t pLength, void** pAllocatedObject)
{
	uint32_t size = pLength << 1;
	if (size >= 0x7FFFFFFF) Panic("GC_AllocateStringFromASCII Size >= 0x7FFFFFFF");
	GC* gc = pDomain->GarbageCollector;
	uint8_t* unicodeString = (uint8_t*)calloc(1, size);
	for (uint32_t index = 0; index < pLength; ++index) unicodeString[index << 1] = pString[index];
	Atomic_AquireLock(&gc->Busy);
    GCObject* object = NULL;
	HASH_FIND(String.HashHandle, gc->StringHashTable, unicodeString, size, object);
	if (!object)
	{
		if (size <= GCHeap__SmallHeap_Size - sizeof(GCObject*))
			object = GCHeap_Allocate(&gc->SmallGeneration0Heaps, &gc->SmallGeneration0HeapCount, GCHeap__SmallHeap_Size, size, TRUE);
		else if (size <= GCHeap__LargeHeap_Size - sizeof(GCObject*))
			object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, GCHeap__LargeHeap_Size, size, TRUE);
		else object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, size + sizeof(GCObject*), size, FALSE);
		object->Type = gc->Domain->IRAssemblies[0]->Types[gc->Domain->CachedType___System_String->TableIndex - 1];
		object->Flags |= GCObjectFlags_String;
		object->String.Length = pLength;
		memcpy(object->Data, unicodeString, size);
		HASH_ADD(String.HashHandle, gc->StringHashTable, Data, size, object);
	}
	*pAllocatedObject = object->Data;
	Atomic_ReleaseLock(&gc->Busy);
	free(unicodeString);
}

void GC_AllocateStringFromUnicode(AppDomain* pDomain, uint8_t* pStackStream, uint16_t* pString, uint32_t pLength, void** pAllocatedObject)
{
	uint32_t size = pLength << 1;
	if (size >= 0x7FFFFFFF) Panic("GC_AllocateStringFromUnicode Size >= 0x7FFFFFFF");
	GC* gc = pDomain->GarbageCollector;
	Atomic_AquireLock(&gc->Busy);
    GCObject* object = NULL;
	HASH_FIND(String.HashHandle, gc->StringHashTable, (uint8_t*)pString, size, object);
	if (!object)
	{
		if (size <= GCHeap__SmallHeap_Size - sizeof(GCObject*))
			object = GCHeap_Allocate(&gc->SmallGeneration0Heaps, &gc->SmallGeneration0HeapCount, GCHeap__SmallHeap_Size, size, TRUE);
		else if (size <= GCHeap__LargeHeap_Size - sizeof(GCObject*))
			object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, GCHeap__LargeHeap_Size, size, TRUE);
		else object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, size + sizeof(GCObject*), size, FALSE);
		object->Type = gc->Domain->IRAssemblies[0]->Types[gc->Domain->CachedType___System_String->TableIndex - 1];
		object->Flags |= GCObjectFlags_String;
		object->String.Length = pLength;
		memcpy(object->Data, (uint8_t*)pString, size);
		HASH_ADD(String.HashHandle, gc->StringHashTable, Data, size, object);
	}
	*pAllocatedObject = object->Data;
	Atomic_ReleaseLock(&gc->Busy);
}

void GC_AllocateArray(AppDomain* pDomain, uint8_t* pStackStream, IRType* pArrayType, uint32_t pElementCount, void** pAllocatedObject)
{
	size_t elementSize = pArrayType->ArrayType->ElementType->Size;
	size_t requiredSize = elementSize * pElementCount;
	if (requiredSize >= 0x7FFFFFFF) Panic("GC_AllocateObject pSize >= 0x7FFFFFFF");
	GC* gc = pDomain->GarbageCollector;
	Atomic_AquireLock(&gc->Busy);
    GCObject* object = NULL;
    if (requiredSize <= GCHeap__SmallHeap_Size - sizeof(GCObject*))
        object = GCHeap_Allocate(&gc->SmallGeneration0Heaps, &gc->SmallGeneration0HeapCount, GCHeap__SmallHeap_Size, requiredSize, TRUE);
    else if (requiredSize <= GCHeap__LargeHeap_Size - sizeof(GCObject*))
        object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, GCHeap__LargeHeap_Size, requiredSize, TRUE);
    else object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, requiredSize + sizeof(GCObject*), requiredSize, FALSE);
	object->Type = pArrayType;
	object->Flags |= GCObjectFlags_Array;
	object->Array.ElementType = pArrayType->ArrayType->ElementType;
	object->Array.Length = pElementCount;
	*pAllocatedObject = object->Data;
	Atomic_ReleaseLock(&gc->Busy);
}

GCObject* GC_AllocatePinnedObject(GC* pGC, IRType* pType, uint32_t pSize)
{
	if (pSize >= 0x7FFFFFFF) Panic("GC_AllocatePinnedObject pSize >= 0x7FFFFFFF");
	Atomic_AquireLock(&pGC->Busy);
    GCObject* object = NULL;
    if (pSize <= GCHeap__SmallHeap_Size - sizeof(GCObject*))
        object = GCHeap_Allocate(&pGC->SmallGeneration0Heaps, &pGC->SmallGeneration0HeapCount, GCHeap__SmallHeap_Size, pSize, TRUE);
    else if (pSize <= GCHeap__LargeHeap_Size - sizeof(GCObject*))
        object = GCHeap_Allocate(&pGC->LargeHeaps, &pGC->LargeHeapCount, GCHeap__LargeHeap_Size, pSize, TRUE);
    else object = GCHeap_Allocate(&pGC->LargeHeaps, &pGC->LargeHeapCount, pSize + sizeof(GCObject*), pSize, FALSE);
	object->Type = pType;
	object->Flags |= GCObjectFlags_Pinned;
	Atomic_ReleaseLock(&pGC->Busy);
	return object;
}

//void GCHeap_Sweep(GCHeap** pGCHeaps, uint32_t pGCHeapCount, AppDomain* pDomain)
//{
//	for (uint32_t heapIndex = 0; heapIndex < pGCHeapCount; ++heapIndex)
//	{
//		GCHeap* heap = pGCHeaps[heapIndex];
//		for (uint32_t objectIndex = 0; objectIndex < heap->ObjectPoolSize; ++objectIndex)
//		{
//			GCObject* object = heap->ObjectPool[objectIndex];
//			if (!(object->Flags & GCObjectFlags_Marked) &&
//				!(object->Flags & GCObjectFlags_Disposed) &&
//				!(object->Flags & GCObjectFlags_Pinned))
//			{
//				if (object->Type->Finalizer) JIT_ExecuteMethod(object->Type->Finalizer, pDomain);
//				object->Flags |= GCObjectFlags_Disposed;
//			}
//			object->Flags &= ~GCObjectFlags_Marked;
//		}
//	}
//}
//
//void GC_MarkObjectAtAddress(void** pObject);
//void GC_MarkStructureAtAddress(void** pStructure, IRType* pType);
//
//void GC_MarkObject(GCObject* pObject)
//{
//	pObject->Flags |= GCObjectFlags_Marked;
//	if (pObject->Flags & GCObjectFlags_Array)
//	{
//		if (pObject->Array.ElementType->IsReferenceType)
//		{
//			for (uint32_t index = 0; index < pObject->Array.Length; index++)
//			{
//				void** addressPointer = (void**)((size_t)pObject->Data + (pObject->Array.ElementType->Size * index));
//				if (*addressPointer)
//				{
//					GC_MarkObjectAtAddress(addressPointer);
//				}
//			}
//		}
//		else if (pObject->Array.ElementType->IsStructureType)
//		{
//			for (uint32_t index = 0; index < pObject->Array.Length; index++)
//			{
//				void** addressPointer = (void**)((size_t)pObject->Data + (pObject->Array.ElementType->Size * index));
//				GC_MarkStructureAtAddress(addressPointer, pObject->Array.ElementType);
//			}
//		}
//
//	}
//	else
//	{
//		for (uint32_t index = 0; index < pObject->Type->FieldCount; ++index)
//		{
//			if (pObject->Type->Fields[index]->FieldType->IsArrayType)
//			{
//				GCObject* object = *(GCObject**)((size_t)*(void**)((size_t)pObject->Data + pObject->Type->Fields[index]->Offset) - gSizeOfPointerInBytes);
//				if (object->Array.ElementType->IsReferenceType)
//				{
//					for (uint32_t index = 0; index < object->Array.Length; index++)
//					{
//						void** addressPointer = (void**)((size_t)object->Data + (object->Array.ElementType->Size * index));
//						if (*addressPointer)
//						{
//							GC_MarkObjectAtAddress(addressPointer);
//						}
//					}
//				}
//				else if (object->Array.ElementType->IsStructureType)
//				{
//					for (uint32_t index = 0; index < object->Array.Length; index++)
//					{
//						void** addressPointer = (void**)((size_t)object->Data + (object->Array.ElementType->Size * index));
//						GC_MarkStructureAtAddress(addressPointer, object->Array.ElementType);
//					}
//				}
//			}
//			else if (pObject->Type->Fields[index]->FieldType->IsReferenceType)
//			{
//				void** addressPointer = (void**)((size_t)pObject->Data + pObject->Type->Fields[index]->Offset);
//				if (*addressPointer)
//				{
//					GC_MarkObjectAtAddress(addressPointer);
//				}
//			}
//			else if (pObject->Type->Fields[index]->FieldType->IsStructureType)
//			{
//				void** addressPointer = (void**)((size_t)pObject->Data + pObject->Type->Fields[index]->Offset);
//				GC_MarkStructureAtAddress(addressPointer, pObject->Type->Fields[index]->FieldType);
//			}
//		}
//	}
//}
//
//void GC_MarkObjectAtAddress(void** pObject)
//{
//	GCObject* object = (GCObject*)(((size_t)*pObject) - sizeof(GCObject*));
//	GC_MarkObject(object);
//}
//
//void GC_MarkStructureAtAddress(void** pStructure, IRType* pType)
//{
//	for (uint32_t index = 0; index < pType->FieldCount; ++index)
//	{
//		if (pType->Fields[index]->FieldType->IsArrayType)
//		{
//			GCObject* object = *(GCObject**)((size_t)*(void**)((size_t)*pStructure + pType->Fields[index]->Offset) - gSizeOfPointerInBytes);
//			if (object->Array.ElementType->IsReferenceType)
//			{
//				for (uint32_t index = 0; index < object->Array.Length; index++)
//				{
//					void** addressPointer = (void**)((size_t)object->Data + (object->Array.ElementType->Size * index));
//					if (*addressPointer)
//					{
//						GC_MarkObjectAtAddress(addressPointer);
//					}
//				}
//			}
//			else if (object->Array.ElementType->IsStructureType)
//			{
//				for (uint32_t index = 0; index < object->Array.Length; index++)
//				{
//					void** addressPointer = (void**)((size_t)object->Data + (object->Array.ElementType->Size * index));
//					GC_MarkStructureAtAddress(addressPointer, object->Array.ElementType);
//				}
//			}
//		}
//		else if (pType->Fields[index]->FieldType->IsReferenceType)
//		{
//			void** addressPointer = (void**)((size_t)*pStructure + pType->Fields[index]->Offset);
//			if (*addressPointer)
//			{
//				GC_MarkObjectAtAddress(addressPointer);
//			}
//		}
//		else if (pType->Fields[index]->FieldType->IsStructureType)
//		{
//			void** addressPointer = (void**)((size_t)*pStructure + pType->Fields[index]->Offset);
//			GC_MarkStructureAtAddress(addressPointer, pType->Fields[index]->FieldType);
//		}
//	}
//}
//
//void GC_MarkThreadStackObjects(Thread* pThread)
//{
//	uint32_t stackTop = pThread->SavedRegisterState.useresp;
//	uint8_t* stackStream = pThread->StackStream;
//	uint32_t stackUsed = (uint32_t)pThread->Stack - stackTop;
//	uint32_t stackStreamSize = stackUsed >> 5;
//	uint32_t stackStreamBlocks = stackStreamSize >> 2;
//
//	uint32_t* stackStreamBlockIterator = (uint32_t*)stackStream;
//	uint32_t* stackIterator = (uint32_t*)pThread->Stack;
//	for (uint32_t index = 0; index < stackStreamBlocks; ++index, ++stackStreamBlockIterator, stackIterator += 32)
//	{
//		// Something is set
//		if (*stackStreamBlockIterator)
//		{
//			// Low half set
//			if (*(uint16_t*)stackStreamBlockIterator)
//			{
//				// First byte set
//				if (*(uint8_t*)stackStreamBlockIterator)
//				{
//					uint8_t bits = *(uint8_t*)stackStreamBlockIterator;
//					if (bits & 0x01) GC_MarkObjectAtAddress((void**)stackIterator);
//					if (bits & 0x02) GC_MarkObjectAtAddress((void**)(stackIterator + 1));
//					if (bits & 0x04) GC_MarkObjectAtAddress((void**)(stackIterator + 2));
//					if (bits & 0x08) GC_MarkObjectAtAddress((void**)(stackIterator + 3));
//					if (bits & 0x10) GC_MarkObjectAtAddress((void**)(stackIterator + 4));
//					if (bits & 0x20) GC_MarkObjectAtAddress((void**)(stackIterator + 5));
//					if (bits & 0x40) GC_MarkObjectAtAddress((void**)(stackIterator + 6));
//					if (bits & 0x80) GC_MarkObjectAtAddress((void**)(stackIterator + 7));
//				}
//				// Second byte set
//				if (*(((uint8_t*)stackStreamBlockIterator) + 1))
//				{
//					uint8_t bits = *(((uint8_t*)stackStreamBlockIterator) + 1);
//					if (bits & 0x01) GC_MarkObjectAtAddress((void**)(stackIterator + 8));
//					if (bits & 0x02) GC_MarkObjectAtAddress((void**)(stackIterator + 9));
//					if (bits & 0x04) GC_MarkObjectAtAddress((void**)(stackIterator + 10));
//					if (bits & 0x08) GC_MarkObjectAtAddress((void**)(stackIterator + 11));
//					if (bits & 0x10) GC_MarkObjectAtAddress((void**)(stackIterator + 12));
//					if (bits & 0x20) GC_MarkObjectAtAddress((void**)(stackIterator + 13));
//					if (bits & 0x40) GC_MarkObjectAtAddress((void**)(stackIterator + 14));
//					if (bits & 0x80) GC_MarkObjectAtAddress((void**)(stackIterator + 15));
//				}
//			}
//			// High half set
//			if (*(((uint16_t*)stackStreamBlockIterator) + 1))
//			{
//				// Third byte set
//				if (*(((uint8_t*)stackStreamBlockIterator) + 2))
//				{
//					uint8_t bits = *(((uint8_t*)stackStreamBlockIterator) + 2);
//					if (bits & 0x01) GC_MarkObjectAtAddress((void**)(stackIterator + 16));
//					if (bits & 0x02) GC_MarkObjectAtAddress((void**)(stackIterator + 17));
//					if (bits & 0x04) GC_MarkObjectAtAddress((void**)(stackIterator + 18));
//					if (bits & 0x08) GC_MarkObjectAtAddress((void**)(stackIterator + 19));
//					if (bits & 0x10) GC_MarkObjectAtAddress((void**)(stackIterator + 20));
//					if (bits & 0x20) GC_MarkObjectAtAddress((void**)(stackIterator + 21));
//					if (bits & 0x40) GC_MarkObjectAtAddress((void**)(stackIterator + 22));
//					if (bits & 0x80) GC_MarkObjectAtAddress((void**)(stackIterator + 23));
//				}
//				// Fourth byte set
//				if (*(((uint8_t*)stackStreamBlockIterator) + 3))
//				{
//					uint8_t bits = *(((uint8_t*)stackStreamBlockIterator) + 3);
//					if (bits & 0x01) GC_MarkObjectAtAddress((void**)(stackIterator + 24));
//					if (bits & 0x02) GC_MarkObjectAtAddress((void**)(stackIterator + 25));
//					if (bits & 0x04) GC_MarkObjectAtAddress((void**)(stackIterator + 26));
//					if (bits & 0x08) GC_MarkObjectAtAddress((void**)(stackIterator + 27));
//					if (bits & 0x10) GC_MarkObjectAtAddress((void**)(stackIterator + 28));
//					if (bits & 0x20) GC_MarkObjectAtAddress((void**)(stackIterator + 29));
//					if (bits & 0x40) GC_MarkObjectAtAddress((void**)(stackIterator + 30));
//					if (bits & 0x80) GC_MarkObjectAtAddress((void**)(stackIterator + 31));
//				}
//			}
//		}
//	}
//	uint32_t remainderBytes = stackStreamSize & 0x03;
//	if (remainderBytes > 0)
//	{
//		if (*(uint8_t*)stackStreamBlockIterator)
//		{
//			uint8_t bits = *(uint8_t*)stackStreamBlockIterator;
//			if (bits & 0x01) GC_MarkObjectAtAddress((void**)stackIterator);
//			if (bits & 0x02) GC_MarkObjectAtAddress((void**)(stackIterator + 1));
//			if (bits & 0x04) GC_MarkObjectAtAddress((void**)(stackIterator + 2));
//			if (bits & 0x08) GC_MarkObjectAtAddress((void**)(stackIterator + 3));
//			if (bits & 0x10) GC_MarkObjectAtAddress((void**)(stackIterator + 4));
//			if (bits & 0x20) GC_MarkObjectAtAddress((void**)(stackIterator + 5));
//			if (bits & 0x40) GC_MarkObjectAtAddress((void**)(stackIterator + 6));
//			if (bits & 0x80) GC_MarkObjectAtAddress((void**)(stackIterator + 7));
//		}
//		if (remainderBytes > 1)
//		{
//			if (*(((uint8_t*)stackStreamBlockIterator) + 1))
//			{
//				uint8_t bits = *(((uint8_t*)stackStreamBlockIterator) + 1);
//				if (bits & 0x01) GC_MarkObjectAtAddress((void**)(stackIterator + 8));
//				if (bits & 0x02) GC_MarkObjectAtAddress((void**)(stackIterator + 9));
//				if (bits & 0x04) GC_MarkObjectAtAddress((void**)(stackIterator + 10));
//				if (bits & 0x08) GC_MarkObjectAtAddress((void**)(stackIterator + 11));
//				if (bits & 0x10) GC_MarkObjectAtAddress((void**)(stackIterator + 12));
//				if (bits & 0x20) GC_MarkObjectAtAddress((void**)(stackIterator + 13));
//				if (bits & 0x40) GC_MarkObjectAtAddress((void**)(stackIterator + 14));
//				if (bits & 0x80) GC_MarkObjectAtAddress((void**)(stackIterator + 15));
//			}
//
//			if (remainderBytes > 2)
//			{
//				if (*(((uint8_t*)stackStreamBlockIterator) + 2))
//				{
//					uint8_t bits = *(((uint8_t*)stackStreamBlockIterator) + 2);
//					if (bits & 0x01) GC_MarkObjectAtAddress((void**)(stackIterator + 16));
//					if (bits & 0x02) GC_MarkObjectAtAddress((void**)(stackIterator + 17));
//					if (bits & 0x04) GC_MarkObjectAtAddress((void**)(stackIterator + 18));
//					if (bits & 0x08) GC_MarkObjectAtAddress((void**)(stackIterator + 19));
//					if (bits & 0x10) GC_MarkObjectAtAddress((void**)(stackIterator + 20));
//					if (bits & 0x20) GC_MarkObjectAtAddress((void**)(stackIterator + 21));
//					if (bits & 0x40) GC_MarkObjectAtAddress((void**)(stackIterator + 22));
//					if (bits & 0x80) GC_MarkObjectAtAddress((void**)(stackIterator + 23));
//				}
//			}
//		}
//	}
//}
//
//void GC_MarkAndSweep(AppDomain* pDomain)
//{
//	Thread* currentThread = GetCurrentThread();
//	Thread* thread = NULL;
//
//	// Suspend all threads for this appdomain, except the current thread,
//	// and make sure the threads have stopped executing before we continue
//	for (uint32_t index = 0; index < pDomain->ThreadCount; ++index)
//	{
//		thread = pDomain->Threads[index];
//		if (thread && thread != currentThread)
//		{
//			ThreadScheduler_Suspend(thread);
//			if (!thread->SleepRemaining)
//			{
//				while (thread->CurrentAPIC) ;
//			}
//		}
//	}
//
//	// All threads in this app domain are now safe to work with their stacks,
//	// including the current thread stack, so scan the stack streams and mark
//	// the living objects, including references they contain to other objects
//	for (uint32_t index = 0; index < pDomain->ThreadCount; ++index)
//	{
//		thread = pDomain->Threads[index];
//		if (thread) GC_MarkThreadStackObjects(thread);
//	}
//
//	// Now we need to mark all the static field variables that are objects,
//	// and all the objects connected to them like the stack variables
//	for (uint32_t assemblyIndex = 0; assemblyIndex < pDomain->IRAssemblyCount; ++assemblyIndex)
//	{
//		IRAssembly* assembly = pDomain->IRAssemblies[assemblyIndex];
//		for (uint32_t fieldIndex = 0; fieldIndex < assembly->StaticFieldCount; ++fieldIndex)
//		{
//			IRField* field = assembly->StaticFields[fieldIndex];
//			if (field->FieldType->IsReferenceType)
//			{
//				void** addressPointer = (void**)((size_t)pDomain->StaticValues[assemblyIndex] + field->Offset);
//				if (*addressPointer)
//				{
//					GC_MarkObjectAtAddress(addressPointer);
//				}
//			}
//			else if (field->FieldType->IsStructureType)
//			{
//				void** addressPointer = (void**)((size_t)pDomain->StaticValues[assemblyIndex] + field->Offset);
//				GC_MarkStructureAtAddress(addressPointer, field->FieldType);
//			}
//		}
//	}
//
//	// At this point we should have a complete marked object pool,
//	// anything that is not marked is no longer alive and can have finalizers
//	// called
//	GCHeap_Sweep(pDomain->GarbageCollector->SmallGeneration2Heaps, pDomain->GarbageCollector->SmallGeneration2HeapCount, pDomain);
//	GCHeap_Sweep(pDomain->GarbageCollector->SmallGeneration1Heaps, pDomain->GarbageCollector->SmallGeneration1HeapCount, pDomain);
//	GCHeap_Sweep(pDomain->GarbageCollector->SmallGeneration0Heaps, pDomain->GarbageCollector->SmallGeneration0HeapCount, pDomain);
//	GCHeap_Sweep(pDomain->GarbageCollector->LargeHeaps, pDomain->GarbageCollector->LargeHeapCount, pDomain);
//
//	// Everything that was no longer alive should have had it's finalizer called
//	// if it had one, and is now set disposed, and all objects have been unmarked
//}

//void GC_ApplyPressure(AppDomain* pDomain, uint32_t pBytes)
//{
//	Atomic_AquireLock(&pDomain->GarbageCollector->Busy);
//	pDomain->GarbageCollector->Pressure += pBytes;
//	if (pDomain->GarbageCollector->Pressure >= GC__PressureTriggerInBytes)
//	{
//		GC_MarkAndSweep(pDomain);
//		pDomain->GarbageCollector->Pressure = 0;
//	}
//	Atomic_ReleaseLock(&pDomain->GarbageCollector->Busy);
//}


void* GCObject_Internal_IsInst(void* pObject, IRType* pType)
{
	if (!pObject) return NULL;
	if (!pType) Panic("This shouldn't be happening");
	GCObject* object = *(GCObject**)((size_t)pObject - gSizeOfPointerInBytes);
	if (IRType_IsSubclassOf(object->Type, pType)) return pObject;
	return NULL;
}
