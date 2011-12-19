#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* Property_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Property)) != 0)
    {
        pFile->PropertyCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Properties = (Property*)calloc(pFile->PropertyCount, sizeof(Property));
    }
    return pTableData;
}

void Property_Cleanup(CLIFile* pFile)
{
    if (pFile->Properties)
    {
        for (uint32_t index = 0; index < pFile->PropertyCount; ++index)
        {
        }
        free(pFile->Properties);
        pFile->Properties = NULL;
    }
}

const uint8_t* Property_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->PropertyCount; ++index)
    {
        pFile->Properties[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Properties[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Properties[index].Signature = MetaData_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->Properties[index].SignatureLength);
    }
    return pTableData;
}