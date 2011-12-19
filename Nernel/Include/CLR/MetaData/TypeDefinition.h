#pragma once

#include <CLR/MetaData/MetaData.h>

struct _TypeDefinition
{
    uint32_t Flags;
    const char* Name;
    const char* Namespace;
    TypeDefOrRefUnion(Extends);
    Field* FieldList;
	uint32_t FieldListCount;
    MethodDefinition* MethodDefinitionList;
	uint32_t MethodDefinitionListCount;
	bool_t HasStaticConstructor;
	bool_t HasStaticConstructorBeenRun;
};

const uint8_t* TypeDefinition_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void TypeDefinition_Cleanup(CLIFile* pFile);
const uint8_t* TypeDefinition_Load(CLIFile* pFile, const uint8_t* pTableData);
