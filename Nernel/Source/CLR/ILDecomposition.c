#include <Inline.h>
#include <CLR/ILDecomposition.h>
#include <CLR/ILDecompositionMacros.h>
#include <CLR/ILOpcode.h>
#include <CLR/IROpcode.h>
#include <CLR/SyntheticStack.h>
#include <System/Multiboot.h>


IRAssembly* ILDecomposition_CreateAssembly(AppDomain* pDomain, CLIFile* pFile)
{
	IRAssembly* assembly = IRAssembly_Create(pDomain, pFile);
	pFile->Assembly = assembly;

	if (pFile->AssemblyReferenceCount > 0)
	{
		char buf[FILENAME_MAX];
		char buf2[FILENAME_MAX];
		for (uint32_t i = 1; i <= pFile->AssemblyReferenceCount; i++)
		{
			AssemblyReference* r = &pFile->AssemblyReferences[i];
			bool_t foundAssembly = FALSE;
			for (uint32_t i2 = 0; i2 < pDomain->IRAssemblyCount; i2++)
			{
				if (!strcmp(pDomain->IRAssemblies[i2]->ParentFile->AssemblyDefinitions[1].Name, r->Name))
				{
					foundAssembly = TRUE;
					break;
				}
			}
			if (!foundAssembly)
			{
				snprintf(buf, FILENAME_MAX, "/gac/%s.dll", r->Name);
				snprintf(buf2, FILENAME_MAX, "%s.dll", r->Name);
				LoadedModule* loadedModule = Multiboot_GetLoadedModule(buf);
				if (!loadedModule)
				{
					snprintf(buf, FILENAME_MAX, "/gac/proton/%s.dll", r->Name);
					loadedModule = Multiboot_GetLoadedModule(buf);
				}
				if (!loadedModule)
				{
					Panic("Unable to resolve dependancy!");
				}
				CLIFile* file = CLIFile_Create((uint8_t*)loadedModule->Address, loadedModule->Size, buf2);
				ILDecomposition_CreateAssembly(pDomain, file);
			}
		}
		AppDomain_ResolveReferences(pDomain, pFile);
	}

	if (!pDomain->IRAssemblies[0]->Types[pDomain->CachedType___System_Object->TableIndex - 1])
	{
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Array);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Boolean);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Byte);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Char);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Double);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Enum);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Exception);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Int16);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Int32);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Int64);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_IntPtr);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Object);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_RuntimeType);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_SByte);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Single);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_String);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Type);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_TypedReference);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_UInt16);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_UInt32);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_UInt64);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_UIntPtr);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_ValueType);
		IRType_Create(pDomain->IRAssemblies[0], pDomain->CachedType___System_Void);
	}

	if (pFile->CLIHeader->EntryPointToken)
	{
		MetadataToken* token = CLIFile_ExpandMetadataToken(pFile, pFile->CLIHeader->EntryPointToken);
		if (token->Table != MetadataTable_MethodDefinition)
			Panic("Unknown entry point table!");
		assembly->EntryPoint = IRMethod_Create(assembly, (MethodDefinition*)token->Data);
		CLIFile_DestroyMetadataToken(token);
	}

	return assembly;
}


IRMethod** ILDecomposition_GetMethodLayout(IRType* pType, TypeDefinition* pTypeDefinition)
{
	if (pType->Methods)
	{
		Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "Returning cached layout of the methods of %s.%s, TypeOfExtends = %u", pTypeDefinition->Namespace, pTypeDefinition->Name, (unsigned int)pTypeDefinition->TypeOfExtends);
		return pType->Methods;
	}
	Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "Laying out the methods of %s.%s, TypeOfExtends = %u", pTypeDefinition->Namespace, pTypeDefinition->Name, (unsigned int)pTypeDefinition->TypeOfExtends);
	IRMethod** methods = NULL;
	uint32_t methodCount = 0;
	if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeDefinition)
	{
		if (pTypeDefinition->Extends.TypeDefinition != NULL)
		{
			IRType* type = pType->ParentAssembly->Types[pTypeDefinition->Extends.TypeDefinition->TableIndex - 1];
			if (!type)
			{
				type = IRType_Create(pType->ParentAssembly, pTypeDefinition->Extends.TypeDefinition);
			}
			methods = ILDecomposition_GetMethodLayout(type, pTypeDefinition->Extends.TypeDefinition);
			methodCount = type->MethodCount;
		}	
	}
	else if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeReference)
	{
		if (pTypeDefinition->Extends.TypeReference != NULL)
		{
			IRType* type = pTypeDefinition->Extends.TypeReference->ResolvedType->File->Assembly->Types[pTypeDefinition->Extends.TypeReference->ResolvedType->TableIndex - 1];
			if (!type)
			{
				type = IRType_Create(pTypeDefinition->Extends.TypeReference->ResolvedType->File->Assembly, pTypeDefinition->Extends.TypeReference->ResolvedType);
			}
			methods = ILDecomposition_GetMethodLayout(type, pTypeDefinition->Extends.TypeReference->ResolvedType);
			methodCount = type->MethodCount;
		}
		else
			Panic("TypeRef Extends NULL!");
	}
	else if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeSpecification)
	{
		//*pTotalMethodCount = 0;
		return NULL;
		//Panic("Type Spec!");
	}

	uint32_t newMethodsCount = 0;
	for (uint32_t index = 0; index < pTypeDefinition->MethodDefinitionListCount; index++)
	{
		if (pTypeDefinition->MethodDefinitionList[index].Flags & MethodAttributes_Virtual)
		{
			if (pTypeDefinition->MethodDefinitionList[index].Flags & MethodAttributes_NewSlot)
			{
				newMethodsCount++;
			}
		}
		else
		{
			if (!(pTypeDefinition->MethodDefinitionList[index].Flags & MethodAttributes_Static) && 
				!(pTypeDefinition->MethodDefinitionList[index].Flags & MethodAttributes_RTSpecialName)
				)
			{
				newMethodsCount++;
			}
		}
	}
	Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "MethodCount: %i NewMethodCount: %i", (int)methodCount, (int)newMethodsCount);

	IRMethod** finalMethods = (IRMethod**)calloc(methodCount + newMethodsCount, sizeof(IRMethod*));
	memcpy(finalMethods, methods, methodCount * sizeof(IRMethod*));

	uint32_t methodIndex = 0;
	for (uint32_t index = 0; index < pTypeDefinition->MethodDefinitionListCount; index++)
	{
		if (pTypeDefinition->MethodDefinitionList[index].Flags & MethodAttributes_Virtual)
		{
			if (pTypeDefinition->MethodDefinitionList[index].Flags & MethodAttributes_NewSlot)
			{
				IRMethod* method = pTypeDefinition->File->Assembly->Methods[pTypeDefinition->MethodDefinitionList[index].TableIndex - 1];
				if (!method)
				{
					method = IRMethod_Create(pTypeDefinition->File->Assembly, &pTypeDefinition->MethodDefinitionList[index]);
				}
				finalMethods[methodCount + methodIndex] = method;
				Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "Adding method %s.%s.%s from table index %i at %i", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->MethodDefinitionList[index].Name, (int)pTypeDefinition->MethodDefinitionList[index].TableIndex, (int)(methodCount + methodIndex));
				methodIndex++;
			}
			else
			{
				bool_t found = FALSE;
				Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "Looking for base method of %s.%s.%s (Table Index %i)", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->MethodDefinitionList[index].Name, (int)pTypeDefinition->MethodDefinitionList[index].TableIndex);
				for (uint32_t index2 = 0; index2 < methodCount; index2++)
				{
					MethodDefinition* methodDefinition = methods[index2]->MethodDefinition;
					if (methodDefinition->TableIndex)
					{
						Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "Checking Method %s.%s.%s from table index %i", methodDefinition->TypeDefinition->Namespace, methodDefinition->TypeDefinition->Name, methodDefinition->Name, (int)methodDefinition->TableIndex);
					}
					else
					{
						Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "Method Table Index 0! This shouldn't happen!");
					}

					if (!strcmp(pTypeDefinition->MethodDefinitionList[index].Name, methodDefinition->Name))
					{
						Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "Name is the same");
						if (Signature_Equals(pTypeDefinition->MethodDefinitionList[index].Signature, pTypeDefinition->MethodDefinitionList[index].SignatureLength, methodDefinition->Signature, methodDefinition->SignatureLength))
						{
							Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "Found Match!");
							IRMethod* method = pTypeDefinition->File->Assembly->Methods[pTypeDefinition->MethodDefinitionList[index].TableIndex - 1];
							if (!method)
							{
								method = IRMethod_Create(pTypeDefinition->File->Assembly, &pTypeDefinition->MethodDefinitionList[index]);
							}
							finalMethods[index2] = method; 
							Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "Overloading method %s.%s.%s from table index %i at %i", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->MethodDefinitionList[index].Name, (int)pTypeDefinition->MethodDefinitionList[index].TableIndex, (int)index2);
							found = TRUE;
							break;
						}
					}
				}
				if (!found)
				{
					Panic("Unable to resolve base method of override!");
				}
			}
		}
		else
		{
			if (!(pTypeDefinition->MethodDefinitionList[index].Flags & MethodAttributes_Static) && 
				!(pTypeDefinition->MethodDefinitionList[index].Flags & MethodAttributes_RTSpecialName)
				)
			{
				IRMethod* method = pTypeDefinition->File->Assembly->Methods[pTypeDefinition->MethodDefinitionList[index].TableIndex - 1];
				if (!method)
				{
					method = IRMethod_Create(pTypeDefinition->File->Assembly, &pTypeDefinition->MethodDefinitionList[index]);
				}
				finalMethods[methodCount + methodIndex] = method;
				Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "Adding method %s.%s.%s from table index %i at %i", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->MethodDefinitionList[index].Name, (int)pTypeDefinition->MethodDefinitionList[index].TableIndex, (int)(methodCount + methodIndex));
				methodIndex++;
			}
			else
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_MethodLayout, "Ignoring method %s.%s.%s at table index %i", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->MethodDefinitionList[index].Name, (int)pTypeDefinition->MethodDefinitionList[index].TableIndex);
			}
		}
	}

	//*pTotalMethodCount = methodCount + newMethodsCount;
	pType->Methods = finalMethods;
	pType->MethodCount = methodCount + newMethodsCount;
	return pType->Methods;
}

