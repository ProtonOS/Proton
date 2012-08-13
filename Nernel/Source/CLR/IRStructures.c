#include <CLR/ILDecomposition.h>
#include <CLR/IRStructures.h>

IRAssembly* IRAssembly_Create(AppDomain* pDomain, CLIFile *pFile)
{
    IRAssembly* assembly = (IRAssembly*)calloc(1, sizeof(IRAssembly));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRAssembly_Create @ 0x%x", (unsigned int)assembly);
	assembly->ParentDomain = pDomain;
	assembly->ParentFile = pFile;
	assembly->MethodCount = pFile->MethodDefinitionCount;
	assembly->Methods = (IRMethod**)calloc(1, pFile->MethodDefinitionCount * sizeof(IRMethod*));
	assembly->FieldCount = pFile->FieldCount;
	assembly->Fields = (IRField**)calloc(1, pFile->FieldCount * sizeof(IRField*));
	assembly->TypeCount = pFile->TypeDefinitionCount;
	assembly->Types = (IRType**)calloc(1, pFile->TypeDefinitionCount * sizeof(IRType*));
	for (uint32_t index = 1; index <= pFile->FieldCount; ++index) if ((pFile->Fields[index].Flags & (FieldAttributes_Static | FieldAttributes_Literal)) == FieldAttributes_Static) ++assembly->StaticFieldCount;
	assembly->StaticFields = (IRField**)calloc(1, assembly->StaticFieldCount * sizeof(IRField*));
	AppDomain_AddAssembly(pDomain, assembly);
    return assembly;
}

void IRAssembly_Destroy(IRAssembly* pAssembly)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRAssembly_Destroy @ 0x%x", (unsigned int)pAssembly);
	//AppDomain_RemoveAssembly(pAssembly);
	for (uint32_t index = 0; index < pAssembly->TypeCount; ++index)
	{
		if (pAssembly->Types[index])
		{
			IRType_Destroy(pAssembly->Types[index]);
		}
	}
	for (uint32_t index = 0; index < pAssembly->FieldCount; ++index)
	{
		if (pAssembly->Fields[index])
		{
			IRField_Destroy(pAssembly->Fields[index]);
		}
	}
	for (uint32_t index = 0; index < pAssembly->MethodCount; ++index)
	{
		if (pAssembly->Methods[index])
		{
			IRMethod_Destroy(pAssembly->Methods[index]);
		}
	}
	free(pAssembly->StaticFields);
	free(pAssembly->Types);
	free(pAssembly->Fields);
	free(pAssembly->Methods);
    free(pAssembly);
}

IRType* IRAssembly_MakePointerType(IRAssembly* pAssembly, IRType* pType)
{
	IRType* type = NULL;
	IRPointerType* lookupType = NULL;
	HASH_FIND(HashHandle, pAssembly->PointerTypesHashTable, (void*)&pType, sizeof(void*), lookupType);
	if (!lookupType)
	{
		IRType* sysPointerType = pAssembly->ParentDomain->IRAssemblies[0]->Types[pAssembly->ParentDomain->CachedType___System_IntPtr->TableIndex - 1];
		type = IRType_Copy(sysPointerType);
		type->IsPointerType = TRUE;
		type->PointerType = IRPointerType_Create(type, pType);
		HASH_ADD(HashHandle, pAssembly->PointerTypesHashTable, TypePointedTo, sizeof(void*), type->PointerType);
	}
	else
	{
		type = lookupType->PointerType;
	}
	return type;
}

