#include "AppDomain.h"
#include "Assembly.h"
#include "IR.h"

CLR::Assembly::Assembly(AppDomain* pAppDomain, CLIFile* pFile)
{
	pFile->LoadedAssembly = this;

	ParentDomain = pAppDomain;
	ParentFile = pFile;

	EntryPoint = nullptr;

	MethodCount = pFile->MethodDefinitionCount;
	Methods = new IRMethod*[MethodCount];
	FieldCount = pFile->FieldCount;
	Fields = new IRField*[FieldCount];
	TypeCount = pFile->TypeDefinitionCount;
	Types = new IRType*[TypeCount];

	StaticFieldIndex = 0;
	StaticFieldCount = 0;
	for (uint32_t index = 1; index <= pFile->FieldCount; ++index) if ((pFile->Fields[index].Flags & (FieldAttributes::Static | FieldAttributes::Literal)) == FieldAttributes::Static) ++StaticFieldCount;
	StaticFields = new IRField*[StaticFieldCount];

	ArrayTypesHashTable = nullptr;
	PointerTypesHashTable = nullptr;
	GenericTypesHashTable = nullptr;
	GenericMethodsHashTable = nullptr;

	pAppDomain->AddAssembly(this);
}

CLR::Assembly::~Assembly()
{
	delete [] Methods;
	delete [] Fields;
	delete [] Types;
	delete [] StaticFields;
}

CLR::IRType* CLR::Assembly::CreateType(TypeDefinitionData* pTypeDefinition)
{
	IRType* type = new IRType();
	type->ParentAssembly = this;
	type->TypeDefinition = pTypeDefinition;
	type->TypeIndex = pTypeDefinition->TableIndex - 1;
	Types[type->TypeIndex] = type;
	type->IsInterface = pTypeDefinition->Flags & TypeAttributes::Interface;
	type->IsAbstract = pTypeDefinition->Flags & TypeAttributes::Abstract;

	type->LayoutMethods();
	type->LayoutFields();

	type->IsGeneric = pTypeDefinition->GenericParameterCount > 0;
	type->GenericParameterCount = pTypeDefinition->GenericParameterCount;

	type->IsValueType = pTypeDefinition->File->LoadedAssembly->ParentDomain->IsStructure(pTypeDefinition);
	type->IsReferenceType = !type->IsValueType;

	for (uint32_t index = 0; index < pTypeDefinition->MethodDefinitionListCount; index++)
	{
		if ((pTypeDefinition->MethodDefinitionList[index].Flags & MethodAttributes::Static) != 0)
		{
			type->HasStaticConstructor = !strcmp(pTypeDefinition->MethodDefinitionList[index].Name, ".cctor");
		}
	}

	pTypeDefinition->File->LoadedAssembly->LinkType(type);
	return type;
}

