#pragma once

#include <CLR/MetaData/MetaData.h>

struct _Parameter
{
	uint32_t TableIndex;
    uint16_t Flags;
    uint16_t Sequence;
    const char* Name;

    Constant* Constant;
    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    FieldMarshal* FieldMarshal;
};

const uint8_t* Parameter_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void Parameter_Cleanup(CLIFile* pFile);
const uint8_t* Parameter_Load(CLIFile* pFile, const uint8_t* pTableData);
void Parameter_Link(CLIFile* pFile);