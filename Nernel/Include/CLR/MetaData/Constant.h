#pragma once

#include <CLR/MetaData/MetaData.h>

struct _Constant
{
    uint8_t Type;
    HasConstantUnion(Parent)
    uint32_t ValueLength;
    const uint8_t* Value;
};

const uint8_t* Constant_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void Constant_Cleanup(CLIFile* pFile);
const uint8_t* Constant_Load(CLIFile* pFile, const uint8_t* pTableData);
