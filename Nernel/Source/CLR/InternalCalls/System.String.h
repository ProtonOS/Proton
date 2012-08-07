#pragma once

#include <CLR/AppDomain.h>

void System_String_Ctor_CharPointer(AppDomain* pAppDomain, uint8_t* pStackStream, void** pAllocatedObject, uint16_t* pCharPointer);
void System_String_Ctor_SBytePointer(AppDomain* pAppDomain, uint8_t* pStackStream, void** pAllocatedObject, int8_t* pSBytePointer);
void System_String_Ctor_CharArray(AppDomain* pAppDomain, uint8_t* pStackStream, void** pAllocatedObject, void* pCharArray);
void System_String_Ctor_CharAndCount(AppDomain* pAppDomain, uint8_t* pStackStream, void** pAllocatedObject, uint32_t pChar, int32_t pCount);
void System_String_Ctor_CharArrayWithStartAndLength(AppDomain* pAppDomain, uint8_t* pStackStream, void** pAllocatedObject, void* pCharArray, int32_t pStart, int32_t pLength);
