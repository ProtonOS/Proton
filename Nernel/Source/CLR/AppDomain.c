#include <CLR/AppDomain.h>
#include <CLR/ILDecomposition.h>
#include <System/Atomics.h>
#include <System/Multiboot.h>

AppDomain** gDomainRegistry = NULL;
uint32_t gDomainRegistry_NextIndex = 0;
uint8_t gDomainRegistry_Busy = 0;

void AppDomainRegistry_AddDomain(AppDomain* pDomain)
{
	Atomic_AquireLock(&gDomainRegistry_Busy);
	gDomainRegistry = (AppDomain**)realloc(gDomainRegistry, sizeof(AppDomain*) * (gDomainRegistry_NextIndex + 1));
	gDomainRegistry[gDomainRegistry_NextIndex] = pDomain;
	pDomain->DomainIndex = gDomainRegistry_NextIndex;
	gDomainRegistry_NextIndex++;
	Atomic_ReleaseLock(&gDomainRegistry_Busy);
}



AppDomain* AppDomain_Create()
{
	LoadedModule* loadedModule = Multiboot_GetLoadedModule("/gac/corlib.dll");
	if (loadedModule)
	{
		CLIFile* cliFile = CLIFile_Create((uint8_t*)loadedModule->Address, loadedModule->Size, "corlib.dll");
		if (cliFile)
		{
			AppDomain* domain = (AppDomain*)calloc(1, sizeof(AppDomain));
			Log_WriteLine(LOGLEVEL__Memory, "Memory: AppDomain_Create @ 0x%x", (unsigned int)domain);
			AppDomainRegistry_AddDomain(domain);
			AppDomain_LinkCorlib(domain, cliFile);
			//IRAssembly* assembly = ILDecomposition_CreateAssembly(cliFile, domain);
			ILDecomposition_CreateAssembly(domain, cliFile);
			return domain;
		}
		else
		{
			Panic("An error occured while loading corlib!");
		}
	}
	else
	{
		Panic("An error occured while loading corlib module!");
	}
}

void AppDomain_AddAssembly(AppDomain* pDomain, IRAssembly* pAssembly)
{
	pAssembly->AssemblyIndex = pDomain->IRAssemblyCount;
	pDomain->IRAssemblyCount++;
	pDomain->IRAssemblies = (IRAssembly**)realloc(pDomain->IRAssemblies, sizeof(IRAssembly*) * pDomain->IRAssemblyCount);
	pDomain->IRAssemblies[pDomain->IRAssemblyCount - 1] = pAssembly;
}

void AppDomain_LinkCorlib(AppDomain* pDomain, CLIFile* pCorlibFile)
{
	for (uint32_t i = 1; i <= pCorlibFile->TypeDefinitionCount; i++)
	{
		TypeDefinition* type = &(pCorlibFile->TypeDefinitions[i]);
		if (!strcmp(type->Namespace, "System"))
		{
			if (!strcmp(type->Name, "Array"))
			{
				pDomain->CachedType___System_Array = type;
			}
			else if (!strcmp(type->Name, "Boolean"))
			{
				pDomain->CachedType___System_Boolean = type;
			}
			else if (!strcmp(type->Name, "Byte"))
			{
				pDomain->CachedType___System_Byte = type;
			}
			else if (!strcmp(type->Name, "Char"))
			{
				pDomain->CachedType___System_Char = type;
			}
			else if (!strcmp(type->Name, "Double"))
			{
				pDomain->CachedType___System_Double = type;
			}
			else if (!strcmp(type->Name, "Enum"))
			{
				pDomain->CachedType___System_Enum = type;
			}
			else if (!strcmp(type->Name, "Exception"))
			{
				pDomain->CachedType___System_Exception = type;
			}
			else if (!strcmp(type->Name, "Int16"))
			{
				pDomain->CachedType___System_Int16 = type;
			}
			else if (!strcmp(type->Name, "Int32"))
			{
				pDomain->CachedType___System_Int32 = type;
			}
			else if (!strcmp(type->Name, "Int64"))
			{
				pDomain->CachedType___System_Int64 = type;
			}
			else if (!strcmp(type->Name, "IntPtr"))
			{
				pDomain->CachedType___System_IntPtr = type;
			}
			else if (!strcmp(type->Name, "Object"))
			{
				pDomain->CachedType___System_Object = type;
			}
			else if (!strcmp(type->Name, "RuntimeType"))
			{
				pDomain->CachedType___System_RuntimeType = type;
			}
			else if (!strcmp(type->Name, "SByte"))
			{
				pDomain->CachedType___System_SByte = type;
			}
			else if (!strcmp(type->Name, "Single"))
			{
				pDomain->CachedType___System_Single = type;
			}
			else if (!strcmp(type->Name, "String"))
			{
				pDomain->CachedType___System_String = type;
			}
			else if (!strcmp(type->Name, "Type"))
			{
				pDomain->CachedType___System_Type = type;
			}
			else if (!strcmp(type->Name, "TypedReference"))
			{
				pDomain->CachedType___System_TypedReference = type;
			}
			else if (!strcmp(type->Name, "UInt16"))
			{
				pDomain->CachedType___System_UInt16 = type;
			}
			else if (!strcmp(type->Name, "UInt32"))
			{
				pDomain->CachedType___System_UInt32 = type;
			}
			else if (!strcmp(type->Name, "UInt64"))
			{
				pDomain->CachedType___System_UInt64 = type;
			}
			else if (!strcmp(type->Name, "UIntPtr"))
			{
				pDomain->CachedType___System_UIntPtr = type;
			}
			else if (!strcmp(type->Name, "ValueType"))
			{
				pDomain->CachedType___System_ValueType = type;
			}
			else if (!strcmp(type->Name, "Void"))
			{
				pDomain->CachedType___System_Void = type;
			}
		}
	}
}

