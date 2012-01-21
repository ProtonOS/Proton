#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* Property_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Property)) != 0)
    {
        pFile->PropertyCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Properties = (Property*)calloc(pFile->PropertyCount + 1, sizeof(Property));
    }
    return pTableData;
}

void Property_Cleanup(CLIFile* pFile)
{
    if (pFile->Properties)
    {
        for (uint32_t index = 1; index <= pFile->PropertyCount; ++index)
        {
            if (pFile->Properties[index].CustomAttributes) free(pFile->Properties[index].CustomAttributes);
        }
        free(pFile->Properties);
        pFile->Properties = NULL;
    }
}

const uint8_t* Property_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->PropertyCount; ++index)
    {
        pFile->Properties[index].TableIndex = index;
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

void Property_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->ConstantCount; ++index)
    {
        if (pFile->Constants[index].TypeOfParent == HasConstant_Type_Property) pFile->Constants[index].Parent.Property->Constant = &pFile->Constants[index];
    }
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_Property) ++pFile->CustomAttributes[index].Parent.Property->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->PropertyCount; ++index)
    {
        if (pFile->Properties[index].CustomAttributeCount > 0)
        {
            pFile->Properties[index].CustomAttributes = (CustomAttribute**)calloc(pFile->Properties[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_Property &&
                    pFile->CustomAttributes[searchIndex].Parent.Property == &pFile->Properties[index])
                {
                    pFile->Properties[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->MethodSemanticsCount; ++index)
    {
        if (pFile->MethodSemantics[index].TypeOfAssociation == HasSemantics_Type_Property) pFile->MethodSemantics[index].Association.Property->MethodSemantics = &pFile->MethodSemantics[index];
    }
}