void CLR::Assembly::LinkType(IRType* pType)
{
	TypeDefinitionData* typeDefinition = pType->TypeDefinition;
	for (uint32_t index2 = 0; index2 < typeDefinition->InterfaceImplementationCount; ++index2)
	{
		InterfaceImplementationData* interfaceImplementation = typeDefinition->InterfaceImplementations[index2];
		IRType* interfaceType = NULL;
		switch (interfaceImplementation->TypeOfInterface)
		{
		case TypeDefRefOrSpecType::TypeDefinition:
		{
			if (!(interfaceType = Types[interfaceImplementation->Interface.TypeDefinition->TableIndex - 1]))
			{
				interfaceType = CreateType(interfaceImplementation->Interface.TypeDefinition);
			}
			break;
		}
		case TypeDefRefOrSpecType::TypeReference:
		{
			if (!interfaceImplementation->Interface.TypeReference->ResolvedType)
			{
				interfaceImplementation->Interface.TypeReference->ResolvedType = ParentDomain->ResolveTypeReference(interfaceImplementation->Interface.TypeReference);
			}
			if (!(interfaceType = interfaceImplementation->Interface.TypeReference->ResolvedType->File->LoadedAssembly->Types[interfaceImplementation->Interface.TypeReference->ResolvedType->TableIndex - 1]))
			{
				interfaceType = interfaceImplementation->Interface.TypeReference->ResolvedType->File->LoadedAssembly->CreateType(interfaceImplementation->Interface.TypeReference->ResolvedType);
			}
			break;
		}
		case TypeDefRefOrSpecType::TypeSpecification:
		{
			printf("WARNING: Generics not yet supported\n");
			continue;
		}
		default:
			Panic("Unknown table for type of interface");
			break;
		}
		if (interfaceType->IsInterface || interfaceType->IsAbstract) continue;
		IRInterfaceImpl* interfaceImpl = new IRInterfaceImpl(interfaceType);
		for (uint32_t index3 = 0; index3 < typeDefinition->MethodImplementationCount; ++index3)
		{
			MethodImplementationData* methodImplementation = typeDefinition->MethodImplementations[index3];
			IRMethod* interfaceMethod = NULL;
			switch (methodImplementation->TypeOfMethodDeclaration)
			{
			case MethodDefOrRefType::MethodDefinition:
			{
				if (!(interfaceMethod = Methods[methodImplementation->MethodDeclaration.MethodDefinition->TableIndex - 1]))
				{
					interfaceMethod = CreateMethod(methodImplementation->MethodDeclaration.MethodDefinition);
				}
				break;
			}
			case MethodDefOrRefType::MemberReference:
			{
				ParentDomain->ResolveMemberReference(methodImplementation->MethodDeclaration.MemberReference);
				if (!(interfaceMethod = methodImplementation->MethodDeclaration.MemberReference->Resolved.MethodDefinition->File->LoadedAssembly->Methods[methodImplementation->MethodDeclaration.MemberReference->Resolved.MethodDefinition->TableIndex - 1]))
				{
					interfaceMethod = methodImplementation->MethodDeclaration.MemberReference->Resolved.MethodDefinition->File->LoadedAssembly->CreateMethod(methodImplementation->MethodDeclaration.MemberReference->Resolved.MethodDefinition);
				}
				break;
			}
			default:
				Panic("Unknown table for type of method declaration 1");
				break;
			}

			if (interfaceMethod->MethodDefinition->TypeDefinition == interfaceType->TypeDefinition)
			{
				for (uint32_t index4 = 0; index4 < interfaceType->MethodCount; ++index4)
				{
					if (interfaceMethod == interfaceType->Methods[index4])
					{
						IRMethod* checkingMethod = NULL;
						switch (methodImplementation->TypeOfMethodBody)
						{
						case MethodDefOrRefType::MethodDefinition:
						{
							if (!(checkingMethod = Methods[methodImplementation->MethodBody.MethodDefinition->TableIndex - 1]))
							{
								checkingMethod = CreateMethod(methodImplementation->MethodBody.MethodDefinition);
							}
							break;
						}
						case MethodDefOrRefType::MemberReference:
						{
							ParentDomain->ResolveMemberReference(methodImplementation->MethodBody.MemberReference);
							if (!(checkingMethod = methodImplementation->MethodBody.MemberReference->Resolved.MethodDefinition->File->LoadedAssembly->Methods[methodImplementation->MethodBody.MemberReference->Resolved.MethodDefinition->TableIndex - 1]))
							{
								checkingMethod = methodImplementation->MethodBody.MemberReference->Resolved.MethodDefinition->File->LoadedAssembly->CreateMethod(methodImplementation->MethodBody.MemberReference->Resolved.MethodDefinition);
							}
							break;
						}
						default:
							Panic("Unknown table for type of method declaration 2");
							break;
						}

						for (uint32_t index5 = 0; index5 < pType->MethodCount; ++index5)
						{
							if (pType->Methods[index5] == checkingMethod)
							{
								interfaceImpl->MethodIndexes[index4] = index5;
								break;
							}
						}

						break;
					}
				}
			}
		}

		for (uint32_t index3 = 0; index3 < interfaceImpl->MethodCount; ++index3)
		{
			if (!interfaceImpl->MethodIndexes[index3])
			{
				MethodDefinitionData* methodDef1 = interfaceType->Methods[index3]->MethodDefinition;
				bool found = false;
				for (uint32_t index4 = 0; index4 < pType->MethodCount; ++index4)
				{
					MethodDefinitionData* methodDef2 = pType->Methods[index4]->MethodDefinition;
					if (!strcmp(methodDef1->Name, methodDef2->Name))
					{
						if (methodDef1->Flags & MethodAttributes::HideBySignature)
						{
							if (!methodDef1->SignatureCache)
							{
								methodDef1->SignatureCache = new MethodSignature(methodDef1->Signature, methodDef1->File);
							}
							if (!methodDef2->SignatureCache)
							{
								methodDef2->SignatureCache = new MethodSignature(methodDef2->Signature, methodDef2->File);
							}
							if (methodDef1->SignatureCache->Equals(methodDef2->SignatureCache))
							{
								interfaceImpl->MethodIndexes[index3] = index4;
								found = true;
								break;
							}
						}
						else
						{
							interfaceImpl->MethodIndexes[index3] = index4;
							found = true;
							break;
						}
					}
				}

				if (!found)
				{
					printf("Unable to resolve method for interface %s @ 0x%x implemented on %s\n", interfaceType->TypeDefinition->Name, (unsigned int)interfaceType->TypeDefinition, pType->TypeDefinition->Name);
					Panic("Unable to resolve interface method");
				}
			}
		}

		HASH_ADD(HashHandle, pType->InterfaceTable, InterfaceType, sizeof(void*), interfaceImpl);
	}
}

