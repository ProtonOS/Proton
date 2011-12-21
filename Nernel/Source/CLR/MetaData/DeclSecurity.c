#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* DeclSecurity_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_DeclSecurity)) != 0)
    {
        pFile->DeclSecurityCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->DeclSecurities = (DeclSecurity*)calloc(pFile->DeclSecurityCount + 1, sizeof(DeclSecurity));
    }
    return pTableData;
}

void DeclSecurity_Cleanup(CLIFile* pFile)
{
    if (pFile->DeclSecurities)
    {
        for (uint32_t index = 1; index <= pFile->DeclSecurityCount; ++index)
        {
            if (pFile->DeclSecurities[index].CustomAttributes) free(pFile->DeclSecurities[index].CustomAttributes);
        }
        free(pFile->DeclSecurities);
        pFile->DeclSecurities = NULL;
    }
}

const uint8_t* DeclSecurity_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->DeclSecurityCount; ++index)
    {
        pFile->DeclSecurities[index].Action = *(uint16_t*)pTableData; pTableData += 2;

        if (pFile->TypeDefinitionCount > HasDeclSecurity_Type_MaxRows16Bit ||
            pFile->MethodDefinitionCount > HasDeclSecurity_Type_MaxRows16Bit ||
            pFile->AssemblyDefinitionCount > HasDeclSecurity_Type_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->DeclSecurities[index].TypeOfParent = parentIndex & HasDeclSecurity_Type_Mask;
        parentRow = parentIndex >> HasDeclSecurity_Type_Bits;
        switch (pFile->DeclSecurities[index].TypeOfParent)
        {
        case HasDeclSecurity_Type_TypeDefinition: pFile->DeclSecurities[index].Parent.TypeDefinition = &pFile->TypeDefinitions[parentRow]; break;
        case HasDeclSecurity_Type_MethodDefinition: pFile->DeclSecurities[index].Parent.MethodDefinition = &pFile->MethodDefinitions[parentRow]; break;
        case HasDeclSecurity_Type_AssemblyDefinition: pFile->DeclSecurities[index].Parent.AssemblyDefinition = &pFile->AssemblyDefinitions[parentRow]; break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->DeclSecurities[index].PermissionSet = MetaData_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->DeclSecurities[index].PermissionSetLength);
    }
    return pTableData;
}

void DeclSecurity_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_DeclSecurity) ++pFile->CustomAttributes[index].Parent.DeclSecurity->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->DeclSecurityCount; ++index)
    {
        if (pFile->DeclSecurities[index].CustomAttributeCount > 0)
        {
            pFile->DeclSecurities[index].CustomAttributes = (CustomAttribute**)calloc(pFile->DeclSecurities[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_DeclSecurity &&
                    pFile->CustomAttributes[searchIndex].Parent.DeclSecurity == &pFile->DeclSecurities[index])
                {
                    pFile->DeclSecurities[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
