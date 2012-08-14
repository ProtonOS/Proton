#pragma once

#include <CLR/AppDomain.h>

void Mono_Interop_ComInteropProxy_AddProxy(AppDomain* pAppDomain, void* pInterface, void* pProxy);
void* Mono_Interop_ComInteropProxy_FindProxy(AppDomain* pAppDomain, void* pInterface);
