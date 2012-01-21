#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* TypeSpecification_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_TypeSpecification)) != 0)
    {
        pFile->TypeSpecificationCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->TypeSpecifications = (TypeSpecification*)calloc(pFile->TypeSpecificationCount + 1, sizeof(TypeSpecification));
    }
    return pTableData;
}

void TypeSpecification_Cleanup(CLIFile* pFile)
{
    if (pFile->TypeSpecifications)
    {
        for (uint32_t index = 1; index <= pFile->TypeSpecificationCount; ++index)
        {
            if (pFile->TypeSpecifications[index].CustomAttributes) free(pFile->TypeSpecifications[index].CustomAttributes);
            if (pFile->TypeSpecifications[index].MemberReferences) free(pFile->TypeSpecifications[index].MemberReferences);
        }
        free(pFile->TypeSpecifications);
        pFile->TypeSpecifications = NULL;
    }
}

const uint8_t* TypeSpecification_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->TypeSpecificationCount; ++index)
    {
        pFile->TypeSpecifications[index].TableIndex = index;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeSpecifications[index].Signature = MetaData_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->TypeSpecifications[index].SignatureLength);
    }
    return pTableData;
}

void TypeSpecification_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_TypeSpecification) ++pFile->CustomAttributes[index].Parent.TypeSpecification->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->TypeSpecificationCount; ++index)
    {
        if (pFile->TypeSpecifications[index].CustomAttributeCount > 0)
        {
            pFile->TypeSpecifications[index].CustomAttributes = (CustomAttribute**)calloc(pFile->TypeSpecifications[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_TypeSpecification &&
                    pFile->CustomAttributes[searchIndex].Parent.TypeSpecification == &pFile->TypeSpecifications[index])
                {
                    pFile->TypeSpecifications[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
    {
        if (pFile->MemberReferences[index].TypeOfParent == MemberRefParent_Type_TypeSpecification) ++pFile->MemberReferences[index].Parent.TypeSpecification->MemberReferenceCount;
    }
    for (uint32_t index = 1; index <= pFile->TypeSpecificationCount; ++index)
    {
        if (pFile->TypeSpecifications[index].MemberReferenceCount > 0)
        {
            pFile->TypeSpecifications[index].MemberReferences = (MemberReference**)calloc(pFile->TypeSpecifications[index].MemberReferenceCount, sizeof(MemberReference*));
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= pFile->MemberReferenceCount; ++searchIndex)
            {
                if (pFile->MemberReferences[searchIndex].TypeOfParent == MemberRefParent_Type_TypeSpecification &&
                    pFile->MemberReferences[searchIndex].Parent.TypeSpecification == &pFile->TypeSpecifications[index])
                {
                    pFile->TypeSpecifications[index].MemberReferences[memberReferenceIndex] = &pFile->MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
    }
}
