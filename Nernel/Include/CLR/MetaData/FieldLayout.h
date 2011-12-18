#pragma once

#include <CLR/MetaData/MetaData.h>

struct _FieldLayout
{
    uint32_t Offset;
    Field* Field;
};

const uint8_t* FieldLayout_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void FieldLayout_Cleanup(CLIFile* pFile);
const uint8_t* FieldLayout_Load(CLIFile* pFile, const uint8_t* pTableData);
