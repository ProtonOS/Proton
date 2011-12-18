#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* NestedClass_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_NestedClass)) != 0)
    {
        pFile->NestedClassCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->NestedClasses = (NestedClass*)calloc(pFile->NestedClassCount, sizeof(NestedClass));
    }
    return pTableData;
}

void NestedClass_Cleanup(CLIFile* pFile)
{
    if (pFile->NestedClasses)
    {
        for (uint32_t index = 0; index < pFile->NestedClassCount; ++index)
        {
        }
        free(pFile->NestedClasses);
        pFile->NestedClasses = NULL;
    }
}

const uint8_t* NestedClass_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t nestedIndex = 0;
    uint32_t enclosingIndex = 0;
    for (uint32_t index = 0; index < pFile->NestedClassCount; ++index)
    {
        if (pFile->TypeDefinitionCount > 0xFFFF) { nestedIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { nestedIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->NestedClasses[index].Nested = &pFile->TypeDefinitions[nestedIndex];
        if (pFile->TypeDefinitionCount > 0xFFFF) { enclosingIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { enclosingIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->NestedClasses[index].Enclosing = &pFile->TypeDefinitions[enclosingIndex];
    }
    return pTableData;
}