#include "../SystemClock.h"
#include "../RTC.h"
#include "../PIT.h"

namespace SystemClock
{
	uint32_t sStartupTime = 0;

	void Startup()
	{
	    sStartupTime = RTC::GetSecondsSinceEpoch();
	}

	bool IsReady()
	{
		return sStartupTime != 0;
	}

	uint32_t GetSecondsSinceEpoch()
	{
		return sStartupTime + PIT::GetSecondsElapsed();
	}

	uint16_t GetMilliseconds()
	{
		return PIT::GetMillisecondsElapsed();
	}

	uint64_t GetTicks()
	{
		return PIT::GetMillisecondsElapsed() + (PIT::GetSecondsElapsed() * 1000);
	}
}
