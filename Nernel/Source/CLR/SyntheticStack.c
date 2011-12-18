#include <CLR/SyntheticStack.h>
#include <stdlib.h>


StackObject* StackObject_Create(StackObjectType tp)
{
    StackObject* obj = (StackObject*)calloc(1, sizeof(StackObject));
    obj->Type = tp;
    obj->Name = "Default Name";
    return obj;
}

void StackObject_Destroy(StackObject* obj)
{
    free(obj->Data);
    free(obj);
}


SyntheticStack* SyntheticStack_Create()
{
    SyntheticStack* stack = (SyntheticStack*)calloc(1, sizeof(SyntheticStack));
    
    return stack;
}

void SyntheticStack_Destroy(SyntheticStack* stack)
{

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