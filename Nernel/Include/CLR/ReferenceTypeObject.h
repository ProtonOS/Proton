#pragma once

#define ReferenceTypeObject_Flags_String            0x01
#define ReferenceTypeObject_Flags_Pinned            0x02
#define ReferenceTypeObject_Flags_Marked			0x10
#define ReferenceTypeObject_Flags_Allocated         0x20
#define ReferenceTypeObject_Flags_Disposed			0x80

typedef struct _ReferenceTypeObject ReferenceTypeObject;
typedef struct _GCHeapStack GCHeapStack;

// Do not change this structure without
// consulting the JIT first, because a
// large portion of the JIT requires offsets
// in this to be hard-coded in.
struct _ReferenceTypeObject
{
    uint8_t* Object; // Do not cache this pointer anywhere, GC can change it
    uint8_t Flags;
    uint32_t Size;
    GCHeapStack* Stack;
    uint8_t Age;
    uint32_t ReferenceCount;
    uint32_t DependancyPoolSize;
    uint32_t DependancyPoolCount;
    ReferenceTypeObject** DependancyPool;
	uint32_t DomainIndex;
	uint32_t AssemblyIndex;
	uint32_t TypeIndex;
	bool_t HasCalledFinalizer;
};

void ReferenceTypeObject_AddReference(ReferenceTypeObject* pParent, ReferenceTypeObject* pReferenced);
void ReferenceTypeObject_RemoveReference(ReferenceTypeObject* pParent, ReferenceTypeObject* pReferenced);
void ReferenceTypeObject_Dispose(ReferenceTypeObject* pObject);
void ReferenceTypeObject_Reset(ReferenceTypeObject* pObject);
