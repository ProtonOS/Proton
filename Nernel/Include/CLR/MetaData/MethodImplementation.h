#pragma once

#include <CLR/MetaData/MetaData.h>

struct _MethodImplementation
{
	uint32_t TableIndex;
    TypeDefinition* Parent;
    MethodDefOrRefUnion(MethodBody)
    MethodDefOrRefUnion(MethodDeclaration)
};

const uint8_t* MethodImplementation_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void MethodImplementation_Cleanup(CLIFile* pFile);
const uint8_t* MethodImplementation_Load(CLIFile* pFile, const uint8_t* pTableData);
void MethodImplementation_Link(CLIFile* pFile);