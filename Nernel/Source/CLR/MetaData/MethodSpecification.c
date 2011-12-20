#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* MethodSpecification_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_MethodSpecification)) != 0)
    {
        pFile->MethodSpecificationCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->MethodSpecifications = (MethodSpecification*)calloc(pFile->MethodSpecificationCount, sizeof(MethodSpecification));
    }
    return pTableData;
}

void MethodSpecification_Cleanup(CLIFile* pFile)
{
    if (pFile->MethodSpecifications)
    {
        for (uint32_t index = 0; index < pFile->MethodSpecificationCount; ++index)
        {
            if (pFile->MethodSpecifications[index].CustomAttributes) free(pFile->MethodSpecifications[index].CustomAttributes);
        }
        free(pFile->MethodSpecifications);
        pFile->MethodSpecifications = NULL;
    }
}

const uint8_t* MethodSpecification_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t methodIndex = 0;
    uint32_t methodRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->MethodSpecificationCount; ++index)
    {
        if (pFile->MethodDefinitionCount > MethodDefOrRef_Type_MaxRows16Bit ||
            pFile->MemberReferenceCount > MethodDefOrRef_Type_MaxRows16Bit) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodSpecifications[index].TypeOfMethod = methodIndex & MethodDefOrRef_Type_Mask;
        methodRow = methodIndex >> MethodDefOrRef_Type_Bits;
        switch (pFile->MethodSpecifications[index].TypeOfMethod)
        {
        case MethodDefOrRef_Type_MethodDefinition: pFile->MethodSpecifications[index].Method.MethodDefinition = &pFile->MethodDefinitions[methodRow]; break;
        case MethodDefOrRef_Type_MemberReference: pFile->MethodSpecifications[index].Method.MemberReference = &pFile->MemberReferences[methodRow]; break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodSpecifications[index].Instantiation = MetaData_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->MethodSpecifications[index].InstantiationLength);
    }
    return pTableData;
}

void MethodSpecification_Link(CLIFile* pFile)
{
    for (uint32_t index = 0; index < pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_MethodSpecification) ++pFile->CustomAttributes[index].Parent.MethodSpecification->CustomAttributeCount;
    }
    for (uint32_t index = 0; index < pFile->MethodSpecificationCount; ++index)
    {
        if (pFile->MethodSpecifications[index].CustomAttributeCount > 0)
        {
            pFile->MethodSpecifications[index].CustomAttributes = (CustomAttribute**)calloc(pFile->MethodSpecifications[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 0, customAttributeIndex = 0; searchIndex < pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_MethodSpecification &&
                    pFile->CustomAttributes[searchIndex].Parent.MethodSpecification == &pFile->MethodSpecifications[index])
                {
                    pFile->MethodSpecifications[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
