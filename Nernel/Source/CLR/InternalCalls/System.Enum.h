#pragma once

#include <CLR/AppDomain.h>

void System_Enum_get_value(AppDomain* pAppDomain, void* pEnumObject, void** pReturnObjectDestination);
void System_Enum_get_underlying_type(AppDomain* pAppDomain, void* pEnumTypeObject, void** pReturnObjectDestination);
int32_t System_Enum_compare_value_to(AppDomain* pAppDomain, void* pEnumObjectA, void* pEnumObjectB);
void System_Enum_ToObject(AppDomain* pAppDomain, void* pEnumTypeObject, void* pValueObject, void** pReturnObjectDestination);
int32_t System_Enum_get_hashcode(AppDomain* pAppDomain, void* pEnumObject);
