#pragma once

#include <CLR/MetaData/MetaData.h>

struct _TypeReference
{
    ResolutionScopeUnion(ResolutionScope)
    const char* Name;
    const char* Namespace;
	bool_t IsResolved;
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