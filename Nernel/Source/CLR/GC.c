#include <System/Atomics.h>
#include <System/ThreadScheduler.h>
#include <CLR/GC.h>
#include <CLR/JIT.h>

Thread* GetCurrentThread();

GC* GC_Create(AppDomain* pDomain)
{
	GC* gc = (GC*)calloc(1, sizeof(GC));
	pDomain->GarbageCollector = gc;
	gc->Domain = pDomain;
	gc->SmallGeneration0HeapCount = 1;
	//gc->SmallGeneration1HeapCount = 1;
	//gc->SmallGeneration2HeapCount = 1;
	gc->LargeHeapCount = 1;
	gc->SmallGeneration0Heaps = (GCHeap**)calloc(1, sizeof(GCHeap*));
	//gc->SmallGeneration1Heaps = (GCHeap**)calloc(1, sizeof(GCHeap*));
	//gc->SmallGeneration2Heaps = (GCHeap**)calloc(1, sizeof(GCHeap*));
	gc->LargeHeaps = (GCHeap**)calloc(1, sizeof(GCHeap*));
	gc->SmallGeneration0Heaps[0] = GCHeap_Create(GCHeap__SmallHeap_Size, GCHeap__SmallHeap_InitialPoolSize, TRUE);
	//gc->SmallGeneration1Heaps[0] = GCHeap_Create(GCHeap__SmallHeap_Size, GCHeap__SmallHeap_InitialPoolSize, TRUE);
	//gc->SmallGeneration2Heaps[0] = GCHeap_Create(GCHeap__SmallHeap_Size, GCHeap__SmallHeap_InitialPoolSize, TRUE);
	gc->LargeHeaps[0] = GCHeap_Create(GCHeap__LargeHeap_Size, GCHeap__LargeHeap_InitialPoolSize, TRUE);

	GC_AllocateStringFromASCII(pDomain, (int8_t*)"", 0, &gc->EmptyStringObject);
	GCObject* object = *(GCObject**)((size_t)gc->EmptyStringObject - sizeof(GCObject*));
	object->Flags |= GCObjectFlags_Pinned;
	return gc;
}

