#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* File_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_File)) != 0)
    {
        pFile->FileCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Files = (File*)calloc(pFile->FileCount, sizeof(File));
    }
    return pTableData;
}

void File_Cleanup(CLIFile* pFile)
{
    if (pFile->Files)
    {
        for (uint32_t index = 0; index < pFile->FileCount; ++index)
        {
        }
        free(pFile->Files);
        pFile->Files = NULL;
    }
}

const uint8_t* File_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->FileCount; ++index)
    {
        pFile->Files[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Files[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Files[index].HashValue = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}