IRType* IRAssembly_MakeArrayType(IRAssembly* pAssembly, IRType* pType)
{
	IRType* type = NULL;
	IRArrayType* lookupType = NULL;
	HASH_FIND(HashHandle, pAssembly->ArrayTypesHashTable, (void*)&pType, sizeof(void*), lookupType);
	if (!lookupType)
	{
		IRType* sysArrayType = pAssembly->ParentDomain->IRAssemblies[0]->Types[pAssembly->ParentDomain->CachedType___System_Array->TableIndex - 1];
		type = IRType_Copy(sysArrayType);
		type->IsArrayType = TRUE;
		type->ArrayType = IRArrayType_Create(type, pType);
		HASH_ADD(HashHandle, pAssembly->ArrayTypesHashTable, ElementType, sizeof(void*), type->ArrayType);
	}
	else
	{
		type = lookupType->ArrayType;
	}
	return type;
}

IRType* IRAssembly_CreateGenericParameter(IRAssembly* pAssembly, bool_t pIsFromParentType, uint32_t pIndex)
{
	IRType* type = (IRType*)calloc(1, sizeof(IRType));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: Generic Parameter @ 0x%x", (unsigned int)type);
	type->IsGeneric = TRUE;
	type->IsGenericParameter = TRUE;
	type->IsGenericParameterFromParentType = pIsFromParentType;
	type->GenericParameterIndex = pIndex;
	return type;
}

IRType* IRType_Create(IRAssembly* pAssembly, TypeDefinition* pTypeDefinition)
{
	if (!pAssembly || !pTypeDefinition)
	{
		printf("Invalid Assembly @ 0x%x, or TypeDefinition @ 0x%x\n", (unsigned int)pAssembly, (unsigned int)pTypeDefinition);
		Panic("Ruhroh!");
	}
	IRType* type = (IRType*)calloc(1, sizeof(IRType));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRType_Create @ 0x%x", (unsigned int)type);
	type->ParentAssembly = pAssembly;
	type->TypeDefinition = pTypeDefinition;
	type->TypeIndex = pTypeDefinition->TableIndex - 1;
	pAssembly->Types[type->TypeIndex] = type;
	if (pTypeDefinition->Flags & TypeAttributes_Interface)
	{
		type->IsInterface = TRUE;
	}
	if (pTypeDefinition->Flags & TypeAttributes_Abstract)
	{
		type->IsAbstract = TRUE;
	}

	ILDecomposition_GetMethodLayout(type, pTypeDefinition);
	ILDecomposition_GetFieldLayout(type, pTypeDefinition);

	type->IsGeneric = pTypeDefinition->GenericParameterCount > 0;
	type->GenericParameterCount = pTypeDefinition->GenericParameterCount;

	if (pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_Byte ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_Char ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_Double ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_Int16 ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_Int32 ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_Int64 ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_IntPtr ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_SByte ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_Single ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_UInt16 ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_UInt32 ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_UInt64 ||
		pTypeDefinition == pTypeDefinition->File->Assembly->ParentDomain->CachedType___System_UIntPtr)
	{
		type->IsValueType = TRUE;
	}
	else if (AppDomain_IsStructure(pTypeDefinition->File->Assembly->ParentDomain, pTypeDefinition))
	{
		type->IsValueType = TRUE;
		if (AppDomain_GetElementTypeFromIRType(pTypeDefinition->File->Assembly->ParentDomain, type) == (ElementType)-1)
		{
			type->IsStructureType = TRUE;
		}
	}
	else
	{
		type->IsReferenceType = TRUE;
	}

	for (uint32_t index = 0; index < pTypeDefinition->MethodDefinitionListCount; index++)
	{
		if ((pTypeDefinition->MethodDefinitionList[index].Flags & MethodAttributes_Static) != 0)
		{
			if (!strcmp(pTypeDefinition->MethodDefinitionList[index].Name, ".cctor"))
			{
				type->HasStaticConstructor = TRUE;
				if (pTypeDefinition->File->Assembly->Methods[pTypeDefinition->MethodDefinitionList[index].TableIndex - 1])
				{
					//printf("Method Existed\n");
					type->StaticConstructor = pTypeDefinition->File->Assembly->Methods[pTypeDefinition->MethodDefinitionList[index].TableIndex - 1];
				}
				else
				{
					//printf("Method didn't exist!\n");
					type->StaticConstructor = IRMethod_Create(pTypeDefinition->File->Assembly, &pTypeDefinition->MethodDefinitionList[index]);
				}
			}
		}
	}

	ILDecomposition_LinkType(pTypeDefinition->File->Assembly->ParentDomain, pTypeDefinition->File->Assembly, type);
	return type;
}

