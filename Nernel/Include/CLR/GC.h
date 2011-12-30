#pragma once

#include <CLR/ReferenceTypeObject.h>

#define GC_Generation0ToGeneration1_RequiredAge         25
#define GC_Generation1ToGeneration2_RequiredAge         200

#define GCHeapStack_SmallHeap_Size                      (4 * 1024)
#define GCHeapStack_LargeHeap_Size                      (GCHeapStack_SmallHeap_Size * 1024)

typedef struct _GC GC;
typedef struct _GCHeap GCHeap;
typedef struct _GCHeapStack GCHeapStack;

struct _GCHeapStack
{
    uint32_t ObjectPoolSize;
    ReferenceTypeObject** ObjectPool;
    uint32_t Size;
    uint32_t Available;
    uint32_t Allocated;
    uint32_t Disposed;
    uint8_t* Bottom;
    uint8_t* Top;
};

struct _GCHeap
{
    uint32_t StackCount;
    GCHeapStack** Stacks;
};

struct _GC
{
    GCHeap SmallGeneration0Heap;
    GCHeap SmallGeneration1Heap;
    GCHeap SmallGeneration2Heap;
    GCHeap LargeHeap;
};

void Panic(const char* pMessage);

GC* GC_Create();
void GC_Destroy(GC* pGC);
ReferenceTypeObject* GC_Allocate(GC* pGC, ReferenceTypeObject* pInitialReference, uint32_t pSize);
void GC_Collect(GC* pGC);
