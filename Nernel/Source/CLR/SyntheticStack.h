#pragma once

typedef struct _SyntheticStack SyntheticStack;
typedef struct _StackObject StackObject;
typedef struct _StackLinearizationData StackLinearizationData;

#include <CLR/IRStructures.h>

struct _StackLinearizationData
{
	SourceType Source;
	SourceData SourceData;
};

typedef enum StackObjectSourceType
{
	StackObjectSourceType_Constant,
	StackObjectSourceType_Parameter,
	StackObjectSourceType_Local,
	StackObjectSourceType_PinnedLocal,
	StackObjectSourceType_StaticField,
	StackObjectSourceType_Stack,
} StackObjectSourceType;

struct _StackObject
{
	union
	{
		struct
		{
			StackObjectSourceType SourceType;
			IRType* Type;
			IRInstruction* Source;
		};
		StackLinearizationData LinearData;
	};
    StackObject* PrevObj;
    StackObject* NextObj;
};

StackObject* StackObject_Create();
void StackObject_Destroy(StackObject* obj);

struct _SyntheticStack
{
	uint32_t StackID;
    StackObject* TopObject;
    uint32_t StackDepth;
	SyntheticStack* ObjectPool;
};

SyntheticStack* SyntheticStack_Create();
void SyntheticStack_Destroy(SyntheticStack* pStack);
void SyntheticStack_Push(SyntheticStack* pStack, StackObject* pObject);
StackObject* SyntheticStack_Pop(SyntheticStack* pStack);
StackObject* SyntheticStack_Peek(SyntheticStack* pStack);

void StackObjectPool_Initialize(SyntheticStack* pStack);
void StackObjectPool_Destroy(SyntheticStack* pStack);
StackObject* StackObjectPool_Allocate(SyntheticStack* pStack);
void StackObjectPool_Release(SyntheticStack* pStack, StackObject* pObject);
