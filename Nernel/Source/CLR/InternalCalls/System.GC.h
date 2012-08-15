#pragma once

#include <CLR/AppDomain.h>

uint32_t System_GC_get_MaxGeneration(AppDomain* pAppDomain);
void System_GC_InternalCollect(AppDomain* pAppDomain, uint32_t pGeneration);
uint32_t System_GC_GetGeneration(AppDomain* pAppDomain, void* pObject);
uint64_t System_GC_GetTotalMemory(AppDomain* pAppDomain, uint32_t pForceFullCollection);
