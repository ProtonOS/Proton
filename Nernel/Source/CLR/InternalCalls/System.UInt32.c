#include <CLR/InternalCalls/System.UInt32.h>
#include <CLR/InternalCalls/System.IntCommon.h>

ReferenceTypeObject* System_UInt32_ToString(AppDomain* pAppDomain, uint32_t* pThis)
{
	return System_IntCommon_ToString(pAppDomain, (uint64_t*)pThis, FALSE, 10);
}
