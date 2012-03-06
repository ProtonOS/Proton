#include <CLR/InternalCalls/System.RuntimeType.h>
#include <CLR/InternalCalls/System.String.h>
#include <CLR/Architecture.h>
#include <stdio.h>

#define GetIRType(dest, obj) IRType* dest = (IRType*)*(size_t*)(obj)->Object;

ReferenceTypeObject* System_RuntimeType_CreateFromObject(ReferenceTypeObject* obj)
{
	AppDomain* dom = AppDomainRegistry_GetDomain(obj->DomainIndex);
	ReferenceTypeObject* Rt = GC_AllocateObject(dom->GarbageCollector, dom->RootObject, global_SizeOfPointerInBytes, dom->DomainIndex, 0, dom->CachedType___System_RuntimeType->TableIndex - 1);
	*(size_t*)Rt->Object = (size_t)dom->IRAssemblies[0]->Types[obj->TypeIndex];
	return Rt;
}

ReferenceTypeObject* System_RuntimeType_CreateFromIRType(IRType* tp)
{
	AppDomain* dom = AppDomainRegistry_GetDomain(tp->ParentAssembly->ParentDomain->DomainIndex);
	ReferenceTypeObject* Rt = GC_AllocateObject(dom->GarbageCollector, dom->RootObject, global_SizeOfPointerInBytes, dom->DomainIndex, 0, dom->CachedType___System_RuntimeType->TableIndex - 1);
	*(size_t*)Rt->Object = (size_t)tp;
	return Rt;
}

ReferenceTypeObject* System_RuntimeType_get_BaseType(AppDomain* dom, ReferenceTypeObject* pThis)
{
	GetIRType(tp, pThis);
	switch (tp->TypeDef->TypeOfExtends)
	{
		case TypeDefOrRef_Type_TypeDefinition:
			return System_RuntimeType_CreateFromIRType(tp->ParentAssembly->Types[tp->TypeDef->Extends.TypeDefinition->TableIndex - 1]);
		default:
			Panic("Unkown type of extends!");
			break;
	}
	return NULL;
}

ReferenceTypeObject* System_RuntimeType_get_Namespace(AppDomain* dom, ReferenceTypeObject* pThis)
{
	GetIRType(tp, pThis);
	return System_String_Ctor_SBytePtr(dom, 0, (int8_t*)tp->TypeDef->Namespace);
}

ReferenceTypeObject* System_RuntimeType_get_Name(AppDomain* dom, ReferenceTypeObject* pThis)
{
	GetIRType(tp, pThis);
	return System_String_Ctor_SBytePtr(dom, 0, (int8_t*)tp->TypeDef->Name);
}
