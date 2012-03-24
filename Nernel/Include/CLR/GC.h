#pragma once

typedef struct _GCString GCString;
typedef struct _GCArray GCArray;
typedef struct _GC GC;
typedef struct _GCHeap GCHeap;
typedef struct _GCHeapStack GCHeapStack;

#include <uthash.h>
#include <CLR/ReferenceTypeObject.h>
struct _GCString
{
	uint32_t Size;
	uint8_t* Data;
	ReferenceTypeObject* Object;
	UT_hash_handle HashHandle;
};

#include <CLR/AppDomain.h>

#define GC_Generation0ToGeneration1_RequiredAge         32
#define GC_Generation1ToGeneration2_RequiredAge         128

#define GC_CollectCounterDelay							16

#define GCHeapStack_SmallHeap_Size                      (4 * 1024)
#define GCHeapStack_LargeHeap_Size                      (GCHeapStack_SmallHeap_Size * 1024)

#define GCHeapStack_SmallHeap_InitialPoolSize           32
#define GCHeapStack_LargeHeap_InitialPoolSize           4


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
	AppDomain* Domain;
    GCHeap SmallGeneration0Heap;
    GCHeap SmallGeneration1Heap;
    GCHeap SmallGeneration2Heap;
    GCHeap LargeHeap;
	GCString* StringHashTable;
	uint32_t CollectCounter;
};


struct _GCArray
{
	uint32_t Length;
	IRType* Type;
};

void Panic(const char* pMessage);

GC* GC_Create(AppDomain* pAppDomain);
void GC_Destroy(GC* pGC);
ReferenceTypeObject* GC_AllocateObject(GC* pGC, ReferenceTypeObject* pInitialReference, uint32_t pSize, IRType* pType);
ReferenceTypeObject* GC_AllocateString(GC* pGC, ReferenceTypeObject* pInitialReference, uint8_t* pData, uint32_t pSize);
ReferenceTypeObject* GC_AllocateStringFromCharAndCount(GC* pGC, ReferenceTypeObject* pInitialReference, uint16_t pChar, uint32_t pCount);
ReferenceTypeObject* GC_ConcatenateStrings(GC* pGC, ReferenceTypeObject* pInitialReference, ReferenceTypeObject* pString1, ReferenceTypeObject* pString2);
ReferenceTypeObject* GC_SubstituteString(GC* pGC, ReferenceTypeObject* pInitialReference, ReferenceTypeObject* pSource, ReferenceTypeObject* pPattern, ReferenceTypeObject* pSubstitute);
ReferenceTypeObject* GC_AllocateArray(GC* pGC, ReferenceTypeObject* pInitialReference, uint32_t pLength, uint32_t pElementSize, IRType* pType);
void GC_Collect(GC* pGC);
