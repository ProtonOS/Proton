#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* MemberReference_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_MemberReference)) != 0)
    {
        pFile->MemberReferenceCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->MemberReferences = (MemberReference*)calloc(pFile->MemberReferenceCount, sizeof(MemberReference));
    }
    return pTableData;
}

void MemberReference_Cleanup(CLIFile* pFile)
{
    if (pFile->MemberReferences)
    {
        for (uint32_t index = 0; index < pFile->MemberReferenceCount; ++index)
        {
            if (pFile->MemberReferences[index].CustomAttributes) free(pFile->MemberReferences[index].CustomAttributes);
        }
        free(pFile->MemberReferences);
        pFile->MemberReferences = NULL;
    }
}

const uint8_t* MemberReference_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->MemberReferenceCount; ++index)
    {
        if (pFile->TypeDefinitionCount > MemberRefParent_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > MemberRefParent_Type_MaxRows16Bit ||
            pFile->ModuleReferenceCount > MemberRefParent_Type_MaxRows16Bit ||
            pFile->MethodDefinitionCount > MemberRefParent_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > MemberRefParent_Type_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MemberReferences[index].TypeOfParent = parentIndex & MemberRefParent_Type_Mask;
        parentRow = parentIndex >> MemberRefParent_Type_Bits;
        switch (pFile->MemberReferences[index].TypeOfParent)
        {
        case MemberRefParent_Type_TypeDefinition: pFile->MemberReferences[index].Parent.TypeDefinition = &pFile->TypeDefinitions[parentRow]; break;
        case MemberRefParent_Type_TypeReference: pFile->MemberReferences[index].Parent.TypeReference = &pFile->TypeReferences[parentRow]; break;
        case MemberRefParent_Type_ModuleReference: pFile->MemberReferences[index].Parent.ModuleReference = &pFile->ModuleReferences[parentRow]; break;
        case MemberRefParent_Type_MethodDefinition: pFile->MemberReferences[index].Parent.MethodDefinition = &pFile->MethodDefinitions[parentRow]; break;
        case MemberRefParent_Type_TypeSpecification: pFile->MemberReferences[index].Parent.TypeSpecification = &pFile->TypeSpecifications[parentRow]; break;
        default: break;
        }

        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MemberReferences[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MemberReferences[index].Signature = MetaData_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->MemberReferences[index].SignatureLength);
    }
    return pTableData;
}

void MemberReference_Link(CLIFile* pFile)
{
    for (uint32_t index = 0; index < pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_MemberReference) ++pFile->CustomAttributes[index].Parent.MemberReference->CustomAttributeCount;
    }
    for (uint32_t index = 0; index < pFile->MemberReferenceCount; ++index)
    {
        if (pFile->MemberReferences[index].CustomAttributeCount > 0)
        {
            pFile->MemberReferences[index].CustomAttributes = (CustomAttribute**)calloc(pFile->MemberReferences[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 0, customAttributeIndex = 0; searchIndex < pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_MemberReference &&
                    pFile->CustomAttributes[searchIndex].Parent.MemberReference == &pFile->MemberReferences[index])
                {
                    pFile->MemberReferences[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
