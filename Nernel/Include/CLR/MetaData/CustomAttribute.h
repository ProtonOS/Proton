#pragma once

#include <CLR/MetaData/MetaData.h>

struct _CustomAttribute
{
    HasCustomAttributeUnion(Parent)
    CustomAttributeTypeUnion(Type)
    uint32_t ValueLength;
    const uint8_t* Value;
};

const uint8_t* CustomAttribute_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void CustomAttribute_Cleanup(CLIFile* pFile);
const uint8_t* CustomAttribute_Load(CLIFile* pFile, const uint8_t* pTableData);
void CustomAttribute_Link(CLIFile* pFile);