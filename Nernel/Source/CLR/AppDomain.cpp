#include "AppDomain.h"
#include "../Kernel/Atomics.h"
#include "CLIFile.h"

CLR::AppDomain** CLR::AppDomain::sDomains = nullptr;
uint32_t CLR::AppDomain::sDomainCount = 0;
uint8_t CLR::AppDomain::sBusy = 0;

void CLR::AppDomain::Register(AppDomain* pAppDomain)
{
	AtomicAquireLock(&sBusy);
	uint32_t emptyIndex = 0;
	if (!sDomains)
	{
		sDomains = new AppDomain*[1];
		sDomainCount = 1;
	}
	else
	{
		bool foundEmptyIndex = false;
		for (uint32_t index = 0; !foundEmptyIndex && index < sDomainCount; ++index)
		{
			if (!sDomains[index])
			{
				emptyIndex = index;
				foundEmptyIndex = true;
			}
		}
		if (!foundEmptyIndex)
		{
			AppDomain** oldDomainArray = sDomains;
			emptyIndex = sDomainCount;
			sDomains = new AppDomain*[sDomainCount + 1];
			for (uint32_t index = 0; index < sDomainCount; ++index)
			{
				sDomains[index] = oldDomainArray[index];
			}
			delete [] oldDomainArray;
			sDomains[sDomainCount] = nullptr;
			++sDomainCount;
		}
	}
	pAppDomain->DomainIndex = emptyIndex;
	sDomains[pAppDomain->DomainIndex] = pAppDomain;
	AtomicReleaseLock(&sBusy);
}

CLR::AppDomain::AppDomain()
{
	Multiboot::LoadedModule* loadedModule = Multiboot::GetLoadedModule("/gac/mscorlib.dll");
	if (!loadedModule) Panic("Unable to get loaded corlib module");
	CLIFile* cliFile = new CLIFile();
	if (!cliFile->Load((uint8_t*)loadedModule->Address, loadedModule->Size, "mscorlib.dll")) Panic("Unable to load corlib");
	AssemblyCount = 0;
	Assemblies = nullptr;

	Register(this);
	LinkCorlib(cliFile);
}

CLR::AppDomain::~AppDomain()
{
	AtomicAquireLock(&sBusy);
	sDomains[DomainIndex] = nullptr;
	AtomicReleaseLock(&sBusy);
}

void CLR::AppDomain::LinkCorlib(CLIFile* pFile)
{
	for (uint32_t i = 1; i <= pFile->TypeDefinitionCount; i++)
	{
		TypeDefinitionData* type = &(pFile->TypeDefinitions[i]);
		if (!strcmp(type->Namespace, "System"))
		{
			if (!strcmp(type->Name, "Array")) { CachedType___System_Array = type; }
			else if (!strcmp(type->Name, "Boolean")) { CachedType___System_Boolean = type; }
			else if (!strcmp(type->Name, "Byte")) { CachedType___System_Byte = type; }
			else if (!strcmp(type->Name, "Char")) { CachedType___System_Char = type; }
			else if (!strcmp(type->Name, "Double")) { CachedType___System_Double = type; }
			else if (!strcmp(type->Name, "Enum")) { CachedType___System_Enum = type; }
			else if (!strcmp(type->Name, "Exception")) { CachedType___System_Exception = type; }
			else if (!strcmp(type->Name, "Int16")) { CachedType___System_Int16 = type; }
			else if (!strcmp(type->Name, "Int32")) { CachedType___System_Int32 = type; }
			else if (!strcmp(type->Name, "Int64")) { CachedType___System_Int64 = type; }
			else if (!strcmp(type->Name, "IntPtr")) { CachedType___System_IntPtr = type; }
			else if (!strcmp(type->Name, "Object")) { CachedType___System_Object = type; }
			else if (!strcmp(type->Name, "RuntimeFieldHandle")) { CachedType___System_RuntimeFieldHandle = type; }
			else if (!strcmp(type->Name, "RuntimeMethodHandle")) { CachedType___System_RuntimeMethodHandle = type; }
			else if (!strcmp(type->Name, "RuntimeTypeHandle")) { CachedType___System_RuntimeTypeHandle = type; }
			else if (!strcmp(type->Name, "SByte")) { CachedType___System_SByte = type; }
			else if (!strcmp(type->Name, "Single")) { CachedType___System_Single = type; }
			else if (!strcmp(type->Name, "String")) { CachedType___System_String = type; }
			else if (!strcmp(type->Name, "Type")) { CachedType___System_Type = type; }
			else if (!strcmp(type->Name, "TypedReference")) { CachedType___System_TypedReference = type; }
			else if (!strcmp(type->Name, "UInt16")) { CachedType___System_UInt16 = type; }
			else if (!strcmp(type->Name, "UInt32")) { CachedType___System_UInt32 = type; }
			else if (!strcmp(type->Name, "UInt64")) { CachedType___System_UInt64 = type; }
			else if (!strcmp(type->Name, "UIntPtr")) { CachedType___System_UIntPtr = type; }
			else if (!strcmp(type->Name, "ValueType")) { CachedType___System_ValueType = type; }
			else if (!strcmp(type->Name, "Void")) { CachedType___System_Void = type; }
		}
	}
}

