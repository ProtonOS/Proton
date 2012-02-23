#include <Core/SystemClock.h>
#include <CLR/InternalCalls/System.DateTime.h>
#include <CLR/InternalCalls/Helpers.h>

uint64_t System_DateTime_InternalUtcNow(AppDomain* pAppDomain)
{
	return SystemClock_GetTicks();
}
