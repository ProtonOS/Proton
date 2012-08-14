#pragma once

#include <CLR/AppDomain.h>

void System_ArgIterator_Setup(AppDomain* pAppDomain, void* pArgIterator, void* pArgStackPointer, void* pStart);
TypedReference System_ArgIterator_IntGetNextArg(AppDomain* pAppDomain, void* pArgIterator);
TypedReference System_ArgIterator_IntGetNextArg_RuntimeTypeHandle(AppDomain* pAppDomain, void* pArgIterator, void* pRuntimeTypeHandle);
void* System_ArgIterator_IntGetNextArgType(AppDomain* pAppDomain, void* pArgIterator);
