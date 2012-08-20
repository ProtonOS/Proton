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


void System_String_InternalSplit(AppDomain* pAppDomain, void* pString, void* pSeparatorArray, int32_t pCount, int32_t pOptions, void** pReturnObjectDestination)
{
	uint32_t stringLength = *(uint32_t*)pString;
	IRType* stringArrayType = IRAssembly_MakeArrayType(pAppDomain->IRAssemblies[0], pAppDomain->IRAssemblies[0]->Types[pAppDomain->CachedType___System_String->TableIndex - 1]);
	if (pCount == 0 || (pOptions == 1 && stringLength == 0))
	{
		GC_AllocateArray(pAppDomain, stringArrayType, 0, pReturnObjectDestination);
		return;
	}
	void** stringArrayObject = NULL;
	if (pCount == 1)
	{
		GC_AllocateArray(pAppDomain, stringArrayType, 1, pReturnObjectDestination);
		stringArrayObject = (void**)*pReturnObjectDestination;
		stringArrayObject[0] = pString;
		return;
	}
	uint16_t* stringChars = (uint16_t*)((size_t)pString + 4);
	GCObject* separatorArrayObject = *(GCObject**)((size_t)pSeparatorArray - sizeof(GCObject*));
	uint32_t separatorArrayLength = separatorArrayObject->Array.Length;
	uint16_t* separatorArrayChars = (uint16_t*)pSeparatorArray;

	uint16_t** tempStrings = NULL;
	uint32_t* tempStringLengths = NULL;
	int32_t realCount = 0;
	uint32_t startIndex = 0;
	for (uint32_t index = 0; index < stringLength; ++index)
	{
		for (uint32_t separatorIndex = 0; separatorIndex < separatorArrayLength; ++separatorIndex)
		{
			if (stringChars[index] == separatorArrayChars[separatorIndex])
			{
				uint32_t splitLength = index - startIndex;
				bool_t splitIncluded = (pOptions & 1) == 0 || splitLength > 0;
				if (splitIncluded)
				{
					tempStrings = (uint16_t**)realloc(tempStrings, sizeof(uint16_t*) * (realCount + 1));
					tempStringLengths = (uint32_t*)realloc(tempStringLengths, sizeof(uint32_t) * (realCount + 1));
					tempStringLengths[realCount] = splitLength;
					if (tempStringLengths[realCount] > 0)
					{
						tempStrings[realCount] = (uint16_t*)calloc(1, sizeof(uint16_t) * splitLength);
						memcpy(tempStrings[realCount], stringChars + startIndex, splitLength << 1);
					}
				}
				startIndex = index + 1;
				if (splitIncluded)
				{
					++realCount;
					if (realCount == (pCount - 1))
					{
						tempStrings = (uint16_t**)realloc(tempStrings, sizeof(uint16_t*) * (realCount + 1));
						tempStringLengths = (uint32_t*)realloc(tempStringLengths, sizeof(uint32_t) * (realCount + 1));
						tempStringLengths[realCount] = stringLength - startIndex;
						if (tempStringLengths[realCount])
						{
							tempStrings[realCount] = (uint16_t*)calloc(1, sizeof(uint16_t) * tempStringLengths[realCount]);
							memcpy(tempStrings[realCount], stringChars + startIndex, tempStringLengths[realCount] << 1);
						}
						++realCount;
						index = stringLength - 1;
						break;
					}
				}
			}
		}
	}
	if (startIndex < stringLength)
	{
		tempStrings = (uint16_t**)realloc(tempStrings, sizeof(uint16_t*) * (realCount + 1));
		tempStringLengths = (uint32_t*)realloc(tempStringLengths, sizeof(uint32_t) * (realCount + 1));
		tempStringLengths[realCount] = stringLength - startIndex;
		tempStrings[realCount] = (uint16_t*)calloc(1, sizeof(uint16_t) * tempStringLengths[realCount]);
		memcpy(tempStrings[realCount], stringChars + startIndex, tempStringLengths[realCount] << 1);
		++realCount;
	}
	GC_AllocateArray(pAppDomain, stringArrayType, realCount, pReturnObjectDestination);
	//printf("StrArr @ 0x%X\n", (unsigned int)*pReturnObjectDestination);
	stringArrayObject = (void**)(*pReturnObjectDestination);
	for (int32_t index = 0; index < realCount; ++index)
	{
		if (tempStrings[index])
		{
			GC_AllocateStringFromUnicode(pAppDomain, tempStrings[index], tempStringLengths[index], &stringArrayObject[index]);
			//printf("Str @ 0x%X\n", (unsigned int)stringArrayObject[index]);
			free(tempStrings[index]);
		}
		else stringArrayObject[index] = pAppDomain->GarbageCollector->EmptyStringObject;
	}
	if (tempStrings) free(tempStrings);
	if (tempStringLengths) free(tempStringLengths);
}

void System_String_InternalAllocateStr(AppDomain* pAppDomain, int32_t pLength, void** pReturnObjectDestination)
{
	GC_AllocateEmptyStringFromLength(pAppDomain, pLength, pReturnObjectDestination);
}

void System_String_InternalIntern(AppDomain* pAppDomain, void* pString, void** pReturnObjectDestination)
{
	GCObject* object = *(GCObject**)((size_t)pString - sizeof(GCObject*));
	if (!(object->Flags & GCObjectFlags_Interned))
	{
		object->Flags |= GCObjectFlags_Interned;
		uint32_t size = (*(uint32_t*)pString) << 1;
		HASH_ADD_KEYPTR(String.HashHandle, pAppDomain->GarbageCollector->StringHashTable, ((uint8_t*)pString + 4), size, object);
	}
	*pReturnObjectDestination = pString;
}

void System_String_InternalIsInterned(AppDomain* pAppDomain, void* pString, void** pReturnObjectDestination)
{
	GCObject* object = *(GCObject**)((size_t)pString - sizeof(GCObject*));
	*pReturnObjectDestination = NULL;
	if (object->Flags & GCObjectFlags_Interned) *pReturnObjectDestination = pString;
}

int32_t System_String_GetLOSLimit(AppDomain* pAppDomain)
{
	return 0x7FFFFFFF;
}
