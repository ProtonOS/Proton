#include <Core/SystemClock.h>
#include <CLR/InternalCalls/System.DateTime.h>
#include <CLR/InternalCalls/Helpers.h>

void System_DateTime_InternalUtcNow(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(uint64_t*)pReturn = SystemClock_GetTicks();
}
