#pragma once

#include <CLR/AppDomain.h>

bool_t System_Object_Equals(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pObject);
uint32_t System_Object_GetHashCode(AppDomain* pAppDomain, ReferenceTypeObject* pThis);