void GC_Destroy(GC* pGC)
{
    for (uint32_t index = 0; index < pGC->LargeHeapCount; ++index)
        GCHeap_Destroy(pGC->LargeHeaps[index]);
    //for (uint32_t index = 0; index < pGC->SmallGeneration2HeapCount; ++index)
    //    GCHeap_Destroy(pGC->SmallGeneration2Heaps[index]);
    //for (uint32_t index = 0; index < pGC->SmallGeneration1HeapCount; ++index)
    //    GCHeap_Destroy(pGC->SmallGeneration1Heaps[index]);
    for (uint32_t index = 0; index < pGC->SmallGeneration0HeapCount; ++index)
        GCHeap_Destroy(pGC->SmallGeneration0Heaps[index]);
    free (pGC->LargeHeaps);
    //free (pGC->SmallGeneration2Heaps);
    //free (pGC->SmallGeneration1Heaps);
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
		uint32_t blockSize = pHeapSize;
		uint32_t bitsRequired = 1;
		blockSize >>= 1;
		heap->AllocationTreeLevels = 1;
		while (blockSize >= 8)
		{
			bitsRequired <<= 1;
			bitsRequired |= 1;
			blockSize >>= 1;
			++heap->AllocationTreeLevels;
		}
		uint32_t bytesRequired = (bitsRequired + (8 - (bitsRequired & 0x0F))) >> 3;
		uint32_t dwordsRequired = bytesRequired >> 2;
		if (bytesRequired & 0x03) ++dwordsRequired;
		if (!dwordsRequired) dwordsRequired = 1;
		heap->AllocationTree = (uint32_t*)calloc(1, sizeof(uint32_t) * dwordsRequired);
		heap->AllocationTreeSize = dwordsRequired;
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
	uint32_t nextBlockSize = (1 << (pMaxLevels - pLevel - 1)) << 2;
	int32_t result = -1;
	if (!GCHeap_IsAllocatedInTree(pAllocationTree, pLevel, pIndex))
	{
		if ((pLevel + 1) < pMaxLevels && pSize <= nextBlockSize)
		{
			result = GCHeap_FindAvailableInTree(pAllocationTree, pLevel + 1, pMaxLevels, pIndex << 1, pSize, pAllocatedSize);
			if (result >= 0) return result;
		}
		else
		{
			*pAllocatedSize = (1 << (pMaxLevels - pLevel - 1)) << 3;
			return pIndex << (pMaxLevels - pLevel - 1);
		}
	}

	if (!GCHeap_IsAllocatedInTree(pAllocationTree, pLevel, pIndex + 1))
	{
		if ((pLevel + 1) < pMaxLevels && pSize <= nextBlockSize)
		{
			result = GCHeap_FindAvailableInTree(pAllocationTree, pLevel + 1, pMaxLevels, (pIndex << 1) + 1, pSize, pAllocatedSize);
		}
		else
		{
			*pAllocatedSize = (1 << (pMaxLevels - pLevel - 1)) << 3;
			return (pIndex + 1) << (pMaxLevels - pLevel - 1);
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

void GCHeap_DumpTree(uint32_t* pAllocationTree, uint32_t pLevel, uint32_t pMaxLevels)
{
	uint32_t totalBlocks = (1 << pLevel);
	printf("---------- %u Blocks of %u Bytes, Level %u/%u\n", (unsigned int)totalBlocks, (unsigned int)((1 << (pMaxLevels - pLevel - 1)) << 3), (unsigned int)pLevel, (unsigned int)pMaxLevels);
	for (uint32_t blockIndex = 0; blockIndex < totalBlocks; ++blockIndex)
	{
		if (blockIndex && !(blockIndex % 64)) printf("\n");
		if (GCHeap_IsAllocatedInTree(pAllocationTree, pLevel, blockIndex)) printf("1");
		else printf("0");
	}
	printf("\n");
	++pLevel;
	if (pLevel < pMaxLevels) GCHeap_DumpTree(pAllocationTree, pLevel, pMaxLevels);
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
			if (heap->AllocationTree)
			{
				GCHeap_SetBitsInTree(heap->AllocationTree, heap->AllocationTreeLevels - 1, index, allocatedSize >> 3, TRUE);
				//GCHeap_DumpTree(heap->AllocationTree, 0, heap->AllocationTreeLevels);
			}
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
		if (heap->AllocationTree)
		{
			GCHeap_SetBitsInTree(heap->AllocationTree, heap->AllocationTreeLevels - 1, index, allocatedSize >> 3, TRUE);
			//GCHeap_DumpTree(heap->AllocationTree, 0, heap->AllocationTreeLevels);
		}
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
		printf("Allocated %u, Available %u\n", (unsigned int)heap->Allocated, (unsigned int)heap->Available);
    }
	if (!object) Panic("Whoa, how did this happen?!");
	memset(object->Data, 0x00, pSize);
	*(GCObject**)((size_t)object->Data - sizeof(GCObject*)) = object;
    return object;
}

void GC_AllocateObject(AppDomain* pDomain, IRType* pType, uint32_t pSize, void** pAllocatedObject)
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

void GC_AllocateStringFromASCII(AppDomain* pDomain, int8_t* pString, uint32_t pLength, void** pAllocatedObject)
{
	if (pLength == 0 && pDomain->GarbageCollector->EmptyStringObject)
	{
		*pAllocatedObject = pDomain->GarbageCollector->EmptyStringObject;
		return;
	}
	uint32_t size = 4 + (pLength << 1);
	if (size >= 0x7FFFFFFF) Panic("GC_AllocateStringFromASCII Size >= 0x7FFFFFFF");
	GC* gc = pDomain->GarbageCollector;
	uint8_t* unicodeString = (uint8_t*)calloc(1, size);
	for (uint32_t index = 0; index < pLength; ++index) unicodeString[index << 1] = pString[index];
	Atomic_AquireLock(&gc->Busy);
    GCObject* object = NULL;
	// Here we will check if there is already an interned string matching, and use it if so,
	// but we don't add this string to the hash if it's not found
	HASH_FIND(String.HashHandle, gc->StringHashTable, (uint8_t*)pString, size - 4, object);
	if (!object)
	{
		if (size <= GCHeap__SmallHeap_Size - sizeof(GCObject*))
			object = GCHeap_Allocate(&gc->SmallGeneration0Heaps, &gc->SmallGeneration0HeapCount, GCHeap__SmallHeap_Size, size, TRUE);
		else if (size <= GCHeap__LargeHeap_Size - sizeof(GCObject*))
			object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, GCHeap__LargeHeap_Size, size, TRUE);
		else object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, size + sizeof(GCObject*), size, FALSE);
		object->Type = gc->Domain->IRAssemblies[0]->Types[gc->Domain->CachedType___System_String->TableIndex - 1];
		object->Flags |= GCObjectFlags_String;
		*(uint32_t*)object->Data = pLength;
		if (pLength > 0) memcpy((uint8_t*)object->Data + 4, unicodeString, size - 4);
	}

	*pAllocatedObject = object->Data;
	Atomic_ReleaseLock(&gc->Busy);
	free(unicodeString);
}

