#pragma once

#include <CLR/AppDomain.h>

uint32_t System_Array_FastCopy(AppDomain* pAppDomain, void* pSourceArray, uint32_t pSourceIndex, void* pDestinationArray, uint32_t pDestinationIndex, uint32_t pLength);
void System_Array_ClearInternal(AppDomain* pAppDomain, void* pArray, uint32_t pIndex, uint32_t pCount);
