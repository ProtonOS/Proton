#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* TypeDefinition_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_TypeDefinition)) != 0)
    {
        pFile->TypeDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->TypeDefinitions = (TypeDefinition*)calloc(pFile->TypeDefinitionCount, sizeof(TypeDefinition));
    }
    return pTableData;
}

void TypeDefinition_Cleanup(CLIFile* pFile)
{
    if (pFile->TypeDefinitions)
    {
        for (uint32_t index = 0; index < pFile->TypeDefinitionCount; ++index)
        {
        }
        free(pFile->TypeDefinitions);
        pFile->TypeDefinitions = NULL;
    }
}

const uint8_t* TypeDefinition_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t extendsIndex = 0;
    uint8_t extendsTable = 0;
    uint32_t extendsRow = 0;
    uint32_t fieldListIndex = 0;
    uint32_t methodDefinitionListIndex = 0;
    uint32_t* fieldListIndexes = (uint32_t*)calloc(pFile->TypeDefinitionCount, sizeof(uint32_t));
    uint32_t* methodDefinitionListIndexes = (uint32_t*)calloc(pFile->TypeDefinitionCount, sizeof(uint32_t));
    for (uint32_t index = 0, heapIndex = 0; index < pFile->TypeDefinitionCount; ++index)
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
        extendsTable = extendsIndex & TypeDefOrRef_Type_Mask;
        extendsRow = extendsIndex >> TypeDefOrRef_Type_Bits;
        switch (extendsTable)
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
    uint8_t fieldListCount = 0;
    for (uint32_t index = 0, used = 0; index < pFile->TypeDefinitionCount; ++index, used += fieldListCount)
    {
        if (index == (pFile->TypeDefinitionCount - 1)) fieldListCount = pFile->FieldCount - used;
        else fieldListCount = fieldListIndexes[index + 1] - fieldListIndexes[index];
        pFile->TypeDefinitions[index].FieldListCount = fieldListCount;
    }
    free(fieldListIndexes);
    uint8_t methodDefinitionListCount = 0;
    for (uint32_t index = 0, used = 0; index < pFile->TypeDefinitionCount; ++index, used += methodDefinitionListCount)
    {
        if (index == (pFile->TypeDefinitionCount - 1)) methodDefinitionListCount = pFile->MethodDefinitionCount - used;
        else methodDefinitionListCount = methodDefinitionListIndexes[index + 1] - methodDefinitionListIndexes[index];
        pFile->TypeDefinitions[index].MethodDefinitionListCount = methodDefinitionListCount;
    }
    free(methodDefinitionListIndexes);
    return pTableData;
}