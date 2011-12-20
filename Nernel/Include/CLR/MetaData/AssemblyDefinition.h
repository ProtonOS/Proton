#pragma once

#include <CLR/MetaData/MetaData.h>

struct _AssemblyDefinition
{
    uint32_t HashAlgorithmID;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint16_t Build;
    uint16_t Revision;
    uint32_t Flags;
    uint32_t PublicKeyLength;
    const uint8_t* PublicKey;
    const char* Name;
    const char* Culture;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    DeclSecurity* DeclSecurity;
};

const uint8_t* AssemblyDefinition_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void AssemblyDefinition_Cleanup(CLIFile* pFile);
const uint8_t* AssemblyDefinition_Load(CLIFile* pFile, const uint8_t* pTableData);
void AssemblyDefinition_Link(CLIFile* pFile);
