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
        case TypeDefOrRef_Type_TypeDefinition: pFile->TypeDefinitions[index].Extends.TypeDefinition = &pFile->TypeDefinitions[extendsRow]; break;
        case TypeDefOrRef_Type_TypeReference: pFile->TypeDefinitions[index].Extends.TypeReference = &pFile->TypeReferences[extendsRow]; break;
        case TypeDefOrRef_Type_TypeSpecification: pFile->TypeDefinitions[index].Extends.TypeSpecification = &pFile->TypeSpecifications[extendsRow]; break;
        default: break;
        }
        if (pFile->FieldCount > 0xFFFF) { fieldListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].FieldList = &pFile->Fields[fieldListIndex];
        fieldListIndexes[index] = fieldListIndex;
        if (pFile->MethodDefinitionCount > 0xFFFF) { methodDefinitionListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodDefinitionListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].MethodDefinitionList = &pFile->MethodDefinitions[methodDefinitionListIndex];
        methodDefinitionListIndexes[index] = methodDefinitionListIndex;
    }
    uint32_t fieldListCount = 0;
    for (uint32_t index = 1, used = 0; index <= pFile->TypeDefinitionCount; ++index, used += fieldListCount)
    {
        if (index == pFile->TypeDefinitionCount) fieldListCount = pFile->FieldCount - used;
        else fieldListCount = fieldListIndexes[index + 1] - fieldListIndexes[index];
        pFile->TypeDefinitions[index].FieldListCount = fieldListCount;
    }
    free(fieldListIndexes);
    uint32_t methodDefinitionListCount = 0;
    for (uint32_t index = 1, used = 0; index <= pFile->TypeDefinitionCount; ++index, used += methodDefinitionListCount)
    {
        if (index == pFile->TypeDefinitionCount) methodDefinitionListCount = pFile->MethodDefinitionCount - used;
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
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_TypeDefinition) ++pFile->CustomAttributes[index].Parent.TypeDefinition->CustomAttributeCount;
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
        if (pFile->GenericParameters[index].TypeOfOwner == TypeOrMethodDef_Type_TypeDefinition) ++pFile->GenericParameters[index].Owner.TypeDefinition->GenericParameterCount;
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
}
