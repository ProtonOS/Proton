#pragma once

#include <CLR/MetaData/MetaData.h>

struct _File
{
	uint32_t TableIndex;
    uint32_t Flags;
    const char* Name;
    uint32_t HashValueLength;
    const uint8_t* HashValue;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

const uint8_t* File_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void File_Cleanup(CLIFile* pFile);
const uint8_t* File_Load(CLIFile* pFile, const uint8_t* pTableData);
void File_Link(CLIFile* pFile);