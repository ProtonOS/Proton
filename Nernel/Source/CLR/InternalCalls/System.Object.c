#include <CLR/ReferenceTypeObject.h>
#include <CLR/InternalCalls/System.Object.h>
#include <CLR/InternalCalls/Helpers.h>

void System_Object_Equals(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(bool_t*)pReturn = INTERNAL_CALL_PARAM(0, ReferenceTypeObject*) == INTERNAL_CALL_PARAM(1, ReferenceTypeObject*);
}

void System_Object_GetHashCode(AppDomain* pAppDomain, uint32_t pArgCount, void** pArgs, void* pReturn)
{
	*(uint32_t*)pReturn = ((uint32_t)INTERNAL_CALL_PARAM(0, ReferenceTypeObject*) >> 2) * 2654435761UL;
}