void GC_AllocateStringFromUnicode(AppDomain* pDomain, uint16_t* pString, uint32_t pLength, void** pAllocatedObject)
{
	if (pLength == 0)
	{
		*pAllocatedObject = pDomain->GarbageCollector->EmptyStringObject;
		return;
	}
	uint32_t size = 4 + (pLength << 1);
	if (size >= 0x7FFFFFFF) Panic("GC_AllocateStringFromUnicode Size >= 0x7FFFFFFF");
	GC* gc = pDomain->GarbageCollector;
	Atomic_AquireLock(&gc->Busy);
    GCObject* object = NULL;
	// Here we will check if there is already an interned string matching, and use it if so,
	// but we don't add this string to the hash if it's not found
	HASH_FIND(String.HashHandle, gc->StringHashTable, (uint8_t*)pString, size - 4, object);
	if (!object)
	{
		if (size <= GCHeap__SmallHeap_Size - sizeof(GCObject*))
			object = GCHeap_Allocate(&gc->SmallGeneration0Heaps, &gc->SmallGeneration0HeapCount, GCHeap__SmallHeap_Size, size, TRUE);
		else if (size <= GCHeap__LargeHeap_Size - sizeof(GCObject*))
			object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, GCHeap__LargeHeap_Size, size, TRUE);
		else object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, size + sizeof(GCObject*), size, FALSE);
		object->Type = gc->Domain->IRAssemblies[0]->Types[gc->Domain->CachedType___System_String->TableIndex - 1];
		object->Flags |= GCObjectFlags_String;
		*(uint32_t*)object->Data = pLength;
		memcpy((uint8_t*)object->Data + 4, (uint8_t*)pString, size - 4);
	}

	*pAllocatedObject = object->Data;
	Atomic_ReleaseLock(&gc->Busy);
}

void GC_AllocateInternedStringFromUnicode(AppDomain* pDomain, uint16_t* pString, uint32_t pLength, void** pAllocatedObject)
{
	if (pLength == 0)
	{
		*pAllocatedObject = pDomain->GarbageCollector->EmptyStringObject;
		return;
	}
	uint32_t size = 4 + (pLength << 1);
	if (size >= 0x7FFFFFFF) Panic("GC_AllocateInternedStringFromUnicode Size >= 0x7FFFFFFF");
	GC* gc = pDomain->GarbageCollector;
	Atomic_AquireLock(&gc->Busy);
    GCObject* object = NULL;
	HASH_FIND(String.HashHandle, gc->StringHashTable, (uint8_t*)pString, size - 4, object);
	if (!object)
	{
		if (size <= GCHeap__SmallHeap_Size - sizeof(GCObject*))
			object = GCHeap_Allocate(&gc->SmallGeneration0Heaps, &gc->SmallGeneration0HeapCount, GCHeap__SmallHeap_Size, size, TRUE);
		else if (size <= GCHeap__LargeHeap_Size - sizeof(GCObject*))
			object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, GCHeap__LargeHeap_Size, size, TRUE);
		else object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, size + sizeof(GCObject*), size, FALSE);
		object->Type = gc->Domain->IRAssemblies[0]->Types[gc->Domain->CachedType___System_String->TableIndex - 1];
		object->Flags |= (GCObjectFlags_String | GCObjectFlags_Interned);
		*(uint32_t*)object->Data = pLength;
		memcpy((uint8_t*)object->Data + 4, (uint8_t*)pString, size - 4);
		HASH_ADD_KEYPTR(String.HashHandle, gc->StringHashTable, ((uint8_t*)object->Data + 4), size - 4, object);
	}
	*pAllocatedObject = object->Data;
	Atomic_ReleaseLock(&gc->Busy);
}

