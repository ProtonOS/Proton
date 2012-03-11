#pragma once

#include <CLR/AppDomain.h>
#include <CLR/GC.h>

ReferenceTypeObject* System_IntCommon_ToString(AppDomain* pAppDomain, uint64_t* pThis, bool_t pSigned, uint32_t pSize);