IRType* IRType_Copy(IRType* pType)
{
	IRType* type = (IRType*)calloc(1, sizeof(IRType));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRType_Copy @ 0x%x, of 0x%x", (unsigned int)type, (unsigned int)pType);
	*type = *pType;
	return type;
}

IRType* IRType_GenericDeepCopy(IRType* pType, IRAssembly* pAssembly)
{
	IRType* type = (IRType*)calloc(1, sizeof(IRType));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRType_GenericDeepCopy @ 0x%x, of 0x%x", (unsigned int)type, (unsigned int)pType);
	*type = *pType;
	type->ParentAssembly = pAssembly;
	type->FieldsLayedOut = FALSE;
	type->IsGenericInstantiation = TRUE;
	if (pType->StackSizeCalculated || pType->SizeCalculated) Panic("This should not be happening");

	type->Fields = (IRField**)calloc(1, pType->FieldCount * sizeof(IRField*));
	for (uint32_t index = 0; index < pType->FieldCount; ++index)
	{
		type->Fields[index] = IRField_Copy(pType->Fields[index]);
	}
	type->Methods = (IRMethod**)calloc(1, pType->MethodCount * sizeof(IRMethod*));
	for (uint32_t index = 0; index < pType->MethodCount; ++index)
	{
		if (ILDecomposition_MethodUsesGenerics(pType->Methods[index]))
		{
			type->Methods[index] = IRMethod_GenericDeepCopy(pType->Methods[index], pAssembly);
		}
		else
		{
			type->Methods[index] = pType->Methods[index];
		}
	}
	type->InterfaceTable = NULL;
	for (IRInterfaceImpl* iterator = pType->InterfaceTable; iterator; iterator = (IRInterfaceImpl*)iterator->HashHandle.next)
	{
		IRInterfaceImpl* newInterface = IRInterfaceImpl_Create(iterator->InterfaceType);
		HASH_ADD(HashHandle, type->InterfaceTable, InterfaceType, sizeof(void*), newInterface);
	}
	return type;
}

void IRType_Destroy(IRType* pType)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRType_Destroy @ 0x%x", (unsigned int)pType);
	// TODO: Deal with copies not freeing their fields/methods, or having their own deep copies
	free(pType->Fields);
	free(pType->Methods);
	free(pType);
}

bool_t IRType_IsSubclassOf(IRType* pType, IRType* pParentType)
{
	if (pType == pParentType) return TRUE;
	if (pParentType->IsInterface)
	{
		IRInterfaceImpl* lookupType = NULL;
		HASH_FIND(HashHandle, pType->InterfaceTable, pParentType, sizeof(void*), lookupType);
		if (lookupType) return TRUE;
	}

	if (pType->TypeDefinition->Extends.TypeDefinition == NULL) return FALSE;
	switch(pType->TypeDefinition->TypeOfExtends)
	{
		case TypeDefRefOrSpecType_TypeDefinition:
			return IRType_IsSubclassOf(pType->TypeDefinition->Extends.TypeDefinition->File->Assembly->Types[pType->TypeDefinition->Extends.TypeDefinition->TableIndex - 1], pParentType);
			break;
		case TypeDefRefOrSpecType_TypeReference:
			return IRType_IsSubclassOf(pType->TypeDefinition->Extends.TypeReference->ResolvedType->File->Assembly->Types[pType->TypeDefinition->Extends.TypeReference->ResolvedType->TableIndex - 1], pParentType);
			break;
		case TypeDefRefOrSpecType_TypeSpecification:
			Panic("This isn't supported yet, cannot determine the inheritence chain of a generic class yet.");
			break;
	}
	Panic("Invalid TypeOfExtends!");
}

