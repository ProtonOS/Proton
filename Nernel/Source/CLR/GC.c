#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <CLR/GC.h>

GC* GC_Create(ReferenceTypeObject* pRoot)
{
    if (!pRoot) Panic("GC_Create pRoot == NULL");

    GC* gc = (GC*)calloc(1, sizeof(GC));
    gc->Root = pRoot;
    gc->SmallGeneration0Heap.StackCount = 1;
    gc->SmallGeneration1Heap.StackCount = 1;
    gc->SmallGeneration2Heap.StackCount = 1;
    gc->LargeHeap.StackCount = 1;

    gc->SmallGeneration0Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration0Heap.StackCount, sizeof(GCHeapStack*));
    gc->SmallGeneration1Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration1Heap.StackCount, sizeof(GCHeapStack*));
    gc->SmallGeneration2Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration2Heap.StackCount, sizeof(GCHeapStack*));
    gc->LargeHeap.Stacks = (GCHeapStack**)calloc(gc->LargeHeap.StackCount, sizeof(GCHeapStack*));

    gc->SmallGeneration0Heap.Stacks[0] = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));
    gc->SmallGeneration1Heap.Stacks[0] = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));
    gc->SmallGeneration2Heap.Stacks[0] = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));
    gc->LargeHeap.Stacks[0] = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));

    gc->SmallGeneration0Heap.Stacks[0]->ObjectPoolSize = 1;
    gc->SmallGeneration1Heap.Stacks[0]->ObjectPoolSize = 1;
    gc->SmallGeneration2Heap.Stacks[0]->ObjectPoolSize = 1;
    gc->LargeHeap.Stacks[0]->ObjectPoolSize = 1;

    gc->SmallGeneration0Heap.Stacks[0]->ObjectPool = (ReferenceTypeObject**)calloc(gc->SmallGeneration0Heap.Stacks[0]->ObjectPoolSize, sizeof(ReferenceTypeObject*));
    gc->SmallGeneration1Heap.Stacks[0]->ObjectPool = (ReferenceTypeObject**)calloc(gc->SmallGeneration1Heap.Stacks[0]->ObjectPoolSize, sizeof(ReferenceTypeObject*));
    gc->SmallGeneration2Heap.Stacks[0]->ObjectPool = (ReferenceTypeObject**)calloc(gc->SmallGeneration2Heap.Stacks[0]->ObjectPoolSize, sizeof(ReferenceTypeObject*));
    gc->LargeHeap.Stacks[0]->ObjectPool = (ReferenceTypeObject**)calloc(gc->LargeHeap.Stacks[0]->ObjectPoolSize, sizeof(ReferenceTypeObject*));

    gc->SmallGeneration0Heap.Stacks[0]->ObjectPool[0] = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
    gc->SmallGeneration1Heap.Stacks[0]->ObjectPool[0] = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
    gc->SmallGeneration2Heap.Stacks[0]->ObjectPool[0] = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
    gc->LargeHeap.Stacks[0]->ObjectPool[0] = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));

    gc->SmallGeneration0Heap.Stacks[0]->Size = GCHeapStack_SmallHeap_Size;
    gc->SmallGeneration1Heap.Stacks[0]->Size = GCHeapStack_SmallHeap_Size;
    gc->SmallGeneration2Heap.Stacks[0]->Size = GCHeapStack_SmallHeap_Size;
    gc->LargeHeap.Stacks[0]->Size = GCHeapStack_LargeHeap_Size;

    gc->SmallGeneration0Heap.Stacks[0]->Available = gc->SmallGeneration0Heap.Stacks[0]->Size;
    gc->SmallGeneration1Heap.Stacks[0]->Available = gc->SmallGeneration1Heap.Stacks[0]->Size;
    gc->SmallGeneration2Heap.Stacks[0]->Available = gc->SmallGeneration2Heap.Stacks[0]->Size;
    gc->LargeHeap.Stacks[0]->Available = gc->LargeHeap.Stacks[0]->Size;

    gc->SmallGeneration0Heap.Stacks[0]->Bottom = (uint8_t*)calloc(1, gc->SmallGeneration0Heap.Stacks[0]->Size);
    gc->SmallGeneration1Heap.Stacks[0]->Bottom = (uint8_t*)calloc(1, gc->SmallGeneration1Heap.Stacks[0]->Size);
    gc->SmallGeneration2Heap.Stacks[0]->Bottom = (uint8_t*)calloc(1, gc->SmallGeneration2Heap.Stacks[0]->Size);
    gc->LargeHeap.Stacks[0]->Bottom = (uint8_t*)calloc(1, gc->LargeHeap.Stacks[0]->Size);

    gc->SmallGeneration0Heap.Stacks[0]->Top = gc->SmallGeneration0Heap.Stacks[0]->Bottom;
    gc->SmallGeneration1Heap.Stacks[0]->Top = gc->SmallGeneration1Heap.Stacks[0]->Bottom;
    gc->SmallGeneration2Heap.Stacks[0]->Top = gc->SmallGeneration2Heap.Stacks[0]->Bottom;
    gc->LargeHeap.Stacks[0]->Top = gc->LargeHeap.Stacks[0]->Bottom;

    return gc;
}

