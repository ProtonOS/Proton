#include <Core/SystemClock.h>
#include <CLR/InternalCalls/System.DateTime.h>
#include <CLR/InternalCalls/Helpers.h>

uint32_t System_DateTime_InternalUtcNow(AppDomain* pAppDomain)
{
	return (uint32_t)SystemClock_GetTicks();
}
