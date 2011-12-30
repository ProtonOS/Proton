#pragma once

#define ReferenceTypeObject_Flags_Pinned            0x01
#define ReferenceTypeObject_Flags_Allocated         0x20
#define ReferenceTypeObject_Flags_Disposing         0x40
#define ReferenceTypeObject_Flags_Disposed          0x80

typedef struct _ReferenceTypeObject ReferenceTypeObject;
typedef struct _GCHeapStack GCHeapStack;

struct _ReferenceTypeObject
{
    uint8_t Flags;
    uint32_t Size;
    uint8_t* Object; // Do not cache this pointer anywhere, GC can change it
    GCHeapStack* Stack;
    uint8_t Age;
    uint32_t ReferenceCount;
    uint32_t DependancyPoolSize;
    uint32_t DependancyPoolCount;
    ReferenceTypeObject** DependancyPool;
};

void ReferenceTypeObject_AddReference(ReferenceTypeObject* pReference, ReferenceTypeObject* pDependancy);
void ReferenceTypeObject_RemoveReference(ReferenceTypeObject* pReference, ReferenceTypeObject* pDependancy);
void ReferenceTypeObject_Dispose(ReferenceTypeObject* pObject);
void ReferenceTypeObject_Reset(ReferenceTypeObject* pObject);
