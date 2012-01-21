#pragma once

#include <CLR/MetaData/MetaData.h>

struct _TypeDefinition
{
	uint32_t TableIndex;
    uint32_t Flags;
    const char* Name;
    const char* Namespace;
    TypeDefOrRefUnion(Extends);
    Field* FieldList;
	uint32_t FieldListCount;
    MethodDefinition* MethodDefinitionList;
	uint32_t MethodDefinitionListCount;

    ClassLayout* ClassLayout;
    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    DeclSecurity* DeclSecurity;
    EventMap* EventMap;
    uint32_t GenericParameterCount;
    GenericParameter** GenericParameters;
    uint32_t InterfaceImplementationCount;
    InterfaceImplementation** InterfaceImplementations;
    uint32_t MemberReferenceCount;
    MemberReference** MemberReferences;
    uint32_t MethodImplementationCount;
    MethodImplementation** MethodImplementations;
    uint32_t NestedClassCount;
    NestedClass** NestedClasses;
    PropertyMap* PropertyMap;

	bool_t HasStaticConstructor;
	bool_t HasStaticConstructorBeenRun;
};

const uint8_t* TypeDefinition_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void TypeDefinition_Cleanup(CLIFile* pFile);
const uint8_t* TypeDefinition_Load(CLIFile* pFile, const uint8_t* pTableData);
void TypeDefinition_Link(CLIFile* pFile);