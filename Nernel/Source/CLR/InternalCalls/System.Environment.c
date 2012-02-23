#include <Core/Config.h>
#include <Core/SystemClock.h>
#include <CLR/GC.h>
#include <CLR/InternalCalls/System.Environment.h>
#include <CLR/InternalCalls/Helpers.h>

#define PLATFORM 7

uint32_t System_Environment_getTickCount(AppDomain* pAppDomain)
{
	return (uint32_t)SystemClock_GetTicks();
}

uint16_t gUnicodeOSVersionData[32];
uint8_t gUnicodeOSVersionDataLength = 0;

ReferenceTypeObject* System_Environment_GetOSVersionString(AppDomain* pAppDomain)
{
	if (gUnicodeOSVersionDataLength == 0)
	{
		gUnicodeOSVersionDataLength = strlen(VERSION);
		for (size_t index = 0; index < gUnicodeOSVersionDataLength; ++index) 
			gUnicodeOSVersionData[index] = VERSION[index];
		gUnicodeOSVersionDataLength *= 2;
	}
	ReferenceTypeObject* unicodeVersionString = GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, (uint8_t*)gUnicodeOSVersionData, gUnicodeOSVersionDataLength);
	return unicodeVersionString;
}

uint32_t System_Environment_getPlatform(AppDomain* pAppDomain)
{
	return PLATFORM;
}