IRType* AppDomain_GetIRTypeFromSignatureType(AppDomain* pDomain, IRAssembly* pAssembly, SignatureType* pType)
{
	IRType* type = NULL;
	switch(pType->ElementType)
	{
		case SignatureElementType_I1:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_SByte->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_SByte);
			}
			break;
		case SignatureElementType_U1:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Byte->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Byte);
			}
			break;
		case SignatureElementType_I2:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Int16->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Int16);
			}
			break;
		case SignatureElementType_U2:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_UInt16->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_UInt16);
			}
			break;
		case SignatureElementType_I4:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Int32->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Int32);
			}
			break;
		case SignatureElementType_U4:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_UInt32->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_UInt32);
			}
			break;
		case SignatureElementType_I8:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Int64->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Int64);
			}
			break;
		case SignatureElementType_U8:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_UInt64->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_UInt64);
			}
			break;
		case SignatureElementType_R4:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Single->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Single);
			}
			break;
		case SignatureElementType_R8:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Double->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Double);
			}
			break;
		case SignatureElementType_Boolean:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Boolean->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Boolean);
			}
			break;
		case SignatureElementType_Char:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Char->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Char);
			}
			break;
		case SignatureElementType_IPointer:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_IntPtr->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_IntPtr);
			}
			break;
		case SignatureElementType_UPointer:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_UIntPtr->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_UIntPtr);
			}
			break;
		case SignatureElementType_Object:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Object->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Object);
			}
			break;
		case SignatureElementType_String:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_String->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_String);
			}
			break;
		case SignatureElementType_Type:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Type->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Type);
			}
			break;
		case SignatureElementType_Void:
			if (!(type = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Void->TableIndex - 1]))
			{
				type = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Void);
			}
			break;
		case SignatureElementType_ValueType:
		case SignatureElementType_Class:
		{
			MetadataToken* token = CLIFile_ExpandTypeDefRefOrSpecToken(pAssembly->ParentFile, pType->ClassTypeDefOrRefOrSpecToken);
			switch(token->Table)
			{
				case MetadataTable_TypeDefinition:
					if (!(type = pAssembly->Types[((TypeDefinition*)token->Data)->TableIndex - 1]))
					{
						type = IRType_Create(pAssembly, (TypeDefinition*)token->Data);
					}
					break;

				case MetadataTable_TypeReference:
					if (!(type = ((TypeReference*)token->Data)->ResolvedType->File->Assembly->Types[((TypeReference*)token->Data)->ResolvedType->TableIndex - 1]))
					{
						type = IRType_Create(((TypeReference*)token->Data)->ResolvedType->File->Assembly, ((TypeReference*)token->Data)->ResolvedType);
					}
					break;

				default:
					Panic("AppDomain_GetIRTypeFromSignatureType Invalid Class Table");
					break;
			}
			CLIFile_DestroyMetadataToken(token);
			break;
		}
		case SignatureElementType_SingleDimensionArray:
		{
			IRType* sysArrayType = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Array->TableIndex - 1];
			IRType* elementType = AppDomain_GetIRTypeFromSignatureType(pDomain, pAssembly, pType->SZArrayType);
			IRArrayType* lookupType = NULL;
			HASH_FIND(HashHandle, pAssembly->ArrayTypesHashTable, (void*)&elementType, sizeof(void*), lookupType);
			if (!lookupType)
			{
				type = IRType_Copy(sysArrayType);
				type->IsArrayType = TRUE;
				type->ArrayType = IRArrayType_Create(type, elementType);
				HASH_ADD(HashHandle, pAssembly->ArrayTypesHashTable, ElementType, sizeof(void*), type->ArrayType);
			}
			else type = lookupType->ArrayType;
			break;
		}
		case SignatureElementType_Pointer:
		{
			IRType* typePointedTo = NULL;
			if (pType->PtrVoid)
			{
				if (!(typePointedTo = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Void->TableIndex - 1]))
				{
					typePointedTo = IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Void);
				}
			}
			else
			{		
				typePointedTo = AppDomain_GetIRTypeFromSignatureType(pDomain, pAssembly, pType->PtrType);
			}
			IRPointerType* lookupType = NULL;
			HASH_FIND(HashHandle, pAssembly->PointerTypesHashTable, (void*)&typePointedTo, sizeof(void*), lookupType);
			if (!lookupType)
			{
				IRType* sysPointerType = pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_IntPtr->TableIndex - 1];
				type = IRType_Copy(sysPointerType);
				type->IsPointerType = TRUE;
				type->PointerType = IRPointerType_Create(type, typePointedTo);
				HASH_ADD(HashHandle, pAssembly->PointerTypesHashTable, TypePointedTo, sizeof(void*), type->PointerType);
			}
			else
			{
				type = lookupType->PointerType;
			}
			break;
		}
		case SignatureElementType_MethodVar:
			printf("WARNING: Generics aren't supported yet!\n");
			break;
		case SignatureElementType_Var:
			printf("WARNING: Generics aren't supported yet!\n");
			break;
		case SignatureElementType_GenericInstantiation:
			printf("WARNING: Generics aren't supported yet!\n");
			break;
		default:
			Panic("AppDomain_GetIRTypeFromSignatureType Unknown Signature Element Type");
			break;
	}
	return type;
}

