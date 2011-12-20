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
            if (pFile->ModuleReferences[index].CustomAttributes) free(pFile->ModuleReferences[index].CustomAttributes);
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
    for (uint32_t index = 0; index < pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_ModuleReference) ++pFile->CustomAttributes[index].Parent.ModuleReference->CustomAttributeCount;
    }
    for (uint32_t index = 0; index < pFile->ModuleReferenceCount; ++index)
    {
        if (pFile->ModuleReferences[index].CustomAttributeCount > 0)
        {
            pFile->ModuleReferences[index].CustomAttributes = (CustomAttribute**)calloc(pFile->ModuleReferences[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 0, customAttributeIndex = 0; searchIndex < pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_ModuleReference &&
                    pFile->CustomAttributes[searchIndex].Parent.ModuleReference == &pFile->ModuleReferences[index])
                {
                    pFile->ModuleReferences[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
