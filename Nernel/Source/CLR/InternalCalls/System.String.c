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
		if (sizeOfString >= 0x7FFFFFFF) Panic("System_String_CtorCharPtr parameter is not null terminated");
	}
	return GC_AllocateString(pAppDomain->GarbageCollector, pAppDomain->RootObject, (uint8_t*)pCharPtr, sizeOfString);
}

ReferenceTypeObject* System_String_InternalConcat(AppDomain* pAppDomain, ReferenceTypeObject* pString1, ReferenceTypeObject* pString2)
{
	return GC_ConcatenateStrings(pAppDomain->GarbageCollector, pAppDomain->RootObject, pString1, pString2);
}

ReferenceTypeObject* System_String_InternalReplace(AppDomain* pAppDomain, ReferenceTypeObject* pThis, ReferenceTypeObject* pPattern, ReferenceTypeObject* pSubstitute)
{
	printf("InternalReplace: SizeOf pThis Object = %u @ 0x%x\n", (unsigned int)pThis->Size, (unsigned int)pThis);
	return GC_SubstituteString(pAppDomain->GarbageCollector, pAppDomain->RootObject, pThis, pPattern, pSubstitute);
}