IRField** ILDecomposition_GetFieldLayout(IRType* pType, TypeDefinition* pTypeDefinition)
{
	if (pType->Fields)
	{
		Log_WriteLine(LOGLEVEL__ILDecomposition_FieldLayout, "Returning cached layout of the fields of %s.%s, TypeOfExtends = %u", pTypeDefinition->Namespace, pTypeDefinition->Name, (unsigned int)pTypeDefinition->TypeOfExtends);
		return pType->Fields;
	}
	Log_WriteLine(LOGLEVEL__ILDecomposition_FieldLayout, "Laying out the fields of %s.%s, TypeOfExtends = %u", pTypeDefinition->Namespace, pTypeDefinition->Name, (unsigned int)pTypeDefinition->TypeOfExtends);
	IRField** fields = NULL;
	uint32_t fieldCount = 0;
	if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeDefinition)
	{
		if (pTypeDefinition->Extends.TypeDefinition != NULL)
		{
			IRType* type = pType->ParentAssembly->Types[pTypeDefinition->Extends.TypeDefinition->TableIndex - 1];
			if (!type)
			{
				type = IRType_Create(pType->ParentAssembly, pTypeDefinition->Extends.TypeDefinition);
			}
			fields = ILDecomposition_GetFieldLayout(type, pTypeDefinition->Extends.TypeDefinition);
			fieldCount = type->FieldCount;
		}
	}
	else if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeReference)
	{
		if (pTypeDefinition->Extends.TypeReference != NULL)
		{
			IRType* type = pTypeDefinition->Extends.TypeReference->ResolvedType->File->Assembly->Types[pTypeDefinition->Extends.TypeReference->ResolvedType->TableIndex - 1];
			if (!type)
			{
				type = IRType_Create(pTypeDefinition->Extends.TypeReference->ResolvedType->File->Assembly, pTypeDefinition->Extends.TypeReference->ResolvedType);
			}
			fields = ILDecomposition_GetFieldLayout(type, pTypeDefinition->Extends.TypeReference->ResolvedType);
			fieldCount = type->FieldCount;
		}
		else 
			Panic("TypeRef Extends NULL!");
	}
	else if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeSpecification)
	{
		//*pTotalFieldCount = 0;
		return NULL;
	}

	uint32_t newFieldsCount = 0;
	for (uint32_t i = 0; i < pTypeDefinition->FieldListCount; i++)
	{
		if (!(pTypeDefinition->FieldList[i].Flags & FieldAttributes_Static))
		{
			newFieldsCount++;
		}
	}
	Log_WriteLine(LOGLEVEL__ILDecomposition_FieldLayout, "Field count: %i NewFieldCount: %i", (int)fieldCount, (int)newFieldsCount);

	IRField** finalFields = (IRField**)calloc(fieldCount + newFieldsCount, sizeof(IRField*));
	memcpy(finalFields, fields, fieldCount * sizeof(IRField*));

	uint32_t fieldIndex = 0;
	for (uint32_t index = 0; index < pTypeDefinition->FieldListCount; index++)
	{
		if (!(pTypeDefinition->FieldList[index].Flags & FieldAttributes_Static))
		{
			IRField* field = pTypeDefinition->File->Assembly->Fields[pTypeDefinition->FieldList[index].TableIndex - 1];
			if (!field)
			{
				field = IRField_Create(pType, &pTypeDefinition->FieldList[index]);
			}
			finalFields[fieldCount + fieldIndex] = field;
			Log_WriteLine(LOGLEVEL__ILDecomposition_FieldLayout, "Adding Field %s.%s.%s from table index %i at %i", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->FieldList[index].Name, (int)pTypeDefinition->FieldList[index].TableIndex, (int)(fieldCount + fieldIndex));
			fieldIndex++;
		}
	}

	//*pTotalFieldCount = fieldCount + newFieldsCount;
	pType->Fields = finalFields;
	pType->FieldCount = fieldCount + newFieldsCount;
	return pType->Fields;
}

ALWAYS_INLINE void ILDecomposition_CheckBinaryNumericOperandTypesAndSetResult(IRType* pOperandA, IRType* pOperandB, BinaryNumericOperation pBinaryNumericOperation, StackObject* pResultObject)
{
	AppDomain* domain = pOperandA->ParentAssembly->ParentDomain;
	TypeDefinition* operandAType = pOperandA->TypeDefinition;
	TypeDefinition* operandBType = pOperandB->TypeDefinition;
	//Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Operand A: 0x%x", (unsigned int)pOperandA); 
	//Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_IREmitter, "Operand B: 0x%x", (unsigned int)pOperandB);
	if (operandAType == domain->CachedType___System_Byte ||
		operandAType == domain->CachedType___System_UInt16 ||
		operandAType == domain->CachedType___System_UInt32 ||
		operandAType == domain->CachedType___System_SByte ||
		operandAType == domain->CachedType___System_Int16 ||
		operandAType == domain->CachedType___System_Int32
		)
	{
		if (pBinaryNumericOperation == BinaryNumericOperation_Add)
		{
			if (pOperandB->IsPointerType)
			{
				pResultObject->Type = pOperandB;
			}
		}
		if (operandBType == domain->CachedType___System_Byte ||
			operandBType == domain->CachedType___System_UInt16 ||
			operandBType == domain->CachedType___System_UInt32 ||
			operandBType == domain->CachedType___System_SByte ||
			operandBType == domain->CachedType___System_Int16 ||
			operandBType == domain->CachedType___System_Int32
			)
		{
			pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
		}
		else if(operandBType == domain->CachedType___System_IntPtr ||
				operandBType == domain->CachedType___System_UIntPtr)
		{
			pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1];
		}
		else
		{
			Panic("Invalid operands for binary numeric operation");
		}
	}
	else if (operandAType == domain->CachedType___System_Int64 ||
			 operandAType == domain->CachedType___System_UInt64)
	{
		if (operandBType == domain->CachedType___System_Int64 ||
			operandBType == domain->CachedType___System_UInt64)
		{
			pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int64->TableIndex - 1];
		}
		else
		{
			Panic("Invalid operands for binary numeric operation");
		}
	}
	else if (operandAType == domain->CachedType___System_Single ||
			 operandAType == domain->CachedType___System_Double)
	{
		if (operandBType == domain->CachedType___System_Single ||
			operandBType == domain->CachedType___System_Double)
		{
			pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Double->TableIndex - 1];
		}
		else
		{
			Panic("Invalid operands for binary numeric operation");
		}
	}
	else if (operandAType == domain->CachedType___System_IntPtr ||
			 operandAType == domain->CachedType___System_UIntPtr)
	{
		if (pBinaryNumericOperation == BinaryNumericOperation_Add)
		{
			if (pOperandB->IsPointerType)
			{
				pResultObject->Type = pOperandB;
			}
		}
		if (operandBType == domain->CachedType___System_Byte ||
			operandBType == domain->CachedType___System_UInt16 ||
			operandBType == domain->CachedType___System_UInt32 ||
			operandBType == domain->CachedType___System_SByte ||
			operandBType == domain->CachedType___System_Int16 ||
			operandBType == domain->CachedType___System_Int32 ||
			operandBType == domain->CachedType___System_IntPtr ||
			operandBType == domain->CachedType___System_UIntPtr
			)
		{
			pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1];
		}
		else
		{
			Panic("Invalid operands for binary numeric operation");
		}
	}
	else if (pOperandA->IsPointerType)
	{
		if (pBinaryNumericOperation == BinaryNumericOperation_Add || pBinaryNumericOperation == BinaryNumericOperation_Sub)
		{
			if (pBinaryNumericOperation == BinaryNumericOperation_Sub && pOperandB->IsPointerType)
			{
				pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1];
			}
		}
		if (operandBType == domain->CachedType___System_Byte ||
			operandBType == domain->CachedType___System_UInt16 ||
			operandBType == domain->CachedType___System_UInt32 ||
			operandBType == domain->CachedType___System_SByte ||
			operandBType == domain->CachedType___System_Int16 ||
			operandBType == domain->CachedType___System_Int32 ||
			operandBType == domain->CachedType___System_IntPtr ||
			operandBType == domain->CachedType___System_UIntPtr
			)
		{
			pResultObject->Type = pOperandA;
		}
		else
		{
			Panic("Invalid operands for binary numeric operation");
		}
	}
	else
	{
		Panic("Invalid operands for binary numeric operation");
	}
}