CLR::IRType* CLR::Assembly::GetIRTypeFromSignatureType(SignatureType* pType)
{
	IRType* type = NULL;
	switch(pType->ElementType)
	{
	case SignatureElementType::I1:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_SByte->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_SByte);
		}
		break;
	case SignatureElementType::U1:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Byte->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Byte);
		}
		break;
	case SignatureElementType::I2:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Int16->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Int16);
		}
		break;
	case SignatureElementType::U2:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_UInt16->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_UInt16);
		}
		break;
	case SignatureElementType::I4:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Int32->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Int32);
		}
		break;
	case SignatureElementType::U4:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_UInt32->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_UInt32);
		}
		break;
	case SignatureElementType::I8:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Int64->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Int64);
		}
		break;
	case SignatureElementType::U8:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_UInt64->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_UInt64);
		}
		break;
	case SignatureElementType::R4:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Single->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Single);
		}
		break;
	case SignatureElementType::R8:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Double->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Double);
		}
		break;
	case SignatureElementType::Boolean:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Boolean->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Boolean);
		}
		break;
	case SignatureElementType::Char:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Char->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Char);
		}
		break;
	case SignatureElementType::IPointer:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_IntPtr->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_IntPtr);
		}
		break;
	case SignatureElementType::UPointer:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_UIntPtr->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_UIntPtr);
		}
		break;
	case SignatureElementType::Object:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Object->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Object);
		}
		break;
	case SignatureElementType::String:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_String->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_String);
		}
		break;
	case SignatureElementType::Type:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Type->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Type);
		}
		break;
	case SignatureElementType::Void:
		if (!(type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Void->TableIndex - 1]))
		{
			type = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Void);
		}
		break;
	case SignatureElementType::ValueType:
	case SignatureElementType::Class:
	{
		MetadataToken* token = nullptr;
		if (pType->ElementType == SignatureElementType::ValueType) token = ParentFile->ExpandTypeDefRefOrSpecToken(pType->ValueTypeDefOrRefOrSpecToken);
		else token = ParentFile->ExpandTypeDefRefOrSpecToken(pType->ClassTypeDefOrRefOrSpecToken);
		switch(token->Table)
		{
		case MetadataTable::TypeDefinition:
			if (!(type = Types[((TypeDefinitionData*)token->Data)->TableIndex - 1]))
			{
				type = CreateType((TypeDefinitionData*)token->Data);
			}
			break;

		case MetadataTable::TypeReference:
			if (!((TypeReferenceData*)token->Data)->ResolvedType)
			{
				((TypeReferenceData*)token->Data)->ResolvedType = ParentDomain->ResolveTypeReference((TypeReferenceData*)token->Data);
			}
			if (!(type = ((TypeReferenceData*)token->Data)->ResolvedType->File->LoadedAssembly->Types[((TypeReferenceData*)token->Data)->ResolvedType->TableIndex - 1]))
			{
				type = ((TypeReferenceData*)token->Data)->ResolvedType->File->LoadedAssembly->CreateType(((TypeReferenceData*)token->Data)->ResolvedType);
			}
			break;

		default:
			Panic("GetIRTypeFromSignatureType Invalid Class Table");
			break;
		}
		delete token;
		break;
	}
	case SignatureElementType::Array:
	{
		IRType* sysArrayType = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Array->TableIndex - 1];
		IRType* elementType = GetIRTypeFromSignatureType(pType->ArrayType);
		IRArrayType* lookupType = NULL;
		HASH_FIND(HashHandle, ArrayTypesHashTable, (void*)&elementType, sizeof(void*), lookupType);
		if (!lookupType)
		{
			type = sysArrayType->Copy();
			type->IsArrayType = true;
			type->ArrayType = new IRArrayType(type, elementType);
			HASH_ADD(HashHandle, ArrayTypesHashTable, ElementType, sizeof(void*), type->ArrayType);
		}
		else type = lookupType->ArrayType;
		break;
	}
	case SignatureElementType::SingleDimensionArray:
	{
		IRType* sysArrayType = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Array->TableIndex - 1];
		IRType* elementType = GetIRTypeFromSignatureType(pType->SZArrayType);
		IRArrayType* lookupType = NULL;
		HASH_FIND(HashHandle, ArrayTypesHashTable, (void*)&elementType, sizeof(void*), lookupType);
		if (!lookupType)
		{
			type = sysArrayType->Copy();
			type->IsArrayType = true;
			type->ArrayType = new IRArrayType(type, elementType);
			HASH_ADD(HashHandle, ArrayTypesHashTable, ElementType, sizeof(void*), type->ArrayType);
		}
		else type = lookupType->ArrayType;
		break;
	}
	case SignatureElementType::Pointer:
	{
		IRType* typePointedTo = NULL;
		if (pType->PtrVoid)
		{
			if (!(typePointedTo = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_Void->TableIndex - 1]))
			{
				typePointedTo = ParentDomain->Assemblies[0]->CreateType(ParentDomain->CachedType___System_Void);
			}
		}
		else
		{		
			typePointedTo = GetIRTypeFromSignatureType(pType->PtrType);
		}
		IRPointerType* lookupType = NULL;
		HASH_FIND(HashHandle, PointerTypesHashTable, (void*)&typePointedTo, sizeof(void*), lookupType);
		if (!lookupType)
		{
			IRType* sysPointerType = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_IntPtr->TableIndex - 1];
			type = sysPointerType->Copy();
			type->IsPointerType = true;
			type->PointerType = new IRPointerType(type, typePointedTo);
			HASH_ADD(HashHandle, PointerTypesHashTable, TypePointedTo, sizeof(void*), type->PointerType);
		}
		else
		{
			type = lookupType->PointerType;
		}
		break;
	}
	case SignatureElementType::MethodVar:
	{
		type = CreateGenericParameter(false, pType->MVarNumber);
		break;
	}
	case SignatureElementType::Var:
	{
		type = CreateGenericParameter(true, pType->VarNumber);
		break;
	}
	case SignatureElementType::GenericInstantiation:
	{
		MetadataToken* token = ParentFile->ExpandTypeDefRefOrSpecToken(pType->GenericInstTypeDefOrRefOrSpecToken);
		switch(token->Table)
		{
		case MetadataTable::TypeDefinition:
			if (!(type = Types[((TypeDefinitionData*)token->Data)->TableIndex - 1]))
			{
				type = CreateType((TypeDefinitionData*)token->Data);
			}
			break;

		case MetadataTable::TypeReference:
			if (!((TypeReferenceData*)token->Data)->ResolvedType)
			{
				((TypeReferenceData*)token->Data)->ResolvedType = ParentDomain->ResolveTypeReference((TypeReferenceData*)token->Data);
			}
			if (!(type = ((TypeReferenceData*)token->Data)->ResolvedType->File->LoadedAssembly->Types[((TypeReferenceData*)token->Data)->ResolvedType->TableIndex - 1]))
			{
				type = ((TypeReferenceData*)token->Data)->ResolvedType->File->LoadedAssembly->CreateType(((TypeReferenceData*)token->Data)->ResolvedType);
			}
			break;

			default:
				Panic("GetIRTypeFromSignatureType Invalid GenericInstantiation Table");
				break;
		}
		IRGenericType key;
		IRGenericType* keyPtr = &key;
		key.GenericType = type;
		if (type->GenericParameterCount != pType->GenericInstGenericArgumentCount)
		{
			//printf("type->TypeDef->GenericParameterCount (%u), type->GenericParameterCount (%u) != pType->GenericInstGenericArgumentCount (%u)\n", (unsigned int)type->TypeDefinition->GenericParameterCount, (unsigned int)type->GenericParameterCount, (unsigned int)pType->GenericInstGenericArgumentCount);
			//Panic("AppDomain_GetIRTypeFromSignatureType GenericInstantiation Parameter Count Mismatch");
		}
		key.ParameterCount = pType->GenericInstGenericArgumentCount;
		for (uint32_t index = 0; index < pType->GenericInstGenericArgumentCount; ++index)
		{
			key.Parameters[index] = GetIRTypeFromSignatureType(pType->GenericInstGenericArguments[index]);
		}
		IRGenericType* lookupType = NULL;
		HASH_FIND(HashHandle, GenericTypesHashTable, (void*)&keyPtr, offsetof(IRGenericType, ImplementationType), lookupType);
		IRType* implementationType = NULL;
		if (!lookupType)
		{
			implementationType = type->GenericDeepCopy();
			implementationType->GenericType = new IRGenericType(type, implementationType);
			implementationType->GenericType->ParameterCount = pType->GenericInstGenericArgumentCount;
			for (uint32_t index = 0; index < pType->GenericInstGenericArgumentCount; ++index)
			{
				implementationType->GenericType->Parameters[index] = GetIRTypeFromSignatureType(pType->GenericInstGenericArguments[index]);
				//printf("GenericInstantiation Type %s.%s, Param %u Type %s.%s\n", implementationType->TypeDefinition->Namespace, implementationType->TypeDefinition->Name, (unsigned int)index, implementationType->GenericType->Parameters[index]->TypeDefinition->Namespace, implementationType->GenericType->Parameters[index]->TypeDefinition->Name);
			}
			HASH_ADD(HashHandle, GenericTypesHashTable, GenericType, offsetof(IRGenericType, ImplementationType), implementationType->GenericType);
			ResolveGenericTypeParameters(implementationType);
		}
		else
		{
			implementationType = lookupType->ImplementationType;
		}
		type = implementationType;
		delete token;
		break;
	}
	default:
		printf("Unknown Signature Element Type = 0x%x\n", (unsigned int)pType->ElementType);
		Panic("GetIRTypeFromSignatureType Unknown Signature Element Type");
		break;
	}
	if (!type) Panic("GetIRTypeFromSignatureType returning NULL!");
	return type;
}

