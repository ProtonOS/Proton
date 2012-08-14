#include <CLR/InternalCalls/System.Array.h>

uint32_t System_Array_FastCopy(AppDomain* pAppDomain, void* pSourceArray, uint32_t pSourceIndex, void* pDestinationArray, uint32_t pDestinationIndex, uint32_t pLength)
{
	GCObject* sourceObject = *(GCObject**)((size_t)pSourceArray - sizeof(GCObject*));
	GCObject* destinationObject = *(GCObject**)((size_t)pDestinationArray - sizeof(GCObject*));
	// These should throw ArgumentException, returning FALSE is used to deal with complex multidimensional arrays
	if (pSourceIndex + pLength >= sourceObject->Array.Length) return FALSE;
	if (pDestinationIndex + pLength >= destinationObject->Array.Length) return FALSE;
	uint32_t sizeOfElement = sourceObject->Array.ElementType->Size;
	uint32_t sizeToCopy = sizeOfElement * pLength;
	void* sourceStart = (uint8_t*)pSourceArray + (sizeOfElement * pSourceIndex);
	void* destinationStart = (uint8_t*)pDestinationArray + (sizeOfElement * pDestinationIndex);
	// This ensures that our copy of data occurs atomically as far as the GC is concerned,
	// and it won't potentially see a destination array with half written invalid references
	Thread_EnterCriticalRegion();
	memcpy(destinationStart, sourceStart, sizeToCopy);
	Thread_LeaveCriticalRegion();
	return TRUE;
}

int32_t System_Array_GetRank(AppDomain* pAppDomain, void* pArray)
{
	// This should NOT be changed to an intrinsic, as this must support
	// multidimensional arrays later, but is also required for single
	// dimension arrays to work correctly
	return 1;
}

int32_t System_Array_GetLength(AppDomain* pAppDomain, void* pArray, uint32_t pDimension)
{
	if (pDimension != 0) Panic("System_Array_GetLength Multidimension Arrays not yet supported");
	GCObject* object = *(GCObject**)((size_t)pArray - sizeof(GCObject*));
	return object->Array.Length;
}

int32_t System_Array_GetLowerBound(AppDomain* pAppDomain, void* pArray, uint32_t pDimension)
{
	if (pDimension != 0) Panic("System_Array_GetLowerBound Multidimension Arrays not yet supported");
	// This should NOT be changed to an intrinsic, as this must support
	// multidimensional arrays later, but is also required for single
	// dimension arrays to work correctly
	return 0;
}

void System_Array_GetValueImpl(AppDomain* pAppDomain, void* pArray, uint32_t pIndex, void** pReturnObjectDestination)
{
	GCObject* arrayObject = *(GCObject**)((size_t)pArray - sizeof(GCObject*));
	IRType* elementType = arrayObject->Array.ElementType;
	if (elementType->IsReferenceType)
	{
		*pReturnObjectDestination = *(size_t**)((uint8_t*)pArray + (sizeof(size_t*) * pIndex));
		return;
	}
	uint32_t sizeOfElement = elementType->Size;
	GC_AllocateObject(pAppDomain, elementType, sizeOfElement, pReturnObjectDestination);
	GCObject* object = *(GCObject**)((size_t)*pReturnObjectDestination - sizeof(GCObject*));
	Thread_EnterCriticalRegion();
	memcpy(object->Data, (uint8_t*)pArray + (sizeOfElement * pIndex), sizeOfElement);
	Thread_LeaveCriticalRegion();
}

void System_Array_SetValueImpl(AppDomain* pAppDomain, void* pArray, void* pValue, uint32_t pIndex)
{
	GCObject* arrayObject = *(GCObject**)((size_t)pArray - sizeof(GCObject*));
	IRType* elementType = arrayObject->Array.ElementType;
	if (elementType->IsReferenceType)
	{
		*(void**)((uint8_t*)pArray + (sizeof(size_t*) * pIndex)) = pValue;
		return;
	}
	uint32_t sizeOfElement = elementType->Size;
	Thread_EnterCriticalRegion();
	memcpy((uint8_t*)pArray + (sizeOfElement * pIndex), pValue, sizeOfElement);
	Thread_LeaveCriticalRegion();
}

void System_Array_ClearInternal(AppDomain* pAppDomain, void* pArray, uint32_t pIndex, uint32_t pCount)
{
	GCObject* object = *(GCObject**)((size_t)pArray - sizeof(GCObject*));
	if (pIndex + pCount >= object->Array.Length) return; // Should throw ArgumentException
	uint32_t sizeOfElement = object->Array.ElementType->Size;
	uint32_t sizeToClear = sizeOfElement * pCount;
	void* start = (uint8_t*)pArray + (sizeOfElement * pIndex);
	// Same reason for this as FastCopy, see comments above
	Thread_EnterCriticalRegion();
	memset(start, 0x00, sizeToClear);
	Thread_LeaveCriticalRegion();
}
