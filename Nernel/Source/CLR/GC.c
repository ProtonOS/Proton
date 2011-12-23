#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <CLR/GC.h>

GC* GC_Create(ReferenceTypeObject* pRoot)
{
    if (!pRoot) Panic("GC_Create pRoot == NULL");

    GC* gc = (GC*)calloc(1, sizeof(GC));
    gc->Root = pRoot;
    gc->SmallGeneration0Heap.StackCount = GCHeap_SmallHeap_MinimumStacks;
    gc->SmallGeneration1Heap.StackCount = GCHeap_SmallHeap_MinimumStacks;
    gc->SmallGeneration2Heap.StackCount = GCHeap_SmallHeap_MinimumStacks;
    gc->LargeHeap.StackCount = GCHeap_LargeHeap_MinimumStacks;

    gc->SmallGeneration0Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration0Heap.StackCount, sizeof(GCHeapStack*));
    gc->SmallGeneration1Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration1Heap.StackCount, sizeof(GCHeapStack*));
    gc->SmallGeneration2Heap.Stacks = (GCHeapStack**)calloc(gc->SmallGeneration2Heap.StackCount, sizeof(GCHeapStack*));
    gc->LargeHeap.Stacks = (GCHeapStack**)calloc(gc->LargeHeap.StackCount, sizeof(GCHeapStack*));

    for (uint32_t index = 0; index < GCHeap_SmallHeap_MinimumStacks; ++index)
    {
        gc->SmallGeneration0Heap.Stacks[index] = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));
        gc->SmallGeneration1Heap.Stacks[index] = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));
        gc->SmallGeneration2Heap.Stacks[index] = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));

        gc->SmallGeneration0Heap.Stacks[index]->Size = GCHeapStack_SmallHeap_Size;
        gc->SmallGeneration1Heap.Stacks[index]->Size = GCHeapStack_SmallHeap_Size;
        gc->SmallGeneration2Heap.Stacks[index]->Size = GCHeapStack_SmallHeap_Size;

        gc->SmallGeneration0Heap.Stacks[index]->Bottom = (uint8_t*)calloc(1, gc->SmallGeneration0Heap.Stacks[index]->Size);
        gc->SmallGeneration1Heap.Stacks[index]->Bottom = (uint8_t*)calloc(1, gc->SmallGeneration1Heap.Stacks[index]->Size);
        gc->SmallGeneration2Heap.Stacks[index]->Bottom = (uint8_t*)calloc(1, gc->SmallGeneration2Heap.Stacks[index]->Size);

        gc->SmallGeneration0Heap.Stacks[index]->Top = gc->SmallGeneration0Heap.Stacks[index]->Bottom;
        gc->SmallGeneration1Heap.Stacks[index]->Top = gc->SmallGeneration1Heap.Stacks[index]->Bottom;
        gc->SmallGeneration2Heap.Stacks[index]->Top = gc->SmallGeneration2Heap.Stacks[index]->Bottom;

    }
    for (uint32_t index = 0; index < GCHeap_LargeHeap_MinimumStacks; ++index)
    {
        gc->LargeHeap.Stacks[index] = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));
        gc->LargeHeap.Stacks[index]->Size = GCHeapStack_LargeHeap_Size;
        gc->LargeHeap.Stacks[index]->Bottom = (uint8_t*)calloc(1, gc->LargeHeap.Stacks[index]->Size);
        gc->LargeHeap.Stacks[index]->Top = gc->LargeHeap.Stacks[index]->Bottom;
    }
}

void GC_Destroy(GC* pGC)
{
    for (uint32_t index = 0; index < pGC->LargeHeap.StackCount; ++index)
    {
        free(pGC->LargeHeap.Stacks[index]->Bottom);
        free(pGC->LargeHeap.Stacks[index]);
    }
    for (uint32_t index = 0; index < pGC->SmallGeneration2Heap.StackCount; ++index)
    {
        free(pGC->SmallGeneration2Heap.Stacks[index]->Bottom);
        free(pGC->SmallGeneration2Heap.Stacks[index]);
    }
    for (uint32_t index = 0; index < pGC->SmallGeneration1Heap.StackCount; ++index)
    {
        free(pGC->SmallGeneration1Heap.Stacks[index]->Bottom);
        free(pGC->SmallGeneration1Heap.Stacks[index]);
    }
    for (uint32_t index = 0; index < pGC->SmallGeneration0Heap.StackCount; ++index)
    {
        free(pGC->SmallGeneration0Heap.Stacks[index]->Bottom);
        free(pGC->SmallGeneration0Heap.Stacks[index]);
    }
    free (pGC->LargeHeap.Stacks);
    free (pGC->SmallGeneration2Heap.Stacks);
    free (pGC->SmallGeneration1Heap.Stacks);
    free (pGC->SmallGeneration0Heap.Stacks);
    free(pGC);
}

