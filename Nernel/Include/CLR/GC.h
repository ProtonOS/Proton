#pragma once

#include <CLR/ReferenceTypeObject.h>

#define GC_Generation0_CollectCount                 25
#define GC_Generation1_CollectCount                 200

#define GCHeapStack_SmallHeap_Size                  (4 * 1024)
#define GCHeapStack_LargeHeap_Size                  (GCHeapStack_SmallHeap_Size * 1024)

#define GCHeap_SmallHeap_MinimumStacks              1
#define GCHeap_LargeHeap_MinimumStacks              1


#define GC_SmallHeap_MaxAllocationSize              (GCHeapStack_SmallHeap_Size - sizeof(ReferenceTypeObject))

typedef struct
{
    uint32_t Size;
    uint8_t* Bottom;
    uint8_t* Top;
} GCHeapStack;

typedef struct
{
    uint32_t StackCount;
    GCHeapStack** Stacks;
} GCHeap;

typedef struct
{
    ReferenceTypeObject* Root;
    uint32_t SmallGeneration0CollectCount;
    uint32_t SmallGeneration1CollectCount;
    GCHeap SmallGeneration0Heap;
    GCHeap SmallGeneration1Heap;
    GCHeap SmallGeneration2Heap;
    GCHeap LargeHeap;
} GC;

void Panic(const char* pMessage);

GC* GC_Create(ReferenceTypeObject* pRoot);
void GC_Destroy(GC* pGC);
ReferenceTypeObject* GC_Allocate(GC* pGC, uint32_t pSize);
void GC_Collect(GC* pGC);