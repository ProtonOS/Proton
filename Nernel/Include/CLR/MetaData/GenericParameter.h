#pragma once

#include <CLR/MetaData/MetaData.h>

struct _GenericParameter
{
    uint16_t Index;
    uint16_t Flags;
    TypeOrMethodDefUnion(Owner)
    const char* Name;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    uint32_t GenericParameterConstraintCount;
    GenericParameterConstraint** GenericParameterConstraints;
};

const uint8_t* GenericParameter_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void GenericParameter_Cleanup(CLIFile* pFile);
const uint8_t* GenericParameter_Load(CLIFile* pFile, const uint8_t* pTableData);
void GenericParameter_Link(CLIFile* pFile);