void ILDecomposition_CheckBitwiseNumericOperandTypesAndSetResult(IRType* pOperandA, IRType* pOperandB, BitwiseNumericOperation pBitwiseNumericOperation, StackObject* pResultObject)
{
	AppDomain* domain = pOperandA->ParentAssembly->ParentDomain;
	TypeDefinition* operandAType = pOperandA->TypeDefinition;
	TypeDefinition* operandBType = pOperandB->TypeDefinition;

	if (operandAType == domain->CachedType___System_Byte ||
		operandAType == domain->CachedType___System_UInt16 ||
		operandAType == domain->CachedType___System_UInt32 ||
		operandAType == domain->CachedType___System_SByte ||
		operandAType == domain->CachedType___System_Int16 ||
		operandAType == domain->CachedType___System_Int32
		)
	{
		if (operandBType == domain->CachedType___System_Byte ||
			operandBType == domain->CachedType___System_UInt16 ||
			operandBType == domain->CachedType___System_UInt32 ||
			operandBType == domain->CachedType___System_SByte ||
			operandBType == domain->CachedType___System_Int16 ||
			operandBType == domain->CachedType___System_Int32
			)
		{
			pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
		}
		else if(operandBType == domain->CachedType___System_IntPtr ||
				operandBType == domain->CachedType___System_UIntPtr)
		{
			pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1];
		}
		else
		{
			Panic("Invalid operands for bitwise numeric operation");
		}
	}
	else if (operandAType == domain->CachedType___System_Int64 ||
			 operandAType == domain->CachedType___System_UInt64)
	{
		if (operandBType == domain->CachedType___System_Int64 ||
			operandBType == domain->CachedType___System_UInt64)
		{
			pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int64->TableIndex - 1];
		}
		else
		{
			Panic("Invalid operands for bitwise numeric operation");
		}
	}
	else if (operandAType == domain->CachedType___System_Single ||
			 operandAType == domain->CachedType___System_Double)
	{
		if (operandBType == domain->CachedType___System_Single ||
			operandBType == domain->CachedType___System_Double)
		{
			pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Double->TableIndex - 1];
		}
		else
		{
			Panic("Invalid operands for bitwise numeric operation");
		}
	}
	else if (operandAType == domain->CachedType___System_IntPtr ||
			 operandAType == domain->CachedType___System_UIntPtr)
	{
		if (operandBType == domain->CachedType___System_Byte ||
			operandBType == domain->CachedType___System_UInt16 ||
			operandBType == domain->CachedType___System_UInt32 ||
			operandBType == domain->CachedType___System_SByte ||
			operandBType == domain->CachedType___System_Int16 ||
			operandBType == domain->CachedType___System_Int32 ||
			operandBType == domain->CachedType___System_IntPtr ||
			operandBType == domain->CachedType___System_UIntPtr
			)
		{
			pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1];
		}
		else
		{
			Panic("Invalid operands for bitwise numeric operation");
		}
	}
	else if (pOperandA->IsPointerType)
	{
		if (operandBType == domain->CachedType___System_Byte ||
			operandBType == domain->CachedType___System_UInt16 ||
			operandBType == domain->CachedType___System_UInt32 ||
			operandBType == domain->CachedType___System_SByte ||
			operandBType == domain->CachedType___System_Int16 ||
			operandBType == domain->CachedType___System_Int32 ||
			operandBType == domain->CachedType___System_IntPtr ||
			operandBType == domain->CachedType___System_UIntPtr
			)
		{
			pResultObject->Type = pOperandA;
		}
		else
		{
			Panic("Invalid operands for bitwise numeric operation");
		}
	}
	else
	{
		Panic("Invalid operands for bitwise numeric operation");
	}
}

void ILDecomposition_CheckUnaryNumericOperandTypesAndSetResult(IRType* pOperand, UnaryNumericOperation pUnaryNumericOperation, StackObject* pResultObject)
{
	AppDomain* domain = pOperand->ParentAssembly->ParentDomain;
	TypeDefinition* operandType = pOperand->TypeDefinition;

	if (operandType == domain->CachedType___System_Byte ||
		operandType == domain->CachedType___System_UInt16 ||
		operandType == domain->CachedType___System_UInt32 ||
		operandType == domain->CachedType___System_SByte ||
		operandType == domain->CachedType___System_Int16 ||
		operandType == domain->CachedType___System_Int32
		)
	{
		pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
	}
	else if (operandType == domain->CachedType___System_Int64 ||
			 operandType == domain->CachedType___System_UInt64)
	{
		pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int64->TableIndex - 1];
	}
	else if (operandType == domain->CachedType___System_IntPtr)
	{
		pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1];
	}
	else if (operandType == domain->CachedType___System_UIntPtr)
	{
		pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_UIntPtr->TableIndex - 1];
	}
	else if (operandType == domain->CachedType___System_Single)
	{
		pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Single->TableIndex - 1];
	}
	else if (operandType == domain->CachedType___System_Double)
	{
		pResultObject->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Double->TableIndex - 1];
	}
	else
	{
		Panic("Invalid operands for unary numeric operation");
	}
}

void ILDecomposition_CheckShiftNumericOperandTypesAndSetResult(IRType* pOperandA, IRType* pOperandB, ShiftNumericOperation pShiftNumericOperation, StackObject* pResultObject, IRType** pOperandAGeneralType, IRType** pOperandBGeneralType)
{
	AppDomain* domain = pOperandA->ParentAssembly->ParentDomain;
	TypeDefinition* operandAType = pOperandA->TypeDefinition;
	TypeDefinition* operandBType = pOperandB->TypeDefinition;
	if (operandAType == domain->CachedType___System_Byte ||
		operandAType == domain->CachedType___System_UInt16 ||
		operandAType == domain->CachedType___System_UInt32 ||
		operandAType == domain->CachedType___System_SByte ||
		operandAType == domain->CachedType___System_Int16 ||
		operandAType == domain->CachedType___System_Int32
		)
	{
		*pOperandAGeneralType = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
	}
	else if (operandAType == domain->CachedType___System_IntPtr ||
			 operandAType == domain->CachedType___System_UIntPtr)
	{
		*pOperandAGeneralType = domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1];
	}
	else
	{
		Panic("Invalid operands for shift numeric operation");
	}

	if (operandBType == domain->CachedType___System_Byte ||
		operandBType == domain->CachedType___System_UInt16 ||
		operandBType == domain->CachedType___System_UInt32 ||
		operandBType == domain->CachedType___System_SByte ||
		operandBType == domain->CachedType___System_Int16 ||
		operandBType == domain->CachedType___System_Int32
		)
	{
		*pOperandBGeneralType = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
	}
	else if (operandBType == domain->CachedType___System_Int64 ||
			 operandBType == domain->CachedType___System_UInt64)
	{
		*pOperandBGeneralType = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int64->TableIndex - 1];
	}
	else if (operandAType == domain->CachedType___System_IntPtr ||
			 operandAType == domain->CachedType___System_UIntPtr)
	{
		*pOperandBGeneralType = domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1];
	}
	else
	{
		Panic("Invalid operands for shift numeric operation");
	}

	pResultObject->Type = *pOperandBGeneralType;
}

void ILDecomposition_CheckUncheckedConversionNumericOperandType(IRType* pOperand, ElementType* pSourceType)
{
	AppDomain* domain = pOperand->ParentAssembly->ParentDomain;
	TypeDefinition* operandType = pOperand->TypeDefinition;

	if (operandType == domain->CachedType___System_Byte)
	{
		*pSourceType = ElementType_U1;
	}
	else if (operandType == domain->CachedType___System_UInt16)
	{
		*pSourceType = ElementType_U2;
	}
	else if (operandType == domain->CachedType___System_UInt32)
	{
		*pSourceType = ElementType_U4;
	}
	else if (operandType == domain->CachedType___System_SByte)
	{
		*pSourceType = ElementType_I1;
	}
	else if (operandType == domain->CachedType___System_Int16)
	{
		*pSourceType = ElementType_I2;
	}
	else if (operandType == domain->CachedType___System_Int32)
	{
		*pSourceType = ElementType_I4;
	}
	else if (operandType == domain->CachedType___System_Int64)
	{
		*pSourceType = ElementType_I8;
	}
	else if (operandType == domain->CachedType___System_UInt64)
	{
		*pSourceType = ElementType_U8;
	}
	else if (operandType == domain->CachedType___System_Single)
	{
		*pSourceType = ElementType_R4;
	}
	else if (operandType == domain->CachedType___System_Double)
	{
		*pSourceType = ElementType_R8;
	}
	else if (pOperand->IsPointerType)
	{
		*pSourceType = ElementType_I;
	}
	else
	{
		Panic("Invalid operands for conversion numeric operation");
	}
}