void GC_AllocateEmptyStringFromLength(AppDomain* pDomain, uint32_t pLength, void** pAllocatedObject)
{
	if (pLength == 0)
	{
		*pAllocatedObject = pDomain->GarbageCollector->EmptyStringObject;
		return;
	}
	uint32_t size = 4 + (pLength << 1);
	if (size >= 0x7FFFFFFF) Panic("GC_AllocateEmptyStringFromLength Size >= 0x7FFFFFFF");
	GC* gc = pDomain->GarbageCollector;
	Atomic_AquireLock(&gc->Busy);
    GCObject* object = NULL;
	// Here we don't even check the interned strings because we have no value to check
	if (size <= GCHeap__SmallHeap_Size - sizeof(GCObject*))
		object = GCHeap_Allocate(&gc->SmallGeneration0Heaps, &gc->SmallGeneration0HeapCount, GCHeap__SmallHeap_Size, size, TRUE);
	else if (size <= GCHeap__LargeHeap_Size - sizeof(GCObject*))
		object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, GCHeap__LargeHeap_Size, size, TRUE);
	else object = GCHeap_Allocate(&gc->LargeHeaps, &gc->LargeHeapCount, size + sizeof(GCObject*), size, FALSE);
	object->Type = gc->Domain->IRAssemblies[0]->Types[gc->Domain->CachedType___System_String->TableIndex - 1];
	object->Flags |= GCObjectFlags_String;
	*(uint32_t*)object->Data = pLength;

	*pAllocatedObject = object->Data;
	Atomic_ReleaseLock(&gc->Busy);
}

void GC_AllocateArray(AppDomain* pDomain, IRType* pArrayType, uint32_t pElementCount, void** pAllocatedObject)
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

void GCHeap_Sweep(GCHeap*** pGCHeaps, uint32_t* pGCHeapCount, AppDomain* pDomain)
{
	for (uint32_t heapIndex = 0; heapIndex < *pGCHeapCount; ++heapIndex)
	{
		GCHeap* heap = (*pGCHeaps)[heapIndex];
		for (GCObject* object = heap->AllocatedObjectList, *nextObject = NULL, *prevObject = NULL; object; object = nextObject)
		{
			nextObject = object;
			if (!(object->Flags & GCObjectFlags_Marked) &&
				!(object->Flags & GCObjectFlags_Pinned))
			{
				if (object->Type->Finalizer) JIT_ExecuteMethod(object->Type->Finalizer, pDomain);
				if (heap->AllocationTree)
				{
					uint32_t index = (uint32_t)(((uint8_t*)object->Data - gSizeOfPointerInBytes) - heap->Bottom) >> 3;
					GCHeap_SetBitsInTree(heap->AllocationTree, heap->AllocationTreeLevels - 1, index, object->AllocatedSize >> 3, FALSE);
				}
				if ((object->Flags & (GCObjectFlags_String | GCObjectFlags_Interned)) == (GCObjectFlags_String | GCObjectFlags_Interned))
				{
					HASH_DELETE(String.HashHandle, pDomain->GarbageCollector->StringHashTable, object);
				}
				if (object == heap->AllocatedObjectList) heap->AllocatedObjectList = object->NextObject;
				else prevObject->NextObject = object->NextObject;
				object->NextObject = heap->FreeObjectList;
				heap->FreeObjectList = object;
			}
			else prevObject = object;
			object->Flags &= ~GCObjectFlags_Marked;
		}
	}
	// Now we will delete any single large object heaps that no longer have an allocated object
	// These would be the custom large heaps that contain only one object, and no allocation tree
	for (uint32_t heapIndex = 0; heapIndex < *pGCHeapCount; )
	{
		GCHeap* heap = (*pGCHeaps)[heapIndex];
		if (!heap->AllocationTree && !heap->AllocatedObjectList)
		{
			GCHeap_Destroy(heap);
			if (heapIndex + 1 < *pGCHeapCount)
			{
				memcpy((void*)((*pGCHeaps) + heapIndex), (void*)((*pGCHeaps) + (heapIndex + 1)), sizeof(GCHeap*) * (*pGCHeapCount - (heapIndex + 1)));
			}
			*pGCHeapCount = *pGCHeapCount - 1;
			*pGCHeaps = realloc(*pGCHeaps, sizeof(GCHeap*) * (*pGCHeapCount));
		}
		else ++heapIndex;
	}
}

