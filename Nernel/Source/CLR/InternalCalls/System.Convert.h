#pragma once

#include <CLR/AppDomain.h>

void System_Convert_InternalFromBase64String(AppDomain* pAppDomain, void* pString, uint32_t pWhitespaceOnly, void** pReturnObjectDestination);
void System_Convert_InternalFromBase64CharArray(AppDomain* pAppDomain, void* pCharArray, uint32_t pOffset, uint32_t pLength, void** pReturnObjectDestination);
