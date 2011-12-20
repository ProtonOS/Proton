#pragma once

#include <CLR/MetaData/MetaData.h>

struct _ManifestResource
{
    uint32_t Offset;
    uint32_t Flags;
    const char* Name;
    ImplementationUnion(Implementation)

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

const uint8_t* ManifestResource_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void ManifestResource_Cleanup(CLIFile* pFile);
const uint8_t* ManifestResource_Load(CLIFile* pFile, const uint8_t* pTableData);
void ManifestResource_Link(CLIFile* pFile);
