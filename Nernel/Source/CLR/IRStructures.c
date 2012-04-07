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
	free(pAssembly->Types);
	free(pAssembly->Fields);
	free(pAssembly->Methods);
    free(pAssembly);
}



IRType* IRType_Create(IRAssembly* pAssembly, TypeDefinition* pTypeDefinition)
{
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

	ILDecomposition_GetMethodLayout(type, pTypeDefinition);
	ILDecomposition_GetFieldLayout(type, pTypeDefinition);

	type->IsGeneric = pTypeDefinition->GenericParameterCount > 0;

	if (AppDomain_IsStructure(pTypeDefinition->File->Assembly->ParentDomain, pTypeDefinition))
	{
		type->IsValueType = TRUE;
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
			}
		}
	}
	return type;
}

IRType* IRType_Copy(IRType* pType)
{
	IRType* type = (IRType*)calloc(1, sizeof(IRType));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRType_Copy @ 0x%x, of 0x%x", (unsigned int)type, (unsigned int)pType);
	*type = *pType;
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
			type = pAssembly->Types[pMethodDefinition->TypeDefinition->TableIndex - 1];
		}
		IRParameter* parameter = IRParameter_Create(method, type);
		parameter->ParameterIndex = parameterIndex++;
		method->Parameters[parameter->ParameterIndex] = parameter;
	}
	for (uint32_t index = 0; index < pMethodDefinition->SignatureCache->ParameterCount; index++)
	{
		type = AppDomain_GetIRTypeFromSignatureType(pAssembly->ParentDomain, pAssembly, pMethodDefinition->SignatureCache->Parameters[index]->Type);
		IRParameter* parameter = IRParameter_Create(method, type);
		parameter->ParameterIndex = parameterIndex++;
		method->Parameters[parameter->ParameterIndex] = parameter;
	}

	MetadataToken* localsSignatureToken = CLIFile_ExpandMetadataToken(pMethodDefinition->File, pMethodDefinition->Body.LocalVariableSignatureToken);
	LocalsSignature* localsSignature = LocalsSignature_Expand(((StandAloneSignature*)localsSignatureToken->Data)->Signature, pMethodDefinition->File);
	method->LocalVariableCount = localsSignature->LocalVariableCount;
	method->LocalVariables = (IRLocalVariable**)calloc(1, method->LocalVariableCount * sizeof(IRLocalVariable*));
	uint32_t localVariableIndex = 0;
	for (uint32_t index = 0; index < localsSignature->LocalVariableCount; index++)
	{
		type = AppDomain_GetIRTypeFromSignatureType(pAssembly->ParentDomain, pAssembly, localsSignature->LocalVariables[index]->Type);
		IRLocalVariable* localVariable = IRLocalVariable_Create(method, type);
		localVariable->LocalVariableIndex = localVariableIndex++;
		method->LocalVariables[localVariable->LocalVariableIndex] = localVariable;
	}
	LocalsSignature_Destroy(localsSignature);
	CLIFile_DestroyMetadataToken(localsSignatureToken);
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


IRParameter* IRParameter_Create(IRMethod* pMethod, IRType* pType)
{
	IRParameter* parameter = (IRParameter*)calloc(1, sizeof(IRParameter));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRParameter_Create @ 0x%x", (unsigned int)parameter);
	parameter->Type = pType;
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

void IRLocalVariable_Destroy(IRLocalVariable* pLocalVariable)
{
	Log_WriteLine(LOGLEVEL__Memory, "Memory: IRLocalVariable_Destroy @ 0x%x", (unsigned int)pLocalVariable);
	free(pLocalVariable);
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
	if (pField->Flags & FieldAttributes_Static)
	{
		field->IsStatic = TRUE;
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
