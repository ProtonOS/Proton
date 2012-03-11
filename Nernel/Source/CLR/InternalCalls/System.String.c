#include <CLR/InternalCalls/System.String.h>
#include <CLR/InternalCalls/Helpers.h>
#include <stdio.h>

ReferenceTypeObject* System_String_Ctor_CharPtr(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint16_t* pCharPtr)
{
	uint32_t sizeOfString = 0;
	uint16_t* iterator = pCharPtr;
	while (iterator != 0)
	{
		sizeOfString += 2;
		++iterator;
		if (sizeOfString >= 0x7FFFFFFF) Panic("System_String_Ctor_CharPtr parameter is not null terminated");
	}
	return GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, (uint8_t*)pCharPtr, sizeOfString);
}

ReferenceTypeObject* System_String_Ctor_CharPtrAndStartAndLength(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint16_t* pCharPtr, uint32_t pStart, uint32_t pLength)
{
	uint32_t sizeOfString = pLength * 2;
	if (sizeOfString >= 0x7FFFFFFF) Panic("System_String_Ctor_CharPtrAndStartAndLength parameter is too long");
	return GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, (uint8_t*)(pCharPtr + pStart), sizeOfString);
}

ReferenceTypeObject* System_String_Ctor_SBytePtr(AppDomain* pAppDomain, ReferenceTypeObject* pThis, int8_t* pSBytePtr)
{
	uint32_t sizeOfString = 0;
	int8_t* iterator = pSBytePtr;
	while (*iterator)
	{
		++sizeOfString;
		++iterator;
		if (sizeOfString >= 0x3FFFFFFF) Panic("System_String_Ctor_SBytePtr parameter is not null terminated");
	}
	int8_t* workspace = (int8_t*)calloc(1, sizeOfString * 2);
	for (uint32_t index = 0; index < sizeOfString; ++index)
	{
		workspace[index * 2] = pSBytePtr[index];
	}
	ReferenceTypeObject* object = GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, (uint8_t*)workspace, sizeOfString * 2);
	free(workspace);
	return object;
}

ReferenceTypeObject* System_String_Ctor_SBytePtrAndStartAndLength(AppDomain* pAppDomain, ReferenceTypeObject* pThis, int8_t* pSBytePtr, uint32_t pStart, uint32_t pLength)
{
	uint32_t sizeOfString = pLength;
	if (sizeOfString >= 0x3FFFFFFF) Panic("System_String_Ctor_SBytePtrAndStartAndLength parameter is too long");
	int8_t* workspace = (int8_t*)calloc(1, sizeOfString * 2);
	for (uint32_t index = 0; index < sizeOfString; ++index)
	{
		workspace[index * 2] = pSBytePtr[pStart + index];
	}
	ReferenceTypeObject* object = GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, (uint8_t*)workspace, sizeOfString * 2);
	free(workspace);
	return object;
}

ReferenceTypeObject* System_String_Ctor_CharAndCount(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint32_t pChar, uint32_t pCount)
{
	return GC_AllocateStringFromCharAndCount(pAppDomain->GarbageCollector, pAppDomain->RootObject, (uint16_t)pChar, pCount);
}

ReferenceTypeObject* System_String_Ctor_StringAndStartAndLength(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pString, uint32_t pStart, uint32_t pLength)
{
	uint32_t sizeOfString = pLength * 2;
	if (sizeOfString >= 0x7FFFFFFF) Panic("System_String_Ctor_StringAndStartAndLength parameter is too long");
	return GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, (((GCString*)pString->Object)->Data) + (pStart * 2), sizeOfString);
}

ReferenceTypeObject* System_String_Ctor_CharArray(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pCharArray)
{
	GCArray* header = (GCArray*)pCharArray->Object;
	uint32_t sizeOfString = header->Length * 2;
	if (sizeOfString >= 0x7FFFFFFF) Panic("System_String_Ctor_CharArray parameter is too long");
	return GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, header->Data, sizeOfString);
}

ReferenceTypeObject* System_String_Ctor_CharArrayAndStartAndLength(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pCharArray, uint32_t pStart, uint32_t pLength)
{
	GCArray* header = (GCArray*)pCharArray->Object;
	uint32_t sizeOfString = pLength * 2;
	if (sizeOfString >= 0x7FFFFFFF) Panic("System_String_Ctor_CharArrayAndStartAndLength parameter is too long");
	return GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, header->Data + (pStart * 2), sizeOfString);
}