ReferenceTypeObject* GCHeap_Allocate(GCHeap* pGCHeap, uint32_t pStackSize, uint32_t pSize, uint32_t pMinimumRequired, uint32_t pMaximumRequired)
{
    ReferenceTypeObject* object = NULL;
    GCHeapStack* stack = NULL; // Maybe tack on register prefix for optimization?
    for (uint32_t index = 0; !object && index < pGCHeap->StackCount; ++index)
    {
        // Let's cache the stack pointer, this should be faster
        stack = pGCHeap->Stacks[index];
        // Let's find out how much is available in this stack
        uint32_t available = stack->Size - (stack->Top - stack->Bottom);
        // If there is enough to store minimum required, that's a start
        if (available >= pMinimumRequired)
        {
            // This little bit of magic needs explanation, if the amount available is exactly
            // what we need, then we will use up the rest of this stack, otherwise we will only
            // use it if the amount available is enough to contain at least another 1 byte object
            if (available == pMinimumRequired || available >= pMaximumRequired)
            {
                // The available memory matched our criteria, so let's use it
                object = (ReferenceTypeObject*)stack->Top;
                // Set the object active, this should flag should be removed when you want GC
                // to finalize this object, and compact the memory
                object->Flags = ReferenceTypeObject_Flags_Active;
                object->ReferenceCount = 1; // Set initial reference count
                object->Size = pSize; // Set object size, object data starts after this header

                // Now, even though we may have used maximumRequired to get here we only need
                // minimumRequired bytes, just wanted to ensure that if we were splitting this
                // stack that we had enough space for another entry to use the rest, so now we
                // adjust the top of the stack for the minimumRequired
                stack->Top += pMinimumRequired;
            }
        }
    }
    if (!object)
    {
        // If we get here, there was no current stack that could hold the object, or the optimizations
        // forced us to skip a stack that it might have fit in with wasted memory at the end, so now
        // we should allocate a new stack

        // Increment the StackCount, as we need a new one
        ++pGCHeap->StackCount;
        // Reallocate the Stacks array, to accomadate the new stack pointer at the end
        pGCHeap->Stacks = (GCHeapStack**)realloc(pGCHeap->Stacks, sizeof(GCHeapStack*) * pGCHeap->StackCount);
        // Allocate the new GCHeapStack and assign it to the last spot of the Stacks array, reallocated
        // above, which ensures we have space for the pointer at StackCount - 1
        pGCHeap->Stacks[pGCHeap->StackCount - 1] = (GCHeapStack*)calloc(1, sizeof(GCHeapStack));
        // Let's cache that stack pointer again, it's even longer this time
        stack = pGCHeap->Stacks[pGCHeap->StackCount - 1];
        // Let's finish initializing the new stack, set the size for the stack
        stack->Size = pStackSize;
        // Allocate the memory for the stack
        stack->Bottom = (uint8_t*)calloc(1, stack->Size);
        // Initialize the Top of the stack to the Bottom, so it is empty
        stack->Top = stack->Bottom;

        // This looks familiar, object construction again, see the above loop for details
        object = (ReferenceTypeObject*)stack->Top;
        object->Flags = ReferenceTypeObject_Flags_Active;
        object->ReferenceCount = 1;
        object->Size = pSize;
        stack->Top += pMinimumRequired;
    }
}

ReferenceTypeObject* GC_Allocate(GC* pGC, uint32_t pSize)
{
    // Calculate minumumRequired and maximumRequired, these determine some memory optimizations
    uint32_t minimumRequired = sizeof(ReferenceTypeObject) + pSize;
    uint32_t maximumRequired = minimumRequired + (sizeof(ReferenceTypeObject) + 1);
    // Let's do some sanity checking, we can remove this when we know nothing crazy is happening
    if (minimumRequired >= 0x7FFFFFFF || maximumRequired >= 0x7FFFFFFF) Panic("GC_Allocate minimumRequired or maximumRequired >= 0x7FFFFFFF");
    // If the maximumRequired fits in 1 whole SmallHeap stack, then it's a SmallHeap allocation
    if (maximumRequired <= GCHeapStack_SmallHeap_Size) return GCHeap_Allocate(&pGC->SmallGeneration0Heap, GCHeapStack_SmallHeap_Size, pSize, minimumRequired, maximumRequired);
    // If the maximumRequired fits in 1 whole LargeHeap stack, then it's a standard LargeHeap allocation
    if (maximumRequired <= GCHeapStack_LargeHeap_Size) return GCHeap_Allocate(&pGC->LargeHeap, GCHeapStack_LargeHeap_Size, pSize, minimumRequired, maximumRequired);
    // If the maximumRequired hasn't fit yet, then it's a custom LargeHeap allocation
    return GCHeap_Allocate(&pGC->LargeHeap, minimumRequired, pSize, minimumRequired, maximumRequired);
}

void GC_Collect(GC* pGC)
{
}

