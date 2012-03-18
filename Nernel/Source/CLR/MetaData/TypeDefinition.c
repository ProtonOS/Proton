#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* TypeDefinition_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_TypeDefinition)) != 0)
    {
        pFile->TypeDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->TypeDefinitions = (TypeDefinition*)calloc(pFile->TypeDefinitionCount + 1, sizeof(TypeDefinition));
    }
    return pTableData;
}

void TypeDefinition_Cleanup(CLIFile* pFile)
{
    if (pFile->TypeDefinitions)
    {
        for (uint32_t index = 1; index <= pFile->TypeDefinitionCount; ++index)
        {
            if (pFile->TypeDefinitions[index].CustomAttributes) free(pFile->TypeDefinitions[index].CustomAttributes);
            if (pFile->TypeDefinitions[index].GenericParameters) free(pFile->TypeDefinitions[index].GenericParameters);
            if (pFile->TypeDefinitions[index].InterfaceImplementations) free(pFile->TypeDefinitions[index].InterfaceImplementations);
            if (pFile->TypeDefinitions[index].MemberReferences) free(pFile->TypeDefinitions[index].MemberReferences);
            if (pFile->TypeDefinitions[index].MethodImplementations) free(pFile->TypeDefinitions[index].MethodImplementations);
            if (pFile->TypeDefinitions[index].NestedClasses) free(pFile->TypeDefinitions[index].NestedClasses);
        }
        free(pFile->TypeDefinitions);
        pFile->TypeDefinitions = NULL;
    }
}

