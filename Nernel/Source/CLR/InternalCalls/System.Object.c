#include <CLR/InternalCalls/System.Object.h>
#include <CLR/JIT/Layout.h>

void System_Object_GetType(AppDomain* pAppDomain, void* pObject, void** pReturnObjectDestination)
{
	GCObject* object = *(GCObject**)((size_t)pObject - sizeof(GCObject*));
	GC_AllocateObject(pAppDomain, pAppDomain->IRAssemblies[0]->Types[pAppDomain->CachedType___System_Type->TableIndex - 1], gSizeOfPointerInBytes, pReturnObjectDestination);
	void* objectTypeObject = *pReturnObjectDestination;
	*((IRType**)objectTypeObject) = object->Type;
}

int32_t System_Object_InternalGetHashCode(AppDomain* pAppDomain, void* pObject)
{
	return (int32_t)pObject * 2654435761u;
}

size_t System_Object_obj_address(AppDomain* pAppDomain, void* pObject)
{
	return (size_t)pObject;
}
