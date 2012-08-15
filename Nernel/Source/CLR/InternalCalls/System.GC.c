#include <CLR/InternalCalls/System.GC.h>

uint32_t System_GC_get_MaxGeneration(AppDomain* pAppDomain)
{
	return 0;
}

void System_GC_InternalCollect(AppDomain* pAppDomain, uint32_t pGeneration)
{
	pAppDomain->GarbageCollector->ForceCollect = TRUE;
}

uint32_t System_GC_GetGeneration(AppDomain* pAppDomain, void* pObject)
{
	return 0;
}

uint64_t System_GC_GetTotalMemory(AppDomain* pAppDomain, uint32_t pForceFullCollection)
{
	if (pForceFullCollection) pAppDomain->GarbageCollector->ForceCollect = TRUE;
	uint64_t allocated = GC_TotalAllocatedMemory(pAppDomain);
	return allocated;
}