IRType* AppDomain_GetIRTypeFromMetadataToken(AppDomain* pDomain, IRAssembly* pAssembly, uint32_t pToken, uint32_t* pFieldIndex)
{
	MetadataToken* token = CLIFile_ExpandMetadataToken(pAssembly->ParentFile, pToken);
	IRType* type = NULL;
	switch(token->Table)
	{
		case MetadataTable_TypeDefinition:
		{
			TypeDefinition* typeDef = (TypeDefinition*)token->Data;
			if (AppDomain_IsStructure(pDomain, typeDef))
			{
				type = IRAssembly_MakePointerType(pAssembly, pAssembly->Types[typeDef->TableIndex - 1]);
			}
			else
			{
				type = pAssembly->Types[typeDef->TableIndex - 1];
			}
			break;
		}
		case MetadataTable_TypeReference:
		{
			TypeDefinition* typeDef = ((TypeReference*)token->Data)->ResolvedType;
			if (AppDomain_IsStructure(pDomain, typeDef))
			{
				type = IRAssembly_MakePointerType(pAssembly, typeDef->File->Assembly->Types[typeDef->TableIndex - 1]);
			}
			else
			{
				type = typeDef->File->Assembly->Types[typeDef->TableIndex - 1];
			}
			break;
		}
		case MetadataTable_TypeSpecification:
			Panic("AppDomain_GetIRTypeFromMetadataToken: TypeSpecifications not yet supported\n");
			break;
		case MetadataTable_StandAloneSignature:
		{
			Panic("AppDomain_GetIRTypeFromMetadataToken: StandAloneSignatures not yet supported\n");
			break;
		}
		case MetadataTable_Field:
		{
			Field* fieldDef = (Field*)token->Data;
			TypeDefinition* typeDef = fieldDef->TypeDefinition;
			for (uint32_t index = 0; index < typeDef->FieldListCount; index++)
			{
				if (!strcmp(fieldDef->Name, typeDef->FieldList[index].Name))
				{
					if (Signature_Equals(fieldDef->Signature, fieldDef->SignatureLength, typeDef->FieldList[index].Signature, typeDef->FieldList[index].SignatureLength))
					{
						*pFieldIndex = index;
						type = pAssembly->Types[typeDef->TableIndex - 1];
						type = type->Fields[index]->FieldType;
						break;
					}
				}
			}
			if (!type)
			{
				Panic("AppDomain_GetIRTypeFromMetadataToken: Unable to resolve Field");
			}
			break;
		}
		case MetadataTable_MemberReference:
		{
			Field* fieldDef = ((MemberReference*)token->Data)->Resolved.Field;
			TypeDefinition* typeDef = fieldDef->TypeDefinition;
			for (uint32_t index = 0; index < typeDef->FieldListCount; index++)
			{
				if (!strcmp(fieldDef->Name, typeDef->FieldList[index].Name))
				{
					if (Signature_Equals(fieldDef->Signature, fieldDef->SignatureLength, typeDef->FieldList[index].Signature, typeDef->FieldList[index].SignatureLength))
					{
						*pFieldIndex = index;
						type = typeDef->File->Assembly->Types[typeDef->TableIndex - 1];
						type = type->Fields[index]->FieldType;
						break;
					}
				}
			}
			if (!type)
			{
				Panic("AppDomain_GetIRTypeFromMetadataToken: Unable to resolve MemberReference");
			}
			break;
		}
		default:
			Panic("AppDomain_GetIRTypeFromMetadataToken: Unknown Table");
			break;
	}
	CLIFile_DestroyMetadataToken(token);
	return type;
}


