#pragma once

typedef struct _SyntheticStack SyntheticStack;
typedef struct _StackObject StackObject;

typedef enum StackObjectType
{
    StackObjectType_Int32,
    StackObjectType_Int64,
    StackObjectType_NativeInt,
    StackObjectType_Float,
    StackObjectType_ReferenceType,
    StackObjectType_DataType,
    StackObjectType_UnmanagedPointer,
    StackObjectType_ManagedPointer,
} StackObjectType;

typedef enum StackObjectNumericType
{
    StackObjectNumericType_UInt8,
    StackObjectNumericType_UInt16,
    StackObjectNumericType_UInt32,
    StackObjectNumericType_UInt64,
    StackObjectNumericType_Int8,
    StackObjectNumericType_Int16,
    StackObjectNumericType_Int32,
    StackObjectNumericType_Int64,
    StackObjectNumericType_Float32,
    StackObjectNumericType_Float64,
    StackObjectNumericType_Pointer,
} StackObjectNumericType;

struct _StackObject
{
    StackObjectType Type;
    StackObjectNumericType NumericType;
    StackObject* PrevObj;
    StackObject* NextObj;
    const char* Name;
    void* Data;
};

StackObject* StackObject_Create();
void StackObject_Destroy(StackObject* obj);

struct _SyntheticStack
{
    StackObject* TopObject;
    uint32_t StackDepth;
};

SyntheticStack* SyntheticStack_Create();
void SyntheticStack_Destroy(SyntheticStack* stack);
void SyntheticStack_Push(SyntheticStack* stack, StackObject* obj);
StackObject* SyntheticStack_Pop(SyntheticStack* stack);
StackObject* SyntheticStack_Peek(SyntheticStack* stack);



void StackObjectPool_Initialize();
void StackObjectPool_Destroy();
StackObject* StackObjectPool_Allocate();
void StackObjectPool_Release(StackObject* obj);
