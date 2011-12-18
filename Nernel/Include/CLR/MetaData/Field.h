#pragma once

#include <CLR/MetaData/MetaData.h>

struct _Field
{
    uint16_t Flags;
    const char* Name;
    const uint8_t* Signature;
};

const uint8_t* Field_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void Field_Cleanup(CLIFile* pFile);
const uint8_t* Field_Load(CLIFile* pFile, const uint8_t* pTableData);
