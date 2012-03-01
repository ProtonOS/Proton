#pragma once

#include <CLR/AppDomain.h>

ReferenceTypeObject* System_Array_InternalGetValue(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint32_t pIndex);
uint32_t System_Array_InternalSetValue(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pValue, uint32_t pIndex);
void System_Array_Clear(AppDomain* pAppDomain, ReferenceTypeObject* pArray, uint32_t pIndex, uint32_t pLength);
uint32_t System_Array_InternalCopy(AppDomain* pAppDomain, ReferenceTypeObject* pSource, uint32_t pSourceIndex, ReferenceTypeObject* pDestination, uint32_t pDestinationIndex, uint32_t pLength);
void System_Array_Reverse(AppDomain* pAppDomain, ReferenceTypeObject* pArray, uint32_t pIndex, uint32_t pLength);
