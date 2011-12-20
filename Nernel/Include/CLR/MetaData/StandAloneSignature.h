#pragma once

#include <CLR/MetaData/MetaData.h>

struct _StandAloneSignature
{
    uint32_t SignatureLength;
    const uint8_t* Signature;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

const uint8_t* StandAloneSignature_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void StandAloneSignature_Cleanup(CLIFile* pFile);
const uint8_t* StandAloneSignature_Load(CLIFile* pFile, const uint8_t* pTableData);
void StandAloneSignature_Link(CLIFile* pFile);