CLR::IRMethod* CLR::Assembly::CreateMethod(MethodDefinitionData* pMethodDefinition)
{
	IRMethod* method = new IRMethod();
	method->ParentAssembly = this;
	method->MethodDefinition = pMethodDefinition;
	method->MethodIndex = pMethodDefinition->TableIndex - 1;
	Methods[method->MethodIndex] = method;
	if (!pMethodDefinition->SignatureCache)
	{
		pMethodDefinition->SignatureCache = new MethodSignature(pMethodDefinition->Signature, pMethodDefinition->File);
	}

	if (!pMethodDefinition->SignatureCache->ReturnType->Void)
	{
		method->Returns = true;
		method->ReturnType = GetIRTypeFromSignatureType(pMethodDefinition->SignatureCache->ReturnType->Type);
	}

	method->IsGeneric = pMethodDefinition->GenericParameterCount > 0;
	method->GenericParameterCount = pMethodDefinition->GenericParameterCount;

	method->ParameterCount = pMethodDefinition->SignatureCache->ParameterCount;
	bool addingThis = false;
	if (pMethodDefinition->SignatureCache->HasThis && !pMethodDefinition->SignatureCache->ExplicitThis)
	{
		addingThis = true;
		method->ParameterCount++;
	}
	method->Parameters = (IRParameter**)calloc(1, method->ParameterCount * sizeof(IRParameter*));
	IRType* type = NULL;
	uint32_t parameterIndex = 0;
	if (addingThis)
	{
		if (ParentDomain->IsStructure(pMethodDefinition->TypeDefinition))
		{
			type = Types[ParentDomain->CachedType___System_IntPtr->TableIndex - 1];
		}
		else
		{
			if (!(type = Types[pMethodDefinition->TypeDefinition->TableIndex - 1]))
			{
				type = CreateType(pMethodDefinition->TypeDefinition);
			}
		}
		IRParameter* parameter = new IRParameter(type);
		parameter->ParameterIndex = parameterIndex++;
		method->Parameters[parameter->ParameterIndex] = parameter;
	}
	for (uint32_t index = 0; index < pMethodDefinition->SignatureCache->ParameterCount; index++)
	{
		if (pMethodDefinition->SignatureCache->Parameters[index]->TypedByReference)
		{
			type = ParentDomain->Assemblies[0]->Types[ParentDomain->CachedType___System_TypedReference->TableIndex - 1];
		}
		else
		{
			type = GetIRTypeFromSignatureType(pMethodDefinition->SignatureCache->Parameters[index]->Type);
		}
		IRParameter* parameter = new IRParameter(type);
		parameter->ParameterIndex = parameterIndex++;
		method->Parameters[parameter->ParameterIndex] = parameter;
	}
	return method;
}

