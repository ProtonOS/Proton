#pragma once

#include <CLR/MetaData/MetaData.h>

struct _FieldRVA
{
    const uint8_t* InitialValue;
    Field* Field;
};

const uint8_t* FieldRVA_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void FieldRVA_Cleanup(CLIFile* pFile);
const uint8_t* FieldRVA_Load(CLIFile* pFile, const uint8_t* pTableData);
