#pragma once

#include <CLR/AppDomain.h>

void System_Environment_getTickCount(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
void System_Environment_GetOSVersionString(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
void System_Environment_getPlatform(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
