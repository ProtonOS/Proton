#include <CLR/InternalCalls/System.DateTime.h>
#include <System/SystemClock.h>

int64_t System_DateTime_GetTimeMonotonic(AppDomain* pAppDomain)
{
	return (int64_t)(SystemClock_GetTicks() & 0x7FFFFFFFFFFFFFFF) * 10000; // Number of 100 nanosecond periods since 0AD
}

int64_t System_DateTime_GetNow(AppDomain* pAppDomain)
{
	return (int64_t)SystemClock_GetTicks() * 10000; // Number of 100 nanosecond periods since 0AD
}
