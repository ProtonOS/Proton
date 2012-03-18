#pragma once

#include <CLR/MetaData/MetaData.h>

struct _MemberReference
{
	uint32_t TableIndex;
    MemberRefParentUnion(Parent)
    const char* Name;
    uint32_t SignatureLength;
    const uint8_t* Signature;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;

	uint8_t TypeOfResolved;
	union
	{
		MethodDefinition* MethodDefinition;
		Field* Field;
	} Resolved;
};

const uint8_t* MemberReference_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void MemberReference_Cleanup(CLIFile* pFile);
const uint8_t* MemberReference_Load(CLIFile* pFile, const uint8_t* pTableData);
void MemberReference_Link(CLIFile* pFile);