CLR::Assembly* CLR::AppDomain::CreateAssembly(CLIFile* pFile)
{
	Assembly* assembly = new Assembly(this, pFile);

	if (!Assemblies[0]->Types[CachedType___System_Object->TableIndex - 1])
	{
		Assemblies[0]->CreateType(CachedType___System_Array);
		Assemblies[0]->CreateType(CachedType___System_Boolean);
		Assemblies[0]->CreateType(CachedType___System_Byte);
		Assemblies[0]->CreateType(CachedType___System_Char);
		Assemblies[0]->CreateType(CachedType___System_Double);
		Assemblies[0]->CreateType(CachedType___System_Enum);
		Assemblies[0]->CreateType(CachedType___System_Exception);
		Assemblies[0]->CreateType(CachedType___System_Int16);
		Assemblies[0]->CreateType(CachedType___System_Int32);
		Assemblies[0]->CreateType(CachedType___System_Int64);
		Assemblies[0]->CreateType(CachedType___System_IntPtr);
		Assemblies[0]->CreateType(CachedType___System_Object);
		Assemblies[0]->CreateType(CachedType___System_RuntimeFieldHandle);
		Assemblies[0]->CreateType(CachedType___System_RuntimeMethodHandle);
		Assemblies[0]->CreateType(CachedType___System_RuntimeTypeHandle);
		Assemblies[0]->CreateType(CachedType___System_SByte);
		Assemblies[0]->CreateType(CachedType___System_Single);
		Assemblies[0]->CreateType(CachedType___System_String);
		Assemblies[0]->CreateType(CachedType___System_Type);
		Assemblies[0]->CreateType(CachedType___System_TypedReference);
		Assemblies[0]->CreateType(CachedType___System_UInt16);
		Assemblies[0]->CreateType(CachedType___System_UInt32);
		Assemblies[0]->CreateType(CachedType___System_UInt64);
		Assemblies[0]->CreateType(CachedType___System_UIntPtr);
		Assemblies[0]->CreateType(CachedType___System_ValueType);
		Assemblies[0]->CreateType(CachedType___System_Void);
	}

	if (pFile->AssemblyReferenceCount > 0)
	{
		char buf[FILENAME_MAX];
		char buf2[FILENAME_MAX];
		for (uint32_t i = 1; i <= pFile->AssemblyReferenceCount; i++)
		{
			AssemblyReferenceData* r = &pFile->AssemblyReferences[i];
			bool foundAssembly = false;
			for (uint32_t i2 = 0; i2 < AssemblyCount; i2++)
			{
				if (!strcmp(Assemblies[i2]->ParentFile->AssemblyDefinitions[1].Name, r->Name))
				{
					foundAssembly = true;
					break;
				}
			}
			if (!foundAssembly)
			{
				snprintf(buf, FILENAME_MAX, "/gac/%s.dll", r->Name);
				snprintf(buf2, FILENAME_MAX, "%s.dll", r->Name);
				Multiboot::LoadedModule* loadedModule = Multiboot::GetLoadedModule(buf);
				if (!loadedModule)
				{
					snprintf(buf, FILENAME_MAX, "/gac/proton/%s.dll", r->Name);
					loadedModule = Multiboot::GetLoadedModule(buf);
				}
				if (!loadedModule)
				{
					Panic("Unable to resolve dependancy!");
				}
				CLIFile* file = new CLIFile();
				if (!file->Load((uint8_t*)loadedModule->Address, loadedModule->Size, buf2)) Panic("Loading referenced assembly failed");
				CreateAssembly(file);
			}
		}
	}

	if (pFile->CLIHeader->EntryPointToken)
	{
		MetadataToken* token = pFile->ExpandMetadataToken(pFile->CLIHeader->EntryPointToken);
		if (token->Table != MetadataTable::MethodDefinition)
			Panic("Unknown entry point table!");
		assembly->EntryPoint = assembly->CreateMethod((MethodDefinitionData*)token->Data);
		delete token;
	}

	IRType* type = nullptr;
	for (uint32_t index = 1; index <= assembly->FieldCount; ++index)
	{
		if ((assembly->ParentFile->Fields[index].Flags & (FieldAttributes::Static | FieldAttributes::Literal)) == FieldAttributes::Static)
		{
			if (!(type = assembly->Types[assembly->ParentFile->Fields[index].TypeDefinition->TableIndex - 1]))
			{
				type = assembly->CreateType(assembly->ParentFile->Fields[index].TypeDefinition);
			}

			IRField* field = new IRField(type, &assembly->ParentFile->Fields[index]);
		}
	}
	if (assembly->StaticFieldIndex != assembly->StaticFieldCount) Panic("Somehow we found more statics than we previously allocated space for, means we would corrupted memory!");
	
	return assembly;
}