void CLR::Assembly::ResolveGenericTypeParameters(IRType* pType)
{
	if (!pType->IsGeneric || pType->IsGenericInstantiation)
		return;

	for(uint32_t index = 0; index < pType->FieldCount; index++)
	{
		IRField* field = pType->Fields[index];
		if (field->FieldType->IsGeneric && !field->FieldType->IsGenericInstantiation)
		{
			IRType* fieldType = field->FieldType;
			IRGenericType key;
			IRGenericType* keyPtr = &key;
			key.GenericType = pType->GenericType->GenericType;
			key.ParameterCount = pType->GenericType->ParameterCount;
			for (uint32_t index = 0; index < pType->GenericType->ParameterCount; ++index)
			{
				if (fieldType->GenericType->Parameters[index]->IsGenericParameter)
				{
					key.Parameters[index] = pType->GenericType->Parameters[fieldType->GenericType->Parameters[index]->GenericParameterIndex];
				}
				else
				{
					key.Parameters[index] = fieldType->GenericType->Parameters[index];
				}
			}
			IRGenericType* lookupType = nullptr;
			HASH_FIND(HashHandle, GenericTypesHashTable, (void*)&keyPtr, offsetof(IRGenericType, ImplementationType), lookupType);
			if (!lookupType)
			{
				IRType* implementationType = fieldType->GenericDeepCopy();
				implementationType->GenericType = new IRGenericType(fieldType, implementationType);
				implementationType->GenericType->ParameterCount = pType->GenericType->ParameterCount;
				for (uint32_t index = 0; index < pType->GenericType->ParameterCount; ++index)
				{
					if (fieldType->GenericType->Parameters[index]->IsGenericParameter)
					{
						implementationType->GenericType->Parameters[index] = pType->GenericType->Parameters[fieldType->GenericType->Parameters[index]->GenericParameterIndex];
					}
					else
					{
						implementationType->GenericType->Parameters[index] = fieldType->GenericType->Parameters[index];
					}
					printf("ResolveGenericTypeParameters, GenericInstantiation Type %s.%s, Param %u Type %s.%s\n", implementationType->TypeDefinition->Namespace, implementationType->TypeDefinition->Name, (unsigned int)index, implementationType->GenericType->Parameters[index]->TypeDefinition->Namespace, implementationType->GenericType->Parameters[index]->TypeDefinition->Name);
				}
				field->FieldType = implementationType;
				HASH_ADD(HashHandle, GenericTypesHashTable, GenericType, offsetof(IRGenericType, ImplementationType), implementationType->GenericType);
				ResolveGenericTypeParameters(implementationType);
			}
			else
			{
				field->FieldType = lookupType->ImplementationType;
			}
		}
		else if (field->FieldType->IsGenericParameter)
		{
			field->FieldType = pType->GenericType->Parameters[field->FieldType->GenericParameterIndex];
		}
	}

	for(uint32_t index = 0; index < pType->MethodCount; index++)
	{
		IRMethod* method = pType->Methods[index];
		ResolveGenericMethodParameters(pType, method);
	}

	IRInterfaceImpl* removed = nullptr;
	for (IRInterfaceImpl* iterator = pType->InterfaceTable, *iteratorNext = NULL; iterator; iterator = iteratorNext)
	{
		iteratorNext = (IRInterfaceImpl*)iterator->HashHandle.next;
		if (iterator->InterfaceType->IsGeneric && !iterator->InterfaceType->IsGenericInstantiation)
		{
			HASH_DELETE(HashHandle, pType->InterfaceTable, iterator);
			HASH_ADD(HashHandle, removed, InterfaceType, sizeof(void*), iterator);
		}
	}
	for (IRInterfaceImpl* iterator = removed, *iteratorNext = NULL; iterator; iterator = iteratorNext)
	{
		iteratorNext = (IRInterfaceImpl*)iterator->HashHandle.next;
		
		IRType* interfaceType = iterator->InterfaceType;
		IRGenericType key;
		IRGenericType* keyPtr = &key;
		key.GenericType = pType->GenericType->GenericType;
		key.ParameterCount = pType->GenericType->ParameterCount;
		for (uint32_t index = 0; index < pType->GenericType->ParameterCount; ++index)
		{
			if (interfaceType->GenericType->Parameters[index]->IsGenericParameter)
			{
				key.Parameters[index] = pType->GenericType->Parameters[interfaceType->GenericType->Parameters[index]->GenericParameterIndex];
			}
			else
			{
				key.Parameters[index] = interfaceType->GenericType->Parameters[index];
			}
		}
		IRGenericType* lookupType = nullptr;
		HASH_FIND(HashHandle, GenericTypesHashTable, (void*)&keyPtr, offsetof(IRGenericType, ImplementationType), lookupType);
		if (!lookupType)
		{
			IRType* implementationType = interfaceType->GenericDeepCopy();
			implementationType->GenericType = new IRGenericType(interfaceType, implementationType);
			implementationType->GenericType->ParameterCount = pType->GenericType->ParameterCount;
			for (uint32_t index = 0; index < pType->GenericType->ParameterCount; ++index)
			{
				if (interfaceType->GenericType->Parameters[index]->IsGenericParameter)
				{
					implementationType->GenericType->Parameters[index] = pType->GenericType->Parameters[interfaceType->GenericType->Parameters[index]->GenericParameterIndex];
				}
				else
				{
					implementationType->GenericType->Parameters[index] = interfaceType->GenericType->Parameters[index];
				}
				printf("ResolveGenericTypeParameters, GenericInstantiation Interface Type %s.%s, Param %u Type %s.%s\n", implementationType->TypeDefinition->Namespace, implementationType->TypeDefinition->Name, (unsigned int)index, implementationType->GenericType->Parameters[index]->TypeDefinition->Namespace, implementationType->GenericType->Parameters[index]->TypeDefinition->Name);
			}
			iterator->InterfaceType = implementationType;
			HASH_ADD(HashHandle, GenericTypesHashTable, GenericType, offsetof(IRGenericType, ImplementationType), implementationType->GenericType);
			ResolveGenericTypeParameters(implementationType);
		}
		else
		{
			iterator->InterfaceType = lookupType->ImplementationType;
		}

		HASH_DELETE(HashHandle, removed, iterator);
		HASH_ADD(HashHandle, pType->InterfaceTable, InterfaceType, sizeof(void*), iterator);
	}
}

