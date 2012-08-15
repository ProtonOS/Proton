#include <CLR/InternalCalls/System.GC.h>

uint32_t System_GC_get_MaxGeneration(AppDomain* pAppDomain)
{
	return 0;
}

void System_GC_InternalCollect(AppDomain* pAppDomain, uint32_t pGeneration)
{
	pAppDomain->GarbageCollector->ForceCollect = TRUE;
	while (pAppDomain->GarbageCollector->ForceCollect) usleep(1000);
}

uint32_t System_GC_GetGeneration(AppDomain* pAppDomain, void* pObject)
{
	return 0;
}

uint64_t System_GC_GetTotalMemory(AppDomain* pAppDomain, uint32_t pForceFullCollection)
{
	if (pForceFullCollection)
	{
		usleep(10000);
		pAppDomain->GarbageCollector->ForceCollect = TRUE;
	}
	uint64_t allocated = GC_TotalAllocatedMemory(pAppDomain);
	return allocated;
}

void System_GC_KeepAlive(AppDomain* pAppDomain, void* pObject)
{
}

void System_GC_ReRegisterForFinalize(AppDomain* pAppDomain, void* pObject)
{
	GCObject* object = *(GCObject**)((size_t)pObject - sizeof(GCObject*));
	object->Flags = (object->Flags | GCObjectFlags_PostponeDispose) & ~GCObjectFlags_SuppressFinalizer;
}

void System_GC_SuppressFinalize(AppDomain* pAppDomain, void* pObject)
{
	GCObject* object = *(GCObject**)((size_t)pObject - sizeof(GCObject*));
	object->Flags |= GCObjectFlags_SuppressFinalizer;
}

void System_GC_WaitForPendingFinalizers(AppDomain* pAppDomain)
{
	while(pAppDomain->GarbageCollector->ForceCollect) usleep(1000);
}

uint32_t System_GC_CollectionCount(AppDomain* pAppDomain, uint32_t pGeneration)
{
	return pAppDomain->GarbageCollector->CollectionCount;
}
