#include <CLR/InternalCalls/System.String.h>

void System_String_Ctor_CharPointer(AppDomain* pAppDomain, uint8_t* pStackStream, void** pAllocatedObject, uint16_t* pCharPointer)
{
	uint32_t length = 0;
	for (uint16_t* iterator = 0; *iterator; ++iterator, ++length);
	GC_AllocateStringFromUnicode(pAppDomain, pStackStream, pCharPointer, length, pAllocatedObject);
}

void System_String_Ctor_SBytePointer(AppDomain* pAppDomain, uint8_t* pStackStream, void** pAllocatedObject, int8_t* pSBytePointer)
{
	uint32_t length = 0;
	for (int8_t* iterator = 0; *iterator; ++iterator, ++length);
	GC_AllocateStringFromASCII(pAppDomain, pStackStream, pSBytePointer, length, pAllocatedObject);
}

void System_String_Ctor_CharArray(AppDomain* pAppDomain, uint8_t* pStackStream, void** pAllocatedObject, void* pCharArray)
{
	GCObject* object = *(GCObject**)((size_t)pCharArray - sizeof(GCObject*));
	GC_AllocateStringFromUnicode(pAppDomain, pStackStream, (uint16_t*)pCharArray, object->Array.Length, pAllocatedObject);
}

void System_String_Ctor_CharAndCount(AppDomain* pAppDomain, uint8_t* pStackStream, void** pAllocatedObject, uint32_t pChar, int32_t pCount)
{
	uint16_t* unicodeData = (uint16_t*)calloc(1, sizeof(uint16_t) * pCount);
	for (int32_t index = 0; index < pCount; ++index) unicodeData[index] = (uint16_t)pChar;
	GC_AllocateStringFromUnicode(pAppDomain, pStackStream, unicodeData, pCount, pAllocatedObject);
	free(unicodeData);
}

void System_String_Ctor_CharArrayWithStartAndLength(AppDomain* pAppDomain, uint8_t* pStackStream, void** pAllocatedObject, void* pCharArray, int32_t pStart, int32_t pLength)
{
	GC_AllocateStringFromUnicode(pAppDomain, pStackStream, ((uint16_t*)pCharArray) + pStart, pLength, pAllocatedObject);
}

