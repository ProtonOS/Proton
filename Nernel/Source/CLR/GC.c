#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <CLR/GC.h>

GCHeapStack* GCHeapStack_Create(uint32_t pSize)
{
    GCHeapStack* stack = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));
    stack->ObjectPoolSize = 1;
    stack->ObjectPool = (ReferenceTypeObject**)calloc(stack->ObjectPoolSize, sizeof(ReferenceTypeObject*));
    stack->ObjectPool[0] = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
    stack->Size = pSize;
    stack->Available = stack->Size;
    stack->Bottom = (uint8_t*)calloc(1, stack->Size);
    stack->Top = stack->Bottom;
    return stack;
}

void GCHeapStack_Destroy(GCHeapStack* pGCHeapStack)
{
    for (uint32_t index = 0; index < pGCHeapStack->ObjectPoolSize; ++index)
    {
        free (pGCHeapStack->Bottom);
        if (pGCHeapStack->ObjectPool[index]->DependancyPool)
            free(pGCHeapStack->ObjectPool[index]->DependancyPool);
        free (pGCHeapStack->ObjectPool[index]);
    }
    free (pGCHeapStack->ObjectPool);
    free(pGCHeapStack);
}

GC* GC_Create()
{
    GC* gc = (GC*)calloc(1, sizeof(GC));
    gc->SmallGeneration0Heap.StackCount = 1;
    gc->SmallGeneration1Heap.StackCount = 1;
    gc->SmallGeneration2Heap.StackCount = 1;
    gc->LargeHeap.StackCount = 1;

    gc->SmallGeneration0Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration0Heap.StackCount, sizeof(GCHeapStack*));
    gc->SmallGeneration1Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration1Heap.StackCount, sizeof(GCHeapStack*));
    gc->SmallGeneration2Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration2Heap.StackCount, sizeof(GCHeapStack*));
    gc->LargeHeap.Stacks = (GCHeapStack**)calloc(gc->LargeHeap.StackCount, sizeof(GCHeapStack*));

    gc->SmallGeneration0Heap.Stacks[0] = GCHeapStack_Create(GCHeapStack_SmallHeap_Size);
    gc->SmallGeneration1Heap.Stacks[0] = GCHeapStack_Create(GCHeapStack_SmallHeap_Size);
    gc->SmallGeneration2Heap.Stacks[0] = GCHeapStack_Create(GCHeapStack_SmallHeap_Size);
    gc->LargeHeap.Stacks[0] = GCHeapStack_Create(GCHeapStack_LargeHeap_Size);

    return gc;
}

void GC_Destroy(GC* pGC)
{
    for (uint32_t index = 0; index < pGC->LargeHeap.StackCount; ++index)
        GCHeapStack_Destroy(pGC->LargeHeap.Stacks[index]);
    for (uint32_t index = 0; index < pGC->SmallGeneration2Heap.StackCount; ++index)
        GCHeapStack_Destroy(pGC->SmallGeneration2Heap.Stacks[index]);
    for (uint32_t index = 0; index < pGC->SmallGeneration1Heap.StackCount; ++index)
        GCHeapStack_Destroy(pGC->SmallGeneration1Heap.Stacks[index]);
    for (uint32_t index = 0; index < pGC->SmallGeneration0Heap.StackCount; ++index)
        GCHeapStack_Destroy(pGC->SmallGeneration0Heap.Stacks[index]);
    free (pGC->LargeHeap.Stacks);
    free (pGC->SmallGeneration2Heap.Stacks);
    free (pGC->SmallGeneration1Heap.Stacks);
    free (pGC->SmallGeneration0Heap.Stacks);
    free(pGC);
}

