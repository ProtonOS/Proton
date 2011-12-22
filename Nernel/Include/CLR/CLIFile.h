#pragma once

#include <CLR/PEFile.h>
#include <CLR/MetaData/MetaData.h>
#include <CLR/MetaData/Signature.h>

#define CLIFile_Machine                             0x014C

typedef struct
{
    uint32_t SizeOfHeader;
    uint16_t MajorRuntimeVersion;
    uint16_t MinorRuntimeVersion;
    PEDataDirectory MetaData;
    uint32_t Flags;
    union
    {
        uint32_t EntryPointToken;
        uint32_t EntryPointRVA;
    };

    PEDataDirectory Resources;
    PEDataDirectory StrongNameSignature;

    PEDataDirectory CodeManagerTable;
    PEDataDirectory VTableFixups;
    PEDataDirectory ExportAddressTableJumps;

    PEDataDirectory ManagedNativeHeader;
} CLIHeader;

typedef struct
{
    const PEFile* PEFile;
    const CLIHeader* CLIHeader;
    const uint8_t* MetaDataHeader;
    const uint8_t* Tables;
    const uint8_t* StringsHeap;
    const uint8_t* UserStringsHeap;
    const uint8_t* BlobsHeap;
    const uint8_t* GUIDsHeap;

    const MetaDataTablesHeader* TablesHeader;
    uint32_t ModuleDefinitionCount;
    ModuleDefinition* ModuleDefinitions;
    uint32_t TypeReferenceCount;
    TypeReference* TypeReferences;
    uint32_t TypeDefinitionCount;
    TypeDefinition* TypeDefinitions;
    uint32_t FieldCount;
    Field* Fields;
    uint32_t MethodDefinitionCount;
    MethodDefinition* MethodDefinitions;
    uint32_t ParameterCount;
    Parameter* Parameters;
    uint32_t InterfaceImplementationCount;
    InterfaceImplementation* InterfaceImplementations;
    uint32_t MemberReferenceCount;
    MemberReference* MemberReferences;
    uint32_t ConstantCount;
    Constant* Constants;
    uint32_t CustomAttributeCount;
    CustomAttribute* CustomAttributes;
    uint32_t FieldMarshalCount;
    FieldMarshal* FieldMarshals;
    uint32_t DeclSecurityCount;
    DeclSecurity* DeclSecurities;
    uint32_t ClassLayoutCount;
    ClassLayout* ClassLayouts;
    uint32_t FieldLayoutCount;
    FieldLayout* FieldLayouts;
    uint32_t StandAloneSignatureCount;
    StandAloneSignature* StandAloneSignatures;
    uint32_t EventMapCount;
    EventMap* EventMaps;
    uint32_t EventCount;
    Event* Events;
    uint32_t PropertyMapCount;
    PropertyMap* PropertyMaps;
    uint32_t PropertyCount;
    Property* Properties;
    uint32_t MethodSemanticsCount;
    MethodSemantics* MethodSemantics;
    uint32_t MethodImplementationCount;
    MethodImplementation* MethodImplementations;
    uint32_t ModuleReferenceCount;
    ModuleReference* ModuleReferences;
    uint32_t TypeSpecificationCount;
    TypeSpecification* TypeSpecifications;
    uint32_t ImplementationMapCount;
    ImplementationMap* ImplementationMaps;
    uint32_t FieldRVACount;
    FieldRVA* FieldRVAs;
    uint32_t AssemblyDefinitionCount;
    AssemblyDefinition* AssemblyDefinitions;
    uint32_t AssemblyProcessorCount;
    AssemblyProcessor* AssemblyProcessors;
    uint32_t AssemblyOperatingSystemCount;
    AssemblyOperatingSystem* AssemblyOperatingSystems;
    uint32_t AssemblyReferenceCount;
    AssemblyReference* AssemblyReferences;
    uint32_t AssemblyReferenceProcessorCount;
    AssemblyReferenceProcessor* AssemblyReferenceProcessors;
    uint32_t AssemblyReferenceOperatingSystemCount;
    AssemblyReferenceOperatingSystem* AssemblyReferenceOperatingSystems;
    uint32_t FileCount;
    File* Files;
    uint32_t ExportedTypeCount;
    ExportedType* ExportedTypes;
    uint32_t ManifestResourceCount;
    ManifestResource* ManifestResources;
    uint32_t NestedClassCount;
    NestedClass* NestedClasses;
    uint32_t GenericParameterCount;
    GenericParameter* GenericParameters;
    uint32_t GenericParameterConstraintCount;
    GenericParameterConstraint* GenericParameterConstraints;
    uint32_t MethodSpecificationCount;
    MethodSpecification* MethodSpecifications;
} CLIFile;

CLIFile* CLIFile_Create(PEFile* pFile);
void CLIFile_Destroy(CLIFile* pFile);
const char* CLIFile_GetString(CLIFile* pFile, uint32_t pVirtualAddress);
const uint8_t* CLIFile_GetUserString(CLIFile* pFile, uint32_t pVirtualAddress);
const uint8_t* CLIFile_GetBlob(CLIFile* pFile, uint32_t pVirtualAddress);
const uint8_t* CLIFile_GetGUID(CLIFile* pFile, uint32_t pVirtualAddress);
MetaDataToken CLIFile_ResolveToken(CLIFile* pFile, uint32_t pToken);

#include <CLR/MetaData/ModuleDefinition.h>
#include <CLR/MetaData/TypeReference.h>
#include <CLR/MetaData/TypeDefinition.h>
#include <CLR/MetaData/Field.h>
#include <CLR/MetaData/MethodDefinition.h>
#include <CLR/MetaData/Parameter.h>
#include <CLR/MetaData/InterfaceImplementation.h>
#include <CLR/MetaData/MemberReference.h>
#include <CLR/MetaData/Constant.h>
#include <CLR/MetaData/CustomAttribute.h>
#include <CLR/MetaData/FieldMarshal.h>
#include <CLR/MetaData/DeclSecurity.h>
#include <CLR/MetaData/ClassLayout.h>
#include <CLR/MetaData/FieldLayout.h>
#include <CLR/MetaData/StandAloneSignature.h>
#include <CLR/MetaData/EventMap.h>
#include <CLR/MetaData/Event.h>
#include <CLR/MetaData/PropertyMap.h>
#include <CLR/MetaData/Property.h>
#include <CLR/MetaData/MethodSemantics.h>
#include <CLR/MetaData/MethodImplementation.h>
#include <CLR/MetaData/ModuleReference.h>
#include <CLR/MetaData/TypeSpecification.h>
#include <CLR/MetaData/ImplementationMap.h>
#include <CLR/MetaData/FieldRVA.h>
#include <CLR/MetaData/AssemblyDefinition.h>
#include <CLR/MetaData/AssemblyProcessor.h>
#include <CLR/MetaData/AssemblyOperatingSystem.h>
#include <CLR/MetaData/AssemblyReference.h>
#include <CLR/MetaData/AssemblyReferenceProcessor.h>
#include <CLR/MetaData/AssemblyReferenceOperatingSystem.h>
#include <CLR/MetaData/File.h>
#include <CLR/MetaData/ExportedType.h>
#include <CLR/MetaData/ManifestResource.h>
#include <CLR/MetaData/NestedClass.h>
#include <CLR/MetaData/GenericParameter.h>
#include <CLR/MetaData/MethodSpecification.h>
#include <CLR/MetaData/GenericParameterConstraint.h>

void Panic(const char* pMessage);
