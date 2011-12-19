#pragma once

#include <CLR/MetaData/MetaData.h>

struct _DeclSecurity
{
    uint16_t Action;
    HasDeclSecurityUnion(Parent)
    uint32_t PermissionSetLength;
    const uint8_t* PermissionSet;
};


const uint8_t* DeclSecurity_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void DeclSecurity_Cleanup(CLIFile* pFile);
const uint8_t* DeclSecurity_Load(CLIFile* pFile, const uint8_t* pTableData);
