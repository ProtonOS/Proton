#include <CLR/InternalCalls/System.Runtime.CompilerServices.RuntimeHelpers.h>

int32_t System_Buffer_ByteLengthInternal(AppDomain* pAppDomain, void* pArray)
{
	GCObject* object = *(GCObject**)((size_t)pArray - sizeof(GCObject*));
	return (int32_t)object->ActualSize;
}

uint32_t System_Buffer_GetByteInternal(AppDomain* pAppDomain, void* pArray, uint32_t pIndex)
{
	return *((uint8_t*)pArray + pIndex);
}

void System_Buffer_SetByteInternal(AppDomain* pAppDomain, void* pArray, uint32_t pIndex, uint32_t pValue)
{
	*((uint8_t*)pArray + pIndex) = (uint8_t)pValue;
}

uint32_t System_Buffer_BlockCopyInternal(AppDomain* pAppDomain, void* pSourceArray, uint32_t pSourceOffset, void* pDestinationArray, uint32_t pDestinationOffset, uint32_t pCount)
{
	GCObject* sourceObject = *(GCObject**)((size_t)pSourceArray - sizeof(GCObject*));
	GCObject* destinationObject = *(GCObject**)((size_t)pDestinationArray - sizeof(GCObject*));
	if (pSourceOffset + pCount >= sourceObject->Array.Length) return FALSE;
	if (pDestinationOffset + pCount >= destinationObject->Array.Length) return FALSE;
	memcpy((uint8_t*)pDestinationArray + pDestinationOffset, (uint8_t*)pSourceArray + pSourceOffset, pCount);
	return TRUE;
}
