#pragma once

#include <CLR/AppDomain.h>

void System_Environment_GetNewLine(AppDomain* pAppDomain, void** pReturnObjectDestination);
int32_t System_Environment_get_Platform(AppDomain* pAppDomain);
void System_Environment_GetOSVersionString(AppDomain* pAppDomain, void** pReturnObjectDestination);
int32_t System_Environment_get_TickCount(AppDomain* pAppDomain);
void System_Environment_GetWindowsFolderPath(AppDomain* pAppDomain, int32_t pSpecialFolder, void** pReturnObjectDestination);
int32_t System_Environment_get_ProcessorCount(AppDomain* pAppDomain);
void System_Environment_internalGetGacPath(AppDomain* pAppDomain, void** pReturnObjectDestination);
int32_t System_Environment_GetPageSize(AppDomain* pAppDomain);
