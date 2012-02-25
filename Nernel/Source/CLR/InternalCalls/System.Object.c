#include <CLR/InternalCalls/System.Object.h>
#include <CLR/InternalCalls/Helpers.h>

bool_t System_Object_Equals(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pObject)
{
	return pThis == pObject;
}

uint32_t System_Object_GetHashCode(AppDomain* pAppDomain, ReferenceTypeObject* pThis)
{
	return ((uint32_t)pThis >> 2) * 2654435761UL;
}

