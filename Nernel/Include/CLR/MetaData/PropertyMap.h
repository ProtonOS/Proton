#pragma once

#include <CLR/MetaData/MetaData.h>

struct _PropertyMap
{
    TypeDefinition* Parent;
    Property* PropertyList;
    uint32_t PropertyListCount;
};

const uint8_t* PropertyMap_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void PropertyMap_Cleanup(CLIFile* pFile);
const uint8_t* PropertyMap_Load(CLIFile* pFile, const uint8_t* pTableData);
void PropertyMap_Link(CLIFile* pFile);