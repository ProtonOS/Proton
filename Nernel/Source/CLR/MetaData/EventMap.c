#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* EventMap_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_EventMap)) != 0)
    {
        pFile->EventMapCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->EventMaps = (EventMap*)calloc(pFile->EventMapCount, sizeof(EventMap));
    }
    return pTableData;
}

void EventMap_Cleanup(CLIFile* pFile)
{
    if (pFile->EventMaps)
    {
        for (uint32_t index = 0; index < pFile->EventMapCount; ++index)
        {
        }
        free(pFile->EventMaps);
        pFile->EventMaps = NULL;
    }
}

const uint8_t* EventMap_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t eventListIndex = 0;
    for (uint32_t index = 0; index < pFile->EventMapCount; ++index)
    {
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->EventMaps[index].Parent = &pFile->TypeDefinitions[parentIndex];
        if (pFile->EventCount > 0xFFFF) { eventListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { eventListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->EventMaps[index].EventList = &pFile->Events[eventListIndex];
    }
    return pTableData;
}