#include <CLR/InternalCalls/System.String.h>
#include <CLR/InternalCalls/Helpers.h>

ReferenceTypeObject* System_String_InternalConcat(AppDomain* pAppDomain, ReferenceTypeObject* pString1, ReferenceTypeObject* pString2)
{
	return GC_ConcatenateStrings(pAppDomain->GarbageCollector, pAppDomain->RootObject, pString1, pString2);
}

ReferenceTypeObject* System_String_InternalReplace(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pPattern, ReferenceTypeObject* pSubstitute)
{
	return GC_SubstituteString(pAppDomain->GarbageCollector, pAppDomain->RootObject, pThis, pPattern, pSubstitute);
}
