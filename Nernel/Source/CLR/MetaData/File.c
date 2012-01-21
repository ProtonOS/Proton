#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* File_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_File)) != 0)
    {
        pFile->FileCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Files = (File*)calloc(pFile->FileCount + 1, sizeof(File));
    }
    return pTableData;
}

void File_Cleanup(CLIFile* pFile)
{
    if (pFile->Files)
    {
        for (uint32_t index = 1; index <= pFile->FileCount; ++index)
        {
            if (pFile->Files[index].CustomAttributes) free(pFile->Files[index].CustomAttributes);
        }
        free(pFile->Files);
        pFile->Files = NULL;
    }
}

const uint8_t* File_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->FileCount; ++index)
    {
        pFile->Files[index].TableIndex = index;
        pFile->Files[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Files[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Files[index].HashValue = MetaData_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->Files[index].HashValueLength);
    }
    return pTableData;
}

void File_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_File) ++pFile->CustomAttributes[index].Parent.File->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->FileCount; ++index)
    {
        if (pFile->Files[index].CustomAttributeCount > 0)
        {
            pFile->Files[index].CustomAttributes = (CustomAttribute**)calloc(pFile->Files[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_File &&
                    pFile->CustomAttributes[searchIndex].Parent.File == &pFile->Files[index])
                {
                    pFile->Files[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
