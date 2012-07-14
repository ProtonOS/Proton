#pragma once

namespace SystemClock
{
	void Startup();

	bool IsReady();
	uint32_t GetSecondsSinceEpoch();
	uint16_t GetMilliseconds();
	uint64_t GetTicks();
};
