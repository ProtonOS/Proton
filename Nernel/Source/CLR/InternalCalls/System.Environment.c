#include <CLR/InternalCalls/System.Environment.h>
#include <System/SystemClock.h>

extern uint8_t gSMP_StartedProcessors;

void System_Environment_GetNewLine(AppDomain* pAppDomain, void** pReturnObjectDestination)
{
	uint16_t newLineChar = '\n';
	GC_AllocateInternedStringFromUnicode(pAppDomain, &newLineChar, 1, pReturnObjectDestination);
}

int32_t System_Environment_get_Platform(AppDomain* pAppDomain)
{
	return 4; // Unix
}

void System_Environment_GetOSVersionString(AppDomain* pAppDomain, void** pReturnObjectDestination)
{
	GC_AllocateInternedStringFromASCII(pAppDomain, (int8_t*)PROTON_VERSION_STRING, strlen(PROTON_VERSION_STRING), pReturnObjectDestination);
}

int32_t System_Environment_get_TickCount(AppDomain* pAppDomain)
{
	return SystemClock_GetTicks();
}

void System_Environment_GetWindowsFolderPath(AppDomain* pAppDomain, int32_t pSpecialFolder, void** pReturnObjectDestination)
{
	*pReturnObjectDestination = NULL;
	Panic("System_Environment_GetWindowsFolderPath");
}

int32_t System_Environment_get_ProcessorCount(AppDomain* pAppDomain)
{
	return gSMP_StartedProcessors;
}

void System_Environment_internalGetGacPath(AppDomain* pAppDomain, void** pReturnObjectDestination)
{
	*pReturnObjectDestination = NULL;
	Panic("System_Environment_internalGetGacPath");
}

int32_t System_Environment_GetPageSize(AppDomain* pAppDomain)
{
	return 4096;
}
