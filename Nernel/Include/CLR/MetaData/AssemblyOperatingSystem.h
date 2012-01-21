#pragma once

#include <CLR/MetaData/MetaData.h>

struct _AssemblyOperatingSystem
{
	uint32_t TableIndex;
    uint32_t PlatformID;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
};

const uint8_t* AssemblyOperatingSystem_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void AssemblyOperatingSystem_Cleanup(CLIFile* pFile);
const uint8_t* AssemblyOperatingSystem_Load(CLIFile* pFile, const uint8_t* pTableData);
void AssemblyOperatingSystem_Link(CLIFile* pFile);