void GC_MarkObjectAtAddress(void** pObject);
void GC_MarkStructureAtAddress(void** pStructure, IRType* pType);

void GC_MarkObject(GCObject* pObject)
{
	pObject->Flags |= GCObjectFlags_Marked;
	if (pObject->Flags & GCObjectFlags_Array)
	{
		if (pObject->Array.ElementType->IsReferenceType)
		{
			for (uint32_t index = 0; index < pObject->Array.Length; index++)
			{
				void** addressPointer = (void**)((size_t)pObject->Data + (pObject->Array.ElementType->Size * index));
				if (*addressPointer)
				{
					GC_MarkObjectAtAddress(addressPointer);
				}
			}
		}
		else if (pObject->Array.ElementType->IsStructureType)
		{
			for (uint32_t index = 0; index < pObject->Array.Length; index++)
			{
				void** addressPointer = (void**)((size_t)pObject->Data + (pObject->Array.ElementType->Size * index));
				GC_MarkStructureAtAddress(addressPointer, pObject->Array.ElementType);
			}
		}

	}
	else
	{
		for (uint32_t index = 0; index < pObject->Type->FieldCount; ++index)
		{
			if (pObject->Type->Fields[index]->FieldType->IsArrayType)
			{
				GCObject* object = *(GCObject**)((size_t)*(void**)((size_t)pObject->Data + pObject->Type->Fields[index]->Offset) - gSizeOfPointerInBytes);
				if (object->Array.ElementType->IsReferenceType)
				{
					for (uint32_t index = 0; index < object->Array.Length; index++)
					{
						void** addressPointer = (void**)((size_t)object->Data + (object->Array.ElementType->Size * index));
						if (*addressPointer)
						{
							GC_MarkObjectAtAddress(addressPointer);
						}
					}
				}
				else if (object->Array.ElementType->IsStructureType)
				{
					for (uint32_t index = 0; index < object->Array.Length; index++)
					{
						void** addressPointer = (void**)((size_t)object->Data + (object->Array.ElementType->Size * index));
						GC_MarkStructureAtAddress(addressPointer, object->Array.ElementType);
					}
				}
			}
			else if (pObject->Type->Fields[index]->FieldType->IsReferenceType)
			{
				void** addressPointer = (void**)((size_t)pObject->Data + pObject->Type->Fields[index]->Offset);
				if (*addressPointer)
				{
					GC_MarkObjectAtAddress(addressPointer);
				}
			}
			else if (pObject->Type->Fields[index]->FieldType->IsStructureType)
			{
				void** addressPointer = (void**)((size_t)pObject->Data + pObject->Type->Fields[index]->Offset);
				GC_MarkStructureAtAddress(addressPointer, pObject->Type->Fields[index]->FieldType);
			}
		}
	}
}

void GC_MarkObjectAtAddress(void** pObject)
{
	if (!(*pObject)) return;
	GCObject* object = *(GCObject**)(((size_t)*pObject) - sizeof(GCObject*));
	if (object->Flags & GCObjectFlags_Marked) return;
	GC_MarkObject(object);
}

