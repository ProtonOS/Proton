#include <CLR/InternalCalls/System.GC.h>

uint32_t System_GC_get_MaxGeneration(AppDomain* pAppDomain)
{
	return 0;
}

void System_GC_InternalCollect(AppDomain* pAppDomain, uint32_t pGeneration)
{
	pAppDomain->GarbageCollector->ForceCollect = TRUE;
}
