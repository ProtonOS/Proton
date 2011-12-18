#pragma once

#include <CLR/MetaData/MetaData.h>

struct _AssemblyProcessor
{
    uint32_t Processor;
};

const uint8_t* AssemblyProcessor_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void AssemblyProcessor_Cleanup(CLIFile* pFile);
const uint8_t* AssemblyProcessor_Load(CLIFile* pFile, const uint8_t* pTableData);
