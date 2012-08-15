#include <CLR/InternalCalls/System.MonoEnumInfo.h>

void System_MonoEnumInfo_get_enum_info(AppDomain* pAppDomain, void* pType, MonoEnumInfo* pInfo)
{
	IRType* type = *(IRType**)pType;
	IRType* namesArrayType = IRAssembly_MakeArrayType(pAppDomain->IRAssemblies[0], pAppDomain->IRAssemblies[0]->Types[pAppDomain->CachedType___System_String->TableIndex - 1]);
	uint32_t elementCount = type->TypeDefinition->FieldListCount - 1;
	uint32_t elementSize = type->Fields[0]->FieldType->Size;
	GC_AllocateArray(pAppDomain, namesArrayType, elementCount, (void**)&pInfo->Names);
	IRType* valueArrayType = IRAssembly_MakeArrayType(pAppDomain->IRAssemblies[0], type->Fields[0]->FieldType);
	GC_AllocateArray(pAppDomain, valueArrayType, elementCount, &pInfo->Values);
	GC_AllocateObject(pAppDomain, pAppDomain->IRAssemblies[0]->Types[pAppDomain->CachedType___System_Type->TableIndex - 1], sizeof(IRType*), &pInfo->Type);
	*((IRType**)pInfo->Type) = type->Fields[0]->FieldType;
	for (uint32_t fieldIndex = 0, elementIndex = 0; fieldIndex < type->TypeDefinition->FieldListCount; ++fieldIndex)
	{
		Field* field = &type->TypeDefinition->FieldList[fieldIndex];
		if (field->Flags & FieldAttributes_Static)
		{
			GC_AllocateStringFromASCII(pAppDomain, (int8_t*)field->Name, strlen(field->Name), &pInfo->Names[elementIndex]);
			switch (elementSize)
			{
				case 1:
					((uint8_t*)pInfo->Values)[elementIndex] = *field->Constant->Value;
					break;
				case 2:
					((uint16_t*)pInfo->Values)[elementIndex] = *(uint16_t*)field->Constant->Value;
					break;
				case 4:
					((uint32_t*)pInfo->Values)[elementIndex] = *(uint32_t*)field->Constant->Value;
					break;
				case 8:
					((uint64_t*)pInfo->Values)[elementIndex] = *(uint64_t*)field->Constant->Value;
					break;
				default: Panic("System_MonoEnumInfo_get_enum_info");
			}
			++elementIndex;
		}
	}
}
