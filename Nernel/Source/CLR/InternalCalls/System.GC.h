#pragma once

#include <CLR/AppDomain.h>

uint32_t System_GC_get_MaxGeneration(AppDomain* pAppDomain);
void System_GC_InternalCollect(AppDomain* pAppDomain, uint32_t pGeneration);
uint32_t System_GC_GetGeneration(AppDomain* pAppDomain, void* pObject);
uint64_t System_GC_GetTotalMemory(AppDomain* pAppDomain, uint32_t pForceFullCollection);
void System_GC_KeepAlive(AppDomain* pAppDomain, void* pObject);
void System_GC_ReRegisterForFinalize(AppDomain* pAppDomain, void* pObject);
void System_GC_SuppressFinalize(AppDomain* pAppDomain, void* pObject);
void System_GC_WaitForPendingFinalizers(AppDomain* pAppDomain);
uint32_t System_GC_CollectionCount(AppDomain* pAppDomain, uint32_t pGeneration);