#define PA()		StackObjectPool_Allocate(stack)
#define SR(obj)		StackObjectPool_Release(stack, obj)
#define BINARY_NUMERIC_OPERATION(pILOpcode, pIROpcode, pOverflowType) \
	{ Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read " #pILOpcode); \
		StackObject* value1 = SyntheticStack_Pop(stack); \
		StackObject* value2 = SyntheticStack_Pop(stack); \
		StackObject* obj = PA(); \
		ILDecomposition_CheckBinaryNumericOperandTypesAndSetResult(value1->Type, value2->Type, BinaryNumericOperation_##pIROpcode, obj); \
		EMIT_IR_3ARG_NO_DISPOSE(IROpcode_##pIROpcode, (uint32_t*)OverflowType_##pOverflowType, value1->Type, value2->Type); \
		obj->SourceType = StackObjectSourceType_Stack; \
		SyntheticStack_Push(stack, obj); \
        ClearFlags(); \
        break; \
	} 
#define BITWISE_NUMERIC_OPERATION(pIROpcode) \
	{ Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read " #pIROpcode); \
		StackObject* value1 = SyntheticStack_Pop(stack); \
		StackObject* value2 = SyntheticStack_Pop(stack); \
		StackObject* obj = PA(); \
		ILDecomposition_CheckBitwiseNumericOperandTypesAndSetResult(value1->Type, value2->Type, BitwiseNumericOperation_##pIROpcode, obj); \
		EMIT_IR_2ARG_NO_DISPOSE(IROpcode_##pIROpcode, value1->Type, value2->Type); \
		obj->SourceType = StackObjectSourceType_Stack; \
		SyntheticStack_Push(stack, obj); \
		ClearFlags(); \
		break; \
	} 
#define UNARY_NUMERIC_OPERATION(pIROpcode) \
	{ Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read " #pIROpcode); \
		StackObject* value = SyntheticStack_Pop(stack); \
		StackObject* obj = PA(); \
		ILDecomposition_CheckUnaryNumericOperandTypesAndSetResult(value->Type, UnaryNumericOperation_##pIROpcode, obj); \
		EMIT_IR_1ARG_NO_DISPOSE(IROpcode_##pIROpcode, value->Type); \
		obj->SourceType = StackObjectSourceType_Stack; \
		SyntheticStack_Push(stack, obj); \
		ClearFlags(); \
		break; \
	} 
#define SHIFT_NUMERIC_OPERATION(pIROpcode, pShiftNumericOperation) \
	{ Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read " #pIROpcode); \
		StackObject* value1 = SyntheticStack_Pop(stack); \
		StackObject* value2 = SyntheticStack_Pop(stack); \
		IRType* value1GeneralType = NULL; \
		IRType* value2GeneralType = NULL; \
		StackObject* obj = PA(); \
		ILDecomposition_CheckShiftNumericOperandTypesAndSetResult(value1->Type, value2->Type, ShiftNumericOperation_##pShiftNumericOperation, obj, &value1GeneralType, &value2GeneralType); \
		EMIT_IR_3ARG_NO_DISPOSE(IROpcode_Shift, (uint32_t*)ShiftNumericOperation_##pShiftNumericOperation, value1GeneralType, value2GeneralType); \
		obj->SourceType = StackObjectSourceType_Stack; \
		SyntheticStack_Push(stack, obj); \
		ClearFlags(); \
		break; \
	} 
#define UNCHECKED_CONVERSION_NUMERIC_OPERATION(pElementType, pDestinationType) \
	{ Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Conv." #pElementType); \
		StackObject* value = SyntheticStack_Pop(stack); \
		ElementType sourceType = (ElementType)0; \
		ElementType destinationType = ElementType_##pElementType; \
		StackObject* obj = PA(); \
		ILDecomposition_CheckUncheckedConversionNumericOperandType(value->Type, &sourceType); \
		EMIT_IR_2ARG_NO_DISPOSE(IROpcode_Convert_Unchecked, (uint32_t*)sourceType, (uint32_t*)destinationType); \
		obj->Type = pDestinationType; \
		obj->SourceType = StackObjectSourceType_Stack; \
		SyntheticStack_Push(stack, obj); \
		ClearFlags(); \
		break; \
	} 




ALWAYS_INLINE uint8_t ReadUInt8(uint8_t** pData)
{
    uint8_t value = **pData;
    (*pData)++;
    return value;
}

ALWAYS_INLINE uint16_t ReadUInt16(uint8_t** pData)
{
	uint16_t value = *((uint16_t*)*pData);
    *pData += 2;
    return value;
}

ALWAYS_INLINE uint32_t ReadUInt32(uint8_t** pData)
{
	uint32_t value = *((uint32_t*)*pData);
    *pData += 4;
    return value;
}

ALWAYS_INLINE uint64_t ReadUInt64(uint8_t** pData)
{
    uint64_t value = *((uint64_t*)*pData);
    *pData += 8;
    return value;
}

void ILDecomposition_ConvertInstructions(IRMethod* pMethod)
{
	MethodDefinition* methodDefinition = pMethod->MethodDefinition;
	MethodSignature* methodSignature = methodDefinition->SignatureCache;
	CLIFile* file = methodDefinition->File;
	IRAssembly* assembly = file->Assembly;
	AppDomain* domain = assembly->ParentDomain;

	IRType* parentType = NULL;
	if (!(parentType = assembly->Types[methodDefinition->TypeDefinition->TableIndex - 1]))
	{
		parentType = IRType_Create(assembly, methodDefinition->TypeDefinition);
	}

	uint32_t catchHandlerIndex = 0;
	uint32_t tryHandlerIndex = 0;
	uint32_t exceptionHandlerCount = methodDefinition->ExceptionCount;
	uint32_t* exceptionCatchStatementOffsets = (uint32_t*)calloc(1, exceptionHandlerCount * sizeof(uint32_t));
	uint32_t* exceptionCatchStatementLengths = (uint32_t*)calloc(1, exceptionHandlerCount * sizeof(uint32_t));
	uint32_t* exceptionTryStatementOffsets = (uint32_t*)calloc(1, exceptionHandlerCount * sizeof(uint32_t));
	uint32_t* exceptionTryStatementLengths = (uint32_t*)calloc(1, exceptionHandlerCount * sizeof(uint32_t));
	for (uint32_t index = 0; index < exceptionHandlerCount; index++)
	{
		Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_Exceptions, "Catch Handler %i (Length: 0x%x) at offset 0x%x", (int)index, (unsigned int)methodDefinition->Exceptions[index].HandlerLength, (unsigned int)methodDefinition->Exceptions[index].HandlerOffset);
		exceptionCatchStatementOffsets[index] = methodDefinition->Exceptions[index].HandlerOffset;
		exceptionCatchStatementLengths[index] = methodDefinition->Exceptions[index].HandlerLength;
		Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_Exceptions, "Try Statement %i (Length: 0x%x) at offset 0x%x", (int)index, (unsigned int)methodDefinition->Exceptions[index].TryLength, (unsigned int)methodDefinition->Exceptions[index].TryOffset);
		exceptionTryStatementOffsets[index] = methodDefinition->Exceptions[index].TryOffset;
		exceptionTryStatementOffsets[index] = methodDefinition->Exceptions[index].TryLength;
	}

	MetadataToken* localsSignatureToken = CLIFile_ExpandMetadataToken(file, methodDefinition->Body.LocalVariableSignatureToken);
	LocalsSignature* localsSignature = LocalsSignature_Expand(((StandAloneSignature*)localsSignatureToken->Data)->Signature, file);
	pMethod->LocalVariableCount = localsSignature->LocalVariableCount;
	pMethod->LocalVariables = (IRLocalVariable**)calloc(1, pMethod->LocalVariableCount * sizeof(IRLocalVariable*));
	uint32_t localVariableIndex = 0;
	IRType* type = NULL;
	for (uint32_t index = 0; index < localsSignature->LocalVariableCount; index++)
	{
		type = AppDomain_GetIRTypeFromSignatureType(domain, assembly, localsSignature->LocalVariables[index]->Type);
		IRLocalVariable* localVariable = IRLocalVariable_Create(pMethod, type);
		localVariable->LocalVariableIndex = localVariableIndex++;
		pMethod->LocalVariables[localVariable->LocalVariableIndex] = localVariable;
	}
	LocalsSignature_Destroy(localsSignature);
	CLIFile_DestroyMetadataToken(localsSignatureToken);

    SyntheticStack* stack = SyntheticStack_Create();
	StackObjectPool_Initialize(stack);
    bool_t prefixConstrained = FALSE;
	uint32_t prefixConstrainedToken = 0;
    bool_t prefixNo = FALSE;
    bool_t prefixReadOnly = FALSE;
    bool_t prefixTail = FALSE;
    bool_t prefixUnaligned = FALSE;
    bool_t prefixVolatile = FALSE;
    //BranchCondition branchCondition = (BranchCondition)0;
    //uint32_t branchTarget;
	//StackObject* branchArg1 = NULL;
	//StackObject* branchArg2 = NULL;
	uint8_t* localizedDataPointer = methodDefinition->Body.Code;
	uint32_t localizedDataLength = methodDefinition->Body.CodeSize;
	uint8_t** currentDataPointer = &localizedDataPointer;
	size_t originalDataPointer = (size_t)(*currentDataPointer);
    size_t currentILInstructionBase;
    uint8_t currentILOpcode;

	while ((size_t)(*currentDataPointer) - originalDataPointer < localizedDataLength)
	{
        currentILInstructionBase = (size_t)*currentDataPointer;
        Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "currentILInstructionBase: 0x%x", (int)currentILInstructionBase);

		if (catchHandlerIndex < exceptionHandlerCount)
		{
			uint32_t currentILOffset = currentILInstructionBase - originalDataPointer;
			if (exceptionCatchStatementOffsets[catchHandlerIndex] == currentILOffset)
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_Exceptions, "Entered Catch Block %u (Length: 0x%x) at offset 0x%x", (unsigned int)catchHandlerIndex, (unsigned int)exceptionCatchStatementLengths[catchHandlerIndex], (unsigned int)exceptionCatchStatementOffsets[catchHandlerIndex]);
				// We're at the start of a catch handler,
				// so we need to push an exception object
				// to the top of the stack.
				StackObject* obj = PA();
				obj->SourceType = StackObjectSourceType_Stack;
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Exception->TableIndex - 1];
				SyntheticStack_Push(stack, obj);

				catchHandlerIndex++;
			}
		}
		if (tryHandlerIndex < exceptionHandlerCount)
		{
			uint32_t currentILOffset = currentILInstructionBase - originalDataPointer;
			if (exceptionTryStatementOffsets[tryHandlerIndex] == currentILOffset)
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_Exceptions, "Entered Try Block %u (Length: 0x%x) at offset 0x%x", (unsigned int)(tryHandlerIndex), (unsigned int)exceptionTryStatementLengths[tryHandlerIndex], (unsigned int)exceptionTryStatementOffsets[tryHandlerIndex]);
				tryHandlerIndex++;
			}
		}

        currentILOpcode = ReadUInt8(currentDataPointer);

        switch (currentILOpcode)
        {
            case ILOpcode_Nop:				// 0x00
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Nop");

                EMIT_IR(IROpcode_Nop);

                ClearFlags();
                break;

            case ILOpcode_Break:			// 0x01
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Break");

                EMIT_IR(IROpcode_Break);

                ClearFlags();
                break;
			}


			case ILOpcode_LdArg_0:			// 0x02
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdArg.0");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Parameter, (uint32_t*)0);
					
				StackObject* obj = PA();
				if (methodSignature->HasThis && !(methodSignature->ExplicitThis))
				{
					if (AppDomain_IsStructure(domain, methodDefinition->TypeDefinition))
					{
						obj->Type = IRAssembly_MakePointerType(assembly, parentType);
					}
					else
					{
						obj->Type = parentType;
					}
				}
				else
				{
					obj->Type = pMethod->Parameters[0]->Type;
				}
				obj->SourceType = StackObjectSourceType_Parameter;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdArg_1:			// 0x03
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdArg.1");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Parameter, (uint32_t*)1);
					
				StackObject* obj = PA();
				obj->Type = pMethod->Parameters[1]->Type;
				obj->SourceType = StackObjectSourceType_Parameter;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdArg_2:			// 0x04
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdArg.2");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Parameter, (uint32_t*)2);
					
				StackObject* obj = PA();
				obj->Type = pMethod->Parameters[2]->Type;
				obj->SourceType = StackObjectSourceType_Parameter;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdArg_3:			// 0x05
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdArg.3");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Parameter, (uint32_t*)3);
					
				StackObject* obj = PA();
				obj->Type = pMethod->Parameters[3]->Type;
				obj->SourceType = StackObjectSourceType_Parameter;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdArg_S:			// 0x0E
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdArg.S");
				uint32_t paramIndex = ReadUInt8(currentDataPointer);
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Parameter, (uint32_t*)paramIndex);
					
				StackObject* obj = PA();
				if (methodSignature->HasThis && !(methodSignature->ExplicitThis) && !paramIndex)
				{
					if (AppDomain_IsStructure(domain, methodDefinition->TypeDefinition))
					{
						obj->Type = IRAssembly_MakePointerType(assembly, parentType);
					}
					else
					{
						obj->Type = parentType;
					}
				}
				else
				{
					obj->Type = pMethod->Parameters[paramIndex]->Type;
				}
				obj->SourceType = StackObjectSourceType_Parameter;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdArgA_S:			// 0x0F
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdArgA.S");
				uint32_t paramIndex = ReadUInt8(currentDataPointer);
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Parameter_Address, (uint32_t*)paramIndex);
					
				StackObject* obj = PA();
				obj->Type = IRAssembly_MakePointerType(assembly, pMethod->Parameters[paramIndex]->Type);
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_StArg_S:			// 0x10
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StArg.S");
				uint32_t paramIndex = ReadUInt8(currentDataPointer);
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Parameter, (uint32_t*)paramIndex);
				
				SR(SyntheticStack_Pop(stack));

                ClearFlags();
                break;
			}


            case ILOpcode_LdLoc_0:			// 0x06
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdLoc.0");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Local, (uint32_t*)0);
					
				StackObject* obj = PA();
				obj->Type = pMethod->LocalVariables[0]->VariableType;
				obj->SourceType = StackObjectSourceType_Local;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdLoc_1:			// 0x07
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdLoc.1");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Local, (uint32_t*)1);
					
				StackObject* obj = PA();
				obj->Type = pMethod->LocalVariables[1]->VariableType;
				obj->SourceType = StackObjectSourceType_Local;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdLoc_2:			// 0x08
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdLoc.2");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Local, (uint32_t*)2);
					
				StackObject* obj = PA();
				obj->Type = pMethod->LocalVariables[2]->VariableType;
				obj->SourceType = StackObjectSourceType_Local;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdLoc_3:			// 0x09
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdLoc.3");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Local, (uint32_t*)3);
					
				StackObject* obj = PA();
				obj->Type = pMethod->LocalVariables[3]->VariableType;
				obj->SourceType = StackObjectSourceType_Local;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdLoc_S:			// 0x11
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdLoc.S");
				uint32_t localIndex = ReadUInt8(currentDataPointer);
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Local, (uint32_t*)localIndex);
					
				StackObject* obj = PA();
				obj->Type = pMethod->LocalVariables[localIndex]->VariableType;
				obj->SourceType = StackObjectSourceType_Local;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdLocA_S:			// 0x12
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdLocA.S");
				uint32_t localIndex = ReadUInt8(currentDataPointer);
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Local_Address, (uint32_t*)localIndex);
					
				StackObject* obj = PA();
				obj->Type = IRAssembly_MakePointerType(assembly, pMethod->LocalVariables[localIndex]->VariableType);
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_StLoc_0:			// 0x0A
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StLoc.0");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Local, (uint32_t*)0);
				
				SR(SyntheticStack_Pop(stack));

                ClearFlags();
                break;
			}

            case ILOpcode_StLoc_1:			// 0x0B
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StLoc.1");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Local, (uint32_t*)1);
				
				SR(SyntheticStack_Pop(stack));

                ClearFlags();
                break;
			}

            case ILOpcode_StLoc_2:			// 0x0C
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StLoc.2");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Local, (uint32_t*)2);
				
				SR(SyntheticStack_Pop(stack));

                ClearFlags();
                break;
			}

            case ILOpcode_StLoc_3:			// 0x0D
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StLoc.3");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Local, (uint32_t*)3);
				
				SR(SyntheticStack_Pop(stack));

                ClearFlags();
                break;
			}

            case ILOpcode_StLoc_S:			// 0x13
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StLoc.S");
				uint32_t localIndex = ReadUInt8(currentDataPointer);
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Local, (uint32_t*)localIndex);
				
				SR(SyntheticStack_Pop(stack));

                ClearFlags();
                break;
			}


            case ILOpcode_LdNull:			// 0x14
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdNull");
                EMIT_IR(IROpcode_Load_Null);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Object->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdStr:			// 0x72
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdStr");
                MetadataToken* token = CLIFile_ExpandMetadataToken(file, ReadUInt32(currentDataPointer));
                if (!token->IsUserString)
                    Panic("Invalid token after LdStr!");

				uint32_t strLength = 0;
				uint8_t* str = (uint8_t*)CLIFile_GetCompressedUnsigned((uint8_t*)token->Data, &strLength);
				strLength -= 1; // Remove the null terminator
				CLIFile_DestroyMetadataToken(token);

				EMIT_IR_2ARG_NO_DISPOSE(IROpcode_Load_String, (uint32_t*)strLength, str);

				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_String->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4_M1:		// 0x15
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4.M1");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)-1);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4_0:			// 0x16
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4.0");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)0);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4_1:			// 0x17
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4.1");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)1);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4_2:			// 0x18
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4.2");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)2);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4_3:			// 0x19
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4.3");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)3);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4_4:			// 0x1A
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4.4");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)4);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4_5:			// 0x1B
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4.5");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)5);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4_6:			// 0x1C
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4.6");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)6);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4_7:			// 0x1D
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4.7");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)7);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4_8:			// 0x1E
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4.8");
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)8);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4_S:			// 0x1F
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4.S");
				uint32_t value = (uint32_t)(int32_t)(int8_t)ReadUInt8(currentDataPointer);
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)value);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I4:			// 0x20
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I4");
				uint32_t value = ReadUInt32(currentDataPointer);
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)value);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_I8:			// 0x21
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.I8");
				uint64_t* value = (uint64_t*)malloc(sizeof(uint64_t));
				*value = ReadUInt64(currentDataPointer);
                EMIT_IR_1ARG(IROpcode_Load_Int32, value);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int64->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_R4:			// 0x22
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.R4");
				uint32_t value = ReadUInt32(currentDataPointer);
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Int32, (uint32_t*)value);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Single->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Ldc_R8:			// 0x23
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Ldc.R8");
				uint64_t* value = (uint64_t*)malloc(sizeof(uint64_t));
				*value = ReadUInt64(currentDataPointer);
                EMIT_IR_1ARG(IROpcode_Load_Int32, value);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Double->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Constant;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}


			case ILOpcode_Dup:				// 0x25
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Dup");
				StackObject* object = SyntheticStack_Peek(stack);
				EMIT_IR_1ARG(IROpcode_Dup, object->Type);
				
				object->Source = NULL;
				object->SourceType = StackObjectSourceType_Stack;

				StackObject* obj = PA();
				obj->Type = object->Type;
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_Pop:				// 0x26
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Pop");
				StackObject* object = SyntheticStack_Pop(stack);
				EMIT_IR_1ARG(IROpcode_Pop, object->Type);
				SR(object);
				
                ClearFlags();
                break;
			}


            case ILOpcode_Call:				// 0x28
                ClearFlags();
                break;

            case ILOpcode_CallI:			// 0x29
                ClearFlags();
                break;

            case ILOpcode_CallVirt:			// 0x6F
                ClearFlags();
                break;

            case ILOpcode_Jmp:				// 0x27
				ClearFlags();
                break;

            case ILOpcode_Ret:				// 0x2A
                ClearFlags();
                break;


            case ILOpcode_Br:				// 0x38
                ClearFlags();
                break;

            case ILOpcode_Br_S:				// 0x2B
                ClearFlags();
                break;

            case ILOpcode_BrFalse:			// 0x39
                ClearFlags();
                break;

            case ILOpcode_BrFalse_S:		// 0x2C
                ClearFlags();
                break;

            case ILOpcode_BrTrue:			// 0x3A
                ClearFlags();
                break;

            case ILOpcode_BrTrue_S:			// 0x2D
                ClearFlags();
                break;

            case ILOpcode_Beq:				// 0x3B
                ClearFlags();
                break;

            case ILOpcode_Beq_S:			// 0x2E
                ClearFlags();
                break;

            case ILOpcode_Bne_Un:			// 0x40
                ClearFlags();
                break;

            case ILOpcode_Bne_Un_S:			// 0x33
                ClearFlags();
                break;

            case ILOpcode_Bge:				// 0x3C
                ClearFlags();
                break;

            case ILOpcode_Bge_S:			// 0x2F
                ClearFlags();
                break;

            case ILOpcode_Bge_Un:			// 0x41
                ClearFlags();
                break;

            case ILOpcode_Bge_Un_S:			// 0x34
                ClearFlags();
                break;

            case ILOpcode_Bgt:				// 0x3D
                ClearFlags();
                break;

            case ILOpcode_Bgt_S:			// 0x30
                ClearFlags();
                break;

            case ILOpcode_Bgt_Un:			// 0x42
                ClearFlags();
                break;

            case ILOpcode_Bgt_Un_S:			// 0x35
                ClearFlags();
                break;

            case ILOpcode_Ble:				// 0x3E
                ClearFlags();
                break;

            case ILOpcode_Ble_S:			// 0x31
                ClearFlags();
                break;

            case ILOpcode_Ble_Un:			// 0x43
                ClearFlags();
                break;

            case ILOpcode_Ble_Un_S:			// 0x36
                ClearFlags();
                break;

            case ILOpcode_Blt:				// 0x3F
                ClearFlags();
                break;

            case ILOpcode_Blt_S:			// 0x32
                ClearFlags();
                break;

            case ILOpcode_Blt_Un:			// 0x44
                ClearFlags();
                break;

            case ILOpcode_Blt_Un_S:			// 0x37
                ClearFlags();
                break;

            case ILOpcode_Switch:			// 0x45
				ClearFlags();
				break;


            case ILOpcode_LdInd_I:			// 0x4D
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdInd.I");
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1]);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdInd_I1:			// 0x46
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdInd.I1");
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_SByte->TableIndex - 1]);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_SByte->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdInd_U1:			// 0x47
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdInd.U1");
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Byte->TableIndex - 1]);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Byte->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdInd_I2:			// 0x48
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdInd.I2");
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Int16->TableIndex - 1]);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int16->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdInd_U2:			// 0x49
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdInd.U2");
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_UInt16->TableIndex - 1]);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_UInt16->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdInd_I4:			// 0x4A
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdInd.I4");
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1]);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdInd_U4:			// 0x4B
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdInd.U4");
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_UInt32->TableIndex - 1]);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_UInt32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdInd_I8:			// 0x4C
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdInd.I8");
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Int64->TableIndex - 1]);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Int64->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdInd_R4:			// 0x4D
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdInd.R4");
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Single->TableIndex - 1]);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Single->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdInd_R8:			// 0x4F
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdInd.R8");
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Double->TableIndex - 1]);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Double->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_LdInd_Ref:		// 0x50
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdInd.Ref");
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Object->TableIndex - 1]);
				
				StackObject* obj = PA();
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_Object->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_StInd_I:			// 0xDF
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StInd.I");
				SR(SyntheticStack_Pop(stack));
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1]);
				
                ClearFlags();
                break;
			}

            case ILOpcode_StInd_I1:			// 0x52
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StInd.I1");
				SR(SyntheticStack_Pop(stack));
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_SByte->TableIndex - 1]);
				
                ClearFlags();
                break;
			}

            case ILOpcode_StInd_I2:			// 0x53
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StInd.I2");
				SR(SyntheticStack_Pop(stack));
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Int16->TableIndex - 1]);
				
                ClearFlags();
                break;
			}

            case ILOpcode_StInd_I4:			// 0x54
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StInd.I4");
				SR(SyntheticStack_Pop(stack));
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1]);
				
                ClearFlags();
                break;
			}

            case ILOpcode_StInd_I8:			// 0x55
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StInd.I8");
				SR(SyntheticStack_Pop(stack));
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Int64->TableIndex - 1]);
				
                ClearFlags();
                break;
			}

            case ILOpcode_StInd_R4:			// 0x56
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StInd.R4");
				SR(SyntheticStack_Pop(stack));
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Single->TableIndex - 1]);
				
                ClearFlags();
                break;
			}

            case ILOpcode_StInd_R8:			// 0x57
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StInd.R8");
				SR(SyntheticStack_Pop(stack));
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Double->TableIndex - 1]);
				
                ClearFlags();
                break;
			}

            case ILOpcode_StInd_Ref:		// 0x51
			{
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StInd.Ref");
				SR(SyntheticStack_Pop(stack));
				SR(SyntheticStack_Pop(stack));
                EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Indirect, domain->IRAssemblies[0]->Types[domain->CachedType___System_Object->TableIndex - 1]);
				
                ClearFlags();
                break;
			}
                

            case ILOpcode_Add:				// 0x58
				BINARY_NUMERIC_OPERATION(Add, Add, None);

            case ILOpcode_Add_Ovf:			// 0xD6
				BINARY_NUMERIC_OPERATION(Add.Ovf, Add, Signed);

            case ILOpcode_Add_Ovf_Un:		// 0xD7
				BINARY_NUMERIC_OPERATION(Add.Ovf.Un, Add, Unsigned);

            case ILOpcode_Sub:				// 0x59
				BINARY_NUMERIC_OPERATION(Sub, Sub, None);

            case ILOpcode_Sub_Ovf:			// 0xDA
				BINARY_NUMERIC_OPERATION(Sub.Ovf, Sub, Signed);

            case ILOpcode_Sub_Ovf_Un:		// 0xDB
				BINARY_NUMERIC_OPERATION(Sub.Ovf.Un, Sub, Unsigned);
				
            case ILOpcode_Mul:				// 0x5A
				BINARY_NUMERIC_OPERATION(Mul, Mul, None);

            case ILOpcode_Mul_Ovf:			// 0xD8
				BINARY_NUMERIC_OPERATION(Mul.Ovf, Mul, Signed);

            case ILOpcode_Mul_Ovf_Un:		// 0xD9
				BINARY_NUMERIC_OPERATION(Mul.Ovf.Un, Mul, Unsigned);

            case ILOpcode_Div:				// 0x5B
				BINARY_NUMERIC_OPERATION(Div, Div, Signed);

            case ILOpcode_Div_Un:			// 0x5C
				BINARY_NUMERIC_OPERATION(Div.Un, Div, Unsigned);

            case ILOpcode_Rem:				// 0x5D
				BINARY_NUMERIC_OPERATION(Rem, Rem, Signed);

            case ILOpcode_Rem_Un:			// 0x5E
				BINARY_NUMERIC_OPERATION(Rem.Un, Rem, Unsigned);

            case ILOpcode_And:				// 0x5F
				BITWISE_NUMERIC_OPERATION(And);

            case ILOpcode_Or:				// 0x60
				BITWISE_NUMERIC_OPERATION(Or);

            case ILOpcode_Xor:				// 0x61
				BITWISE_NUMERIC_OPERATION(Xor);

            case ILOpcode_Neg:				// 0x65
				UNARY_NUMERIC_OPERATION(Neg);

			case ILOpcode_Not:				// 0x66
				UNARY_NUMERIC_OPERATION(Not);

			case ILOpcode_Shl:				// 0x62
				SHIFT_NUMERIC_OPERATION(Shl, Left);

            case ILOpcode_Shr:				// 0x63
				SHIFT_NUMERIC_OPERATION(Shr, Right_Sign_Extended);

            case ILOpcode_Shr_Un:			// 0x64
				SHIFT_NUMERIC_OPERATION(Shr.Un, Right);


            case ILOpcode_Conv_I1:			// 0x67
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(I1, domain->IRAssemblies[0]->Types[domain->CachedType___System_SByte->TableIndex - 1]);

            case ILOpcode_Conv_U1:			// 0xD2
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(U1, domain->IRAssemblies[0]->Types[domain->CachedType___System_Byte->TableIndex - 1]);

            case ILOpcode_Conv_I2:			// 0x68
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(I2, domain->IRAssemblies[0]->Types[domain->CachedType___System_Int16->TableIndex - 1]);

            case ILOpcode_Conv_U2:			// 0xD1
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(U2, domain->IRAssemblies[0]->Types[domain->CachedType___System_UInt16->TableIndex - 1]);

            case ILOpcode_Conv_I4:			// 0x69
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(I4, domain->IRAssemblies[0]->Types[domain->CachedType___System_Int32->TableIndex - 1]);

            case ILOpcode_Conv_U4:			// 0x6D
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(U4, domain->IRAssemblies[0]->Types[domain->CachedType___System_UInt32->TableIndex - 1]);

            case ILOpcode_Conv_I8:			// 0x6A
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(I8, domain->IRAssemblies[0]->Types[domain->CachedType___System_Int64->TableIndex - 1]);

            case ILOpcode_Conv_U8:			// 0x6E
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(U8, domain->IRAssemblies[0]->Types[domain->CachedType___System_UInt64->TableIndex - 1]);

            case ILOpcode_Conv_R4:			// 0x6B
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(R4, domain->IRAssemblies[0]->Types[domain->CachedType___System_Single->TableIndex - 1]);

            case ILOpcode_Conv_R8:			// 0x6C
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(R8, domain->IRAssemblies[0]->Types[domain->CachedType___System_Double->TableIndex - 1]);

            case ILOpcode_Conv_I:			// 0xD3
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(I, domain->IRAssemblies[0]->Types[domain->CachedType___System_IntPtr->TableIndex - 1]);

            case ILOpcode_Conv_U:			// 0xE0
				UNCHECKED_CONVERSION_NUMERIC_OPERATION(U, domain->IRAssemblies[0]->Types[domain->CachedType___System_UIntPtr->TableIndex - 1]);

            case ILOpcode_Conv_R_Un:		// 0x76
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Conv.R.Un (Unsupported)");

				Panic("Conv.R.Un (Unsupported)");

                ClearFlags();
                break;


            case ILOpcode_NewObj:			// 0x73
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read NewObj");

				MetadataToken* token = CLIFile_ExpandMetadataToken(file, ReadUInt32(currentDataPointer));
				MethodSignature* constructorSignature = NULL;
				IRMethod* constructorMethod = NULL;
				switch(token->Table)
				{
					case MetadataTable_MethodDefinition:
					{
						constructorSignature = ((MethodDefinition*)token->Data)->SignatureCache;
						constructorMethod = assembly->Methods[((MethodDefinition*)token->Data)->TableIndex - 1];
						break;
					}
					case MetadataTable_MemberReference:
					{
						MemberReference* memberRef = (MemberReference*)token->Data;
						if (!(constructorSignature = memberRef->MethodSignatureCache))
						{
							memberRef->MethodSignatureCache = MethodSignature_Expand(memberRef->Signature, file);
							constructorSignature = memberRef->MethodSignatureCache;
						}
						MethodDefinition* methodDef = memberRef->Resolved.MethodDefinition;
						constructorMethod = methodDef->File->Assembly->Methods[methodDef->TableIndex - 1];
						break;
					}

					default:
						Panic("Unknown table for NewObj");
						break;
				}
				for (uint32_t index = 0; index < constructorSignature->ParameterCount; index++)
				{
					SR(SyntheticStack_Pop(stack));
				}
				CLIFile_DestroyMetadataToken(token);

				EMIT_IR_1ARG_NO_DISPOSE(IROpcode_New_Object, constructorMethod);

				StackObject* obj = StackObjectPool_Allocate(stack);
				obj->Type = constructorMethod->ParentAssembly->Types[constructorMethod->MethodDefinition->TypeDefinition->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;
				SyntheticStack_Push(stack, obj);

                ClearFlags();
                break;
			}

            case ILOpcode_NewArr:			// 0x8D
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read NewArr");

				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), NULL);

				EMIT_IR_1ARG_NO_DISPOSE(IROpcode_New_Array, type);

				StackObject* obj = SyntheticStack_Peek(stack);
				obj->Type = IRAssembly_MakeArrayType(assembly, type);
				obj->SourceType = StackObjectSourceType_Stack;

                ClearFlags();
                break;
			}

            case ILOpcode_CastClass:		// 0x74
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read CastClass");

				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), NULL);

				EMIT_IR_1ARG_NO_DISPOSE(IROpcode_CastClass, type);

				StackObject* obj = SyntheticStack_Peek(stack);
				obj->Type = type;
				obj->SourceType = StackObjectSourceType_Stack;

                ClearFlags();
                break;
			}

            case ILOpcode_IsInst:			// 0x75
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read IsInst");

				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), NULL);

				EMIT_IR_1ARG_NO_DISPOSE(IROpcode_IsInst, type);

				StackObject* obj = SyntheticStack_Peek(stack);
				obj->Type = type;
				obj->SourceType = StackObjectSourceType_Stack;

                ClearFlags();
                break;
			}

            case ILOpcode_Unbox:			// 0x79
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Unbox");

				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), NULL);

				EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Unbox, type);

				StackObject* obj = SyntheticStack_Peek(stack);
				obj->Type = IRAssembly_MakePointerType(assembly, type);
				obj->SourceType = StackObjectSourceType_Stack;

                ClearFlags();
                break;
			}

            case ILOpcode_Unbox_Any:		// 0xA5
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Unbox.Any");

				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), NULL);

				EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Unbox_Any, type);

				StackObject* obj = SyntheticStack_Peek(stack);
				obj->Type = type;
				obj->SourceType = StackObjectSourceType_Stack;

                ClearFlags();
                break;
			}
				
            case ILOpcode_Box:				// 0x8C
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Box");

				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), NULL);

				EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Box, type);

				StackObject* obj = SyntheticStack_Peek(stack);
				obj->Type = type;
				obj->SourceType = StackObjectSourceType_Stack;

                ClearFlags();
                break;
			}


			case ILOpcode_Throw:			// 0x7A
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Throw");

				SR(SyntheticStack_Pop(stack));

				EMIT_IR(IROpcode_Throw);

                ClearFlags();
                break;
			}


            case ILOpcode_LdFld:			// 0x7B
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdFld");

				uint32_t fieldIndex = 0;
				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), &fieldIndex);

				StackObject* obj = SyntheticStack_Peek(stack);

				EMIT_IR_3ARG_NO_DISPOSE(IROpcode_Load_Field, obj->Type, type, (uint32_t*)fieldIndex);

				obj->Type = type;
				obj->SourceType = StackObjectSourceType_Stack;

				ClearFlags();
				break;
			}

            case ILOpcode_LdFldA:			// 0x7C
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdFldA");

				uint32_t fieldIndex = 0;
				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), &fieldIndex);

				StackObject* obj = SyntheticStack_Peek(stack);

				EMIT_IR_3ARG_NO_DISPOSE(IROpcode_Load_FieldAddress, obj->Type, type, (uint32_t*)fieldIndex);

				obj->Type = IRAssembly_MakePointerType(assembly, type);
				obj->SourceType = StackObjectSourceType_Stack;

				ClearFlags();
				break;
			}


            case ILOpcode_StFld:			// 0x7D
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StFld");

				uint32_t fieldIndex = 0;
				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), &fieldIndex);

				SR(SyntheticStack_Pop(stack));
				StackObject* obj = SyntheticStack_Pop(stack);

				EMIT_IR_3ARG_NO_DISPOSE(IROpcode_Store_Field, obj->Type, type, (uint32_t*)fieldIndex);

				ClearFlags();
				break;
			}

            case ILOpcode_LdSFld:			// 0x7E
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdSFld");

				uint32_t fieldIndex = 0;
				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), &fieldIndex);

				EMIT_IR_2ARG_NO_DISPOSE(IROpcode_Load_StaticField, type, (uint32_t*)fieldIndex);

				StackObject* obj = StackObjectPool_Allocate(stack);
				obj->Type = type;
				obj->SourceType = StackObjectSourceType_StaticField;
				SyntheticStack_Push(stack, obj);

				ClearFlags();
				break;
			}

            case ILOpcode_LdSFldA:			// 0x7F
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdSFldA");

				uint32_t fieldIndex = 0;
				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), &fieldIndex);

				EMIT_IR_2ARG_NO_DISPOSE(IROpcode_Load_StaticFieldAddress, type, (uint32_t*)fieldIndex);

				StackObject* obj = StackObjectPool_Allocate(stack);
				obj->Type = IRAssembly_MakePointerType(assembly, type);
				obj->SourceType = StackObjectSourceType_StaticField;
				SyntheticStack_Push(stack, obj);

				ClearFlags();
				break;
			}

            case ILOpcode_StSFld:			// 0x80
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StSFld");

				uint32_t fieldIndex = 0;
				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), &fieldIndex);

				SR(SyntheticStack_Pop(stack));

				EMIT_IR_2ARG_NO_DISPOSE(IROpcode_Store_StaticField, type, (uint32_t*)fieldIndex);

				ClearFlags();
				break;
			}


            case ILOpcode_LdObj:			// 0x71
            {
                Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdObj");

				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), NULL);

				StackObject* obj = SyntheticStack_Peek(stack);

				EMIT_IR_2ARG_NO_DISPOSE(IROpcode_Load_Object, obj->Type, type);

				obj->Type = type;
				obj->SourceType = StackObjectSourceType_Stack;

				ClearFlags();
	            break;			
            }

			case ILOpcode_StObj:			// 0x81
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StObj");

				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), NULL);

				SR(SyntheticStack_Pop(stack));
				StackObject* obj = SyntheticStack_Peek(stack);

				EMIT_IR_2ARG_NO_DISPOSE(IROpcode_Store_Object, obj->Type, type);

				obj->Type = type;
				obj->SourceType = StackObjectSourceType_Stack;

				ClearFlags();
				break;
			}

            case ILOpcode_CpObj:			// 0x70
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read CpObj");

				type = AppDomain_GetIRTypeFromMetadataToken(domain, assembly, ReadUInt32(currentDataPointer), NULL);

				SR(SyntheticStack_Pop(stack));
				StackObject* obj = SyntheticStack_Pop(stack);

				EMIT_IR_2ARG_NO_DISPOSE(IROpcode_Copy_Object, obj->Type, type);

				SR(obj);

				ClearFlags();
				break;
			}


            case ILOpcode_Conv_Ovf_I1:		// 0xB3
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_I1_Un:	// 0x82
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_U1:		// 0xB4
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_U1_Un:	// 0x86
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_I2:		// 0xB5
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_I2_Un:	// 0x83
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_U2:		// 0xB6
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_U2_Un:	// 0x87
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_I4:		// 0xB7
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_I4_Un:	// 0x84
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_U4:		// 0xB8
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_U4_Un:	// 0x88
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_I8:		// 0xB9
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_I8_Un:	// 0x85
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_U8:		// 0xBA
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_U8_Un:	// 0x89
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_I:		// 0xD4
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_I_Un:	// 0x8A
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_U:		// 0xD5
                ClearFlags();
                break;

            case ILOpcode_Conv_Ovf_U_Un:	// 0x8B
                ClearFlags();
                break;


            case ILOpcode_LdLen:			// 0x8E
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdLen");

				EMIT_IR(IROpcode_Load_ArrayLength);

				StackObject* obj = SyntheticStack_Peek(stack);
				obj->Type = domain->IRAssemblies[0]->Types[domain->CachedType___System_UInt32->TableIndex - 1];
				obj->SourceType = StackObjectSourceType_Stack;

                ClearFlags();
                break;
			} 

            case ILOpcode_LdElemA:			// 0x8F
	            ClearFlags();
				break;

            case ILOpcode_LdElem_I1:		// 0x90
	            ClearFlags();
				break;

            case ILOpcode_LdElem_U1:		// 0x91
	            ClearFlags();
				break;

            case ILOpcode_LdElem_I2:		// 0x92
	            ClearFlags();
				break;

            case ILOpcode_LdElem_U2:		// 0x93
	            ClearFlags();
				break;

            case ILOpcode_LdElem_I4:		// 0x94
	            ClearFlags();
				break;

            case ILOpcode_LdElem_U4:		// 0x95
	            ClearFlags();
				break;

            case ILOpcode_LdElem_I8:		// 0x96
	            ClearFlags();
				break;

            case ILOpcode_LdElem_I:			// 0x97
	            ClearFlags();
				break;

            case ILOpcode_LdElem_R4:		// 0x98
	            ClearFlags();
				break;

            case ILOpcode_LdElem_R8:		// 0x99
	            ClearFlags();
				break;

            case ILOpcode_LdElem_Ref:		// 0x9A
	            ClearFlags();
				break;

            case ILOpcode_LdElem:			// 0xA3
				ClearFlags();
				break;

            case ILOpcode_StElem_I:			// 0x9B
	            ClearFlags();
				break;

            case ILOpcode_StElem_I1:		// 0x9C
	            ClearFlags();
				break;

            case ILOpcode_StElem_I2:		// 0x9D
	            ClearFlags();
				break;

            case ILOpcode_StElem_I4:		// 0x9E
	            ClearFlags();
				break;

            case ILOpcode_StElem_I8:		// 0x9F
	            ClearFlags();
				break;

            case ILOpcode_StElem_R4:		// 0xA0
	            ClearFlags();
				break;

            case ILOpcode_StElem_R8:		// 0xA1
	            ClearFlags();
				break;

            case ILOpcode_StElem_Ref:		// 0xA2
	            ClearFlags();
				break;

            case ILOpcode_StElem:			// 0xA4
				ClearFlags();
				break;


			case ILOpcode_RefAnyVal:		// 0xC2
                ClearFlags();
                break;

            case ILOpcode_CkFinite:			// 0xC3
			{
				Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read CkFinite");

				EMIT_IR(IROpcode_CheckFinite);

                ClearFlags();
                break;
			} 

			case ILOpcode_MkRefAny:			// 0xC6
                ClearFlags();
                break;

			case ILOpcode_LdToken:			// 0xD0
				ClearFlags();
				break;


            case ILOpcode_EndFinally:		// 0xDC
                ClearFlags();
                break;

            case ILOpcode_Leave:			// 0xDD
                ClearFlags();
                break;

            case ILOpcode_Leave_S:			// 0xDE
                ClearFlags();
                break;


            case ILOpcode_Extended:         // 0xFE
		        currentILOpcode = ReadUInt8(currentDataPointer);

                switch (currentILOpcode)
                {
                    case ILOpcode_Extended_ArgList:		// 0x00
                        ClearFlags();
                        break;


                    case ILOpcode_Extended_Ceq:			// 0x01
                        ClearFlags();
                        break;

                    case ILOpcode_Extended_Cgt:			// 0x02
                        ClearFlags();
                        break;

                    case ILOpcode_Extended_Cgt_Un:			// 0x03
                        ClearFlags();
                        break;

                    case ILOpcode_Extended_Clt:			// 0x04
                        ClearFlags();
                        break;

                    case ILOpcode_Extended_Clt_Un:			// 0x05
                        ClearFlags();
                        break;

                    case ILOpcode_Extended_LdFtn:			// 0x06
						ClearFlags();
						break;

                    case ILOpcode_Extended_LdVirtFtn:		// 0x07
						ClearFlags();
						break;

                    case ILOpcode_Extended_LdArg:			// 0x09
					{
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdArg");
						uint32_t paramIndex = ReadUInt16(currentDataPointer);
						EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Parameter, (uint32_t*)paramIndex);
					
						StackObject* obj = PA();
						if (methodSignature->HasThis && !(methodSignature->ExplicitThis) && !paramIndex)
						{
							if (AppDomain_IsStructure(domain, methodDefinition->TypeDefinition))
							{
								obj->Type = IRAssembly_MakePointerType(assembly, parentType);
							}
							else
							{
								obj->Type = parentType;
							}
						}
						else
						{
							obj->Type = pMethod->Parameters[paramIndex]->Type;
						}
						obj->SourceType = StackObjectSourceType_Parameter;
						SyntheticStack_Push(stack, obj);

						ClearFlags();
						break;
					}

					case ILOpcode_Extended_LdArgA:			// 0x0A
					{
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdArgA");
						uint32_t paramIndex = ReadUInt16(currentDataPointer);
						EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Parameter_Address, (uint32_t*)paramIndex);
					
						StackObject* obj = PA();
						obj->Type = IRAssembly_MakePointerType(assembly, pMethod->Parameters[paramIndex]->Type);
						obj->SourceType = StackObjectSourceType_Stack;
						SyntheticStack_Push(stack, obj);

						ClearFlags();
						break;
					}

                    case ILOpcode_Extended_StArg:			// 0x0B
					{
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StArg");
						uint32_t paramIndex = ReadUInt16(currentDataPointer);
						EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Parameter, (uint32_t*)paramIndex);
				
						SR(SyntheticStack_Pop(stack));

						ClearFlags();
						break;
					}

                    case ILOpcode_Extended_LdLoc:			// 0x0C
					{
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdLoc");
						uint32_t localIndex = ReadUInt16(currentDataPointer);
						EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Local, (uint32_t*)localIndex);
					
						StackObject* obj = PA();
						obj->Type = pMethod->LocalVariables[localIndex]->VariableType;
						obj->SourceType = StackObjectSourceType_Local;
						SyntheticStack_Push(stack, obj);

						ClearFlags();
						break;
					}

                    case ILOpcode_Extended_LdLocA:			// 0x0D
					{
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read LdLocA");
						uint32_t localIndex = ReadUInt16(currentDataPointer);
						EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Load_Local_Address, (uint32_t*)localIndex);
					
						StackObject* obj = PA();
						obj->Type = IRAssembly_MakePointerType(assembly, pMethod->LocalVariables[localIndex]->VariableType);
						obj->SourceType = StackObjectSourceType_Stack;
						SyntheticStack_Push(stack, obj);

						ClearFlags();
						break;
					}

                    case ILOpcode_Extended_StLoc:			// 0x0E
					{
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read StLoc");
						uint32_t localIndex = ReadUInt16(currentDataPointer);
						EMIT_IR_1ARG_NO_DISPOSE(IROpcode_Store_Local, (uint32_t*)localIndex);
				
						SR(SyntheticStack_Pop(stack));

						ClearFlags();
						break;
					}

                    case ILOpcode_Extended_LocAlloc:		// 0x0F
                        ClearFlags();
                        break;

                    case ILOpcode_Extended_EndFilter:		// 0x11
                        ClearFlags();
                        break;

                    case ILOpcode_Extended_InitObj:		// 0x15
						ClearFlags();
						break;

                    case ILOpcode_Extended_CpBlk:			// 0x17
						ClearFlags();
						break;

                    case ILOpcode_Extended_InitBlk:		// 0x18
						ClearFlags();
						break;

                    case ILOpcode_Extended_ReThrow:		// 0x1A
						ClearFlags();
                        break;

                    case ILOpcode_Extended_SizeOf:			// 0x1C
						ClearFlags();
						break;

                    case ILOpcode_Extended_RefAnyType:		// 0x1D
                        ClearFlags();
                        break;


                    case ILOpcode_Extended_Constrained__:	// 0x16
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Prefix: Constrained");
						prefixConstrainedToken = ReadUInt32(currentDataPointer);
						prefixConstrained = TRUE;
                        break;
                    case ILOpcode_Extended_No__:			// 0x19
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Prefix: No");
                        prefixNo = TRUE;
                        break;
                    case ILOpcode_Extended_ReadOnly__:		// 0x1E
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Prefix: ReadOnly");
                        prefixReadOnly = TRUE;
                        break;
                    case ILOpcode_Extended_Tail__:			// 0x14
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Prefix: Tail");
                        prefixTail = TRUE;
                        break;
                    case ILOpcode_Extended_Unaligned__:	// 0x12
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Prefix: Unaligned");
                        prefixUnaligned = TRUE;
                        break;
                    case ILOpcode_Extended_Volatile__:		// 0x13
						Log_WriteLine(LOGLEVEL__ILDecomposition_Convert_ILReader, "Read Prefix: Volatile");
                        prefixVolatile = TRUE;
                        break;
                }
    			break;
    		// 0xFF Doesn't Exist
        }
	}
	SyntheticStack_Destroy(stack);
}