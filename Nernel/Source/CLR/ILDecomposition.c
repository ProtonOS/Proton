#include <CLR/ILDecomposition.h>
#include <System/Multiboot.h>

IRAssembly* ILDecomposition_CreateAssembly(AppDomain* pDomain, CLIFile* pFile)
{
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

	IRAssembly* assembly = IRAssembly_Create(pDomain, pFile);
	pFile->Assembly = assembly;

	if (pFile->CLIHeader->EntryPointToken)
	{
		MetadataToken* token = CLIFile_ExpandMetadataToken(pFile, pFile->CLIHeader->EntryPointToken);
		if (token->Table != MetadataTable_MethodDefinition)
			Panic("Unknown entry point table!");
		assembly->EntryPoint = IRMethod_Create(assembly, (MethodDefinition*)token->Data);
		free(token);
	}

	return assembly;
}


IRMethod** ILDecomposition_GetMethodLayout(IRType* pType, TypeDefinition* pTypeDefinition, uint32_t* pTotalMethodCount)
{
	Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Laying out the methods of %s.%s, TypeOfExtends = %u", pTypeDefinition->Namespace, pTypeDefinition->Name, (unsigned int)pTypeDefinition->TypeOfExtends);
	IRMethod** methods = NULL;
	uint32_t methodCount = 0;
	if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeDefinition)
	{
		if (pTypeDefinition->Extends.TypeDefinition != NULL)
		{
			methods = ILDecomposition_GetMethodLayout(pType, pTypeDefinition->Extends.TypeDefinition, &methodCount);
		}	
	}
	else if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeReference)
	{
		if (pTypeDefinition->Extends.TypeReference != NULL)
		{
			methods = ILDecomposition_GetMethodLayout(pType, pTypeDefinition->Extends.TypeReference->ResolvedType, &methodCount);
		}
		else
			Panic("TypeRef Extends NULL!");
	}
	else if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeSpecification)
	{
		*pTotalMethodCount = 0;
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
	Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "MethodCount: %i NewMethodCount: %i", (int)methodCount, (int)newMethodsCount);

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
				Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Adding method %s.%s.%s from table index %i at %i", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->MethodDefinitionList[index].Name, (int)pTypeDefinition->MethodDefinitionList[index].TableIndex, (int)(methodCount + methodIndex));
				methodIndex++;
			}
			else
			{
				bool_t found = FALSE;
				Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Looking for base method of %s.%s.%s (Table Index %i)", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->MethodDefinitionList[index].Name, (int)pTypeDefinition->MethodDefinitionList[index].TableIndex);
				for (uint32_t index2 = 0; index2 < methodCount; index2++)
				{
					MethodDefinition* methodDefinition = methods[index2]->MethodDefinition;
					if (methodDefinition->TableIndex)
					{
						Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Checking Method %s.%s.%s from table index %i", methodDefinition->TypeDefinition->Namespace, methodDefinition->TypeDefinition->Name, methodDefinition->Name, (int)methodDefinition->TableIndex);
					}
					else
					{
						Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Method Table Index 0! This shouldn't happen!");
					}

					if (!strcmp(pTypeDefinition->MethodDefinitionList[index].Name, methodDefinition->Name))
					{
						Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Name is the same");
						if (Signature_Equals(pTypeDefinition->MethodDefinitionList[index].Signature, pTypeDefinition->MethodDefinitionList[index].SignatureLength, methodDefinition->Signature, methodDefinition->SignatureLength))
						{
							Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Found Match!");
							IRMethod* method = pTypeDefinition->File->Assembly->Methods[pTypeDefinition->MethodDefinitionList[index].TableIndex - 1];
							if (!method)
							{
								method = IRMethod_Create(pTypeDefinition->File->Assembly, &pTypeDefinition->MethodDefinitionList[index]);
							}
							finalMethods[index2] = method; 
							Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Overloading method %s.%s.%s from table index %i at %i", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->MethodDefinitionList[index].Name, (int)pTypeDefinition->MethodDefinitionList[index].TableIndex, (int)index2);
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
				Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Adding method %s.%s.%s from table index %i at %i", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->MethodDefinitionList[index].Name, (int)pTypeDefinition->MethodDefinitionList[index].TableIndex, (int)(methodCount + methodIndex));
				methodIndex++;
			}
			else
			{
				Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Ignoring method %s.%s.%s at table index %i", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->MethodDefinitionList[index].Name, (int)pTypeDefinition->MethodDefinitionList[index].TableIndex);
			}
		}
	}

	if (methods != NULL)
	{
		free(methods);
	}

	*pTotalMethodCount = methodCount + newMethodsCount;
	return finalMethods;
}

IRField** ILDecomposition_GetFieldLayout(IRType* pType, TypeDefinition* pTypeDefinition, uint32_t* pTotalFieldCount)
{
	Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Laying out the fields of %s.%s, TypeOfExtends = %u", pTypeDefinition->Namespace, pTypeDefinition->Name, (unsigned int)pTypeDefinition->TypeOfExtends);
	IRField** fields = NULL;
	uint32_t fieldCount = 0;
	if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeDefinition)
	{
		if (pTypeDefinition->Extends.TypeDefinition != NULL)
		{
			fields = ILDecomposition_GetFieldLayout(pType, pTypeDefinition->Extends.TypeDefinition, &fieldCount);
		}
	}
	else if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeReference)
	{
		if (pTypeDefinition->Extends.TypeReference != NULL)
		{
			fields = ILDecomposition_GetFieldLayout(pType, pTypeDefinition->Extends.TypeReference->ResolvedType, &fieldCount);
		}
		else 
			Panic("TypeRef Extends NULL!");
	}
	else if (pTypeDefinition->TypeOfExtends == TypeDefRefOrSpecType_TypeSpecification)
	{
		*pTotalFieldCount = 0;
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
	Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Field count: %i NewFieldCount: %i", (int)fieldCount, (int)newFieldsCount);

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
			Log_WriteLine(LOGLEVEL__ILDecompositionLayouts, "Adding Field %s.%s.%s from table index %i at %i", pTypeDefinition->Namespace, pTypeDefinition->Name, pTypeDefinition->FieldList[index].Name, (int)pTypeDefinition->FieldList[index].TableIndex, (int)(fieldCount + fieldIndex));
			fieldIndex++;
		}
	}

	if (fields != NULL)
	{
		free(fields);
	}

	*pTotalFieldCount = fieldCount + newFieldsCount;
	return finalFields;
}

