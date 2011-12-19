#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* Field_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Field)) != 0)
    {
        pFile->FieldCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Fields = (Field*)calloc(pFile->FieldCount, sizeof(Field));
    }
    return pTableData;
}

void Field_Cleanup(CLIFile* pFile)
{
    if (pFile->Fields)
    {
        for (uint32_t index = 0; index < pFile->FieldCount; ++index)
        {
        }
        free(pFile->Fields);
        pFile->Fields = NULL;
    }
}

const uint8_t* Field_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->FieldCount; ++index)
    {
        pFile->Fields[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Fields[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Fields[index].Signature = MetaData_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->Fields[index].SignatureLength);
    }
    return pTableData;
}

void Field_Link(CLIFile* pFile)
{
}
