#pragma once

#include <CLR/MetaData/MetaData.h>

struct _AssemblyReferenceOperatingSystem
{
    uint32_t PlatformID;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
    AssemblyReference* AssemblyReference;
};

const uint8_t* AssemblyReferenceOperatingSystem_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void AssemblyReferenceOperatingSystem_Cleanup(CLIFile* pFile);
const uint8_t* AssemblyReferenceOperatingSystem_Load(CLIFile* pFile, const uint8_t* pTableData);
