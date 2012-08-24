#include <CLR/ManagedPlugs.h>


typedef struct MethodEntry
{
	IRMethod* PlugMethod;

	UT_hash_handle HashHandle;
} MethodEntry;

typedef struct TypeEntry
{
	TypeDefinition* TypeDef;

	MethodEntry* MethodTable;

	UT_hash_handle HashHandle;
} TypeEntry;

typedef struct NamespaceEntry
{
	const char* Namespace;

	TypeEntry* TypeTable;

	UT_hash_handle HashHandle;
} NamespaceEntry;

NamespaceEntry* global_NamespaceTable;


void ManagedPlugs_LoadAssembly(AppDomain* pDomain, IRAssembly* pAssembly)
{
	if (pAssembly->ParentFile->AssemblyDefinitionCount != 1)
		Panic("Don't know how to handle this assembly def count!");
	bool_t isPlugAssembly = FALSE;
	for (uint32_t i = 0; i < pAssembly->ParentFile->AssemblyDefinitions[1].CustomAttributeCount; i++)
	{
		CustomAttribute* customAttr = pAssembly->ParentFile->AssemblyDefinitions[1].CustomAttributes[i];
		MethodDefinition* methodDef = NULL;
		switch(customAttr->TypeOfType)
		{
			case CustomAttributeType_MethodDefinition:
				methodDef = customAttr->Type.MethodDefinition;
				break;
			case CustomAttributeType_MemberReference:
				AppDomain_ResolveMemberReference(pDomain, pAssembly->ParentFile, customAttr->Type.MemberReference);
				methodDef = customAttr->Type.MemberReference->Resolved.MethodDefinition;
				break;
			default:
				Panic("Invalid TypeOfType!");
				break;
		}
		
		if (!strcmp(methodDef->TypeDefinition->Namespace, "Proton.Plugs") && !strcmp(methodDef->TypeDefinition->Name, "PlugAssembly"))
		{
			isPlugAssembly = TRUE;
			break;
		}
	}
	if (!isPlugAssembly)
		return;

	for (uint32_t i = 1; i <= pAssembly->ParentFile->TypeDefinitionCount; i++)
	{
		bool_t isPlugType = FALSE;
		TypeDefinition* pluggedType = NULL;
		for (uint32_t i2 = 0; i2 < pAssembly->ParentFile->TypeDefinitions[i].CustomAttributeCount; i2++)
		{
			CustomAttribute* customAttr = pAssembly->ParentFile->TypeDefinitions[i].CustomAttributes[i2];
			MethodDefinition* methodDef = NULL;
			switch(customAttr->TypeOfType)
			{
				case CustomAttributeType_MethodDefinition:
					methodDef = customAttr->Type.MethodDefinition;
					break;
				case CustomAttributeType_MemberReference:
					AppDomain_ResolveMemberReference(pDomain, pAssembly->ParentFile, customAttr->Type.MemberReference);
					methodDef = customAttr->Type.MemberReference->Resolved.MethodDefinition;
					break;
				default:
					Panic("Invalid TypeOfType!");
					break;
			}
		
			if (!strcmp(methodDef->TypeDefinition->Namespace, "Proton.Plugs") && !strcmp(methodDef->TypeDefinition->Name, "PlugType"))
			{
				isPlugType = TRUE;
				CustomAttributeSignature* sig = CustomAttributeSignature_Expand((uint8_t*)customAttr->Value, customAttr, pAssembly->ParentFile, pAssembly->ParentDomain);
				pluggedType = AppDomain_GetTypeDefByCanonicalName(pAssembly->ParentDomain, pAssembly, sig->FixedArgs[0].Elements[0].Type.CanonicalTypeName);
				CustomAttributeSignature_Destroy(sig);
				break;
			}
		}

		if (isPlugType)
		{
			printf("Found a plug type called %s.%s which is plugging %s.%s\n", pAssembly->ParentFile->TypeDefinitions[i].Namespace, pAssembly->ParentFile->TypeDefinitions[i].Name, pluggedType->Namespace, pluggedType->Name);
		}
	}
}