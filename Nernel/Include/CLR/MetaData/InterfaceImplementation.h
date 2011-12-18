#pragma once

#include <CLR/MetaData/MetaData.h>

struct _InterfaceImplementation
{
    TypeDefinition* Implementor;
    TypeDefOrRefUnion(Interface)
};

const uint8_t* InterfaceImplementation_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void InterfaceImplementation_Cleanup(CLIFile* pFile);
const uint8_t* InterfaceImplementation_Load(CLIFile* pFile, const uint8_t* pTableData);
