#pragma once

#include <CLR/AppDomain.h>

uint32_t System_Array_FastCopy(AppDomain* pAppDomain, void* pSourceArray, uint32_t pSourceIndex, void* pDestinationArray, uint32_t pDestinationIndex, uint32_t pLength);
int32_t System_Array_GetRank(AppDomain* pAppDomain, void* pArray);
int32_t System_Array_GetLength(AppDomain* pAppDomain, void* pArray, uint32_t pDimension);
int32_t System_Array_GetLowerBound(AppDomain* pAppDomain, void* pArray, uint32_t pDimension);
void* System_Array_GetValueImpl(AppDomain* pAppDomain, void* pArray, uint32_t pIndex);
void System_Array_ClearInternal(AppDomain* pAppDomain, void* pArray, uint32_t pIndex, uint32_t pCount);