IRMethod* IRMethod_Create(IRAssembly* pAssembly, MethodDefinition* pMethodDefinition)
{
    IRMethod* method = (IRMethod*)calloc(1, sizeof(IRMethod));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRMethod_Create @ 0x%x", (unsigned int)method);
	method->ParentAssembly = pAssembly;
	method->MethodDefinition = pMethodDefinition;
	method->MethodIndex = pMethodDefinition->TableIndex - 1;
	pAssembly->Methods[method->MethodIndex] = method;
	if (!pMethodDefinition->SignatureCache)
	{
		pMethodDefinition->SignatureCache = MethodSignature_Expand(pMethodDefinition->Signature, pMethodDefinition->File);
	}

	if (!pMethodDefinition->SignatureCache->ReturnType->Void)
	{
		method->Returns = TRUE;
		method->ReturnType = AppDomain_GetIRTypeFromSignatureType(pAssembly->ParentDomain, pAssembly, pMethodDefinition->SignatureCache->ReturnType->Type);
	}

	method->IsGeneric = pMethodDefinition->GenericParameterCount > 0;
	method->GenericParameterCount = pMethodDefinition->GenericParameterCount;

	method->ParameterCount = pMethodDefinition->SignatureCache->ParameterCount;
	bool_t addingThis = FALSE;
	if (pMethodDefinition->SignatureCache->HasThis && !pMethodDefinition->SignatureCache->ExplicitThis)
	{
		addingThis = TRUE;
		method->ParameterCount++;
	}
	method->Parameters = (IRParameter**)calloc(1, method->ParameterCount * sizeof(IRParameter*));
	IRType* type = NULL;
	uint32_t parameterIndex = 0;
	if (addingThis)
	{
		if (AppDomain_IsStructure(pAssembly->ParentDomain, pMethodDefinition->TypeDefinition))
		{
			type = pAssembly->Types[pAssembly->ParentDomain->CachedType___System_IntPtr->TableIndex - 1];
		}
		else
		{
			if (!(type = pAssembly->Types[pMethodDefinition->TypeDefinition->TableIndex - 1]))
			{
				type = IRType_Create(pAssembly, pMethodDefinition->TypeDefinition);
			}
		}
		IRParameter* parameter = IRParameter_Create(method, type);
		parameter->ParameterIndex = parameterIndex++;
		method->Parameters[parameter->ParameterIndex] = parameter;
	}
	for (uint32_t index = 0; index < pMethodDefinition->SignatureCache->ParameterCount; index++)
	{
		if (pMethodDefinition->SignatureCache->Parameters[index]->TypedByReference)
		{
			type = pAssembly->ParentDomain->IRAssemblies[0]->Types[pAssembly->ParentDomain->CachedType___System_TypedReference->TableIndex - 1];
		}
		else
		{
			type = AppDomain_GetIRTypeFromSignatureType(pAssembly->ParentDomain, pAssembly, pMethodDefinition->SignatureCache->Parameters[index]->Type);
		}
		IRParameter* parameter = IRParameter_Create(method, type);
		parameter->ParameterIndex = parameterIndex++;
		method->Parameters[parameter->ParameterIndex] = parameter;
	}
	return method;
}

