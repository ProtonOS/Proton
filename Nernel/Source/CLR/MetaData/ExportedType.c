#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* ExportedType_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ExportedType)) != 0)
    {
        pFile->ExportedTypeCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ExportedTypes = (ExportedType*)calloc(pFile->ExportedTypeCount + 1, sizeof(ExportedType));
    }
    return pTableData;
}

void ExportedType_Cleanup(CLIFile* pFile)
{
    if (pFile->ExportedTypes)
    {
        for (uint32_t index = 1; index <= pFile->ExportedTypeCount; ++index)
        {
            if (pFile->ExportedTypes[index].CustomAttributes) free(pFile->ExportedTypes[index].CustomAttributes);
        }
        free(pFile->ExportedTypes);
        pFile->ExportedTypes = NULL;
    }
}

const uint8_t* ExportedType_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t typeDefinitionIndex = 0;
    uint32_t implementationIndex = 0;
    uint32_t implementationRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ExportedTypeCount; ++index)
    {
        pFile->ExportedTypes[index].TableIndex = index;
        pFile->ExportedTypes[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if (pFile->TypeDefinitionCount > 0xFFFF) { typeDefinitionIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { typeDefinitionIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (typeDefinitionIndex == 0 || typeDefinitionIndex > pFile->TypeDefinitionCount) Panic("ExportedType_Load TypeDefinition");
        pFile->ExportedTypes[index].TypeDefinitionID = &pFile->TypeDefinitions[typeDefinitionIndex];
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ExportedTypes[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ExportedTypes[index].Namespace = (const char*)(pFile->StringsHeap + heapIndex);

        if (pFile->FileCount > Implementation_Type_MaxRows16Bit ||
            pFile->AssemblyReferenceCount > Implementation_Type_MaxRows16Bit ||
            pFile->ExportedTypeCount > Implementation_Type_MaxRows16Bit) { implementationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ExportedTypes[index].TypeOfImplementation = implementationIndex & Implementation_Type_Mask;
        implementationRow = implementationIndex >> Implementation_Type_Bits;
        switch (pFile->ExportedTypes[index].TypeOfImplementation)
        {
        case Implementation_Type_File:
            if (implementationRow == 0 || implementationRow > pFile->FileCount) Panic("ExportedType_Load File");
            pFile->ExportedTypes[index].Implementation.File = &pFile->Files[implementationRow];
            break;
        case Implementation_Type_AssemblyReference:
            if (implementationRow == 0 || implementationRow > pFile->AssemblyReferenceCount) Panic("ExportedType_Load AssemblyReference");
            pFile->ExportedTypes[index].Implementation.AssemblyReference = &pFile->AssemblyReferences[implementationRow];
            break;
        case Implementation_Type_ExportedType:
            if (implementationRow == 0 || implementationRow > pFile->ExportedTypeCount) Panic("ExportedType_Load ExportedType");
            pFile->ExportedTypes[index].Implementation.ExportedType = &pFile->ExportedTypes[implementationRow];
            break;
        default: break;
        }
    }
    return pTableData;
}

void ExportedType_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_ExportedType) ++pFile->CustomAttributes[index].Parent.ExportedType->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->ExportedTypeCount; ++index)
    {
        if (pFile->ExportedTypes[index].CustomAttributeCount > 0)
        {
            pFile->ExportedTypes[index].CustomAttributes = (CustomAttribute**)calloc(pFile->ExportedTypes[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_ExportedType &&
                    pFile->CustomAttributes[searchIndex].Parent.ExportedType == &pFile->ExportedTypes[index])
                {
                    pFile->ExportedTypes[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
