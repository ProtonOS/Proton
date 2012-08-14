#pragma once

typedef struct _GCObject GCObject;
typedef struct _GCHeap GCHeap;
typedef struct _GC GC;

#include <CLR/AppDomain.h>
#include <CLR/IRStructures.h>

typedef enum GCObjectFlags
{
	GCObjectFlags_None = 0 << 0,
	GCObjectFlags_String = 1 << 0,
	GCObjectFlags_Array = 1 << 1,

	GCObjectFlags_Pinned = 1 << 4,
	GCObjectFlags_Allocated = 1 << 5,
	GCObjectFlags_Marked = 1 << 6,
	GCObjectFlags_Interned = 1 << 7
} GCObjectFlags;

#define GCHeap__SmallHeap_Size							(128 * 1024)
#define GCHeap__SmallHeap_InitialPoolSize				32

#define GCHeap__LargeHeap_Size							(GCHeap__SmallHeap_Size * 4)
#define GCHeap__LargeHeap_InitialPoolSize				1

#define GC__PressureTriggerInBytes						(1024 * 1024 * 4)

struct _GCObject
{
	IRType* Type;
	GCObjectFlags Flags;
	void* Data;
	size_t AllocatedSize;
	size_t ActualSize;
	GCHeap* Heap;
	GCObject* NextObject;

	union
	{
		struct String
		{
			UT_hash_handle HashHandle;
		} String;
		struct Array
		{
			uint32_t Length;
			IRType* ElementType;
		} Array;
	};
};

struct _GCHeap
{
	uint32_t InitialPoolSize;
	GCObject* AllocatedObjectList;
	GCObject* FreeObjectList;
	uint32_t* AllocationTree;
	uint32_t AllocationTreeSize;
	uint32_t AllocationTreeLevels;
    uint32_t Size;
    uint32_t Available;
    uint32_t Allocated;
    uint8_t* Bottom;
};

struct _GC
{
	AppDomain* Domain;
	uint8_t Busy;
	uint32_t Pressure;
	GCObject* StringHashTable;

    uint32_t SmallGeneration0HeapCount;
    GCHeap** SmallGeneration0Heaps;
    uint32_t SmallGeneration1HeapCount;
    GCHeap** SmallGeneration1Heaps;
    uint32_t SmallGeneration2HeapCount;
    GCHeap** SmallGeneration2Heaps;
    uint32_t LargeHeapCount;
    GCHeap** LargeHeaps;
};


GC* GC_Create(AppDomain* pDomain);
void GC_Destroy(GC* pGC);
GCHeap* GCHeap_Create(uint32_t pHeapSize, uint32_t pInitialPoolSize, bool_t pUseTreeAllocation);
void GCHeap_Destroy(GCHeap* pGCHeap);
void GC_AllocateObject(AppDomain* pDomain, IRType* pType, uint32_t pSize, void** pAllocatedObject);
void GC_AllocateStringFromASCII(AppDomain* pDomain, int8_t* pString, uint32_t pLength, void** pAllocatedObject);
void GC_AllocateStringFromUnicode(AppDomain* pDomain, uint16_t* pString, uint32_t pLength, void** pAllocatedObject);
void GC_AllocateInternedStringFromUnicode(AppDomain* pDomain, uint16_t* pString, uint32_t pLength, void** pAllocatedObject);
void GC_AllocateEmptyStringFromLength(AppDomain* pDomain, uint32_t pLength, void** pAllocatedObject);
void GC_AllocateArray(AppDomain* pDomain, IRType* pArrayType, uint32_t pElementCount, void** pAllocatedObject);
GCObject* GC_AllocatePinnedObject(GC* pGC, IRType* pType, uint32_t pSize);
void GC_ApplyPressure(AppDomain* pDomain, uint32_t pBytes);

void* GCObject_Internal_IsInst(void* pObject, IRType* pType);