ReferenceTypeObject* GCHeap_Allocate(GCHeap* pGCHeap, uint32_t pStackSize, uint32_t pSize)
{
    ReferenceTypeObject* object = NULL;
    GCHeapStack* stack = NULL;
    for (uint32_t index = 0; !object && index < pGCHeap->StackCount; ++index)
    {
        stack = pGCHeap->Stacks[index];
        if (stack->Available >= pSize)
        {
            for (uint32_t objectIndex = 0; !object && objectIndex < stack->ObjectPoolSize; ++objectIndex)
                if (!stack->ObjectPool[objectIndex]->Object)
                    object = stack->ObjectPool[objectIndex];
            if (!object)
            {
                stack->ObjectPoolSize <<= 1;
                stack->ObjectPool = (ReferenceTypeObject**)realloc(stack->ObjectPool, sizeof(ReferenceTypeObject*) * stack->ObjectPoolSize);
                for (uint32_t objectIndex = stack->ObjectPoolSize >> 1; objectIndex < stack->ObjectPoolSize; ++objectIndex)
                    stack->ObjectPool[objectIndex] = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
                object = stack->ObjectPool[stack->ObjectPoolSize >> 1];
            }
            object->Flags = ReferenceTypeObject_Flags_Allocated;
            object->Size = pSize;
            object->Object = stack->Top;
            object->Stack = stack;
            stack->Available -= pSize;
            stack->Allocated += pSize;
            stack->Top += pSize;
        }
    }
    if (!object)
    {
        ++pGCHeap->StackCount;
        pGCHeap->Stacks = (GCHeapStack**)realloc(pGCHeap->Stacks, sizeof(GCHeapStack*) * pGCHeap->StackCount);
        pGCHeap->Stacks[pGCHeap->StackCount - 1] = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));
        stack = pGCHeap->Stacks[pGCHeap->StackCount - 1];
        stack->ObjectPoolSize = 1;
        stack->ObjectPool = (ReferenceTypeObject**)calloc(stack->ObjectPoolSize, sizeof(ReferenceTypeObject*));
        stack->ObjectPool[0] = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
        stack->Size = pStackSize;
        stack->Available = stack->Size;
        stack->Bottom = (uint8_t*)calloc(1, stack->Size);
        stack->Top = stack->Bottom;

        object = stack->ObjectPool[0];
        object->Flags = ReferenceTypeObject_Flags_Allocated;
        object->Size = pSize;
        object->Object = stack->Top;
        object->Stack = stack;
        stack->Available -= pSize;
        stack->Allocated += pSize;
        stack->Top += pSize;
    }
    return object;
}

ReferenceTypeObject* GC_Allocate(GC* pGC, ReferenceTypeObject* pInitialReference, uint32_t pSize)
{
    if (!pInitialReference) Panic("GC_Allocate pInitialReference == NULL");
    if (pSize == 0 || pSize >= 0x7FFFFFFF) Panic("GC_Allocate pSize == 0 || pSize >= 0x7FFFFFFF");
    ReferenceTypeObject* object = NULL;
    if (pSize <= GCHeapStack_SmallHeap_Size)
        object = GCHeap_Allocate(&pGC->SmallGeneration0Heap, GCHeapStack_SmallHeap_Size, pSize);
    else if (pSize <= GCHeapStack_LargeHeap_Size)
        object = GCHeap_Allocate(&pGC->LargeHeap, GCHeapStack_LargeHeap_Size, pSize);
    else object = GCHeap_Allocate(&pGC->LargeHeap, pSize, pSize);
    ReferenceTypeObject_AddReference(pInitialReference, object);
    return object;
}

void GCHeap_Collect(GCHeap* pGCHeap)
{
    ReferenceTypeObject* object = NULL;
    GCHeapStack* stack = NULL;
    for (uint32_t index = 0; index < pGCHeap->StackCount; ++index)
    {
        stack = pGCHeap->Stacks[index];
        for (uint32_t objectIndex = 0; objectIndex < stack->ObjectPoolSize; ++objectIndex)
        {
            if ((stack->ObjectPool[objectIndex]->Flags & ReferenceTypeObject_Flags_Disposing) != 0)
            {
                object = stack->ObjectPool[objectIndex];
                ReferenceTypeObject_Dispose(object);
            }
        }
    }
}

