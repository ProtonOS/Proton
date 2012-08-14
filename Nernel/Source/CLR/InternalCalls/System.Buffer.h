#pragma once

#include <CLR/AppDomain.h>

int32_t System_Buffer_ByteLengthInternal(AppDomain* pAppDomain, void* pArray);
uint32_t System_Buffer_GetByteInternal(AppDomain* pAppDomain, void* pArray, uint32_t pIndex);
void System_Buffer_SetByteInternal(AppDomain* pAppDomain, void* pArray, uint32_t pIndex, uint32_t pValue);
uint32_t System_Buffer_BlockCopyInternal(AppDomain* pAppDomain, void* pSourceArray, uint32_t pSourceOffset, void* pDestinationArray, uint32_t pDestinationOffset, uint32_t pCount);
