#include <Core/Config.h>
#include <Core/SystemClock.h>
#include <CLR/GC.h>
#include <CLR/InternalCalls/System.Environment.h>
#include <CLR/InternalCalls/Helpers.h>

void System_Environment_getTickCount(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(uint32_t*)pReturn = (uint32_t)SystemClock_GetTicks();
}

uint16_t gUnicodeOSVersionData[32];
uint8_t gUnicodeOSVersionDataLength = 0;

void System_Environment_GetOSVersionString(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	if (gUnicodeOSVersionDataLength == 0)
	{
		gUnicodeOSVersionDataLength = strlen(VERSION);
		for (size_t index = 0; index < gUnicodeOSVersionDataLength; ++index) gUnicodeOSVersionData[index] = VERSION[index];
		gUnicodeOSVersionDataLength *= 2;
	}
	ReferenceTypeObject* unicodeVersionString = GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, (uint8_t*)gUnicodeOSVersionData, gUnicodeOSVersionDataLength);
	*(ReferenceTypeObject**)pReturn = unicodeVersionString;
}

void System_Environment_getPlatform(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(int32_t*)pReturn = PLATFORM;
}