IRMethod* IRMethod_GenericDeepCopy(IRMethod* pMethod, IRAssembly* pAssembly)
{
    IRMethod* method = (IRMethod*)calloc(1, sizeof(IRMethod));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRMethod_GenericDeepCopy @ 0x%x, of 0x%x", (unsigned int)method, (unsigned int)pMethod);
	*method = *pMethod;
	method->ParentAssembly = pAssembly;
	method->IRCodes = (IRInstruction**)calloc(1, method->IRCodesCount * sizeof(IRInstruction*));
	for (uint32_t index = 0; index < pMethod->IRCodesCount; ++index)
	{
		method->IRCodes[index] = IRInstruction_Copy(pMethod->IRCodes[index]);
	}
	method->LocalVariables = (IRLocalVariable**)calloc(1, method->LocalVariableCount * sizeof(IRLocalVariable*));
	for (uint32_t index = 0; index < pMethod->LocalVariableCount; ++index)
	{
		method->LocalVariables[index] = IRLocalVariable_Copy(pMethod->LocalVariables[index]);
	}
	method->Parameters = (IRParameter**)calloc(1, method->ParameterCount * sizeof(IRParameter*));
	for (uint32_t index = 0; index < pMethod->ParameterCount; ++index)
	{
		method->Parameters[index] = IRParameter_Copy(pMethod->Parameters[index]);
	}
	if (method->IsGeneric)
	{
		method->GenericParameterCount = 0;
	}
	return method;
}

void IRMethod_Destroy(IRMethod* pMethod)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRMethod_Destroy @ 0x%x", (unsigned int)pMethod);
	for (uint32_t index = 0; index < pMethod->LocalVariableCount; ++index)
	{
		if (pMethod->LocalVariables[index])
		{
			IRLocalVariable_Destroy(pMethod->LocalVariables[index]);
		}
	}
	for (uint32_t index = 0; index < pMethod->ParameterCount; ++index)
	{
		if (pMethod->Parameters[index])
		{
			IRParameter_Destroy(pMethod->Parameters[index]);
		}
	}
	free(pMethod->LocalVariables);
	free(pMethod->Parameters);
    free(pMethod);
}

void IRMethod_AddInstruction(IRMethod* pMethod, IRInstruction* pInstruction)
{
	pInstruction->IRLocation = pMethod->IRCodesCount;
	pMethod->IRCodes = (IRInstruction**)realloc(pMethod->IRCodes, sizeof(IRInstruction*) * (pMethod->IRCodesCount + 1));
	pMethod->IRCodes[pMethod->IRCodesCount] = pInstruction;
	pMethod->IRCodesCount++;
}

void IRMethod_InsertInstruction(IRMethod* pMethod, uint32_t pInsertIndex, IRInstruction* pInstruction)
{
	uint32_t tempCount = (pMethod->IRCodesCount - pInsertIndex);
	IRInstruction** temp = NULL;
	if (tempCount > 0)
	{
		temp = (IRInstruction**)malloc((pMethod->IRCodesCount - pInsertIndex) * sizeof(IRInstruction*));
		for (uint32_t index = 0; index < tempCount; ++index)
		{
			temp[index] = pMethod->IRCodes[pInsertIndex + index];
		}
	}

	pInstruction->IRLocation = pInsertIndex;
	pMethod->IRCodes = (IRInstruction**)realloc(pMethod->IRCodes, sizeof(IRInstruction*) * (pMethod->IRCodesCount + 1));
	pMethod->IRCodes[pInsertIndex] = pInstruction;
	pMethod->IRCodesCount++;
	for (uint32_t index = 0; index < tempCount; ++index)
	{
		temp[index]->IRLocation++;
		pMethod->IRCodes[pInsertIndex + 1 + index] = temp[index];
	}
	free(temp);
}

void IRMethod_AddLocal(IRMethod* pMethod, IRLocalVariable* pLocal)
{
	pLocal->LocalVariableIndex = pMethod->LocalVariableCount;
	pMethod->LocalVariables = (IRLocalVariable**)realloc(pMethod->LocalVariables, sizeof(IRLocalVariable*) * (pMethod->LocalVariableCount + 1));
	pMethod->LocalVariables[pMethod->LocalVariableCount] = pLocal;
	pMethod->LocalVariableCount++;
}


IRParameter* IRParameter_Create(IRMethod* pMethod, IRType* pType)
{
	IRParameter* parameter = (IRParameter*)calloc(1, sizeof(IRParameter));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRParameter_Create @ 0x%x", (unsigned int)parameter);
	parameter->Type = pType;
	return parameter;
}