void CLR::AppDomain::AddAssembly(Assembly* pAssembly)
{
	uint32_t emptyIndex = 0;
	if (!Assemblies)
	{
		Assemblies = new Assembly*[1];
		AssemblyCount = 1;
	}
	else
	{
		bool foundEmptyIndex = false;
		for (uint32_t index = 0; !foundEmptyIndex && index < AssemblyCount; ++index)
		{
			if (!Assemblies[index])
			{
				emptyIndex = index;
				foundEmptyIndex = true;
			}
		}
		if (!foundEmptyIndex)
		{
			Assembly** oldAssemblyArray = Assemblies;
			emptyIndex = AssemblyCount;
			Assemblies = new Assembly*[AssemblyCount + 1];
			for (uint32_t index = 0; index < AssemblyCount; ++index)
			{
				Assemblies[index] = oldAssemblyArray[index];
			}
			Assemblies[AssemblyCount] = nullptr;
			++AssemblyCount;
		}
	}
	pAssembly->AssemblyIndex = emptyIndex;
	Assemblies[pAssembly->AssemblyIndex] = pAssembly;
}

bool CLR::AppDomain::IsStructure(TypeDefinitionData* pTypeDefinition)
{
	if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType::TypeDefinition)
	{ 
		if (pTypeDefinition->Extends.TypeDefinition == CachedType___System_Enum) return true;
		if (pTypeDefinition->Extends.TypeDefinition == CachedType___System_ValueType) return true;
		return false;
	}
	if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType::TypeReference)
	{ 
		if (!pTypeDefinition->Extends.TypeReference->ResolvedType)
		{
			pTypeDefinition->Extends.TypeReference->ResolvedType = ResolveTypeReference(pTypeDefinition->Extends.TypeReference);
		}
		if (pTypeDefinition->Extends.TypeReference->ResolvedType == CachedType___System_Enum) return true;
		if (pTypeDefinition->Extends.TypeReference->ResolvedType == CachedType___System_ValueType) return true;
		return false;
	}
	return false;
}

