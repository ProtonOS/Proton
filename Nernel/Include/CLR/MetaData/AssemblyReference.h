#pragma once

#include <CLR/MetaData/MetaData.h>

struct _AssemblyReference
{
	uint32_t TableIndex;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint16_t Build;
    uint16_t Revision;
    uint32_t Flags;
    uint32_t PublicKeyOrTokenLength;
    const uint8_t* PublicKeyOrToken;
    const char* Name;
    const char* Culture;
    uint32_t HashValueLength;
    const uint8_t* HashValue;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

const uint8_t* AssemblyReference_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void AssemblyReference_Cleanup(CLIFile* pFile);
const uint8_t* AssemblyReference_Load(CLIFile* pFile, const uint8_t* pTableData);
void AssemblyReference_Link(CLIFile* pFile);