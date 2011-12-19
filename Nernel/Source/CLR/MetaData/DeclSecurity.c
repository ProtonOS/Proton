#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* DeclSecurity_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_DeclSecurity)) != 0)
    {
        pFile->DeclSecurityCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->DeclSecurities = (DeclSecurity*)calloc(pFile->DeclSecurityCount, sizeof(DeclSecurity));
    }
    return pTableData;
}

void DeclSecurity_Cleanup(CLIFile* pFile)
{
    if (pFile->DeclSecurities)
    {
        for (uint32_t index = 0; index < pFile->DeclSecurityCount; ++index)
        {
        }
        free(pFile->DeclSecurities);
        pFile->DeclSecurities = NULL;
    }
}

const uint8_t* DeclSecurity_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->DeclSecurityCount; ++index)
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