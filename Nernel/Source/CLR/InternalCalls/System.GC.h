#pragma once

#include <CLR/AppDomain.h>

uint32_t System_GC_get_MaxGeneration(AppDomain* pAppDomain);
void System_GC_InternalCollect(AppDomain* pAppDomain, uint32_t pGeneration);
