#include <CLR/InternalCalls/System.Array.h>
#include <CLR/InternalCalls/Helpers.h>

ReferenceTypeObject* System_Array_InternalGetValue(AppDomain* pAppDomain, ReferenceTypeObject* pThis, uint32_t pIndex)
{
	GCArray* header = (GCArray*)pThis->Object;
	ReferenceTypeObject** data = (ReferenceTypeObject**)header->Data;
	return data[pIndex];
}

uint32_t System_Array_InternalSetValue(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pValue, uint32_t pIndex)
{
	GCArray* header = (GCArray*)pThis->Object;
	ReferenceTypeObject** data = (ReferenceTypeObject**)header->Data;
	if (data[pIndex]) ReferenceTypeObject_RemoveReference(data[pIndex], pThis);
	data[pIndex] = pValue;
	if (pValue) ReferenceTypeObject_AddReference(pValue, pThis);
	return TRUE;
}

void System_Array_Clear(AppDomain* pAppDomain, ReferenceTypeObject* pArray, uint32_t pIndex, uint32_t pLength)
{
	GCArray* header = (GCArray*)pArray->Object;
	ReferenceTypeObject** data = (ReferenceTypeObject**)header->Data;
	for (uint32_t index = 0; index < pLength; ++index)
	{
		if (data[pIndex + index])
		{
			ReferenceTypeObject_RemoveReference(data[pIndex + index], pArray);
			data[pIndex + index] = NULL;
		}
	}
}

uint32_t System_Array_InternalCopy(AppDomain* pAppDomain, ReferenceTypeObject* pSource, uint32_t pSourceIndex, ReferenceTypeObject* pDestination, uint32_t pDestinationIndex, uint32_t pLength)
{
	GCArray* sourceHeader = (GCArray*)pSource->Object;
	GCArray* destinationHeader = (GCArray*)pDestination->Object;
	if (pSourceIndex + pLength > sourceHeader->Length || pDestinationIndex + pLength > destinationHeader->Length) return FALSE;
	ReferenceTypeObject** sourceData = (ReferenceTypeObject**)sourceHeader->Data;
	ReferenceTypeObject** destinationData = (ReferenceTypeObject**)destinationHeader->Data;

	for (uint32_t index = 0; index < pLength; ++index)
	{
		if (destinationData[pDestinationIndex + index])
		{
			ReferenceTypeObject_RemoveReference(destinationData[pDestinationIndex + index], pDestination);
		}
		destinationData[pDestinationIndex + index] = sourceData[pSourceIndex + index];
		if (destinationData[pDestinationIndex + index])
		{
			ReferenceTypeObject_AddReference(destinationData[pDestinationIndex + index], pDestination);
		}
	}
	return TRUE;
}

void System_Array_Reverse(AppDomain* pAppDomain, ReferenceTypeObject* pArray, uint32_t pIndex, uint32_t pLength)
{
	GCArray* header = (GCArray*)pArray->Object;
	if (pIndex + pLength > header->Length) return;
	ReferenceTypeObject** data = (ReferenceTypeObject**)header->Data;
	uint32_t count = pLength / 2;
	for (uint32_t index = 0; index < count; ++index)
	{
		ReferenceTypeObject* temp = data[pIndex + index];
		data[pIndex + index] = data[pIndex + (pLength - (index + 1))];
		data[pIndex + (pLength - (index + 1))] = temp;
	}
}
