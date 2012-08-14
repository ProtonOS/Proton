#include <CLR/InternalCalls/System.ArgIterator.h>

void System_ArgIterator_Setup(AppDomain* pAppDomain, void* pArgIterator, void* pArgStackPointer, void* pStart)
{
	Panic("System_ArgIterator_Setup");
}

TypedReference System_ArgIterator_IntGetNextArg(AppDomain* pAppDomain, void* pArgIterator)
{
	Panic("System_ArgIterator_IntGetNextArg");
}

TypedReference System_ArgIterator_IntGetNextArg_RuntimeTypeHandle(AppDomain* pAppDomain, void* pArgIterator, void* pRuntimeTypeHandle)
{
	Panic("System_ArgIterator_IntGetNextArg_RuntimeTypeHandle");
}

void* System_ArgIterator_IntGetNextArgType(AppDomain* pAppDomain, void* pArgIterator)
{
	Panic("System_ArgIterator_IntGetNextArgType");
}
