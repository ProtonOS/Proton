#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* ModuleReference_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ModuleReference)) != 0)
    {
        pFile->ModuleReferenceCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ModuleReferences = (ModuleReference*)calloc(pFile->ModuleReferenceCount, sizeof(ModuleReference));
    }
    return pTableData;
}

void ModuleReference_Cleanup(CLIFile* pFile)
{
    if (pFile->ModuleReferences)
    {
        for (uint32_t index = 0; index < pFile->ModuleReferenceCount; ++index)
        {
        }
        free(pFile->ModuleReferences);
        pFile->ModuleReferences = NULL;
    }
}

const uint8_t* ModuleReference_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ModuleReferenceCount; ++index)
    {
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ModuleReferences[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}

void ModuleReference_Link(CLIFile* pFile)
{
}
