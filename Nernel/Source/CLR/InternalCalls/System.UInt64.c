#include <CLR/InternalCalls/System.UInt64.h>
#include <CLR/InternalCalls/System.IntCommon.h>
#include <stdio.h>

ReferenceTypeObject* System_UInt64_ToString(AppDomain* pAppDomain, uint64_t* pThis)
{
	return System_IntCommon_ToString(pAppDomain, pThis, FALSE, 20);
}