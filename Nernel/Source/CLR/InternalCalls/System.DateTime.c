#include <CLR/InternalCalls/System.DateTime.h>
#include <System/SystemClock.h>

int64_t System_DateTime_GetTimeMonotonic(AppDomain* pAppDomain)
{
	return (int64_t)(SystemClock_GetTicks() & 0x7FFFFFFFFFFFFFFF);
}

int64_t System_DateTime_GetNow(AppDomain* pAppDomain)
{
	return (int64_t)SystemClock_GetTicks();
}
