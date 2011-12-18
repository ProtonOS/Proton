#pragma once

#include <CLR/MetaData/MetaData.h>

struct _FieldMarshal
{
    HasFieldMarshalUnion(Parent)
    const uint8_t* NativeType;
};

const uint8_t* FieldMarshal_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void FieldMarshal_Cleanup(CLIFile* pFile);
const uint8_t* FieldMarshal_Load(CLIFile* pFile, const uint8_t* pTableData);
