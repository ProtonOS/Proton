#pragma once

#include <CLR/MetaData/MetaData.h>

struct _AssemblyReferenceProcessor
{
	uint32_t TableIndex;
    uint32_t Processor;
    AssemblyReference* AssemblyReference;
};

const uint8_t* AssemblyReferenceProcessor_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void AssemblyReferenceProcessor_Cleanup(CLIFile* pFile);
const uint8_t* AssemblyReferenceProcessor_Load(CLIFile* pFile, const uint8_t* pTableData);
void AssemblyReferenceProcessor_Link(CLIFile* pFile);