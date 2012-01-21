#pragma once

#include <CLR/MetaData/MetaData.h>

struct _InterfaceImplementation
{
	uint32_t TableIndex;
    TypeDefinition* Implementor;
    TypeDefOrRefUnion(Interface)

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

const uint8_t* InterfaceImplementation_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void InterfaceImplementation_Cleanup(CLIFile* pFile);
const uint8_t* InterfaceImplementation_Load(CLIFile* pFile, const uint8_t* pTableData);
void InterfaceImplementation_Link(CLIFile* pFile);
