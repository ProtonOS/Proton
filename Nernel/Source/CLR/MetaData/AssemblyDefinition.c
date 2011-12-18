#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* AssemblyDefinition_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_AssemblyDefinition)) != 0)
    {
        pFile->AssemblyDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->AssemblyDefinitions = (AssemblyDefinition*)calloc(pFile->AssemblyDefinitionCount, sizeof(AssemblyDefinition));
    }
    return pTableData;
}

void AssemblyDefinition_Cleanup(CLIFile* pFile)
{
    if (pFile->AssemblyDefinitions)
    {
        for (uint32_t index = 0; index < pFile->AssemblyDefinitionCount; ++index)
        {
        }
        free(pFile->AssemblyDefinitions);
        pFile->AssemblyDefinitions = NULL;
    }
}

const uint8_t* AssemblyDefinition_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->AssemblyDefinitionCount; ++index)
    {
        pFile->AssemblyDefinitions[index].HashAlgorithmID = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyDefinitions[index].MajorVersion = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].MinorVersion = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].Build = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].Revision = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyDefinitions[index].PublicKey = pFile->BlobsHeap + heapIndex;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyDefinitions[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyDefinitions[index].Culture = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}