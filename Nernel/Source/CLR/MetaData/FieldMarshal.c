#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* FieldMarshal_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_FieldMarshal)) != 0)
    {
        pFile->FieldMarshalCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->FieldMarshals = (FieldMarshal*)calloc(pFile->FieldMarshalCount + 1, sizeof(FieldMarshal));
    }
    return pTableData;
}

void FieldMarshal_Cleanup(CLIFile* pFile)
{
    if (pFile->FieldMarshals)
    {
        for (uint32_t index = 1; index <= pFile->FieldMarshalCount; ++index)
        {
        }
        free(pFile->FieldMarshals);
        pFile->FieldMarshals = NULL;
    }
}

const uint8_t* FieldMarshal_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->FieldMarshalCount; ++index)
    {
        if (pFile->FieldCount > HasFieldMarshal_Type_MaxRows16Bit ||
            pFile->ParameterCount > HasFieldMarshal_Type_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->FieldMarshals[index].TypeOfParent = parentIndex & HasFieldMarshal_Type_Mask;
        parentRow = parentIndex >> HasFieldMarshal_Type_Bits;
        switch (pFile->FieldMarshals[index].TypeOfParent)
        {
        case HasFieldMarshal_Type_Field: pFile->FieldMarshals[index].Parent.Field = &pFile->Fields[parentRow]; break;
        case HasFieldMarshal_Type_Parameter: pFile->FieldMarshals[index].Parent.Parameter = &pFile->Parameters[parentRow]; break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->FieldMarshals[index].NativeType = MetaData_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->FieldMarshals[index].NativeTypeLength);
    }
    return pTableData;
}

void FieldMarshal_Link(CLIFile* pFile)
{
}