void GC_MarkStructureAtAddress(void** pStructure, IRType* pType)
{
	for (uint32_t index = 0; index < pType->FieldCount; ++index)
	{
		if (pType->Fields[index]->FieldType->IsArrayType)
		{
			GCObject* object = *(GCObject**)((size_t)*(void**)((size_t)*pStructure + pType->Fields[index]->Offset) - gSizeOfPointerInBytes);
			if (object->Array.ElementType->IsReferenceType)
			{
				for (uint32_t index = 0; index < object->Array.Length; index++)
				{
					void** addressPointer = (void**)((size_t)object->Data + (object->Array.ElementType->Size * index));
					if (*addressPointer)
					{
						GC_MarkObjectAtAddress(addressPointer);
					}
				}
			}
			else if (object->Array.ElementType->IsStructureType)
			{
				for (uint32_t index = 0; index < object->Array.Length; index++)
				{
					void** addressPointer = (void**)((size_t)object->Data + (object->Array.ElementType->Size * index));
					GC_MarkStructureAtAddress(addressPointer, object->Array.ElementType);
				}
			}
		}
		else if (pType->Fields[index]->FieldType->IsReferenceType)
		{
			void** addressPointer = (void**)((size_t)*pStructure + pType->Fields[index]->Offset);
			if (*addressPointer)
			{
				GC_MarkObjectAtAddress(addressPointer);
			}
		}
		else if (pType->Fields[index]->FieldType->IsStructureType)
		{
			void** addressPointer = (void**)((size_t)*pStructure + pType->Fields[index]->Offset);
			GC_MarkStructureAtAddress(addressPointer, pType->Fields[index]->FieldType);
		}
	}
}

void GC_MarkThreadStackFrameObjects(Thread* pThread, size_t pStackFramePointer, IRMethod* pMethod)
{
	if (pMethod)
	{
		IRLocalVariable* localVariable = NULL;
		void** addressPointer = NULL;
		for (uint32_t index = 0; index < pMethod->LocalVariableCount; ++index)
		{
			localVariable = pMethod->LocalVariables[index];
			addressPointer = (void**)(pStackFramePointer - localVariable->Offset);
			if (localVariable->VariableType->IsStructureType)
			{
				GC_MarkStructureAtAddress(addressPointer, localVariable->VariableType);
			}
			else if (localVariable->VariableType->IsReferenceType)
			{
				GC_MarkObjectAtAddress(addressPointer);
			}
		}
	}

	if (pStackFramePointer == (size_t)(pThread->Stack + pThread->StackSize)) return;

	size_t previousFramePointer = *(size_t*)pStackFramePointer;
	size_t previousInstructionPointer = *(size_t*)(pStackFramePointer + gSizeOfPointerInBytes);
	InstructionPointerMappingNode* node = AppDomain_FindInstructionPointerMapping(pThread->Domain, previousInstructionPointer);
	GC_MarkThreadStackFrameObjects(pThread, previousFramePointer, node ? node->Method : NULL);
}

void GC_MarkThreadStackObjects(Thread* pThread)
{
	InstructionPointerMappingNode* node = AppDomain_FindInstructionPointerMapping(pThread->Domain, pThread->SavedRegisterState.eip);
	GC_MarkThreadStackFrameObjects(pThread, pThread->SavedRegisterState.ebp, node ? node->Method : NULL);
}

