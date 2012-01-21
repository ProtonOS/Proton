#pragma once

#include <CLR/MetaData/MetaData.h>

struct _TypeSpecification
{
	uint32_t TableIndex;
    uint32_t SignatureLength;
    const uint8_t* Signature;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    uint32_t MemberReferenceCount;
    MemberReference** MemberReferences;
};

const uint8_t* TypeSpecification_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void TypeSpecification_Cleanup(CLIFile* pFile);
const uint8_t* TypeSpecification_Load(CLIFile* pFile, const uint8_t* pTableData);
void TypeSpecification_Link(CLIFile* pFile);