void GCHeap_Compact(GCHeap* pGCHeap)
{
    GCHeapStack* stack = NULL;
    for (uint32_t index = 0; index < pGCHeap->StackCount; ++index)
    {
        stack = pGCHeap->Stacks[index];
        if (stack->Disposed >= (stack->Size / 4))
        {
            uint32_t allocatedCount = 0;
            for (uint32_t objectIndex = 0, startIndex = 0; objectIndex < stack->ObjectPoolSize; ++objectIndex)
            {
                if ((stack->ObjectPool[objectIndex]->Flags & ReferenceTypeObject_Flags_Allocated) != 0)
                {
                    ++allocatedCount;
                    for (uint32_t emptyIndex = startIndex; emptyIndex < objectIndex; ++emptyIndex)
                    {
                        if ((stack->ObjectPool[emptyIndex]->Flags & ReferenceTypeObject_Flags_Allocated) == 0)
                        {
                            ReferenceTypeObject* tempObject = stack->ObjectPool[objectIndex];
                            stack->ObjectPool[objectIndex] = stack->ObjectPool[emptyIndex];
                            stack->ObjectPool[emptyIndex] = tempObject;
                            startIndex = emptyIndex + 1;
                        }
                    }
                }
            }
            for (uint32_t objectIndex = 0; objectIndex < allocatedCount; ++objectIndex)
            {
                if (objectIndex > 0 && stack->ObjectPool[objectIndex]->Object < stack->ObjectPool[objectIndex - 1]->Object)
                {
                    ReferenceTypeObject* tempObject = stack->ObjectPool[objectIndex];
                    stack->ObjectPool[objectIndex] = stack->ObjectPool[objectIndex - 1];
                    stack->ObjectPool[objectIndex - 1] = tempObject;
                }
            }
            for (uint32_t objectIndex = 0; objectIndex < allocatedCount; ++objectIndex)
            {
                if ((stack->ObjectPool[objectIndex]->Flags & ReferenceTypeObject_Flags_Disposed) != 0)
                {
                    while (objectIndex < (allocatedCount - 1) && (stack->ObjectPool[objectIndex + 1]->Flags & ReferenceTypeObject_Flags_Disposed) != 0)
                    {
                        stack->ObjectPool[objectIndex]->Size += stack->ObjectPool[objectIndex + 1]->Size;
                        ReferenceTypeObject_Reset(stack->ObjectPool[objectIndex + 1]);

                        for (uint32_t shiftIndex = objectIndex + 2; shiftIndex < allocatedCount; ++shiftIndex)
                        {
                            ReferenceTypeObject* tempObject = stack->ObjectPool[shiftIndex];
                            stack->ObjectPool[shiftIndex] = stack->ObjectPool[shiftIndex - 1];
                            stack->ObjectPool[shiftIndex - 1] = tempObject;
                        }
                        --allocatedCount;
                    }

                    if (objectIndex == (allocatedCount - 1))
                    {
                        stack->Top -= stack->ObjectPool[objectIndex]->Size;
                        stack->Disposed -= stack->ObjectPool[objectIndex]->Size;
                        stack->Available += stack->ObjectPool[objectIndex]->Size;

                        ReferenceTypeObject_Reset(stack->ObjectPool[objectIndex]);
                    }
                    else
                    {
                        ReferenceTypeObject* tempObject = stack->ObjectPool[objectIndex];
                        stack->ObjectPool[objectIndex] = stack->ObjectPool[objectIndex + 1];
                        stack->ObjectPool[objectIndex + 1] = tempObject;

                        memcpy(stack->ObjectPool[objectIndex + 1]->Object, stack->ObjectPool[objectIndex]->Object, stack->ObjectPool[objectIndex]->Size);

                        stack->ObjectPool[objectIndex]->Object = stack->ObjectPool[objectIndex + 1]->Object;
                        stack->ObjectPool[objectIndex + 1]->Object = stack->ObjectPool[objectIndex]->Object + stack->ObjectPool[objectIndex]->Size;
                    }
                }
            }
        }
    }
}

void GC_Collect(GC* pGC)
{
    GCHeap_Collect(&pGC->SmallGeneration0Heap);
    GCHeap_Collect(&pGC->SmallGeneration1Heap);
    GCHeap_Collect(&pGC->SmallGeneration2Heap);
    GCHeap_Collect(&pGC->LargeHeap);

    GCHeap_Compact(&pGC->SmallGeneration0Heap);
    GCHeap_Compact(&pGC->SmallGeneration1Heap);
    GCHeap_Compact(&pGC->SmallGeneration2Heap);
    GCHeap_Compact(&pGC->LargeHeap);

    ++pGC->SmallGeneration1CollectCount;
    if (pGC->SmallGeneration1CollectCount == GC_Generation1_CollectCount)
    {
        pGC->SmallGeneration1CollectCount = 0;
        // Migrate objects in SmallGeneration1Heap to SmallGeneration2Heap
    }
    ++pGC->SmallGeneration0CollectCount;
    if (pGC->SmallGeneration0CollectCount == GC_Generation0_CollectCount)
    {
        pGC->SmallGeneration0CollectCount = 0;
        // Migrate objects in SmallGeneration0Heap to SmallGeneration1Heap
    }
}

