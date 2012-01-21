#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* ModuleDefinition_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ModuleDefinition)) != 0)
    {
        pFile->ModuleDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ModuleDefinitions = (ModuleDefinition*)calloc(pFile->ModuleDefinitionCount + 1, sizeof(ModuleDefinition));
    }
    return pTableData;
}

void ModuleDefinition_Cleanup(CLIFile* pFile)
{
    if (pFile->ModuleDefinitions)
    {
        for (uint32_t index = 1; index <= pFile->ModuleDefinitionCount; ++index)
        {
            if (pFile->ModuleDefinitions[index].CustomAttributes) free(pFile->ModuleDefinitions[index].CustomAttributes);
        }
        free(pFile->ModuleDefinitions);
        pFile->ModuleDefinitions = NULL;
    }
}

const uint8_t* ModuleDefinition_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ModuleDefinitionCount; ++index)
    {
        pFile->ModuleDefinitions[index].TableIndex = index;
        pFile->ModuleDefinitions[index].Generation = *(uint16_t*)pTableData; 
		pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0)  { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else  { heapIndex = *((uint16_t*)pTableData); pTableData += 2; }
        pFile->ModuleDefinitions[index].Name = CLIFile_GetString(pFile, heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2;  }
        pFile->ModuleDefinitions[index].ModuleVersionID = pFile->GUIDsHeap + heapIndex;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2;  }
        pFile->ModuleDefinitions[index].EncID = pFile->GUIDsHeap + heapIndex;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ModuleDefinitions[index].EncBaseID = pFile->GUIDsHeap + heapIndex;
    }
    return pTableData;
}

void ModuleDefinition_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_ModuleDefinition) ++pFile->CustomAttributes[index].Parent.ModuleDefinition->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->ModuleDefinitionCount; ++index)
    {
        if (pFile->ModuleDefinitions[index].CustomAttributeCount > 0)
        {
            pFile->ModuleDefinitions[index].CustomAttributes = (CustomAttribute**)calloc(pFile->ModuleDefinitions[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_ModuleDefinition &&
                    pFile->CustomAttributes[searchIndex].Parent.ModuleDefinition == &pFile->ModuleDefinitions[index])
                {
                    pFile->ModuleDefinitions[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
