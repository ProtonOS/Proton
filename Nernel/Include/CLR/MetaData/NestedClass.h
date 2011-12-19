#pragma once

#include <CLR/MetaData/MetaData.h>

struct _NestedClass
{
    TypeDefinition* Nested;
    TypeDefinition* Enclosing;
};

const uint8_t* NestedClass_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void NestedClass_Cleanup(CLIFile* pFile);
const uint8_t* NestedClass_Load(CLIFile* pFile, const uint8_t* pTableData);
void NestedClass_Link(CLIFile* pFile);