void GC_Destroy(GC* pGC)
{
    for (uint32_t index = 0; index < pGC->LargeHeap.StackCount; ++index)
    {
        free(pGC->LargeHeap.Stacks[index]->Bottom);
        for (uint32_t objectIndex = 0; objectIndex < pGC->LargeHeap.Stacks[index]->ObjectPoolSize; ++objectIndex) free(pGC->LargeHeap.Stacks[index]->ObjectPool[objectIndex]);
        free(pGC->LargeHeap.Stacks[index]->ObjectPool);
        free(pGC->LargeHeap.Stacks[index]);
    }
    for (uint32_t index = 0; index < pGC->SmallGeneration2Heap.StackCount; ++index)
    {
        free(pGC->SmallGeneration2Heap.Stacks[index]->Bottom);
        for (uint32_t objectIndex = 0; objectIndex < pGC->SmallGeneration2Heap.Stacks[index]->ObjectPoolSize; ++objectIndex) free(pGC->SmallGeneration2Heap.Stacks[index]->ObjectPool[objectIndex]);
        free(pGC->SmallGeneration2Heap.Stacks[index]->ObjectPool);
        free(pGC->SmallGeneration2Heap.Stacks[index]);
    }
    for (uint32_t index = 0; index < pGC->SmallGeneration1Heap.StackCount; ++index)
    {
        free(pGC->SmallGeneration1Heap.Stacks[index]->Bottom);
        for (uint32_t objectIndex = 0; objectIndex < pGC->SmallGeneration1Heap.Stacks[index]->ObjectPoolSize; ++objectIndex) free(pGC->SmallGeneration1Heap.Stacks[index]->ObjectPool[objectIndex]);
        free(pGC->SmallGeneration1Heap.Stacks[index]->ObjectPool);
        free(pGC->SmallGeneration1Heap.Stacks[index]);
    }
    for (uint32_t index = 0; index < pGC->SmallGeneration0Heap.StackCount; ++index)
    {
        free(pGC->SmallGeneration0Heap.Stacks[index]->Bottom);
        for (uint32_t objectIndex = 0; objectIndex < pGC->SmallGeneration0Heap.Stacks[index]->ObjectPoolSize; ++objectIndex) free(pGC->SmallGeneration0Heap.Stacks[index]->ObjectPool[objectIndex]);
        free(pGC->SmallGeneration0Heap.Stacks[index]->ObjectPool);
        free(pGC->SmallGeneration0Heap.Stacks[index]);
    }
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
            for (uint32_t objectIndex = 0; !object && objectIndex < stack->ObjectPoolSize; ++objectIndex) if (!stack->ObjectPool[objectIndex]->Object) object = stack->ObjectPool[objectIndex];
            if (!object)
            {
                stack->ObjectPoolSize <<= 1;
                stack->ObjectPool = (ReferenceTypeObject**)realloc(stack->ObjectPool, sizeof(ReferenceTypeObject*) * stack->ObjectPoolSize);
                for (uint32_t objectIndex = stack->ObjectPoolSize >> 1; objectIndex < stack->ObjectPoolSize; ++objectIndex) stack->ObjectPool[objectIndex] = (ReferenceTypeObject*)calloc(1, sizeof(ReferenceTypeObject));
                object = stack->ObjectPool[stack->ObjectPoolSize >> 1];
            }
            object->Flags = ReferenceTypeObject_Flags_Active;
            object->ReferenceCount = 1;
            object->Size = pSize;
            object->Object = stack->Top;
            stack->Available -= pSize;
            stack->Active += pSize;
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
        object->Flags = ReferenceTypeObject_Flags_Active;
        object->ReferenceCount = 1;
        object->Size = pSize;
        object->Object = stack->Top;
        stack->Available -= pSize;
        stack->Active += pSize;
        stack->Top += pSize;
    }
    return object;
}

ReferenceTypeObject* GC_Allocate(GC* pGC, uint32_t pSize)
{
    if (pSize == 0 || pSize >= 0x7FFFFFFF) Panic("GC_Allocate pSize == 0 || pSize >= 0x7FFFFFFF");
    if (pSize <= GCHeapStack_SmallHeap_Size) return GCHeap_Allocate(&pGC->SmallGeneration0Heap, GCHeapStack_SmallHeap_Size, pSize);
    if (pSize <= GCHeapStack_LargeHeap_Size) return GCHeap_Allocate(&pGC->LargeHeap, GCHeapStack_LargeHeap_Size, pSize);
    return GCHeap_Allocate(&pGC->LargeHeap, pSize, pSize);
}

void GC_Collect(GC* pGC)
{
}

