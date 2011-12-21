#include <CLR/SyntheticStack.h>
#include <stdlib.h>
#include <stdio.h>


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
        obj->Name = "Default Name";
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
    if (obj->Data)
        free(obj->Data);
    obj->Name = "Default Name";
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
    obj->Name = "Default Name";
    return obj;
}

void StackObject_Destroy(StackObject* obj)
{
    if (obj->Data)
        free(obj->Data);
    free(obj);
}


SyntheticStack* SyntheticStack_Create()
{
    SyntheticStack* stack = (SyntheticStack*)calloc(1, sizeof(SyntheticStack));
    StackObject* obj = StackObject_Create();
    obj->Type = (StackObjectType)0xFF;
    obj->Name = "Root Stack Object";
    stack->TopObject = obj;
    return stack;
}

void SyntheticStack_Destroy(SyntheticStack* stack)
{
    printf("On Stack Destruction: %i Object(s) left on stack\n", (int)stack->StackDepth);
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
}

StackObject* SyntheticStack_Pop(SyntheticStack* stack)
{
    StackObject* obj = stack->TopObject;
    stack->TopObject = obj->PrevObj;
    stack->StackDepth--;
    obj->NextObj = (StackObject*)0;
    obj->PrevObj = (StackObject*)0;
    return obj;
}

StackObject* SyntheticStack_Peek(SyntheticStack* stack)
{
    return stack->TopObject;
}