IRParameter* IRParameter_Copy(IRParameter* pParameter)
{
	IRParameter* parameter = (IRParameter*)calloc(1, sizeof(IRParameter));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRParameter_Copy @ 0x%x", (unsigned int)parameter);
	*parameter = *pParameter;
	return parameter;
}

void IRParameter_Destroy(IRParameter* pParameter)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRParameter_Destroy @ 0x%x", (unsigned int)pParameter);
	free(pParameter);
}

IRLocalVariable* IRLocalVariable_Create(IRMethod* pMethod, IRType* pType)
{
	IRLocalVariable* localVariable = (IRLocalVariable*)calloc(1, sizeof(IRLocalVariable));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRLocalVariable_Create @ 0x%x", (unsigned int)localVariable);
	localVariable->VariableType = pType;
	return localVariable;
}

IRLocalVariable* IRLocalVariable_Copy(IRLocalVariable* pLocalVariable)
{
	IRLocalVariable* localVariable = (IRLocalVariable*)calloc(1, sizeof(IRLocalVariable));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRLocalVariable_Copy @ 0x%x", (unsigned int)localVariable);
	*localVariable = *pLocalVariable;
	if (pLocalVariable->SSAData)
	{
		localVariable->SSAData = IRLocalSSAData_Copy(pLocalVariable->SSAData);
	}
	return localVariable;
}

void IRLocalVariable_Destroy(IRLocalVariable* pLocalVariable)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRLocalVariable_Destroy @ 0x%x", (unsigned int)pLocalVariable);
	if (pLocalVariable->SSAData)
	{
		IRLocalSSAData_Destroy(pLocalVariable->SSAData);
	}
	free(pLocalVariable);
}

IRLocalSSAData* IRLocalSSAData_Create(IRLocalVariable* pDerivedLocalVariable)
{
	IRLocalSSAData* data = (IRLocalSSAData*)calloc(1, sizeof(IRLocalSSAData));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRLocalSSAData_Create @ 0x%x", (unsigned int)data);
	data->Derived = pDerivedLocalVariable;
	return data;
}

IRLocalSSAData* IRLocalSSAData_Copy(IRLocalSSAData* pLocalSSAData)
{
	IRLocalSSAData* data = (IRLocalSSAData*)calloc(1, sizeof(IRLocalSSAData));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRLocalSSAData_Copy @ 0x%x", (unsigned int)data);
	*data = *pLocalSSAData;
	return data;
}

void IRLocalSSAData_Destroy(IRLocalSSAData* pLocalSSAData)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRLocalSSAData_Destroy @ 0x%x", (unsigned int)pLocalSSAData);
	free(pLocalSSAData);
}

IRField* IRField_Create(IRType* pType, Field* pField)
{
	IRField* field = (IRField*)calloc(1, sizeof(IRField));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRField_Create @ 0x%x", (unsigned int)field);
	field->FieldIndex = pField->TableIndex - 1;
	field->ParentAssembly = pType->ParentAssembly;
	field->FieldDefinition = pField;
	field->ParentType = pType;
	pType->ParentAssembly->Fields[field->FieldIndex] = field;
	if (!pField->SignatureCache)
	{
		pField->SignatureCache = FieldSignature_Expand(pField->Signature, pField->File);
	}
	field->FieldType = AppDomain_GetIRTypeFromSignatureType(field->ParentAssembly->ParentDomain, pField->File->Assembly, pField->SignatureCache->Type);
	if ((pField->Flags & (FieldAttributes_Static | FieldAttributes_Literal)) == FieldAttributes_Static)
	{
		field->IsStatic = TRUE;
		field->StaticFieldIndex = pType->ParentAssembly->StaticFieldIndex;
		pType->ParentAssembly->StaticFields[field->StaticFieldIndex] = field;
		++pType->ParentAssembly->StaticFieldIndex;
	}
	return field;
}

