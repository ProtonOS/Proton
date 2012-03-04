#pragma once

#include <CLR/AppDomain.h>

ReferenceTypeObject* System_String_Ctor_CharPtr(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint16_t* pCharPtr);
ReferenceTypeObject* System_String_Ctor_CharPtrAndStartAndLength(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint16_t* pCharPtr, uint32_t pStart, uint32_t pLength);
ReferenceTypeObject* System_String_Ctor_SBytePtr(AppDomain* pAppDomain, ReferenceTypeObject* pThis, int8_t* pSBytePtr);
ReferenceTypeObject* System_String_Ctor_SBytePtrAndStartAndLength(AppDomain* pAppDomain, ReferenceTypeObject* pThis, int8_t* pSBytePtr, uint32_t pStart, uint32_t pLength);
ReferenceTypeObject* System_String_Ctor_CharAndCount(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint32_t pChar, uint32_t pCount);
ReferenceTypeObject* System_String_Ctor_StringAndStartAndLength(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pString, uint32_t pStart, uint32_t pLength);
ReferenceTypeObject* System_String_Ctor_CharArray(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pCharArray);
ReferenceTypeObject* System_String_Ctor_CharArrayAndStartAndLength(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pCharArray, uint32_t pStart, uint32_t pLength);
ReferenceTypeObject* System_String_InternalConcat(AppDomain* pAppDomain, ReferenceTypeObject* pString1, ReferenceTypeObject* pString2);
ReferenceTypeObject* System_String_InternalReplace(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pPattern, ReferenceTypeObject* pSubstitute);
ReferenceTypeObject* System_String_InternalTrim(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pTrimCharsArray, uint32_t pTrimType);
int32_t System_String_InternalIndexOf(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint32_t pChar, uint32_t pStart, uint32_t pCount, uint32_t pForwards);
int32_t System_String_InternalIndexOfAny(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pCharArray, uint32_t pStart, uint32_t pCount, uint32_t pForwards);
uint32_t System_String_Equals(AppDomain* pAppDomain, ReferenceTypeObject* pString1, ReferenceTypeObject* pString2);
uint32_t System_String_GetHashCode(AppDomain* pAppDomain, ReferenceTypeObject* pThis);
uint32_t System_String_getChars(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint32_t pIndex);