CLR::TypeDefinitionData* CLR::AppDomain::ResolveTypeReference(TypeReferenceData* pTypeReference)
{
	if (pTypeReference->ExportedType)
	{
		switch(pTypeReference->ExportedType->TypeOfImplementation)
		{
		case ImplementationType::File:
		{
			CLIFile* file = nullptr;
			for (uint32_t index = 0; index < AssemblyCount; index++)
			{
				if (!strcmp(Assemblies[index]->ParentFile->Filename, pTypeReference->ExportedType->Implementation.File->Name))
				{
					file = Assemblies[index]->ParentFile;
					break;
				}
			}
			if (!file) Panic("Failed to resolve assembly reference for exported type file resolution.");
			for (uint32_t index = 1; index <= file->TypeDefinitionCount; ++index)
			{
				if (!strcmp(file->TypeDefinitions[index].Name , pTypeReference->Name) &&
					!strcmp(file->TypeDefinitions[index].Namespace, pTypeReference->Namespace))
				{
					return &file->TypeDefinitions[index];
				}
			}
			if (!pTypeReference->ResolvedType) Panic("Failed to resolve type reference through assembly reference.");
			break;
		}
		case ImplementationType::AssemblyReference:
		{
			AssemblyReferenceData* asmRef = pTypeReference->ExportedType->Implementation.AssemblyReference;
			CLIFile* file = NULL;
			for (uint32_t index = 0; index < AssemblyCount; index++)
			{
				if (!strcmp(Assemblies[index]->ParentFile->AssemblyDefinitions[1].Name, asmRef->Name))
				{
					file = Assemblies[index]->ParentFile;
					break;
				}
			}
			if (!file) Panic("Failed to resolve assembly reference for exported type assembly reference resolution.");
			for (uint32_t index = 1; index <= file->TypeDefinitionCount; ++index)
			{
				if (!strcmp(file->TypeDefinitions[index].Name , pTypeReference->Name) &&
					!strcmp(file->TypeDefinitions[index].Namespace, pTypeReference->Namespace))
				{
					return &file->TypeDefinitions[index];
				}
			}
			if (!pTypeReference->ResolvedType) Panic("Failed to resolve type reference through assembly reference.");
			break;
		}
		default: Panic("Unhandled type reference resolution through exported type"); break;
		}
	}
	else
	{
		switch (pTypeReference->TypeOfResolutionScope)
		{
		case ResolutionScopeType::AssemblyReference:
		{
			AssemblyReferenceData* asmRef = pTypeReference->ResolutionScope.AssemblyReference;
			CLIFile* file = NULL;
			for (uint32_t index = 0; index < AssemblyCount; index++)
			{
				if (!strcmp(Assemblies[index]->ParentFile->AssemblyDefinitions[1].Name, asmRef->Name))
				{
					file = Assemblies[index]->ParentFile;
					break;
				}
			}
			if (!file) Panic("Failed to resolve assembly reference for type reference resolution.");
			for (uint32_t index = 1; index <= file->TypeDefinitionCount; ++index)
			{
				if (!strcmp(file->TypeDefinitions[index].Name , pTypeReference->Name) &&
					!strcmp(file->TypeDefinitions[index].Namespace, pTypeReference->Namespace))
				{
					return &file->TypeDefinitions[index];
				}
			}
			Panic("Failed to resolve type reference through assembly reference.");
			break;
		}
		case ResolutionScopeType::TypeReference:
		{
			TypeDefinitionData* resolvedTypeDef = ResolveTypeReference(pTypeReference->ResolutionScope.TypeReference);
			for (uint32_t index = 0; index < resolvedTypeDef->NestedClassCount; index++)
			{
				if (!strcmp(resolvedTypeDef->NestedClasses[index]->Nested->Name, pTypeReference->Name) &&
					!strcmp(resolvedTypeDef->NestedClasses[index]->Nested->Namespace, pTypeReference->Namespace))
				{
					return resolvedTypeDef->NestedClasses[index]->Nested;
				}
			}
			Panic("Failed to resolve type reference through type reference.");
			break;
		}
		default:
			Panic("Unhandled type reference resolution");
			break;
		}
	}

	Panic("Unhandled type reference resolution");
	return nullptr;
}

