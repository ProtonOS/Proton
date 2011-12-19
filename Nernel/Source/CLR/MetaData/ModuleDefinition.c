#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* ModuleDefinition_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ModuleDefinition)) != 0)
    {
        pFile->ModuleDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ModuleDefinitions = (ModuleDefinition*)calloc(pFile->ModuleDefinitionCount, sizeof(ModuleDefinition));
    }
    return pTableData;
}

void ModuleDefinition_Cleanup(CLIFile* pFile)
{
    if (pFile->ModuleDefinitions)
    {
        for (uint32_t index = 0; index < pFile->ModuleDefinitionCount; ++index)
        {
        }
        free(pFile->ModuleDefinitions);
        pFile->ModuleDefinitions = NULL;
    }
}

const uint8_t* ModuleDefinition_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ModuleDefinitionCount; ++index)
    {
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
}
