#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* Constant_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Constant)) != 0)
    {
        pFile->ConstantCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Constants = (Constant*)calloc(pFile->ConstantCount + 1, sizeof(Constant));
    }
    return pTableData;
}

void Constant_Cleanup(CLIFile* pFile)
{
    if (pFile->Constants)
    {
        for (uint32_t index = 1; index <= pFile->ConstantCount; ++index)
        {
        }
        free(pFile->Constants);
        pFile->Constants = NULL;
    }
}

const uint8_t* Constant_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ConstantCount; ++index)
    {
        pFile->Constants[index].Type = *pTableData; pTableData += 2; // 1 unused padding byte
        if (pFile->FieldCount > HasConstant_Type_MaxRows16Bit ||
            pFile->ParameterCount > HasConstant_Type_MaxRows16Bit ||
            pFile->PropertyCount > HasConstant_Type_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Constants[index].TypeOfParent = parentIndex & HasConstant_Type_Mask;
        parentRow = parentIndex >> HasConstant_Type_Bits;
        switch (pFile->Constants[index].TypeOfParent)
        {
        case HasConstant_Type_Field: pFile->Constants[index].Parent.Field = &pFile->Fields[parentRow]; break;
        case HasConstant_Type_Parameter: pFile->Constants[index].Parent.Parameter = &pFile->Parameters[parentRow]; break;
        case HasConstant_Type_Property: pFile->Constants[index].Parent.Property = &pFile->Properties[parentRow]; break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Constants[index].Value = MetaData_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->Constants[index].ValueLength);
    }
    return pTableData;
}

void Constant_Link(CLIFile* pFile)
{
}
