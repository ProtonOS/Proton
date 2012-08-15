#pragma once

#include <CLR/AppDomain.h>

void System_Convert_InternalFromBase64String(AppDomain* pAppDomain, void* pString, uint32_t pWhitespaceOnly, void** pReturnObjectDestination);
