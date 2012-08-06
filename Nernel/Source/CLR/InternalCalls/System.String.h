#pragma once

#include <CLR/AppDomain.h>

void System_String_Ctor_CharAndCount(AppDomain* pAppDomain, uint8_t* pStackStream, void** pAllocatedObject, uint32_t pCharacter, uint32_t pCount);
