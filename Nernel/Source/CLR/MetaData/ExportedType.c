#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* ExportedType_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ExportedType)) != 0)
    {
        pFile->ExportedTypeCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ExportedTypes = (ExportedType*)calloc(pFile->ExportedTypeCount, sizeof(ExportedType));
    }
    return pTableData;
}

void ExportedType_Cleanup(CLIFile* pFile)
{
    if (pFile->ExportedTypes)
    {
        for (uint32_t index = 0; index < pFile->ExportedTypeCount; ++index)
        {
        }
        free(pFile->ExportedTypes);
        pFile->ExportedTypes = NULL;
    }
}

const uint8_t* ExportedType_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t typeDefinitionIndex = 0;
    uint32_t implementationIndex = 0;
    uint8_t implementationTable = 0;
    uint32_t implementationRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ExportedTypeCount; ++index)
    {
        pFile->ExportedTypes[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if (pFile->TypeDefinitionCount > 0xFFFF) { typeDefinitionIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { typeDefinitionIndex = *(uint16_t*)pTableData; pTableData += 2; }
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
        implementationTable = implementationIndex & Implementation_Type_Mask;
        implementationRow = implementationIndex >> Implementation_Type_Bits;
        switch (implementationTable)
        {
        case Implementation_Type_File: pFile->ExportedTypes[index].Implementation.File = &pFile->Files[implementationRow]; break;
        case Implementation_Type_AssemblyReference: pFile->ExportedTypes[index].Implementation.AssemblyReference = &pFile->AssemblyReferences[implementationRow]; break;
        case Implementation_Type_ExportedType: pFile->ExportedTypes[index].Implementation.ExportedType = &pFile->ExportedTypes[implementationRow]; break;
        default: break;
        }
    }
    return pTableData;
}