IRField* IRField_Copy(IRField* pField)
{
	IRField* field = (IRField*)calloc(1, sizeof(IRField));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRField_Copy @ 0x%x, of 0x%x", (unsigned int)field, (unsigned int)pField);
	*field = *pField;
	return field;
}

void IRField_Destroy(IRField* pField)
{
	free(pField);
}

IRArrayType* IRArrayType_Create(IRType* pArrayType, IRType* pElementType)
{
	IRArrayType* type = (IRArrayType*)calloc(1, sizeof(IRArrayType));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRArrayType_Create @ 0x%x", (unsigned int)type);
	type->ArrayType = pArrayType;
	type->ElementType = pElementType;
	return type;
}

void IRArrayType_Destroy(IRArrayType* pArrayType)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRArrayType_Destroy @ 0x%x", (unsigned int)pArrayType);
	free(pArrayType);
}

IRPointerType* IRPointerType_Create(IRType* pPointerType, IRType* pTypePointedTo)
{
	IRPointerType* type = (IRPointerType*)calloc(1, sizeof(IRPointerType));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRPointerType_Create @ 0x%x", (unsigned int)type);
	type->PointerType = pPointerType;
	type->TypePointedTo = pTypePointedTo;
	return type;
}

void IRPointerType_Destroy(IRPointerType* pPointerType)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRPointerType_Destroy @ 0x%x", (unsigned int)pPointerType);
	free(pPointerType);
}

IRInterfaceImpl* IRInterfaceImpl_Create(IRType* pInterfaceType)
{
	IRInterfaceImpl* interfaceImplementation = (IRInterfaceImpl*)calloc(1, sizeof(IRInterfaceImpl));
	interfaceImplementation->InterfaceType = pInterfaceType;
	interfaceImplementation->MethodCount = pInterfaceType->MethodCount;
	interfaceImplementation->MethodIndexes = (uint32_t*)calloc(1, sizeof(uint32_t) * pInterfaceType->MethodCount);
	return interfaceImplementation;
}

void IRInterfaceImpl_Destroy(IRInterfaceImpl* pInterfaceImpl)
{
	free(pInterfaceImpl->MethodIndexes);
	free(pInterfaceImpl);
}

IRGenericType* IRGenericType_Create(IRType* pGenericType, IRType* pImplementationType)
{
	IRGenericType* type = (IRGenericType*)calloc(1, sizeof(IRGenericType));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRGenericType_Create @ 0x%x", (unsigned int)type);
	type->GenericType = pGenericType;
	type->ImplementationType = pImplementationType;
	return type;
}

void IRGenericType_Destroy(IRGenericType* pGenericType)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRGenericType_Destroy @ 0x%x", (unsigned int)pGenericType);
	free(pGenericType);
}


IRInstruction* IRInstruction_Create(uint32_t pILLocation, IROpcode pOpcode)
{
	IRInstruction* instruction = (IRInstruction*)calloc(1, sizeof(IRInstruction));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRInstruction_Create @ 0x%x", (unsigned int)instruction);
	instruction->ILLocation = pILLocation;
	instruction->Opcode = pOpcode;
	return instruction;
}

IRInstruction* IRInstruction_Copy(IRInstruction* pInstruction)
{
	IRInstruction* instruction = (IRInstruction*)calloc(1, sizeof(IRInstruction));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRInstruction_Copy @ 0x%x", (unsigned int)instruction);
	*instruction = *pInstruction;
	return instruction;
}

