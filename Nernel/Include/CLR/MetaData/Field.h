#pragma once

#include <CLR/MetaData/MetaData.h>

struct _Field
{
	uint32_t TableIndex;
    uint16_t Flags;
    const char* Name;
    uint32_t SignatureLength;
    const uint8_t* Signature;

    Constant* Constant;
    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    FieldMarshal* FieldMarshal;
    ImplementationMap* ImplementationMap;
	TypeDefinition* TypeDefinition;
};

const uint8_t* Field_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void Field_Cleanup(CLIFile* pFile);
const uint8_t* Field_Load(CLIFile* pFile, const uint8_t* pTableData);
void Field_Link(CLIFile* pFile);