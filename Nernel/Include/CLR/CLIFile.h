#pragma once

#include <CLR/PEFile.h>
#include <CLR/MetaData.h>

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

const uint8_t* CLIFile_LoadModuleDefinitions(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadTypeReferences(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadTypeDefinitions(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadFields(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadMethodDefinitions(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadParameters(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadInterfaceImplementations(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadMemberReferences(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadConstants(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadCustomAttributes(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadFieldMarshals(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadDeclSecurities(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadClassLayouts(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadFieldLayouts(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadStandAloneSignatures(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadEventMaps(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadEvents(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadPropertyMaps(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadProperties(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadMethodSemantics(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadMethodImplementations(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadModuleReferences(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadTypeSpecifications(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadImplementationMaps(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadFieldRVAs(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadAssemblyDefinitions(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadAssemblyProcessors(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadAssemblyOperatingSystems(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadAssemblyReferences(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadAssemblyReferenceProcessors(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadAssemblyReferenceOperatingSystems(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadFiles(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadExportedTypes(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadManifestResources(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadNestedClasses(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadGenericParameters(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadMethodSpecifications(CLIFile* pFile, const uint8_t* pTableData);
const uint8_t* CLIFile_LoadGenericParameterConstraints(CLIFile* pFile, const uint8_t* pTableData);