bool_t AppDomain_IsStructure(AppDomain* pDomain, TypeDefinition* pTypeDefinition)
{
	if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeDefinition)
	{ 
		if (pTypeDefinition->Extends.TypeDefinition == pDomain->CachedType___System_Enum) 
		{ 
			return TRUE;
		}
		if (pTypeDefinition->Extends.TypeDefinition == pDomain->CachedType___System_ValueType)
		{
			return TRUE;
		}
		return FALSE;
	}
	if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeReference)
	{ 
		if (pTypeDefinition->Extends.TypeReference->ResolvedType == pDomain->CachedType___System_Enum) 
		{ 
			return TRUE;
		}
		if (pTypeDefinition->Extends.TypeReference->ResolvedType == pDomain->CachedType___System_ValueType)
		{
			return TRUE;
		}
		return FALSE;
	}
	Log_WriteLine(LOGLEVEL__ILDecomposition, "ILDecomposition: Unknown type of extends while checking if a type is a struct");
	return FALSE;
}

void AppDomain_ResolveReferences(AppDomain* pDomain, CLIFile* pFile)
{
	for (uint32_t index = 1; index <= pFile->TypeReferenceCount; ++index)
	{
		TypeReference* typeRef = &pFile->TypeReferences[index];

		if (typeRef->ExportedType)
		{
			switch(typeRef->ExportedType->TypeOfImplementation)
			{
				case ImplementationType_File:
				{
					CLIFile* file = NULL;
					for (uint32_t i2 = 0; i2 < pDomain->IRAssemblyCount; i2++)
					{
						if (!strcmp(pDomain->IRAssemblies[i2]->ParentFile->Filename, typeRef->ExportedType->Implementation.File->Name))
						{
							file = pDomain->IRAssemblies[i2]->ParentFile;
							break;
						}
					}
					if (!file) Panic("Failed to resolve assembly reference for exported type file resolution.");
					for (uint32_t i2 = 1; i2 <= file->TypeDefinitionCount; ++i2)
					{
						if (!strcmp(file->TypeDefinitions[i2].Name , typeRef->Name) &&
							!strcmp(file->TypeDefinitions[i2].Namespace, typeRef->Namespace))
						{
							typeRef->ResolvedType = &file->TypeDefinitions[i2];
							break;
						}
					}
					if (!typeRef->ResolvedType) Panic("Failed to resolve type reference through assembly reference.");
					break;
				}
				case ImplementationType_AssemblyReference:
				{
					AssemblyReference* asmRef = typeRef->ExportedType->Implementation.AssemblyReference;
					CLIFile* file = NULL;
					for (uint32_t i2 = 0; i2 < pDomain->IRAssemblyCount; i2++)
					{
						if (!strcmp(pDomain->IRAssemblies[i2]->ParentFile->AssemblyDefinitions[1].Name, asmRef->Name))
						{
							file = pDomain->IRAssemblies[i2]->ParentFile;
							break;
						}
					}
					if (!file) Panic("Failed to resolve assembly reference for exported type assembly reference resolution.");
					for (uint32_t i2 = 1; i2 <= file->TypeDefinitionCount; ++i2)
					{
						if (!strcmp(file->TypeDefinitions[i2].Name , typeRef->Name) &&
							!strcmp(file->TypeDefinitions[i2].Namespace, typeRef->Namespace))
						{
							typeRef->ResolvedType = &file->TypeDefinitions[i2];
							break;
						}
					}
					if (!typeRef->ResolvedType) Panic("Failed to resolve type reference through assembly reference.");
					break;
				}
				default: Panic("Unhandled type reference resolution through exported type"); break;
			}
		}
		else
		{
			switch (typeRef->TypeOfResolutionScope)
			{
				case ResolutionScopeType_AssemblyReference:
				{
					AssemblyReference* asmRef = typeRef->ResolutionScope.AssemblyReference;
					CLIFile* file = NULL;
					for (uint32_t i2 = 0; i2 < pDomain->IRAssemblyCount; i2++)
					{
						if (!strcmp(pDomain->IRAssemblies[i2]->ParentFile->AssemblyDefinitions[1].Name, asmRef->Name))
						{
							file = pDomain->IRAssemblies[i2]->ParentFile;
							break;
						}
					}
					if (!file) Panic("Failed to resolve assembly reference for type reference resolution.");
					for (uint32_t i2 = 1; i2 <= file->TypeDefinitionCount; ++i2)
					{
						if (!strcmp(file->TypeDefinitions[i2].Name , typeRef->Name) &&
							!strcmp(file->TypeDefinitions[i2].Namespace, typeRef->Namespace))
						{
							typeRef->ResolvedType = &file->TypeDefinitions[i2];
							break;
						}
					}
					if (!typeRef->ResolvedType) Panic("Failed to resolve type reference through assembly reference.");
					break;
				}
				default: Panic("Unhandled type reference resolution"); break;
			}
		}
	}

	for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
	{
		MemberReference* membRef = &pFile->MemberReferences[index];

		switch (membRef->TypeOfParent)
		{
			case MemberRefParentType_TypeDefinition:
			{
				TypeDefinition* typeDef = membRef->Parent.TypeDefinition;
				bool_t isField = membRef->Signature[0] == 0x06;
				if (isField)
				{
					for (uint32_t i2 = 0; i2 <= typeDef->FieldListCount; ++i2)
					{
						if (!strcmp(typeDef->FieldList[i2].Name, membRef->Name))
						{
							membRef->TypeOfResolved = FieldOrMethodDefType_Field;
							membRef->Resolved.Field = &typeDef->FieldList[i2];
							break;
						}
					}
					if (!membRef->Resolved.Field) Panic("Failed to resolve member reference through type definition for field.");
				}
				else
				{
					for (uint32_t i2 = 0; i2 <= typeDef->MethodDefinitionListCount; ++i2)
					{
						if (!strcmp(typeDef->MethodDefinitionList[i2].Name, membRef->Name) &&
							!memcmp(typeDef->MethodDefinitionList[i2].Signature, membRef->Signature, membRef->SignatureLength))
						{
							membRef->TypeOfResolved = FieldOrMethodDefType_MethodDefinition;
							membRef->Resolved.MethodDefinition = &typeDef->MethodDefinitionList[i2];
							break;
						}
					}
					if (!membRef->Resolved.MethodDefinition) Panic("Failed to resolve member reference through type reference for method definition.");
				}
				break;
			}
			case MemberRefParentType_TypeReference:
			{
				TypeDefinition* typeDef = membRef->Parent.TypeReference->ResolvedType;
				bool_t isField = membRef->Signature[0] == 0x06;
				if (isField)
				{
					for (uint32_t i2 = 0; i2 <= typeDef->FieldListCount; ++i2)
					{
						if (!strcmp(typeDef->FieldList[i2].Name, membRef->Name))
						{
							membRef->TypeOfResolved = FieldOrMethodDefType_Field;
							membRef->Resolved.Field = &typeDef->FieldList[i2];
							break;
						}
					}
					if (!membRef->Resolved.Field) Panic("Failed to resolve member reference through type reference for field.");
				}
				else
				{
					for (uint32_t i2 = 0; i2 <= typeDef->MethodDefinitionListCount; ++i2)
					{
						if (!strcmp(typeDef->MethodDefinitionList[i2].Name, membRef->Name) &&
							!memcmp(typeDef->MethodDefinitionList[i2].Signature, membRef->Signature, membRef->SignatureLength))
						{
							membRef->TypeOfResolved = FieldOrMethodDefType_MethodDefinition;
							membRef->Resolved.MethodDefinition = &typeDef->MethodDefinitionList[i2];
							break;
						}
					}
					if (!membRef->Resolved.MethodDefinition) Panic("Failed to resolve member reference through type reference for method definition.");
				}
				break;
			}
			case MemberRefParentType_TypeSpecification:
			{
				printf("Member reference resolution through TypeSpec not yet supported!");
				break;
			}
			default: Panic("Unhandled member reference resolution"); break;
		}
	}
}

void AppDomain_ExecuteMethod(AppDomain* pDomain, IRMethod* pMethod)
{
	ILDecomposition_ConvertInstructions(pMethod);
	// TODO: Emit & Execute pMethod->AssembledMethod
}
