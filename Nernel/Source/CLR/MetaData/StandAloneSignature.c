#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* StandAloneSignature_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_StandAloneSignature)) != 0)
    {
        pFile->StandAloneSignatureCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->StandAloneSignatures = (StandAloneSignature*)calloc(pFile->StandAloneSignatureCount + 1, sizeof(StandAloneSignature));
    }
    return pTableData;
}

void StandAloneSignature_Cleanup(CLIFile* pFile)
{
    if (pFile->StandAloneSignatures)
    {
        for (uint32_t index = 1; index <= pFile->StandAloneSignatureCount; ++index)
        {
            if (pFile->StandAloneSignatures[index].CustomAttributes) free(pFile->StandAloneSignatures[index].CustomAttributes);
        }
        free(pFile->StandAloneSignatures);
        pFile->StandAloneSignatures = NULL;
    }
}

const uint8_t* StandAloneSignature_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->StandAloneSignatureCount; ++index)
    {
        pFile->StandAloneSignatures[index].TableIndex = index;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->StandAloneSignatures[index].Signature = MetaData_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->StandAloneSignatures[index].SignatureLength);
    }
    return pTableData;
}

void StandAloneSignature_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_StandAloneSignature) ++pFile->CustomAttributes[index].Parent.StandAloneSignature->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->StandAloneSignatureCount; ++index)
    {
        if (pFile->StandAloneSignatures[index].CustomAttributeCount > 0)
        {
            pFile->StandAloneSignatures[index].CustomAttributes = (CustomAttribute**)calloc(pFile->StandAloneSignatures[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_StandAloneSignature &&
                    pFile->CustomAttributes[searchIndex].Parent.StandAloneSignature == &pFile->StandAloneSignatures[index])
                {
                    pFile->StandAloneSignatures[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
