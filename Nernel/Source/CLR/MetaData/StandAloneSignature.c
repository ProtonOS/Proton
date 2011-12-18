#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* StandAloneSignature_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_StandAloneSignature)) != 0)
    {
        pFile->StandAloneSignatureCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->StandAloneSignatures = (StandAloneSignature*)calloc(pFile->StandAloneSignatureCount, sizeof(StandAloneSignature));
    }
    return pTableData;
}

void StandAloneSignature_Cleanup(CLIFile* pFile)
{
    if (pFile->StandAloneSignatures)
    {
        for (uint32_t index = 0; index < pFile->StandAloneSignatureCount; ++index)
        {
        }
        free(pFile->StandAloneSignatures);
        pFile->StandAloneSignatures = NULL;
    }
}

const uint8_t* StandAloneSignature_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->StandAloneSignatureCount; ++index)
    {
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->StandAloneSignatures[index].Signature = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}