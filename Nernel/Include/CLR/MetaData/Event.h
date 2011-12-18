#pragma once

#include <CLR/MetaData/MetaData.h>

struct _Event
{
    uint16_t Flags;
    const char* Name;
    TypeDefOrRefUnion(EventType)
};

const uint8_t* Event_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void Event_Cleanup(CLIFile* pFile);
const uint8_t* Event_Load(CLIFile* pFile, const uint8_t* pTableData);
