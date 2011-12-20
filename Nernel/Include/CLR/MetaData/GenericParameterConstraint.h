#pragma once

#include <CLR/MetaData/MetaData.h>

struct _GenericParameterConstraint
{
    GenericParameter* Owner;
    TypeDefOrRefUnion(Constraint)

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

const uint8_t* GenericParameterConstraint_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void GenericParameterConstraint_Cleanup(CLIFile* pFile);
const uint8_t* GenericParameterConstraint_Load(CLIFile* pFile, const uint8_t* pTableData);
void GenericParameterConstraint_Link(CLIFile* pFile);