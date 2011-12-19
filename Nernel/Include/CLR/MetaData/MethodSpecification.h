#pragma once

#include <CLR/MetaData/MetaData.h>

struct _MethodSpecification
{
    MethodDefOrRefUnion(Method)
    uint32_t InstantiationLength;
    const uint8_t* Instantiation;
};

const uint8_t* MethodSpecification_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void MethodSpecification_Cleanup(CLIFile* pFile);
const uint8_t* MethodSpecification_Load(CLIFile* pFile, const uint8_t* pTableData);
