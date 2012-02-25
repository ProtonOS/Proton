#pragma once

#include <CLR/AppDomain.h>

ReferenceTypeObject* System_String_Ctor_CharPtr(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint16_t* pCharPtr);
ReferenceTypeObject* System_String_InternalConcat(AppDomain* pAppDomain, ReferenceTypeObject* pString1, ReferenceTypeObject* pString2);
ReferenceTypeObject* System_String_InternalReplace(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pPattern, ReferenceTypeObject* pSubstitute);
