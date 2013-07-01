#pragma once

namespace SystemClock
{
	void Load();

	bool IsReady();
	UInt32 GetSecondsSinceEpoch();
	UInt16 GetMillisecondsSinceSecond();
	UInt64 GetMillisecondsSinceStartup();

	extern UInt32 StartupSecondsSinceEpoch;
};
