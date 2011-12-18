#pragma once

#include <CLR/MetaData/MetaData.h>

struct _ClassLayout
{
    uint16_t PackingSize;
    uint32_t ClassSize;
    TypeDefinition* Parent;
};

const uint8_t* ClassLayout_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void ClassLayout_Cleanup(CLIFile* pFile);
const uint8_t* ClassLayout_Load(CLIFile* pFile, const uint8_t* pTableData);