CLR::IRType* CLR::Assembly::CreateGenericParameter(bool pIsFromParentType, uint32_t pIndex)
{
	IRType* type = new IRType();
	type->IsGeneric = true;
	type->IsGenericParameter = true;
	type->IsGenericParameterFromParentType = pIsFromParentType;
	type->GenericParameterIndex = pIndex;
	return type;
}

void CLR::Assembly::ResolveGenericMethodParameters(IRType* pType, IRMethod* pMethod)
{
	if(pMethod->IsGeneric && !pMethod->IsGenericImplementation)
	{
		printf("Delaying generic parameter resolution of %s.%s.%s\n", pMethod->MethodDefinition->TypeDefinition->Namespace, pMethod->MethodDefinition->TypeDefinition->Name, pMethod->MethodDefinition->Name);
		return;
	}

	if (pMethod->Returns &&
		((pMethod->ReturnType->IsGeneric && !pMethod->ReturnType->IsGenericInstantiation) || pMethod->ReturnType->IsGenericParameter))
	{
		ResolveGenericMethodParametersInternal(pType, pMethod, &pMethod->ReturnType);
	}

	for (uint32_t index = 0; index < pMethod->LocalVariableCount; index++)
	{
		IRLocalVariable* local = pMethod->LocalVariables[index];
		if ((local->VariableType->IsGeneric && !local->VariableType->IsGenericInstantiation) || local->VariableType->IsGenericParameter)
		{
			ResolveGenericMethodParametersInternal(pType, pMethod, &local->VariableType);
		}
	}

	for (uint32_t index = 0; index < pMethod->ParameterCount; index++)
	{
		IRParameter* param = pMethod->Parameters[index];
		if ((param->Type->IsGeneric && !param->Type->IsGenericInstantiation) || param->Type->IsGenericParameter)
		{
			ResolveGenericMethodParametersInternal(pType, pMethod, &param->Type);
		}
	}

	for (uint32_t index = 0; index < pMethod->IRCodesCount; index++)
	{
		IRInstruction* instruction = pMethod->IRCodes[index];
		switch (instruction->Opcode)
		{
		case IROpcode::Dup:
		case IROpcode::Pop:
		case IROpcode::LoadIndirect:
		case IROpcode::StoreIndirect:
		case IROpcode::CastClass:
		case IROpcode::IsInst:
		case IROpcode::Unbox:
		case IROpcode::UnboxAny:
		case IROpcode::Box:
		case IROpcode::NewArray:
		case IROpcode::AllocateLocal:
		case IROpcode::SizeOf:
		case IROpcode::CallVirtual:
		case IROpcode::CallConstrained:
		case IROpcode::LoadVirtualFunction:
		case IROpcode::LoadToken:
		case IROpcode::RefAnyVal:
			if ((((IRType*)instruction->Arg1)->IsGeneric && !((IRType*)instruction->Arg1)->IsGenericInstantiation) ||
				((IRType*)instruction->Arg1)->IsGenericParameter)
			{
				ResolveGenericMethodParametersInternal(pType, pMethod, (IRType**)&instruction->Arg1);
			}
			break;
		case IROpcode::NewObject:
		case IROpcode::Jump:
		case IROpcode::CallAbsolute:
		case IROpcode::CallInternal:
		case IROpcode::LoadFunction:
			if (((IRMethod*)instruction->Arg1)->UsesGenerics())
			{
				Panic("Deal with this later");
			}
			break;
		case IROpcode::LoadStaticField:
		case IROpcode::LoadStaticFieldAddress:
		case IROpcode::StoreStaticField:
			if ((((IRField*)instruction->Arg1)->FieldType->IsGeneric && !((IRField*)instruction->Arg1)->FieldType->IsGenericInstantiation) ||
				((IRField*)instruction->Arg1)->FieldType->IsGenericParameter)
			{
				Panic("Deal with this later");
			}
			break;
		case IROpcode::LoadObject:
		case IROpcode::StoreObject:
		case IROpcode::CopyObject:
		case IROpcode::LoadField:
		case IROpcode::LoadFieldAddress:
		case IROpcode::StoreField:
		case IROpcode::LoadElement:
		case IROpcode::LoadElementAddress:
		case IROpcode::StoreElement:
		case IROpcode::InitializeObject:
		case IROpcode::MkRefAny:
			if ((((IRType*)instruction->Arg1)->IsGeneric && !((IRType*)instruction->Arg1)->IsGenericInstantiation) ||
				((IRType*)instruction->Arg1)->IsGenericParameter)
			{
				ResolveGenericMethodParametersInternal(pType, pMethod, (IRType**)&instruction->Arg1);
			}
			if ((((IRType*)instruction->Arg2)->IsGeneric && !((IRType*)instruction->Arg2)->IsGenericInstantiation) ||
				((IRType*)instruction->Arg2)->IsGenericParameter)
			{
				ResolveGenericMethodParametersInternal(pType, pMethod, (IRType**)&instruction->Arg2);
			}
			break;
		case IROpcode::Branch:
			if ((((IRType*)instruction->Arg3)->IsGeneric && !((IRType*)instruction->Arg3)->IsGenericInstantiation) ||
				((IRType*)instruction->Arg3)->IsGenericParameter)
			{
				ResolveGenericMethodParametersInternal(pType, pMethod, (IRType**)&instruction->Arg3);
			}
			if ((((IRType*)instruction->Arg4)->IsGeneric && !((IRType*)instruction->Arg4)->IsGenericInstantiation) ||
				((IRType*)instruction->Arg4)->IsGenericParameter)
			{
				ResolveGenericMethodParametersInternal(pType, pMethod, (IRType**)&instruction->Arg4);
			}
			break;
		default: break;
		}
	}
}

