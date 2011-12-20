#pragma once

#include <CLR/MetaData/MetaData.h>

struct _ModuleReference
{
    const char* Name;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

const uint8_t* ModuleReference_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void ModuleReference_Cleanup(CLIFile* pFile);
const uint8_t* ModuleReference_Load(CLIFile* pFile, const uint8_t* pTableData);
void ModuleReference_Link(CLIFile* pFile);