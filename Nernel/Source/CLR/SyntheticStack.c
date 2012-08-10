#include <CLR/SyntheticStack.h>
#include <System/Atomics.h>
#include <System/Log.h>

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


void StackObjectPool_PushSet();
void StackObjectPool_TrimPool();

void StackObjectPool_Initialize(SyntheticStack* pStack)
{
    pStack->ObjectPool = SyntheticStack_Create();
    StackObjectPool_PushSet(pStack);
}

void StackObjectPool_Destroy(SyntheticStack* pStack)
{
    SyntheticStack_Destroy(pStack->ObjectPool);
}

#define STACK_OBJECT_NO_TYPE			NULL
#define STACK_OBJECT_ROOTOBJECT			((IRType*)0x01)
#define STACK_OBJECT_POOL_INIT_SIZE		32

ALWAYS_INLINE void StackObjectPool_PushSet(SyntheticStack* pStack)
{
    for (uint32_t i = 0; i < STACK_OBJECT_POOL_INIT_SIZE; i++)
    {
        StackObject* obj = StackObject_Create();
        obj->Type = STACK_OBJECT_NO_TYPE;
        SyntheticStack_Push(pStack->ObjectPool, obj);
    }
}

StackObject* StackObjectPool_Allocate(SyntheticStack* pStack)
{
    if (pStack->ObjectPool->StackDepth <= 0)
    {
        StackObjectPool_PushSet(pStack);
    }
    return SyntheticStack_Pop(pStack->ObjectPool);
}

void StackObjectPool_Release(SyntheticStack* pStack, StackObject* pObject)
{
    pObject->NextObj = NULL;
    pObject->PrevObj = NULL;
    pObject->Type = STACK_OBJECT_NO_TYPE;
	pObject->Source = NULL;
	pObject->SourceType = (StackObjectSourceType)0xFF;
    SyntheticStack_Push(pStack->ObjectPool, pObject);
    StackObjectPool_TrimPool(pStack);
}

ALWAYS_INLINE void StackObjectPool_TrimPool(SyntheticStack* pStack)
{
    if (pStack->ObjectPool->StackDepth > 256)
    {
        uint32_t nToRem = pStack->ObjectPool->StackDepth - 64;
        for (uint32_t i = 0; i < nToRem; i++)
        {
            StackObject_Destroy(SyntheticStack_Pop(pStack->ObjectPool));
        }
    }
}


StackObject* StackObject_Create()
{
    StackObject* object = (StackObject*)calloc(1, sizeof(StackObject));
	//printf("Created stack object at 0x%x\n", (unsigned int)object);
    object->Type = STACK_OBJECT_NO_TYPE;
	object->SourceType = (StackObjectSourceType)0xFF;
    return object;
}

void StackObject_Destroy(StackObject* obj)
{
	//printf("Destroying stack object at 0x%x\n", (unsigned int)obj);
    free(obj);
}


uint32_t gSynethicStack_NextID = 1;

SyntheticStack* SyntheticStack_Create()
{
    SyntheticStack* stack = (SyntheticStack*)calloc(1, sizeof(SyntheticStack));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: SynethicStack_Create @ 0x%x", (unsigned int)stack);
	stack->StackID = Atomic_Increment(&gSynethicStack_NextID);
    StackObject* obj = StackObject_Create();
    obj->Type = STACK_OBJECT_ROOTOBJECT;
    stack->TopObject = obj;
	stack->StackDepth = 0;

    return stack;
}

void SyntheticStack_Destroy(SyntheticStack* pStack)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: SynethicStack_Destroy @ 0x%x", (unsigned int)pStack);
    Log_WriteLine(LOGLEVEL__SyntheticStack, "On Stack Destruction: %i object(s) left on stack", (int)pStack->StackDepth);
    while ((int)(pStack->StackDepth) > 1)
    {
        StackObject_Destroy(SyntheticStack_Pop(pStack));
    }
	if ((int)(pStack->StackDepth) > 0)
		StackObject_Destroy(pStack->TopObject);
    free(pStack);
}

void SyntheticStack_Push(SyntheticStack* pStack, StackObject* pObject)
{
    pObject->NextObj = (StackObject*)0;
    pObject->PrevObj = pStack->TopObject;
    pStack->TopObject->NextObj = pObject;
    pStack->StackDepth++;
    pStack->TopObject = pObject;
	Log_WriteLine(LOGLEVEL__SyntheticStack, "Pushed object to stack 0x%x, TopObject Type = 0x%x, StackDepth = %i", (unsigned int)pStack->StackID, (unsigned int)pObject->Type, (int)pStack->StackDepth);
}

StackObject* SyntheticStack_Pop(SyntheticStack* pStack)
{
    StackObject* object = pStack->TopObject;
    pStack->TopObject = object->PrevObj;
	if (pStack->StackDepth <= 0)
		Panic("Attempted to pop from an empty stack!");
    pStack->StackDepth--;
    object->NextObj = NULL;
    object->PrevObj = NULL;
	Log_WriteLine(LOGLEVEL__SyntheticStack, "Popped object from stack 0x%x, TopObject Type = 0x%x, StackDepth = %i", (unsigned int)pStack->StackID, (unsigned int)object->Type, (int)pStack->StackDepth);
    return object;
}

StackObject* SyntheticStack_Peek(SyntheticStack* pStack)
{
    Log_WriteLine(LOGLEVEL__SyntheticStack, "Peeked at top object on stack 0x%x, StackDepth = %i", (unsigned int)pStack->StackID, (int)pStack->StackDepth);
    return pStack->TopObject;
}
