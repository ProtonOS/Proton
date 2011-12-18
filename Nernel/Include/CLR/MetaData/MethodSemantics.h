#pragma once

#include <CLR/MetaData/MetaData.h>

struct _MethodSemantics
{
    uint16_t Semantics;
    MethodDefinition* Method;
    HasSemanticsUnion(Association)
};

const uint8_t* MethodSemantics_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void MethodSemantics_Cleanup(CLIFile* pFile);
const uint8_t* MethodSemantics_Load(CLIFile* pFile, const uint8_t* pTableData);
