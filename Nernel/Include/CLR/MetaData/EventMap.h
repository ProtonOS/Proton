#pragma once

#include <CLR/MetaData/MetaData.h>

struct _EventMap
{
    TypeDefinition* Parent;
    Event* EventList;
    uint32_t EventListCount;
};

const uint8_t* EventMap_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void EventMap_Cleanup(CLIFile* pFile);
const uint8_t* EventMap_Load(CLIFile* pFile, const uint8_t* pTableData);