void GC_MarkAndSweep(AppDomain* pDomain)
{
	Thread* currentThread = GetCurrentThread();
	Thread* thread = NULL;

	// Need to do this to ensure no threads are currently USING the GC,
	// before we interrupt them, this ensures objects are put to trackable
	// locations between the point of allocation and when the allocated
	// object pointer gets set
	Atomic_AquireLock(&currentThread->Domain->GarbageCollector->Busy);

	// Suspend all threads for this appdomain, except the current thread,
	// and make sure the threads have stopped executing before we continue,
	// also means no need to lock the GC
	for (uint32_t index = 0; index < pDomain->ThreadCount; ++index)
	{
		thread = pDomain->Threads[index];
		if (thread && thread != currentThread)
		{
			ThreadScheduler_Suspend(thread);
			if (!thread->SleepRemaining)
			{
				while (thread->CurrentAPIC) ;
			}
		}
	}
	// Now we can release the lock, knowing no threads are currently in the
	// process of allocating, so we know any allocations finished, and put the
	// reference in a trackable location for all threads before we get here
	Atomic_ReleaseLock(&currentThread->Domain->GarbageCollector->Busy);

	// All threads in this app domain are now safe to work with their stacks,
	// except the current thread stack, so walk the stack and mark the living
	// objects, including references they contain to other objects
	for (uint32_t index = 0; index < pDomain->ThreadCount; ++index)
	{
		thread = pDomain->Threads[index];
		if (thread && thread != currentThread) GC_MarkThreadStackObjects(thread);
	}

	// Now we need to mark all the static field variables that are objects,
	// and all the objects connected to them like the stack variables
	for (uint32_t assemblyIndex = 0; assemblyIndex < pDomain->IRAssemblyCount; ++assemblyIndex)
	{
		IRAssembly* assembly = pDomain->IRAssemblies[assemblyIndex];
		for (uint32_t fieldIndex = 0; fieldIndex < assembly->StaticFieldCount; ++fieldIndex)
		{
			IRField* field = assembly->StaticFields[fieldIndex];
			if (field->FieldType->IsReferenceType)
			{
				void** addressPointer = (void**)((size_t)pDomain->StaticValues[assemblyIndex] + field->Offset);
				if (*addressPointer)
				{
					GC_MarkObjectAtAddress(addressPointer);
				}
			}
			else if (field->FieldType->IsStructureType)
			{
				void** addressPointer = (void**)((size_t)pDomain->StaticValues[assemblyIndex] + field->Offset);
				GC_MarkStructureAtAddress(addressPointer, field->FieldType);
			}
		}
	}

	// At this point we should have a complete marked object pool, anything
	// that is not marked is no longer alive and can have finalizers called
	//GCHeap_Sweep(&pDomain->GarbageCollector->SmallGeneration2Heaps, &pDomain->GarbageCollector->SmallGeneration2HeapCount, pDomain);
	//GCHeap_Sweep(&pDomain->GarbageCollector->SmallGeneration1Heaps, &pDomain->GarbageCollector->SmallGeneration1HeapCount, pDomain);
	GCHeap_Sweep(&pDomain->GarbageCollector->SmallGeneration0Heaps, &pDomain->GarbageCollector->SmallGeneration0HeapCount, pDomain);
	GCHeap_Sweep(&pDomain->GarbageCollector->LargeHeaps, &pDomain->GarbageCollector->LargeHeapCount, pDomain);

	// Everything that was no longer alive should have had it's finalizer called
	// if it had one, and is now disposed, and all objects have been unmarked, so
	// resume all threads for this appdomain, except the current thread
	for (uint32_t index = 0; index < pDomain->ThreadCount; ++index)
	{
		thread = pDomain->Threads[index];
		if (thread && thread != currentThread) ThreadScheduler_Resume(thread);
	}
}

void GC_ApplyPressure(AppDomain* pDomain, uint32_t pBytes)
{
	if (Atomic_Add(&pDomain->GarbageCollector->Pressure, pBytes) >= GC__PressureTriggerInBytes)
	{
		GC_MarkAndSweep(pDomain);
		pDomain->GarbageCollector->Pressure = 0;
	}
	Atomic_ReleaseLock(&pDomain->GarbageCollector->Busy);
}

uint64_t GC_TotalAllocatedMemory(AppDomain* pDomain)
{
	uint64_t totalAllocated = 0;
	for (uint32_t index = 0; index < pDomain->GarbageCollector->SmallGeneration0HeapCount; ++index)
	{
		totalAllocated += pDomain->GarbageCollector->SmallGeneration0Heaps[index]->Allocated;
	}
	for (uint32_t index = 0; index < pDomain->GarbageCollector->LargeHeapCount; ++index)
	{
		totalAllocated += pDomain->GarbageCollector->LargeHeaps[index]->Allocated;
	}
	return totalAllocated;
}

void* GCObject_Internal_IsInst(void* pObject, IRType* pType)
{
	if (!pObject) return NULL;
	if (!pType) Panic("This shouldn't be happening");
	GCObject* object = *(GCObject**)((size_t)pObject - gSizeOfPointerInBytes);
	if (IRType_IsSubclassOf(object->Type, pType)) return pObject;
	return NULL;
}
