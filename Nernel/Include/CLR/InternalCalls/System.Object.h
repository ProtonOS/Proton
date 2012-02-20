#pragma once

#include <CLR/AppDomain.h>

void System_Object_Equals(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
void System_Object_GetHashCode(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn);
