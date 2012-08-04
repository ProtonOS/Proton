#include <System/Atomics.h>
#include <CLR/GC.h>

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
	gc->SmallGeneration0Heaps[0] = GCHeap_Create(GCHeap__SmallHeap_Size, GCHeap__SmallHeap_InitialPoolSize);
	gc->SmallGeneration1Heaps[0] = GCHeap_Create(GCHeap__SmallHeap_Size, GCHeap__SmallHeap_InitialPoolSize);
	gc->SmallGeneration2Heaps[0] = GCHeap_Create(GCHeap__SmallHeap_Size, GCHeap__SmallHeap_InitialPoolSize);
	gc->LargeHeaps[0] = GCHeap_Create(GCHeap__LargeHeap_Size, GCHeap__LargeHeap_InitialPoolSize);

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

GCHeap* GCHeap_Create(uint32_t pHeapSize, uint32_t pInitialPoolSize)
{
    GCHeap* heap = (GCHeap*)calloc(1, sizeof(GCHeap));
    heap->ObjectPoolSize = pInitialPoolSize;
    heap->ObjectPool = (GCObject**)calloc(1, sizeof(GCObject*) * pInitialPoolSize);
	for (uint32_t index = 0; index < pInitialPoolSize; ++index) heap->ObjectPool[index] = (GCObject*)calloc(1, sizeof(GCObject));
    heap->Size = pHeapSize;
    heap->Available = heap->Size;
    heap->Bottom = (uint8_t*)calloc(1, heap->Size);
    heap->Top = heap->Bottom;
    return heap;
}

void GCHeap_Destroy(GCHeap* pGCHeap)
{
	for (uint32_t index = 0; index < pGCHeap->ObjectPoolSize; ++index)
    {
        free (pGCHeap->Bottom);
        free (pGCHeap->ObjectPool[index]);
    }
    free (pGCHeap->ObjectPool);
    free(pGCHeap);
}

GCObject* GCHeap_Allocate(GCHeap*** pGCHeaps, uint32_t* pGCHeapCount, uint32_t pStackSize, uint32_t pSize)
{
    GCObject* object = NULL;
    GCHeap* heap = NULL;
    for (uint32_t index = 0; !object && index < *pGCHeapCount; ++index)
    {
        heap = *pGCHeaps[index];
        if (heap->Available >= (pSize + sizeof(GCObject*)))
        {
            for (uint32_t objectIndex = 0; !object && objectIndex < heap->ObjectPoolSize; ++objectIndex)
			{
				GCObject* obj = heap->ObjectPool[objectIndex];
                if (!obj->Data || ((obj->Flags & GCObjectFlags_Disposed) && (pSize == obj->Size)))
                    object = obj;
			}
            if (!object)
            {
                heap->ObjectPoolSize <<= 1;
                heap->ObjectPool = (GCObject**)realloc(heap->ObjectPool, sizeof(GCObject*) * heap->ObjectPoolSize);
                for (uint32_t objectIndex = heap->ObjectPoolSize >> 1; objectIndex < heap->ObjectPoolSize; ++objectIndex)
                    heap->ObjectPool[objectIndex] = (GCObject*)calloc(1, sizeof(GCObject));
                object = heap->ObjectPool[heap->ObjectPoolSize >> 1];
            }
            object->Flags = GCObjectFlags_Allocated;
            object->Size = pSize;
            object->Data = heap->Top + sizeof(GCObject*);
            object->Heap = heap;
            heap->Available -= pSize + sizeof(GCObject*);
            heap->Allocated += pSize + sizeof(GCObject*);
            heap->Top += pSize + sizeof(GCObject*);
        }
    }
    if (!object)
    {
        *pGCHeapCount = *pGCHeapCount + 1;
        *pGCHeaps = (GCHeap**)realloc(*pGCHeaps, sizeof(GCHeap*) * (*pGCHeapCount));
        *pGCHeaps[*pGCHeapCount - 1] = (GCHeap*)calloc(1, sizeof(GCHeap));
        heap = *pGCHeaps[*pGCHeapCount - 1];
        heap->ObjectPoolSize = 1;
        heap->ObjectPool = (GCObject**)calloc(heap->ObjectPoolSize, sizeof(GCObject*));
        heap->ObjectPool[0] = (GCObject*)calloc(1, sizeof(GCObject));
        heap->Size = pStackSize;
        heap->Available = pStackSize;
        heap->Bottom = (uint8_t*)calloc(1, pStackSize);
        heap->Top = heap->Bottom;

        object = heap->ObjectPool[0];
        object->Flags = GCObjectFlags_Allocated;
        object->Size = pSize;
        object->Data = heap->Top + sizeof(GCObject*);
        object->Heap = heap;
        heap->Available -= pSize + sizeof(GCObject*);
        heap->Allocated += pSize + sizeof(GCObject*);
        heap->Top += pSize + sizeof(GCObject*);
    }
	if (!object) Panic("Whoa, how did this happen?!");
	memset(object->Data, 0x00, pSize);
	*(GCObject**)((size_t)object->Data - sizeof(GCObject*)) = object;
    return object;
}

void GC_AllocateObject(GC* pGC, IRType* pType, uint32_t pSize, GCObject** pAllocatedObject)
{
	if (pSize >= 0x7FFFFFFF) Panic("GC_AllocateObject pSize >= 0x7FFFFFFF");
	Atomic_AquireLock(&pGC->Busy);
    GCObject* object = NULL;
    if (pSize <= GCHeap__SmallHeap_Size - sizeof(GCObject*))
        object = GCHeap_Allocate(&pGC->SmallGeneration0Heaps, &pGC->SmallGeneration0HeapCount, GCHeap__SmallHeap_Size, pSize);
    else if (pSize <= GCHeap__LargeHeap_Size - sizeof(GCObject*))
        object = GCHeap_Allocate(&pGC->LargeHeaps, &pGC->LargeHeapCount, GCHeap__LargeHeap_Size, pSize);
    else object = GCHeap_Allocate(&pGC->LargeHeaps, &pGC->LargeHeapCount, pSize + sizeof(GCObject*), pSize);
	object->Type = pType;
	*pAllocatedObject = object;
	Atomic_ReleaseLock(&pGC->Busy);
}

GCObject* GC_AllocatePinnedObject(GC* pGC, IRType* pType, uint32_t pSize)
{
	if (pSize >= 0x7FFFFFFF) Panic("GC_AllocatePinnedObject pSize >= 0x7FFFFFFF");
	Atomic_AquireLock(&pGC->Busy);
    GCObject* object = NULL;
    if (pSize <= GCHeap__SmallHeap_Size - sizeof(GCObject*))
        object = GCHeap_Allocate(&pGC->SmallGeneration0Heaps, &pGC->SmallGeneration0HeapCount, GCHeap__SmallHeap_Size, pSize);
    else if (pSize <= GCHeap__LargeHeap_Size - sizeof(GCObject*))
        object = GCHeap_Allocate(&pGC->LargeHeaps, &pGC->LargeHeapCount, GCHeap__LargeHeap_Size, pSize);
    else object = GCHeap_Allocate(&pGC->LargeHeaps, &pGC->LargeHeapCount, pSize + sizeof(GCObject*), pSize);
	object->Type = pType;
	object->Flags |= GCObjectFlags_Pinned;
	Atomic_ReleaseLock(&pGC->Busy);
	return object;
}
