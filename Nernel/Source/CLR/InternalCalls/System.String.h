#pragma once

#include <CLR/AppDomain.h>

void System_String_Ctor_CharPointer(AppDomain* pAppDomain, void** pAllocatedObject, uint16_t* pCharPointer);
void System_String_Ctor_SBytePointer(AppDomain* pAppDomain, void** pAllocatedObject, int8_t* pSBytePointer);
void System_String_Ctor_CharArray(AppDomain* pAppDomain, void** pAllocatedObject, void* pCharArray);
void System_String_Ctor_CharAndCount(AppDomain* pAppDomain, void** pAllocatedObject, uint32_t pChar, int32_t pCount);
void System_String_Ctor_CharArrayWithStartAndLength(AppDomain* pAppDomain, void** pAllocatedObject, void* pCharArray, int32_t pStart, int32_t pLength);
void System_String_Ctor_CharPointerWithStartAndLength(AppDomain* pAppDomain, void** pAllocatedObject, uint16_t* pCharPointer, int32_t pStart, int32_t pLength);
void System_String_Ctor_SBytePointerWithStartAndLength(AppDomain* pAppDomain, void** pAllocatedObject, int8_t* pSBytePointer, int32_t pStart, int32_t pLength);

void System_String_InternalSplit(AppDomain* pAppDomain, void* pString, void* pSeparatorArray, int32_t pCount, int32_t pOptions, void** pReturnObjectDestination);
void System_String_InternalAllocateStr(AppDomain* pAppDomain, int32_t pLength, void** pReturnObjectDestination);
int32_t System_String_GetLOSLimit(AppDomain* pAppDomain);