void CLR::Assembly::ResolveGenericMethodParametersInternal(IRType* pType, IRMethod* pMethod, IRType** pResolvingType)
{
	if (pType)
	{
		printf("Resolving for %s.%s.%s\n", pType->TypeDefinition->Namespace, pType->TypeDefinition->Name, pMethod->MethodDefinition->Name);
	}
	else
	{
		Panic("This shouldn't be occuring yet!");
	}
	IRType* type = *pResolvingType;
	if (type->IsGenericParameter)
	{
		if (type->IsGenericParameterFromParentType)
		{
			if (!pType) Panic("This shouldn't happen ever!");
			*pResolvingType = pType->GenericType->Parameters[type->GenericParameterIndex];
		}
		else
		{
			if (!pMethod->GenericMethod) Panic("This better not happen ever!");
			*pResolvingType = pMethod->GenericMethod->Parameters[type->GenericParameterIndex];
		}
		return;
	}
	IRGenericType key;
	IRGenericType* keyPtr = &key;
	key.GenericType = type;
	key.ParameterCount = type->GenericParameterCount;
	for (uint32_t index = 0; index < type->GenericParameterCount; ++index)
	{
		if (type->GenericType->Parameters[index]->IsGenericParameter)
		{
			if (type->IsGenericParameterFromParentType)
			{
				if (!pType) Panic("This shouldn't happen!");
				key.Parameters[index] = pType->GenericType->Parameters[type->GenericParameterIndex];
			}
			else
			{
				if (!pMethod->GenericMethod) Panic("This better not happen!");
				key.Parameters[index] = pMethod->GenericMethod->Parameters[type->GenericParameterIndex];
			}
		}
		else
		{
			key.Parameters[index] = type->GenericType->Parameters[index];
		}
	}
	IRGenericType* lookupType = NULL;
	HASH_FIND(HashHandle, GenericTypesHashTable, (void*)&keyPtr, offsetof(IRGenericType, ImplementationType), lookupType);
	if (!lookupType)
	{
		IRType* implementationType = type->GenericDeepCopy();
		implementationType->GenericType = new IRGenericType(type, implementationType);
		implementationType->GenericType->ParameterCount = pType->GenericType->ParameterCount;
		for (uint32_t index = 0; index < pType->GenericType->ParameterCount; ++index)
		{
			if (type->GenericType->Parameters[index]->IsGenericParameter)
			{
				if (type->IsGenericParameterFromParentType)
				{
					implementationType->GenericType->Parameters[index] = pType->GenericType->Parameters[type->GenericParameterIndex];
					if (pType->GenericType->Parameters[type->GenericParameterIndex]->IsGenericParameter)
					{
						printf("ResolveGenericMethodParameters from Type, GenericInstantiation Type %s.%s, Param %u Still Generic\n", implementationType->TypeDefinition->Namespace, implementationType->TypeDefinition->Name, (unsigned int)index);
					}
					else
					{
						printf("ResolveGenericMethodParameters from Type, GenericInstantiation Type %s.%s, Param %u Type %s.%s\n", implementationType->TypeDefinition->Namespace, implementationType->TypeDefinition->Name, (unsigned int)index, implementationType->GenericType->Parameters[index]->TypeDefinition->Namespace, implementationType->GenericType->Parameters[index]->TypeDefinition->Name);
					}
				}
				else
				{
					implementationType->GenericType->Parameters[index] = pMethod->GenericMethod->Parameters[type->GenericParameterIndex];
					if (pMethod->GenericMethod->Parameters[type->GenericParameterIndex]->IsGenericParameter)
					{
						printf("ResolveGenericMethodParameters from Method, GenericInstantiation Type %s.%s, Param %u Still Generic\n", implementationType->TypeDefinition->Namespace, implementationType->TypeDefinition->Name, (unsigned int)index);
					}
					else
					{
						printf("ResolveGenericMethodParameters from Method, GenericInstantiation Type %s.%s, Param %u Type %s.%s\n", implementationType->TypeDefinition->Namespace, implementationType->TypeDefinition->Name, (unsigned int)index, implementationType->GenericType->Parameters[index]->TypeDefinition->Namespace, implementationType->GenericType->Parameters[index]->TypeDefinition->Name);
					}
				}
			}
			else
			{
				implementationType->GenericType->Parameters[index] = type->GenericType->Parameters[index];
				printf("ResolveGenericMethodParameters, GenericInstantiation Type %s.%s, Param %u was already Type %s.%s\n", implementationType->TypeDefinition->Namespace, implementationType->TypeDefinition->Name, (unsigned int)index, implementationType->GenericType->Parameters[index]->TypeDefinition->Namespace, implementationType->GenericType->Parameters[index]->TypeDefinition->Name);
			}
		}
		*pResolvingType = implementationType;
		HASH_ADD(HashHandle, GenericTypesHashTable, GenericType, offsetof(IRGenericType, ImplementationType), implementationType->GenericType);
		ResolveGenericTypeParameters(implementationType);
	}
	else
	{
		*pResolvingType = lookupType->ImplementationType;
	}
}