void CLR::AppDomain::ResolveMemberReference(MemberReferenceData* pMemberReference)
{
	if (pMemberReference->Resolved.Field || pMemberReference->Resolved.MethodDefinition) return;

	switch (pMemberReference->TypeOfParent)
	{
	case MemberRefParentType::TypeDefinition:
	{
		TypeDefinitionData* typeDef = pMemberReference->Parent.TypeDefinition;
		bool isField = pMemberReference->Signature[0] == 0x06;
		if (isField)
		{
			for (uint32_t i2 = 0; i2 < typeDef->FieldListCount; ++i2)
			{
				if (!strcmp(typeDef->FieldList[i2].Name, pMemberReference->Name))
				{
					pMemberReference->TypeOfResolved = FieldOrMethodDefType::Field;
					pMemberReference->Resolved.Field = &typeDef->FieldList[i2];
					break;
				}
			}
			if (!pMemberReference->Resolved.Field) Panic("Failed to resolve member reference through type definition for field.");
		}
		else
		{
			//printf("Searching for %s, from 0x%x\n", pMemberReference->Name, (unsigned int)typeDef);
			for (uint32_t i2 = 0; i2 < typeDef->MethodDefinitionListCount; ++i2)
			{
				//printf("Checking %s.%s.%s\n", typeDef->Namespace, typeDef->Name, typeDef->MethodDefinitionList[i2].Name);
				if (!typeDef->MethodDefinitionList[i2].SignatureCache)
				{
					typeDef->MethodDefinitionList[i2].SignatureCache = new MethodSignature(typeDef->MethodDefinitionList[i2].Signature, typeDef->File);
				}
				if (!pMemberReference->MethodSignatureCache)
				{
					pMemberReference->MethodSignatureCache = new MethodSignature(pMemberReference->Signature, pMemberReference->File);
				}

				if (!strcmp(typeDef->MethodDefinitionList[i2].Name, pMemberReference->Name) &&
					typeDef->MethodDefinitionList[i2].SignatureCache->Equals(pMemberReference->MethodSignatureCache))
				{
					pMemberReference->TypeOfResolved = FieldOrMethodDefType::MethodDefinition;
					pMemberReference->Resolved.MethodDefinition = &typeDef->MethodDefinitionList[i2];
					break;
				}
			}
			if (!pMemberReference->Resolved.MethodDefinition) Panic("Failed to resolve member reference through type reference for method definition.");
		}
		break;
	}
	case MemberRefParentType::TypeReference:
	{
		if (!pMemberReference->Parent.TypeReference->ResolvedType)
		{
			pMemberReference->Parent.TypeReference->ResolvedType = ResolveTypeReference(pMemberReference->Parent.TypeReference);
		}
		TypeDefinitionData* typeDef = pMemberReference->Parent.TypeReference->ResolvedType;
		bool isField = pMemberReference->Signature[0] == 0x06;
		if (isField)
		{
			for (uint32_t i2 = 0; i2 < typeDef->FieldListCount; ++i2)
			{
				if (!strcmp(typeDef->FieldList[i2].Name, pMemberReference->Name))
				{
					pMemberReference->TypeOfResolved = FieldOrMethodDefType::Field;
					pMemberReference->Resolved.Field = &typeDef->FieldList[i2];
					break;
				}
			}
			if (!pMemberReference->Resolved.Field) Panic("Failed to resolve member reference through type reference for field.");
		}
		else
		{
			//printf("Searching for %s, from 0x%x\n", pMemberReference->Name, (unsigned int)typeDef);
			for (uint32_t i2 = 0; i2 < typeDef->MethodDefinitionListCount; ++i2)
			{
				//printf("Checking %s.%s.%s\n", typeDef->Namespace, typeDef->Name, typeDef->MethodDefinitionList[i2].Name);
				if (!typeDef->MethodDefinitionList[i2].SignatureCache)
				{
					typeDef->MethodDefinitionList[i2].SignatureCache = new MethodSignature(typeDef->MethodDefinitionList[i2].Signature, typeDef->File);
				}
				if (!pMemberReference->MethodSignatureCache)
				{
					pMemberReference->MethodSignatureCache = new MethodSignature(pMemberReference->Signature, pMemberReference->File);
				}

				if (!strcmp(typeDef->MethodDefinitionList[i2].Name, pMemberReference->Name) &&
					typeDef->MethodDefinitionList[i2].SignatureCache->Equals(pMemberReference->MethodSignatureCache))
				{
					pMemberReference->TypeOfResolved = FieldOrMethodDefType::MethodDefinition;
					pMemberReference->Resolved.MethodDefinition = &typeDef->MethodDefinitionList[i2];
					break;
				}
			}
			if (!pMemberReference->Resolved.MethodDefinition) Panic("Failed to resolve member reference through type reference for method definition.");
		}
		break;
	}
	case MemberRefParentType::TypeSpecification:
	{
		TypeSpecificationData* typeSpec = pMemberReference->Parent.TypeSpecification;
		SignatureType* sig = new SignatureType(typeSpec->Signature, typeSpec->File);
		IRType* tp = pMemberReference->File->LoadedAssembly->GetIRTypeFromSignatureType(sig);
		TypeDefinitionData* typeDef = tp->TypeDefinition;
		bool isField = pMemberReference->Signature[0] == 0x06;
		if (isField)
		{
			for (uint32_t i2 = 0; i2 < typeDef->FieldListCount; ++i2)
			{
				if (!strcmp(typeDef->FieldList[i2].Name, pMemberReference->Name))
				{
					pMemberReference->TypeOfResolved = FieldOrMethodDefType::Field;
					pMemberReference->Resolved.Field = &typeDef->FieldList[i2];
					break;
				}
			}
			if (!pMemberReference->Resolved.Field) Panic("Failed to resolve member reference through type specification for field.");
		}
		else
		{
			//printf("Searching for %s, from 0x%x\n", pMemberReference->Name, (unsigned int)typeDef);
			for (uint32_t i2 = 0; i2 < typeDef->MethodDefinitionListCount; ++i2)
			{
				//printf("Checking %s.%s.%s\n", typeDef->Namespace, typeDef->Name, typeDef->MethodDefinitionList[i2].Name);
				if (!typeDef->MethodDefinitionList[i2].SignatureCache)
				{
					typeDef->MethodDefinitionList[i2].SignatureCache = new MethodSignature(typeDef->MethodDefinitionList[i2].Signature, typeDef->File);
				}
				if (!pMemberReference->MethodSignatureCache)
				{
					pMemberReference->MethodSignatureCache = new MethodSignature(pMemberReference->Signature, pMemberReference->File);
				}

				if (!strcmp(typeDef->MethodDefinitionList[i2].Name, pMemberReference->Name) &&
					typeDef->MethodDefinitionList[i2].SignatureCache->Equals(pMemberReference->MethodSignatureCache))
				{
					pMemberReference->TypeOfResolved = FieldOrMethodDefType::MethodDefinition;
					pMemberReference->Resolved.MethodDefinition = &typeDef->MethodDefinitionList[i2];
					break;
				}
			}
			if (!pMemberReference->Resolved.MethodDefinition) Panic("Failed to resolve member reference through type specification for method definition.");
		}
		//Panic("Member reference resolution through TypeSpec not yet supported!\n");
		break;
	}
	default: Panic("Unhandled member reference resolution"); break;
	}
}
