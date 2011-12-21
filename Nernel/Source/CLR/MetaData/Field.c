#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* Field_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Field)) != 0)
    {
        pFile->FieldCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Fields = (Field*)calloc(pFile->FieldCount + 1, sizeof(Field));
    }
    return pTableData;
}

void Field_Cleanup(CLIFile* pFile)
{
    if (pFile->Fields)
    {
        for (uint32_t index = 1; index <= pFile->FieldCount; ++index)
        {
            if (pFile->Fields[index].CustomAttributes) free(pFile->Fields[index].CustomAttributes);
        }
        free(pFile->Fields);
        pFile->Fields = NULL;
    }
}

const uint8_t* Field_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->FieldCount; ++index)
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
    for (uint32_t index = 1; index <= pFile->ConstantCount; ++index)
    {
        if (pFile->Constants[index].TypeOfParent == HasConstant_Type_Field) pFile->Constants[index].Parent.Field->Constant = &pFile->Constants[index];
    }
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_Field) ++pFile->CustomAttributes[index].Parent.Field->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->FieldCount; ++index)
    {
        if (pFile->Fields[index].CustomAttributeCount > 0)
        {
            pFile->Fields[index].CustomAttributes = (CustomAttribute**)calloc(pFile->Fields[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_Field &&
                    pFile->CustomAttributes[searchIndex].Parent.Field == &pFile->Fields[index])
                {
                    pFile->Fields[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->FieldMarshalCount; ++index)
    {
        if (pFile->FieldMarshals[index].TypeOfParent == HasConstant_Type_Field) pFile->FieldMarshals[index].Parent.Field->FieldMarshal = &pFile->FieldMarshals[index];
    }
    for (uint32_t index = 1; index <= pFile->ImplementationMapCount; ++index)
    {
        if (pFile->ImplementationMaps[index].TypeOfMemberForwarded == MemberForwarded_Type_Field) pFile->ImplementationMaps[index].MemberForwarded.Field->ImplementationMap = &pFile->ImplementationMaps[index];
    }
}
