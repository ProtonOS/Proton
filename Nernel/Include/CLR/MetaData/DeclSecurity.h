#pragma once

#include <CLR/MetaData/MetaData.h>

struct _DeclSecurity
{
	uint32_t TableIndex;
    uint16_t Action;
    HasDeclSecurityUnion(Parent)
    uint32_t PermissionSetLength;
    const uint8_t* PermissionSet;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};


const uint8_t* DeclSecurity_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void DeclSecurity_Cleanup(CLIFile* pFile);
const uint8_t* DeclSecurity_Load(CLIFile* pFile, const uint8_t* pTableData);
void DeclSecurity_Link(CLIFile* pFile);