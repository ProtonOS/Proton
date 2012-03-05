#pragma once

#include <CLR/AppDomain.h>

ReferenceTypeObject* System_RuntimeType_CreateFromObject(ReferenceTypeObject* obj);
ReferenceTypeObject* System_RuntimeType_CreateFromIRType(IRType* tp);


ReferenceTypeObject* System_RuntimeType_get_BaseType(AppDomain* dom, ReferenceTypeObject* pThis);
ReferenceTypeObject* System_RuntimeType_get_Namespace(AppDomain* dom, ReferenceTypeObject* pThis);
ReferenceTypeObject* System_RuntimeType_get_Name(AppDomain* dom, ReferenceTypeObject* pThis);