const uint8_t* TypeDefinition_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t extendsIndex = 0;
    uint32_t extendsRow = 0;
    uint32_t fieldListIndex = 0;
    uint32_t methodDefinitionListIndex = 0;
    uint32_t* fieldListIndexes = (uint32_t*)calloc(pFile->TypeDefinitionCount + 1, sizeof(uint32_t));
    uint32_t* methodDefinitionListIndexes = (uint32_t*)calloc(pFile->TypeDefinitionCount + 1, sizeof(uint32_t));
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->TypeDefinitionCount; ++index)
    {
		pFile->TypeDefinitions[index].File = pFile;
        pFile->TypeDefinitions[index].TableIndex = index;
        pFile->TypeDefinitions[index].Flags = *(uint32_t*)pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].Namespace = (const char*)(pFile->StringsHeap + heapIndex);
        if (pFile->TypeDefinitionCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefOrRef_Type_MaxRows16Bit) { extendsIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { extendsIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].TypeOfExtends = extendsIndex & TypeDefOrRef_Type_Mask;
        extendsRow = extendsIndex >> TypeDefOrRef_Type_Bits;
        switch (pFile->TypeDefinitions[index].TypeOfExtends)
        {
        case TypeDefOrRef_Type_TypeDefinition:
            if (extendsRow > pFile->TypeDefinitionCount) Panic("TypeDefinition_Load TypeDefinition");
            if (extendsRow > 0) pFile->TypeDefinitions[index].Extends.TypeDefinition = &pFile->TypeDefinitions[extendsRow];
            break;
        case TypeDefOrRef_Type_TypeReference:
            if (extendsRow > pFile->TypeReferenceCount) Panic("TypeDefinition_Load TypeReference");
            if (extendsRow > 0) pFile->TypeDefinitions[index].Extends.TypeReference = &pFile->TypeReferences[extendsRow];
            break;
        case TypeDefOrRef_Type_TypeSpecification:
            if (extendsRow > pFile->TypeSpecificationCount) Panic("TypeDefinition_Load TypeSpecification");
            if (extendsRow > 0) pFile->TypeDefinitions[index].Extends.TypeSpecification = &pFile->TypeSpecifications[extendsRow];
            break;
        default: break;
        }
        if (pFile->FieldCount > 0xFFFF) { fieldListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (fieldListIndex == 0 || fieldListIndex > pFile->FieldCount + 1) Panic("TypeDefinition_Load Field");
        if (fieldListIndex <= pFile->FieldCount)
        {
			//printf("TypeDef Field Link: %s.%s, start @ %u\n", pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, fieldListIndex);
			//printf("Field List: 0x%x\n", (unsigned int)&pFile->Fields[fieldListIndex]);
            pFile->TypeDefinitions[index].FieldList = &pFile->Fields[fieldListIndex];
            fieldListIndexes[index] = fieldListIndex;
        }
        if (pFile->MethodDefinitionCount > 0xFFFF) { methodDefinitionListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodDefinitionListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (methodDefinitionListIndex == 0 || methodDefinitionListIndex > pFile->MethodDefinitionCount + 1) Panic("TypeDefinition_Load MethodDefinition");
        if (methodDefinitionListIndex <= pFile->MethodDefinitionCount)
        {
            pFile->TypeDefinitions[index].MethodDefinitionList = &pFile->MethodDefinitions[methodDefinitionListIndex];
            methodDefinitionListIndexes[index] = methodDefinitionListIndex;
        }
    }
    uint32_t fieldListCount = 0;
    for (uint32_t index = 1, used = 0; index <= pFile->TypeDefinitionCount; ++index, used += fieldListCount)
    {
        if (index == pFile->TypeDefinitionCount || fieldListIndexes[index + 1] == 0) fieldListCount = pFile->FieldCount - used;
        else fieldListCount = fieldListIndexes[index + 1] - fieldListIndexes[index];
        pFile->TypeDefinitions[index].FieldListCount = fieldListCount;
		//printf("TypeDef: %s.%s, Owns %u Fields starting @ %u\n", pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, fieldListCount, fieldListIndexes[index]);
    }
    free(fieldListIndexes);
    uint32_t methodDefinitionListCount = 0;
    for (uint32_t index = 1, used = 0; index <= pFile->TypeDefinitionCount; ++index, used += methodDefinitionListCount)
    {
        if (index == pFile->TypeDefinitionCount || methodDefinitionListIndexes[index + 1] == 0) methodDefinitionListCount = pFile->MethodDefinitionCount - used;
        else methodDefinitionListCount = methodDefinitionListIndexes[index + 1] - methodDefinitionListIndexes[index];
        pFile->TypeDefinitions[index].MethodDefinitionListCount = methodDefinitionListCount;
    }
    free(methodDefinitionListIndexes);
    return pTableData;
}

void TypeDefinition_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->ClassLayoutCount; ++index) pFile->ClassLayouts[index].Parent->ClassLayout = &pFile->ClassLayouts[index];
        
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_TypeDefinition)
        {
            ++pFile->CustomAttributes[index].Parent.TypeDefinition->CustomAttributeCount;
        }
    }
    for (uint32_t index = 1; index <= pFile->TypeDefinitionCount; ++index)
    {
        if (pFile->TypeDefinitions[index].CustomAttributeCount > 0)
        {
            pFile->TypeDefinitions[index].CustomAttributes = (CustomAttribute**)calloc(pFile->TypeDefinitions[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_TypeDefinition &&
                    pFile->CustomAttributes[searchIndex].Parent.TypeDefinition == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->DeclSecurityCount; ++index)
    {
        if (pFile->DeclSecurities[index].TypeOfParent == HasDeclSecurity_Type_TypeDefinition) pFile->DeclSecurities[index].Parent.TypeDefinition->DeclSecurity = &pFile->DeclSecurities[index];
    }
    for (uint32_t index = 1; index <= pFile->EventMapCount; ++index) pFile->EventMaps[index].Parent->EventMap = &pFile->EventMaps[index];
    for (uint32_t index = 1; index <= pFile->GenericParameterCount; ++index)
    {
        if (pFile->GenericParameters[index].TypeOfOwner == TypeOrMethodDef_Type_TypeDefinition)
        {
            ++pFile->GenericParameters[index].Owner.TypeDefinition->GenericParameterCount;
        }
    }
    for (uint32_t index = 1; index <= pFile->TypeDefinitionCount; ++index)
    {
        if (pFile->TypeDefinitions[index].GenericParameterCount > 0)
        {
            pFile->TypeDefinitions[index].GenericParameters = (GenericParameter**)calloc(pFile->TypeDefinitions[index].GenericParameterCount, sizeof(GenericParameter*));
            for (uint32_t searchIndex = 1, genericParameterIndex = 0; searchIndex <= pFile->GenericParameterCount; ++searchIndex)
            {
                if (pFile->GenericParameters[searchIndex].TypeOfOwner == TypeOrMethodDef_Type_TypeDefinition &&
                    pFile->GenericParameters[searchIndex].Owner.TypeDefinition == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].GenericParameters[genericParameterIndex] = &pFile->GenericParameters[searchIndex];
                    ++genericParameterIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->InterfaceImplementationCount; ++index) ++pFile->InterfaceImplementations[index].Implementor->InterfaceImplementationCount;
    for (uint32_t index = 1; index <= pFile->TypeDefinitionCount; ++index)
    {
        if (pFile->TypeDefinitions[index].InterfaceImplementationCount > 0)
        {
            pFile->TypeDefinitions[index].InterfaceImplementations = (InterfaceImplementation**)calloc(pFile->TypeDefinitions[index].InterfaceImplementationCount, sizeof(InterfaceImplementation*));
            for (uint32_t searchIndex = 1, interfaceImplementationIndex = 0; searchIndex <= pFile->InterfaceImplementationCount; ++searchIndex)
            {
                if (pFile->InterfaceImplementations[searchIndex].Implementor == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].InterfaceImplementations[interfaceImplementationIndex] = &pFile->InterfaceImplementations[searchIndex];
                    ++interfaceImplementationIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
    {
        if (pFile->MemberReferences[index].TypeOfParent == MemberRefParent_Type_TypeDefinition) ++pFile->MemberReferences[index].Parent.TypeDefinition->MemberReferenceCount;
    }
    for (uint32_t index = 1; index <= pFile->TypeDefinitionCount; ++index)
    {
        if (pFile->TypeDefinitions[index].MemberReferenceCount > 0)
        {
            pFile->TypeDefinitions[index].MemberReferences = (MemberReference**)calloc(pFile->TypeDefinitions[index].MemberReferenceCount, sizeof(MemberReference*));
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= pFile->MemberReferenceCount; ++searchIndex)
            {
                if (pFile->MemberReferences[searchIndex].TypeOfParent == MemberRefParent_Type_TypeDefinition &&
                    pFile->MemberReferences[searchIndex].Parent.TypeDefinition == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].MemberReferences[memberReferenceIndex] = &pFile->MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->MethodImplementationCount; ++index)
    {
        ++pFile->MethodImplementations[index].Parent->MethodImplementationCount;
    }
    for (uint32_t index = 1; index <= pFile->TypeDefinitionCount; ++index)
    {
        if (pFile->TypeDefinitions[index].MethodImplementationCount > 0)
        {
            //printf("MethodImplementations: %u %s.%s %u\n", (unsigned int)index, pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, (unsigned int)pFile->TypeDefinitions[index].MethodImplementationCount);
            pFile->TypeDefinitions[index].MethodImplementations = (MethodImplementation**)calloc(pFile->TypeDefinitions[index].MethodImplementationCount, sizeof(MethodImplementation*));
            for (uint32_t searchIndex = 1, methodImplementationIndex = 0; searchIndex <= pFile->MethodImplementationCount; ++searchIndex)
            {
                if (pFile->MethodImplementations[searchIndex].Parent == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].MethodImplementations[methodImplementationIndex] = &pFile->MethodImplementations[searchIndex];
                    ++methodImplementationIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->NestedClassCount; ++index)
    {
        ++pFile->NestedClasses[index].Enclosing->NestedClassCount;
    }
    for (uint32_t index = 1; index <= pFile->TypeDefinitionCount; ++index)
    {
        if (pFile->TypeDefinitions[index].NestedClassCount > 0)
        {
            pFile->TypeDefinitions[index].NestedClasses = (NestedClass**)calloc(pFile->TypeDefinitions[index].NestedClassCount, sizeof(NestedClass*));
            for (uint32_t searchIndex = 1, nestedClassIndex = 0; searchIndex <= pFile->NestedClassCount; ++searchIndex)
            {
                if (pFile->NestedClasses[searchIndex].Enclosing == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].NestedClasses[nestedClassIndex] = &pFile->NestedClasses[searchIndex];
                    ++nestedClassIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->PropertyMapCount; ++index) pFile->PropertyMaps[index].Parent->PropertyMap = &pFile->PropertyMaps[index];
    for (uint32_t index = 1; index <= pFile->TypeDefinitionCount; ++index)
    {
        if (pFile->TypeDefinitions[index].MethodDefinitionListCount > 0)
        {
            //printf("TypeDefinition[%u]: %s.%s, %u methods\n", (unsigned int)index, pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, (unsigned int)pFile->TypeDefinitions[index].MethodDefinitionListCount);
            for (uint32_t searchIndex = 0; searchIndex < pFile->TypeDefinitions[index].MethodDefinitionListCount; ++searchIndex)
            {
                //printf("    %s\n", pFile->TypeDefinitions[index].MethodDefinitionList[searchIndex].Name);
                pFile->TypeDefinitions[index].MethodDefinitionList[searchIndex].TypeDefinition = &pFile->TypeDefinitions[index];
            }
        }
    }

	for (uint32_t index = 1; index <= pFile->TypeDefinitionCount; ++index)
    {
		//printf("Linking Fields for %s.%s, %u\n", pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, pFile->TypeDefinitions[index].FieldListCount);
		for (uint32_t index2 = 0; index2 < pFile->TypeDefinitions[index].FieldListCount; ++index2)
		{
			pFile->TypeDefinitions[index].FieldList[index2].TypeDefinition = &pFile->TypeDefinitions[index];
			//printf("Linking Field %s to TypeDef %s.%s @ 0x%x\n", pFile->TypeDefinitions[index].FieldList[index2].Name, pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, (unsigned int)pFile->TypeDefinitions[index].FieldList[index2].TypeDefinition);
		}
    }
}