ReferenceTypeObject* System_String_InternalConcat(AppDomain* pAppDomain, ReferenceTypeObject* pString1, ReferenceTypeObject* pString2)
{
	return GC_ConcatenateStrings(pAppDomain->GarbageCollector, pAppDomain->RootObject, pString1, pString2);
}

ReferenceTypeObject* System_String_InternalReplace(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pPattern, ReferenceTypeObject* pSubstitute)
{
	return GC_SubstituteString(pAppDomain->GarbageCollector, pAppDomain->RootObject, pThis, pPattern, pSubstitute);
}

ReferenceTypeObject* System_String_InternalTrim(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pTrimCharsArray, uint32_t pTrimType)
{
	GCString* thisHeader = (GCString*)pThis->Object;
	GCArray* trimHeader = (GCArray*)pTrimCharsArray->Object;

	uint32_t thisLength = thisHeader->Size / 2;
	uint32_t trimCount = 0;
	bool_t found = FALSE;

	if (pTrimType == 0)
	{
		for (uint32_t index = 0; index < thisLength; ++index)
		{
			found = FALSE;
			for (uint32_t index2 = 0; index2 < trimHeader->Length; ++index2)
			{
				if (*(((uint16_t*)thisHeader->Data) + index) == *(((uint16_t*)trimHeader->Data) + index2))
				{
					found = TRUE;
					break;
				}
			}
			if (found)
			{
				++trimCount;
			}
			else
			{
				break;
			}
		}
		if (trimCount == 0) return pThis;
		return GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, thisHeader->Data + (trimCount * 2), thisHeader->Size - (trimCount * 2));
	}
	for (uint32_t index = thisLength; index > 0; --index)
	{
		found = FALSE;
		for (uint32_t index2 = 0; index2 < trimHeader->Length; ++index2)
		{
			if (*(((uint16_t*)thisHeader->Data) + (index - 1)) == *(((uint16_t*)trimHeader->Data) + index2))
			{
				found = TRUE;
				break;
			}
		}
		if (found)
		{
			++trimCount;
		}
		else
		{
			break;
		}
	}
	if (trimCount == 0) return pThis;
	return GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, thisHeader->Data, thisHeader->Size - (trimCount * 2));
}


int32_t System_String_InternalIndexOf(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint32_t pChar, uint32_t pStart, uint32_t pCount, uint32_t pForwards)
{
	int32_t result = -1;
	GCString* header = (GCString*)pThis->Object;
	uint16_t* data = (uint16_t*)header->Data;
	for (uint32_t index = 0; result == -1 && index < pCount; ++index)
	{
		if ((pForwards && data[pStart + index] == pChar) ||
			(!pForwards && data[pStart - index] == pChar))
		{
			result = index;
		}
	}
	return result;
}

int32_t System_String_InternalIndexOfAny(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pCharArray, uint32_t pStart, uint32_t pCount, uint32_t pForwards)
{
	int32_t result = -1;
	GCString* thisHeader = (GCString*)pThis->Object;
	GCArray* arrayHeader = (GCArray*)pCharArray->Object;
	uint16_t* thisData = (uint16_t*)thisHeader->Data;
	uint16_t* arrayData = (uint16_t*)arrayHeader->Data;

	for (uint32_t index = 0; result == -1 && index < pCount; ++index)
	{
		for (uint32_t index2 = 0; index2 < arrayHeader->Length; ++index2)
		{
			if ((pForwards && thisData[pStart + index] == arrayData[index2]) ||
				(!pForwards && thisData[pStart - index] == arrayData[index2]))
			{
				result = index;
			}
		}
	}
	return result;
}

uint32_t System_String_Equals(AppDomain* pAppDomain, ReferenceTypeObject* pString1, ReferenceTypeObject* pString2)
{
	return pString1 == pString2;
}

uint32_t System_String_GetHashCode(AppDomain* pAppDomain, ReferenceTypeObject* pThis)
{
	GCString* header = (GCString*)pThis->Object;
	uint16_t* data = (uint16_t*)header->Data;
	uint32_t hash = 0;
	uint32_t length = header->Size / 2;
	for (uint32_t index = 0; index < length; ++index) hash = (hash << 5) - hash + data[index];
	return hash;
}

uint32_t System_String_getChars(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint32_t pIndex)
{
	GCString* header = (GCString*)pThis->Object;
	uint16_t* data = (uint16_t*)header->Data;
	return data[pIndex];
}
