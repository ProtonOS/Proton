#pragma once

#include <CLR/AppDomain.h>

void System_Object_GetType(AppDomain* pAppDomain, void* pObject, void** pReturnObjectDestination);
int32_t System_Object_InternalGetHashCode(AppDomain* pAppDomain, void* pObject);
size_t System_Object_obj_address(AppDomain* pAppDomain, void* pObject);
