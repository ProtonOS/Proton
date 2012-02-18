#include <CLR/SyntheticStack.h>
#include <CLR/Log.h>
#include <stdlib.h>
#include <stdio.h>

// This makes it so VS doesn't complain about the attribute.
// GCC sees the attribute just fine.
#ifdef _WIN32

#define ALWAYS_INLINE

#else

#ifdef ILREADER_DONT_INLINE
#define ALWAYS_INLINE
#else
#define ALWAYS_INLINE __attribute__((always_inline)) 
#endif

#endif 


void Panic(const char* msg);
void StackObjectPool_PushSet();
void StackObjectPool_TrimPool();
SyntheticStack* stackObjPool;

void StackObjectPool_Initialize()
{
    stackObjPool = SyntheticStack_Create();
    StackObjectPool_PushSet();
}

void StackObjectPool_Destroy()
{
    SyntheticStack_Destroy(stackObjPool);
}

#define STACK_OBJECT_NO_TYPE ((StackObjectType)0xFE)
#define STACK_OBJECT_ROOTOBJECT ((StackObjectType)0xFF)
#define STACK_OBJECT_POOL_INIT_SIZE (32)

ALWAYS_INLINE void StackObjectPool_PushSet()
{
    for (uint32_t i = 0; i < STACK_OBJECT_POOL_INIT_SIZE; i++)
    {
        StackObject* obj = StackObject_Create();
        obj->Type = STACK_OBJECT_NO_TYPE;
        SyntheticStack_Push(stackObjPool, obj);
    }
}

StackObject* StackObjectPool_Allocate()
{
    if (!(stackObjPool->StackDepth > 0))
    {
        StackObjectPool_PushSet();
    }
    return SyntheticStack_Pop(stackObjPool);
}

void StackObjectPool_Release(StackObject* obj)
{
    obj->NextObj = (StackObject*)0;
    obj->PrevObj = (StackObject*)0;
    obj->Type = STACK_OBJECT_NO_TYPE;
    obj->NumericType = (StackObjectNumericType)0;
    SyntheticStack_Push(stackObjPool, obj);
    StackObjectPool_TrimPool();
}

ALWAYS_INLINE void StackObjectPool_TrimPool()
{
    if (stackObjPool->StackDepth > 256)
    {
        uint32_t nToRem = stackObjPool->StackDepth - 64;
        for (uint32_t i = 0; i < nToRem; i++)
        {
            StackObject_Destroy(SyntheticStack_Pop(stackObjPool));
        }
    }
}


StackObject* StackObject_Create()
{
    StackObject* obj = (StackObject*)calloc(1, sizeof(StackObject));
    obj->Type = STACK_OBJECT_NO_TYPE;
    return obj;
}

void StackObject_Destroy(StackObject* obj)
{
    free(obj);
}


static int StackNumber = 1;

SyntheticStack* SyntheticStack_Create()
{
    Log_WriteLine(LogFlags_SyntheticStack, "Created a Synthetic Stack");
    SyntheticStack* stack = (SyntheticStack*)calloc(1, sizeof(SyntheticStack));
	stack->StackID = StackNumber;
	StackNumber++;
    StackObject* obj = StackObject_Create();
    obj->Type = STACK_OBJECT_ROOTOBJECT;
    stack->TopObject = obj;

	stack->StackDepth = 0;

    return stack;
}

void SyntheticStack_Destroy(SyntheticStack* stack)
{
    Log_WriteLine(LogFlags_SyntheticStack, "On Stack Destruction: %i Object(s) left on stack", (int)stack->StackDepth);
    while ((int)(stack->StackDepth) > 1)
    {
        StackObject_Destroy(SyntheticStack_Pop(stack));
    }
	if ((int)(stack->StackDepth) > 0)
		StackObject_Destroy(stack->TopObject);
    free(stack);
}

void SyntheticStack_Push(SyntheticStack* stack, StackObject* obj)
{
    obj->NextObj = (StackObject*)0;
    obj->PrevObj = stack->TopObject;
    stack->TopObject->NextObj = obj;
    stack->StackDepth++;
    stack->TopObject = obj;
	if (stack->StackID != 1)
	{
		Log_WriteLine(LogFlags_SyntheticStack, "Stack: 0x%x", (unsigned int)stack->StackID);
		Log_WriteLine(LogFlags_SyntheticStack, "Pushed object to stack. Top Object Type: 0x%x", (unsigned int)obj->Type);
		Log_WriteLine(LogFlags_SyntheticStack, "Pushed object to stack. Top Object Numeric Type: 0x%x", (unsigned int)obj->NumericType);
		Log_WriteLine(LogFlags_SyntheticStack, "Pushed object to stack. Current number on stack: %i", (int)stack->StackDepth);
	}
}

StackObject* SyntheticStack_Pop(SyntheticStack* stack)
{
    StackObject* obj = stack->TopObject;
    stack->TopObject = obj->PrevObj;
	if (stack->StackDepth <= 0)
		Panic("Attempted to pop from an empty stack!");
    stack->StackDepth--;
    obj->NextObj = (StackObject*)0;
    obj->PrevObj = (StackObject*)0;
	if (stack->StackID != 1)
	{
		Log_WriteLine(LogFlags_SyntheticStack, "Stack: 0x%x", (unsigned int)stack->StackID);
		Log_WriteLine(LogFlags_SyntheticStack, "Popped object to stack. Top Object Type: 0x%x", (unsigned int)obj->Type);
		Log_WriteLine(LogFlags_SyntheticStack, "Popped object to stack. Top Object Numeric Type: 0x%x", (unsigned int)obj->NumericType);
		Log_WriteLine(LogFlags_SyntheticStack, "Popped object from stack. Current number on stack: %i", (int)stack->StackDepth);
	}
    return obj;
}

StackObject* SyntheticStack_Peek(SyntheticStack* stack)
{
    Log_WriteLine(LogFlags_SyntheticStack, "Peeked at top object on stack. Current number on stack: %i", (int)stack->StackDepth);
    return stack->TopObject;
}