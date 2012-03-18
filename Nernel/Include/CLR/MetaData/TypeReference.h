#pragma once

#include <CLR/MetaData/MetaData.h>

struct _TypeReference
{
	uint32_t TableIndex;
    ResolutionScopeUnion(ResolutionScope)
	ExportedType* ExportedType;
    const char* Name;
    const char* Namespace;
	TypeDefinition* ResolvedType;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    uint32_t MemberReferenceCount;
    MemberReference** MemberReferences;
};

const uint8_t* TypeReference_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void TypeReference_Cleanup(CLIFile* pFile);
const uint8_t* TypeReference_Load(CLIFile* pFile, const uint8_t* pTableData);
void TypeReference_Link(CLIFile* pFile);