#include <CLR/InternalCalls/System.Runtime.CompilerServices.RuntimeHelpers.h>

int32_t System_Buffer_ByteLengthInternal(AppDomain* pAppDomain, void* pArray)
{
	GCObject* object = *(GCObject**)((size_t)pArray - sizeof(GCObject*));
	return (int32_t)object->ActualSize;
}
