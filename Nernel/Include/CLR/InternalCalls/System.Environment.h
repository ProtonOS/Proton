#pragma once

#include <CLR/AppDomain.h>

uint32_t System_Environment_getTickCount(AppDomain* pAppDomain);
ReferenceTypeObject* System_Environment_GetOSVersionString(AppDomain* pAppDomain);
uint32_t System_Environment_getPlatform(AppDomain* pAppDomain);
