#include <CLR/InternalCalls/System.Enum.h>
#include <CLR/JIT/Layout.h>

void System_Enum_get_value(AppDomain* pAppDomain, void* pEnumObject, void** pReturnObjectDestination)
{
	GCObject* enumObject = *(GCObject**)((size_t)pEnumObject - sizeof(GCObject*));
	IRType* primitiveType = enumObject->Type->Fields[0]->FieldType;
	uint32_t primitiveSize = JIT_GetStackSizeOfType(primitiveType);
	GC_AllocateObject(pAppDomain, primitiveType, primitiveSize, pReturnObjectDestination);
	void* primitiveObject = *pReturnObjectDestination;
	memcpy(primitiveObject, pEnumObject, primitiveSize);
}

void System_Enum_get_underlying_type(AppDomain* pAppDomain, void* pEnumTypeObject, void** pReturnObjectDestination)
{
	IRType* enumType = *(IRType**)pEnumTypeObject;
	IRType* primitiveType = enumType->Fields[0]->FieldType;
	GC_AllocateObject(pAppDomain, pAppDomain->IRAssemblies[0]->Types[pAppDomain->CachedType___System_Type->TableIndex - 1], gSizeOfPointerInBytes, pReturnObjectDestination);
	void* primitiveTypeObject = *pReturnObjectDestination;
	*((IRType**)primitiveTypeObject) = primitiveType;
}

int32_t System_Enum_compare_value_to(AppDomain* pAppDomain, void* pEnumObjectA, void* pEnumObjectB)
{
	GCObject* enumObjectA = *(GCObject**)((size_t)pEnumObjectA - sizeof(GCObject*));
	IRType* primitiveTypeA = enumObjectA->Type->Fields[0]->FieldType;
	uint32_t primitiveSizeA = JIT_GetStackSizeOfType(primitiveTypeA);

	GCObject* enumObjectB = *(GCObject**)((size_t)pEnumObjectB - sizeof(GCObject*));
	IRType* primitiveTypeB = enumObjectB->Type->Fields[0]->FieldType;
	uint32_t primitiveSizeB = JIT_GetStackSizeOfType(primitiveTypeB);

	bool_t isSigned = FALSE;
	if (primitiveTypeA->TypeDefinition == pAppDomain->CachedType___System_SByte ||
		primitiveTypeA->TypeDefinition == pAppDomain->CachedType___System_Int16 ||
		primitiveTypeA->TypeDefinition == pAppDomain->CachedType___System_Int32 ||
		primitiveTypeA->TypeDefinition == pAppDomain->CachedType___System_Int64 ||
		primitiveTypeB->TypeDefinition == pAppDomain->CachedType___System_SByte ||
		primitiveTypeB->TypeDefinition == pAppDomain->CachedType___System_Int16 ||
		primitiveTypeB->TypeDefinition == pAppDomain->CachedType___System_Int32 ||
		primitiveTypeB->TypeDefinition == pAppDomain->CachedType___System_Int64)
	{
		isSigned = TRUE;
	}

	if (isSigned)
	{
		int64_t primitiveValueA = 0;
		switch (primitiveSizeA)
		{
			case 1:
				primitiveValueA = *(int8_t*)pEnumObjectA;
				break;
			case 2:
				primitiveValueA = *(int16_t*)pEnumObjectA;
				break;
			case 4:
				primitiveValueA = *(int32_t*)pEnumObjectA;
				break;
			case 8:
				primitiveValueA = *(int64_t*)pEnumObjectA;
				break;
			default:
				Panic("WTF? This shouldn't happen");
				break;
		}
		int64_t primitiveValueB = 0;
		switch (primitiveSizeB)
		{
			case 1:
				primitiveValueB = *(int8_t*)pEnumObjectB;
				break;
			case 2:
				primitiveValueB = *(int16_t*)pEnumObjectB;
				break;
			case 4:
				primitiveValueB = *(int32_t*)pEnumObjectB;
				break;
			case 8:
				primitiveValueB = *(int64_t*)pEnumObjectB;
				break;
			default:
				Panic("WTF? This shouldn't happen");
				break;
		}
		if (primitiveValueA == primitiveValueB) return 0;
		if (primitiveValueA < primitiveValueB) return -1;
	}
	else
	{
		uint64_t primitiveValueA = 0;
		switch (primitiveSizeA)
		{
			case 1:
				primitiveValueA = *(uint8_t*)pEnumObjectA;
				break;
			case 2:
				primitiveValueA = *(uint16_t*)pEnumObjectA;
				break;
			case 4:
				primitiveValueA = *(uint32_t*)pEnumObjectA;
				break;
			case 8:
				primitiveValueA = *(uint64_t*)pEnumObjectA;
				break;
			default:
				Panic("WTF? This shouldn't happen");
				break;
		}
		uint64_t primitiveValueB = 0;
		switch (primitiveSizeB)
		{
			case 1:
				primitiveValueB = *(uint8_t*)pEnumObjectB;
				break;
			case 2:
				primitiveValueB = *(uint16_t*)pEnumObjectB;
				break;
			case 4:
				primitiveValueB = *(uint32_t*)pEnumObjectB;
				break;
			case 8:
				primitiveValueB = *(uint64_t*)pEnumObjectB;
				break;
			default:
				Panic("WTF? This shouldn't happen");
				break;
		}
		if (primitiveValueA == primitiveValueB) return 0;
		if (primitiveValueA < primitiveValueB) return -1;
	}
	return 1;
}

void System_Enum_ToObject(AppDomain* pAppDomain, void* pEnumTypeObject, void* pValueObject, void** pReturnObjectDestination)
{
	IRType* enumType = *(IRType**)pEnumTypeObject;
	IRType* primitiveType = enumType->Fields[0]->FieldType;
	uint32_t primitiveSize = JIT_GetStackSizeOfType(primitiveType);
	GC_AllocateObject(pAppDomain, enumType, primitiveSize, pReturnObjectDestination);
	void* enumObject = *pReturnObjectDestination;
	memcpy(enumObject, pValueObject, primitiveSize);
}

int32_t System_Enum_get_hashcode(AppDomain* pAppDomain, void* pEnumObject)
{
	GCObject* enumObject = *(GCObject**)((size_t)pEnumObject - sizeof(GCObject*));
	IRType* primitiveType = enumObject->Type->Fields[0]->FieldType;
	if (primitiveType->TypeDefinition == pAppDomain->CachedType___System_SByte) return *(int8_t*)pEnumObject;
	if (primitiveType->TypeDefinition == pAppDomain->CachedType___System_Byte) return *(uint8_t*)pEnumObject;
	if (primitiveType->TypeDefinition == pAppDomain->CachedType___System_Int16) return *(int16_t*)pEnumObject;
	if (primitiveType->TypeDefinition == pAppDomain->CachedType___System_UInt16 ||
		primitiveType->TypeDefinition == pAppDomain->CachedType___System_Char) return *(uint16_t*)pEnumObject;
	if (primitiveType->TypeDefinition == pAppDomain->CachedType___System_Int32) return *(int32_t*)pEnumObject;
	if (primitiveType->TypeDefinition == pAppDomain->CachedType___System_UInt32) return *(uint32_t*)pEnumObject;
	if (primitiveType->TypeDefinition == pAppDomain->CachedType___System_Int64 || 
		primitiveType->TypeDefinition == pAppDomain->CachedType___System_UInt64) return *(int32_t*)pEnumObject ^ *((int32_t*)pEnumObject + 1);
	Panic("Unknown Enum PrimitiveType");
	return 0;
}
