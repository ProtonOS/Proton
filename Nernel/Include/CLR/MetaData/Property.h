#pragma once

#include <CLR/MetaData/MetaData.h>

struct _Property
{
    uint16_t Flags;
    const char* Name;
    uint32_t SignatureLength;
    const uint8_t* Signature;
};

const uint8_t* Property_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void Property_Cleanup(CLIFile* pFile);
const uint8_t* Property_Load(CLIFile* pFile, const uint8_t* pTableData);
