#pragma once

typedef struct _PEDOSHeader PEDOSHeader;
typedef struct _PENTHeader PENTHeader;
typedef struct _PEDataDirectory PEDataDirectory;
typedef struct _PEOptionalHeader PEOptionalHeader;
typedef struct _PESectionHeader PESectionHeader;
typedef struct _PECLIHeader PECLIHeader;
typedef struct _PECLIMetadataTablesHeader PECLIMetadataTablesHeader;
typedef struct _CLIFile CLIFile;
typedef struct _MetadataToken MetadataToken;

#include <CLR/IRStructures.h>
#include <CLR/MetadataStructures.h>

#define CLIFILE__PE__DOS_Signature								0x5A4D
#define CLIFILE__PE__NT_Signature								0x00004550

#define CLIFILE__Machine										0x014C

#define CLIFILE__Metadata__Signature							0x424A5342

#define CLIFILE__Metadata__HeaderOffset_Signature				0x00
#define CLIFILE__Metadata__HeaderOffset_MajorVersion			0x04
#define CLIFILE__Metadata__HeaderOffset_MinorVersion			0x06
#define CLIFILE__Metadata__HeaderOffset_Reserved				0x08
#define CLIFILE__Metadata__HeaderOffset_VersionLength			0x0C
#define CLIFILE__Metadata__HeaderOffset_Version					0x10

#define CLIFILE__Metadata__HeaderVersionOffset_Flags			0x00
#define CLIFILE__Metadata__HeaderVersionOffset_StreamCount		0x02
#define CLIFILE__Metadata__HeaderVersionOffset_Streams			0x04

#define CLIFILE__Metadata__StreamOffset_RVA						0x00
#define CLIFILE__Metadata__StreamOffset_Size					0x04
#define CLIFILE__Metadata__StreamOffset_Name					0x08


struct _PEDOSHeader
{
    uint16_t Signature;
    uint16_t LastPageByteCount;
    uint16_t PageCount;
    uint16_t RelocationCount;
    uint16_t HeaderParagraphCount;
    uint16_t MinimumParagraphCount;
    uint16_t MaximumParagraphCount;
    uint16_t RegisterSS;
    uint16_t RegisterSP;
    uint16_t Checksum;
    uint16_t RegisterIP;
    uint16_t RegisterCS;
    uint16_t RelocationOffset;
    uint16_t Overlay;
    uint16_t Reserved[4];
    uint16_t OEMIdentifier;
    uint16_t OEMInformation;
    uint16_t MoreReserved[10];
    uint32_t NextHeaderOffset;
};

struct _PENTHeader
{
    uint32_t Signature;
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t Timestamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
};

struct _PEDataDirectory
{
    uint32_t VirtualAddress;
    uint32_t Size;
};

struct _PEOptionalHeader
{
    uint16_t Magic;
    uint8_t MajorLinkerVersion;
    uint8_t MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;
    uint32_t BaseOfCode;
    uint32_t BaseOfData;

    uint32_t ImageBase;
    uint32_t SectionAlignment;
    uint32_t FileAlignment;
    uint16_t MajorOperatingSystemVersion;
    uint16_t MinorOperatingSystemVersion;
    uint16_t MajorImageVersion;
    uint16_t MinorImageVersion;
    uint16_t MajorSubsystemVersion;
    uint16_t MinorSubsystemVersion;
    uint32_t Reserved;
    uint32_t SizeOfImage;
    uint32_t SizeOfHeaders;
    uint32_t Checksum;
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
    uint32_t SizeOfStackReserve;
    uint32_t SizeOfStackCommit;
    uint32_t SizeOfHeapReserve;
    uint32_t SizeOfHeapCommit;
    uint32_t LoaderFlags;
    uint32_t NumberOfRvaAndSizes;
    PEDataDirectory DataDirectory[16];
};

struct _PESectionHeader
{
    uint8_t Name[8];
    union
    {
        uint32_t PhysicalAddress;
        uint32_t VirtualSize;
    } Misc;
    uint32_t VirtualAddress;
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;
    uint32_t PointerToRelocations;
    uint32_t PointerToLineNumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLineNumbers;
    uint32_t Characteristics;
};

struct _PECLIHeader
{
    uint32_t SizeOfHeader;
    uint16_t MajorRuntimeVersion;
    uint16_t MinorRuntimeVersion;
    PEDataDirectory Metadata;
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
};

struct _PECLIMetadataTablesHeader
{
    uint32_t Reserved;
    uint8_t MajorVersion;
    uint8_t MinorVersion;
    uint8_t HeapOffsetSizes;
    uint8_t MoreReserved;
    uint64_t PresentTables;
    uint64_t SortedTables;
};

struct _CLIFile
{
    uint8_t* Data;
    uint32_t Length;
	char* Filename;
    PEDOSHeader* DOSHeader;
    PENTHeader* NTHeader;
    PEOptionalHeader* OptionalHeader;
    PESectionHeader* SectionHeaders;
	PECLIHeader* CLIHeader;
    uint8_t* MetadataHeader;
    uint8_t* Tables;
    uint8_t* StringsHeap;
    uint8_t* UserStringsHeap;
    uint8_t* BlobsHeap;
    uint8_t* GUIDsHeap;
    PECLIMetadataTablesHeader* TablesHeader;

	IRAssembly* Assembly;

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
};

struct _MetadataToken
{
    uint8_t Table;
    bool_t IsUserString;
    void* Data;
};


CLIFile* CLIFile_Create(uint8_t* pData, uint32_t pLength, const char* pFilename);
void CLIFile_Destroy(CLIFile* pFile);
PESectionHeader* CLIFile_GetSection(PESectionHeader* pSections, uint16_t pSectionCount, uint32_t pVirtualAddress);
uint8_t* CLIFile_GetCompressedUnsigned(uint8_t* pData, uint32_t* pValue);
uint8_t* CLIFile_GetCompressedSigned(uint8_t* pData, int32_t* pValue);
MetadataToken* CLIFile_ExpandMetadataToken(CLIFile* pFile, uint32_t pToken);
MetadataToken* CLIFile_ExpandTypeDefRefOrSpecToken(CLIFile* pFile, uint32_t pToken);
void CLIFile_DestroyMetadataToken(MetadataToken* pToken);
