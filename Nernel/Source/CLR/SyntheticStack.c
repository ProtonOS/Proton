#include <CLR/SyntheticStack.h>
#include <CLR/Log.h>
#include <stdlib.h>
#include <stdio.h>

// For some insane reason, gcc doesn't
// want me to use strdup, because it refuses
// to find it, so we declare it here
char* strdup(const char* msg);


void StackObjectPool_PushSet();
void StackObjectPool_TrimPool();
SyntheticStack stackObjPool;

void StackObjectPool_Initialize()
{
    stackObjPool = *SyntheticStack_Create();
    StackObjectPool_PushSet();
}

void StackObjectPool_Destroy()
{
    SyntheticStack_Destroy(&stackObjPool);
}

#define STACK_OBJECT_NO_TYPE ((StackObjectType)0xFE)

void StackObjectPool_PushSet()
{
#define STACK_OBJECT_POOL_INIT_SIZE (32)
    for (uint32_t i = 0; i < STACK_OBJECT_POOL_INIT_SIZE; i++)
    {
        StackObject* obj = StackObject_Create();
        obj->Type = STACK_OBJECT_NO_TYPE;
        SyntheticStack_Push(&stackObjPool, obj);
    }
}

StackObject* StackObjectPool_Allocate()
{
    if (!(stackObjPool.StackDepth > 0))
    {
        StackObjectPool_PushSet();
    }
    return SyntheticStack_Pop(&stackObjPool);
}

void StackObjectPool_Release(StackObject* obj)
{
    obj->NextObj = (StackObject*)0;
    obj->PrevObj = (StackObject*)0;
    obj->Type = STACK_OBJECT_NO_TYPE;
    obj->NumericType = (StackObjectNumericType)0;
    SyntheticStack_Push(&stackObjPool, obj);
    StackObjectPool_TrimPool();
}

void StackObjectPool_TrimPool()
{
    if (stackObjPool.StackDepth > 256)
    {
        uint32_t nToRem = stackObjPool.StackDepth - 64;
        for (uint32_t i = 0; i < nToRem; i++)
        {
            StackObject_Destroy(SyntheticStack_Pop(&stackObjPool));
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


SyntheticStack* SyntheticStack_Create()
{
    Log_WriteLine(LogFlags_SyntheticStack, "Created a Synthetic Stack");
    SyntheticStack* stack = (SyntheticStack*)calloc(1, sizeof(SyntheticStack));
    StackObject* obj = StackObject_Create();
    obj->Type = (StackObjectType)0xFF;
    stack->TopObject = obj;
    return stack;
}

void SyntheticStack_Destroy(SyntheticStack* stack)
{
    Log_WriteLine(LogFlags_SyntheticStack, "On Stack Destruction: %i Object(s) left on stack", (int)stack->StackDepth);
    while (stack->TopObject->PrevObj)
    {
        StackObject_Destroy(SyntheticStack_Pop(stack));
    }
    free(stack->TopObject);
    free(stack);
}

void SyntheticStack_Push(SyntheticStack* stack, StackObject* obj)
{
    obj->NextObj = (StackObject*)0;
    obj->PrevObj = stack->TopObject;
    stack->TopObject->NextObj = obj;
    stack->StackDepth++;
    stack->TopObject = obj;
    Log_WriteLine(LogFlags_SyntheticStack, "Pushed object to stack. Current number on stack: %i", (int)stack->StackDepth);
}

StackObject* SyntheticStack_Pop(SyntheticStack* stack)
{
    StackObject* obj = stack->TopObject;
    stack->TopObject = obj->PrevObj;
    stack->StackDepth--;
    obj->NextObj = (StackObject*)0;
    obj->PrevObj = (StackObject*)0;
    Log_WriteLine(LogFlags_SyntheticStack, "Popped object from stack. Current number on stack: %i", (int)stack->StackDepth);
    return obj;
}

StackObject* SyntheticStack_Peek(SyntheticStack* stack)
{
    Log_WriteLine(LogFlags_SyntheticStack, "Peeked at top object on stack. Current number on stack: %i", (int)stack->StackDepth);
    return stack->TopObject;
}