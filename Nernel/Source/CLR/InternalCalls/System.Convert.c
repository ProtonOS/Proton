#include <CLR/InternalCalls/System.Convert.h>

const char gBase64EncodingTable[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
									  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
									  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
									  'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
									  'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
									  'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
									  'w', 'x', 'y', 'z', '0', '1', '2', '3',
									  '4', '5', '6', '7', '8', '9', '+', '/' };
const uint8_t gBase64DecodingTable[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0-31
									     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0, // 32-63
									     0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0, // 64-95
									     0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0, // 96-127
									     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 128-159
									     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 160-191
									     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 192-223
									     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // 223-255

void System_Convert_InternalFromBase64String(AppDomain* pAppDomain, void* pString, uint32_t pWhitespaceOnly, void** pReturnObjectDestination)
{
	uint32_t sourceLength = *(uint32_t*)pString;
	*pReturnObjectDestination = NULL;
	if (sourceLength & 0x03) return;
	if (sourceLength == 0)
	{
		*pReturnObjectDestination = pAppDomain->GarbageCollector->EmptyStringObject;
		return;
	}

	uint16_t* sourceIterator = (uint16_t*)((uint8_t*)pString + sizeof(uint32_t));
	for (uint32_t index = 0; index < sourceLength; ++index)
	{
		if (sourceIterator[index] > 0xFF) return;
		if (sourceIterator[index] != 'A' && gBase64DecodingTable[sourceIterator[index]] == 0) return;
		if (sourceIterator[index] == '=' && (index == (sourceLength - 1) || index == (sourceLength - 2))) return;
	}
	uint32_t bufferLength = (sourceLength >> 2) * 3;
	if (sourceIterator[sourceLength - 1] == '=') --bufferLength;
	if (sourceIterator[sourceLength - 2] == '=') --bufferLength;
	IRType* byteArrayType = IRAssembly_MakeArrayType(pAppDomain->IRAssemblies[0], pAppDomain->IRAssemblies[0]->Types[pAppDomain->CachedType___System_Byte->TableIndex - 1]);
	GC_AllocateArray(pAppDomain, byteArrayType, bufferLength, pReturnObjectDestination);
	uint8_t* destinationIterator = (uint8_t*)(*pReturnObjectDestination);
	for (uint32_t sourceIndex = 0, destinationIndex = 0; sourceIndex < sourceLength; )
	{
		uint32_t sextetA = gBase64DecodingTable[sourceIterator[sourceIndex++]];
		uint32_t sextetB = gBase64DecodingTable[sourceIterator[sourceIndex++]];
		uint32_t sextetC = gBase64DecodingTable[sourceIterator[sourceIndex++]];
		uint32_t sextetD = gBase64DecodingTable[sourceIterator[sourceIndex++]];
		uint32_t block = (sextetA << 18) | (sextetB << 12) | (sextetC << 6) | sextetD;
		destinationIterator[destinationIndex++] = (uint8_t)((block >> 16) & 0xFF);
		destinationIterator[destinationIndex++] = (uint8_t)((block >> 8) & 0xFF);
		destinationIterator[destinationIndex++] = (uint8_t)(block & 0xFF);
	}
}

void System_Convert_InternalFromBase64CharArray(AppDomain* pAppDomain, void* pCharArray, uint32_t pOffset, uint32_t pLength, void** pReturnObjectDestination)
{
	GCObject* arrayObject = *(GCObject**)((size_t)pCharArray - sizeof(GCObject*));
	uint32_t sourceLength = arrayObject->Array.Length;
	*pReturnObjectDestination = NULL;
	if (sourceLength & 0x03) return;
	if (sourceLength == 0)
	{
		*pReturnObjectDestination = pAppDomain->GarbageCollector->EmptyStringObject;
		return;
	}

	uint16_t* sourceIterator = (uint16_t*)pCharArray;
	for (uint32_t index = 0; index < sourceLength; ++index)
	{
		if (sourceIterator[index] > 0xFF) return;
		if (sourceIterator[index] != 'A' && gBase64DecodingTable[sourceIterator[index]] == 0) return;
		if (sourceIterator[index] == '=' && (index == (sourceLength - 1) || index == (sourceLength - 2))) return;
	}
	uint32_t bufferLength = (sourceLength >> 2) * 3;
	if (sourceIterator[sourceLength - 1] == '=') --bufferLength;
	if (sourceIterator[sourceLength - 2] == '=') --bufferLength;
	IRType* byteArrayType = IRAssembly_MakeArrayType(pAppDomain->IRAssemblies[0], pAppDomain->IRAssemblies[0]->Types[pAppDomain->CachedType___System_Byte->TableIndex - 1]);
	GC_AllocateArray(pAppDomain, byteArrayType, bufferLength, pReturnObjectDestination);
	uint8_t* destinationIterator = (uint8_t*)(*pReturnObjectDestination);
	for (uint32_t sourceIndex = 0, destinationIndex = 0; sourceIndex < sourceLength; )
	{
		uint32_t sextetA = gBase64DecodingTable[sourceIterator[sourceIndex++]];
		uint32_t sextetB = gBase64DecodingTable[sourceIterator[sourceIndex++]];
		uint32_t sextetC = gBase64DecodingTable[sourceIterator[sourceIndex++]];
		uint32_t sextetD = gBase64DecodingTable[sourceIterator[sourceIndex++]];
		uint32_t block = (sextetA << 18) | (sextetB << 12) | (sextetC << 6) | sextetD;
		destinationIterator[destinationIndex++] = (uint8_t)((block >> 16) & 0xFF);
		destinationIterator[destinationIndex++] = (uint8_t)((block >> 8) & 0xFF);
		destinationIterator[destinationIndex++] = (uint8_t)(block & 0xFF);
	}
}
