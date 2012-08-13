#include <CLR/InternalCalls/System.String.h>

void System_String_Ctor_CharPointer(AppDomain* pAppDomain, void** pAllocatedObject, uint16_t* pCharPointer)
{
	uint32_t length = 0;
	for (uint16_t* iterator = pCharPointer; *iterator; ++iterator, ++length);
	GC_AllocateStringFromUnicode(pAppDomain, pCharPointer, length, pAllocatedObject);
}

void System_String_Ctor_SBytePointer(AppDomain* pAppDomain, void** pAllocatedObject, int8_t* pSBytePointer)
{
	uint32_t length = 0;
	for (int8_t* iterator = pSBytePointer; *iterator; ++iterator, ++length);
	GC_AllocateStringFromASCII(pAppDomain, pSBytePointer, length, pAllocatedObject);
}

void System_String_Ctor_CharArray(AppDomain* pAppDomain, void** pAllocatedObject, void* pCharArray)
{
	GCObject* object = *(GCObject**)((size_t)pCharArray - sizeof(GCObject*));
	GC_AllocateStringFromUnicode(pAppDomain, (uint16_t*)pCharArray, object->Array.Length, pAllocatedObject);
}

void System_String_Ctor_CharAndCount(AppDomain* pAppDomain, void** pAllocatedObject, uint32_t pChar, int32_t pCount)
{
	uint16_t* unicodeData = (uint16_t*)calloc(1, sizeof(uint16_t) * pCount);
	for (int32_t index = 0; index < pCount; ++index) unicodeData[index] = (uint16_t)pChar;
	GC_AllocateStringFromUnicode(pAppDomain, unicodeData, pCount, pAllocatedObject);
	free(unicodeData);
}

void System_String_Ctor_CharArrayWithStartAndLength(AppDomain* pAppDomain, void** pAllocatedObject, void* pCharArray, int32_t pStart, int32_t pLength)
{
	GC_AllocateStringFromUnicode(pAppDomain, ((uint16_t*)pCharArray) + pStart, pLength, pAllocatedObject);
}

void System_String_Ctor_CharPointerWithStartAndLength(AppDomain* pAppDomain, void** pAllocatedObject, uint16_t* pCharPointer, int32_t pStart, int32_t pLength)
{
	GC_AllocateStringFromUnicode(pAppDomain, pCharPointer + pStart, pLength, pAllocatedObject);
}

void System_String_Ctor_SBytePointerWithStartAndLength(AppDomain* pAppDomain, void** pAllocatedObject, int8_t* pSBytePointer, int32_t pStart, int32_t pLength)
{
	GC_AllocateStringFromASCII(pAppDomain, pSBytePointer + pStart, pLength, pAllocatedObject);
}
