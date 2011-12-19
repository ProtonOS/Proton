#pragma once

#include <CLR/MetaData/MetaData.h>

struct _ImplementationMap
{
    uint16_t MappingFlags;
    MemberForwardedUnion(MemberForwarded)
    const char* ImportName;
    ModuleReference* ImportScope;
};

const uint8_t* ImplementationMap_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void ImplementationMap_Cleanup(CLIFile* pFile);
const uint8_t* ImplementationMap_Load(CLIFile* pFile, const uint8_t* pTableData);
void ImplementationMap_Link(CLIFile* pFile);
