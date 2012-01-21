#pragma once

#include <CLR/MetaData/MetaData.h>

struct _Property
{
	uint32_t TableIndex;
    uint16_t Flags;
    const char* Name;
    uint32_t SignatureLength;
    const uint8_t* Signature;

    Constant* Constant;
    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    MethodSemantics* MethodSemantics;
};

const uint8_t* Property_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void Property_Cleanup(CLIFile* pFile);
const uint8_t* Property_Load(CLIFile* pFile, const uint8_t* pTableData);
void Property_Link(CLIFile* pFile);