void IRInstruction_Destroy(IRInstruction* pInstruction)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRInstruction_Destroy @ 0x%x", (unsigned int)pInstruction);
	if (pInstruction->Source1.Type == SourceType_Indirect)
		free(pInstruction->Source1.Data.Indirect.AddressSource);
	else if (pInstruction->Source1.Type == SourceType_Field)
		free(pInstruction->Source1.Data.Field.FieldSource);
	else if (pInstruction->Source1.Type == SourceType_FieldAddress)
		free(pInstruction->Source1.Data.FieldAddress.FieldSource);
	else if (pInstruction->Source1.Type == SourceType_ArrayLength)
		free(pInstruction->Source1.Data.ArrayLength.ArraySource);
	else if (pInstruction->Source1.Type == SourceType_ArrayElement)
	{
		free(pInstruction->Source1.Data.ArrayElement.ArraySource);
		free(pInstruction->Source1.Data.ArrayElement.IndexSource);
	}
	else if (pInstruction->Source1.Type == SourceType_ArrayElementAddress)
	{
		free(pInstruction->Source1.Data.ArrayElementAddress.ArraySource);
		free(pInstruction->Source1.Data.ArrayElementAddress.IndexSource);
	}

	if (pInstruction->Source2.Type == SourceType_Indirect)
		free(pInstruction->Source2.Data.Indirect.AddressSource);
	else if (pInstruction->Source2.Type == SourceType_Field)
		free(pInstruction->Source2.Data.Field.FieldSource);
	else if (pInstruction->Source2.Type == SourceType_FieldAddress)
		free(pInstruction->Source2.Data.FieldAddress.FieldSource);
	else if (pInstruction->Source2.Type == SourceType_ArrayLength)
		free(pInstruction->Source2.Data.ArrayLength.ArraySource);
	else if (pInstruction->Source2.Type == SourceType_ArrayElement)
	{
		free(pInstruction->Source2.Data.ArrayElement.ArraySource);
		free(pInstruction->Source2.Data.ArrayElement.IndexSource);
	}
	else if (pInstruction->Source2.Type == SourceType_ArrayElementAddress)
	{
		free(pInstruction->Source2.Data.ArrayElementAddress.ArraySource);
		free(pInstruction->Source2.Data.ArrayElementAddress.IndexSource);
	}

	if (pInstruction->Source3.Type == SourceType_Indirect)
		free(pInstruction->Source3.Data.Indirect.AddressSource);
	else if (pInstruction->Source3.Type == SourceType_Field)
		free(pInstruction->Source3.Data.Field.FieldSource);
	else if (pInstruction->Source3.Type == SourceType_FieldAddress)
		free(pInstruction->Source3.Data.FieldAddress.FieldSource);
	else if (pInstruction->Source3.Type == SourceType_ArrayLength)
		free(pInstruction->Source3.Data.ArrayLength.ArraySource);
	else if (pInstruction->Source3.Type == SourceType_ArrayElement)
	{
		free(pInstruction->Source3.Data.ArrayElement.ArraySource);
		free(pInstruction->Source3.Data.ArrayElement.IndexSource);
	}
	else if (pInstruction->Source3.Type == SourceType_ArrayElementAddress)
	{
		free(pInstruction->Source3.Data.ArrayElementAddress.ArraySource);
		free(pInstruction->Source3.Data.ArrayElementAddress.IndexSource);
	}

	if (pInstruction->Destination.Type == SourceType_Indirect)
		free(pInstruction->Destination.Data.Indirect.AddressSource);
	else if (pInstruction->Destination.Type == SourceType_Field)
		free(pInstruction->Destination.Data.Field.FieldSource);
	else if (pInstruction->Destination.Type == SourceType_FieldAddress)
		free(pInstruction->Destination.Data.FieldAddress.FieldSource);
	else if (pInstruction->Destination.Type == SourceType_ArrayLength)
		free(pInstruction->Destination.Data.ArrayLength.ArraySource);
	else if (pInstruction->Destination.Type == SourceType_ArrayElement)
	{
		free(pInstruction->Destination.Data.ArrayElement.ArraySource);
		free(pInstruction->Destination.Data.ArrayElement.IndexSource);
	}
	else if (pInstruction->Destination.Type == SourceType_ArrayElementAddress)
	{
		free(pInstruction->Destination.Data.ArrayElementAddress.ArraySource);
		free(pInstruction->Destination.Data.ArrayElementAddress.IndexSource);
	}

	free(pInstruction);
}
