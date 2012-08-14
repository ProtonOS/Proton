#pragma once

#include <CLR/AppDomain.h>

void System_Runtime_CompilerServices_RuntimeHelpers_InitializeArray(AppDomain* pAppDomain, void* pArray, size_t pFieldHandle);
int32_t System_Runtime_CompilerServices_RuntimeHelpers_get_OffsetToStringData(AppDomain* pAppDomain);
