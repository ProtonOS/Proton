#pragma once

#include <CLR/MetaData/MetaData.h>

struct _MemberReference
{
    MemberRefParentUnion(Parent)
    const char* Name;
    uint32_t SignatureLength;
    const uint8_t* Signature;
};

const uint8_t* MemberReference_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void MemberReference_Cleanup(CLIFile* pFile);
const uint8_t* MemberReference_Load(CLIFile* pFile, const uint8_t* pTableData);
