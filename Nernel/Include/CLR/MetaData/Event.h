#pragma once

#include <CLR/MetaData/MetaData.h>

struct _Event
{
	uint32_t TableIndex;
    uint16_t Flags;
    const char* Name;
    TypeDefOrRefUnion(EventType)

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    MethodSemantics* MethodSemantics;
};

const uint8_t* Event_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void Event_Cleanup(CLIFile* pFile);
const uint8_t* Event_Load(CLIFile* pFile, const uint8_t* pTableData);
void Event_Link(CLIFile* pFile);