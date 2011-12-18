#pragma once

#include <CLR/MetaData/MetaData.h>

struct _StandAloneSignature
{
    const uint8_t* Signature;
};

const uint8_t* StandAloneSignature_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void StandAloneSignature_Cleanup(CLIFile* pFile);
const uint8_t* StandAloneSignature_Load(CLIFile* pFile, const uint8_t* pTableData);
