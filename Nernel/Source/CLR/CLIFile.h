#pragma once

#include "Metadata.h"

namespace CLR
{
	class PEDOSHeader
	{
	public:
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

	class PENTHeader
	{
	public:
		uint32_t Signature;
		uint16_t Machine;
		uint16_t NumberOfSections;
		uint32_t Timestamp;
		uint32_t PointerToSymbolTable;
		uint32_t NumberOfSymbols;
		uint16_t SizeOfOptionalHeader;
		uint16_t Characteristics;
	};

	class PEDataDirectory
	{
	public:
		uint32_t VirtualAddress;
		uint32_t Size;
	};

	class PEOptionalHeader
	{
	public:
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

	class PESectionHeader
	{
	public:
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

	class PECLIHeader
	{
	public:
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

	class PECLIMetadataTablesHeader
	{
	public:
		uint32_t Reserved;
		uint8_t MajorVersion;
		uint8_t MinorVersion;
		uint8_t HeapOffsetSizes;
		uint8_t MoreReserved;
		uint64_t PresentTables;
		uint64_t SortedTables;
	};

	class Assembly;

	class CLIFile
	{
	public:
		static const uint16_t SIGNATURE_DOS = 0x5A4D;
		static const uint32_t SIGNATURE_NT = 0x00004550;
		static const uint16_t SIGNATURE_MACHINE = 0x014C;
		static const uint32_t SIGNATURE_METADATA = 0x424A5342;

		static const uint8_t METADATA_HEADEROFFSET_SIGNATURE = 0x00;
		static const uint8_t METADATA_HEADEROFFSET_MAJORVERSION = 0x04;
		static const uint8_t METADATA_HEADEROFFSET_MINORVERSION = 0x06;
		static const uint8_t METADATA_HEADEROFFSET_RESERVED = 0x08;
		static const uint8_t METADATA_HEADEROFFSET_VERSIONLENGTH = 0x0C;
		static const uint8_t METADATA_HEADEROFFSET_VERSION = 0x10;

		static const uint8_t METADATA_HEADERVERSIONOFFSET_FLAGS = 0x00;
		static const uint8_t METADATA_HEADERVERSIONOFFSET_STREAMCOUNT = 0x02;
		static const uint8_t METADATA_HEADERVERSIONOFFSET_STREAMS = 0x04;

		static const uint8_t METADATA_STREAMOFFSET_RVA = 0x00;
		static const uint8_t METADATA_STREAMOFFSET_SIZE = 0x04;
		static const uint8_t METADATA_STREAMOFFSET_NAME = 0x08;


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

		Assembly* LoadedAssembly;

		uint32_t ModuleDefinitionCount;
		ModuleDefinitionData* ModuleDefinitions;
		uint32_t TypeReferenceCount;
		TypeReferenceData* TypeReferences;
		uint32_t TypeDefinitionCount;
		TypeDefinitionData* TypeDefinitions;
		uint32_t FieldCount;
		FieldData* Fields;
		uint32_t MethodDefinitionCount;
		MethodDefinitionData* MethodDefinitions;
		uint32_t ParameterCount;
		ParameterData* Parameters;
		uint32_t InterfaceImplementationCount;
		InterfaceImplementationData* InterfaceImplementations;
		uint32_t MemberReferenceCount;
		MemberReferenceData* MemberReferences;
		uint32_t ConstantCount;
		ConstantData* Constants;
		uint32_t CustomAttributeCount;
		CustomAttributeData* CustomAttributes;
		uint32_t FieldMarshalCount;
		FieldMarshalData* FieldMarshals;
		uint32_t DeclSecurityCount;
		DeclSecurityData* DeclSecurities;
		uint32_t ClassLayoutCount;
		ClassLayoutData* ClassLayouts;
		uint32_t FieldLayoutCount;
		FieldLayoutData* FieldLayouts;
		uint32_t StandAloneSignatureCount;
		StandAloneSignatureData* StandAloneSignatures;
		uint32_t EventMapCount;
		EventMapData* EventMaps;
		uint32_t EventCount;
		EventData* Events;
		uint32_t PropertyMapCount;
		PropertyMapData* PropertyMaps;
		uint32_t PropertyCount;
		PropertyData* Properties;
		uint32_t MethodSemanticsCount;
		MethodSemanticsData* MethodSemantics;
		uint32_t MethodImplementationCount;
		MethodImplementationData* MethodImplementations;
		uint32_t ModuleReferenceCount;
		ModuleReferenceData* ModuleReferences;
		uint32_t TypeSpecificationCount;
		TypeSpecificationData* TypeSpecifications;
		uint32_t ImplementationMapCount;
		ImplementationMapData* ImplementationMaps;
		uint32_t FieldRVACount;
		FieldRVAData* FieldRVAs;
		uint32_t AssemblyDefinitionCount;
		AssemblyDefinitionData* AssemblyDefinitions;
		uint32_t AssemblyProcessorCount;
		AssemblyProcessorData* AssemblyProcessors;
		uint32_t AssemblyOperatingSystemCount;
		AssemblyOperatingSystemData* AssemblyOperatingSystems;
		uint32_t AssemblyReferenceCount;
		AssemblyReferenceData* AssemblyReferences;
		uint32_t AssemblyReferenceProcessorCount;
		AssemblyReferenceProcessorData* AssemblyReferenceProcessors;
		uint32_t AssemblyReferenceOperatingSystemCount;
		AssemblyReferenceOperatingSystemData* AssemblyReferenceOperatingSystems;
		uint32_t FileCount;
		FileData* Files;
		uint32_t ExportedTypeCount;
		ExportedTypeData* ExportedTypes;
		uint32_t ManifestResourceCount;
		ManifestResourceData* ManifestResources;
		uint32_t NestedClassCount;
		NestedClassData* NestedClasses;
		uint32_t GenericParameterCount;
		GenericParameterData* GenericParameters;
		uint32_t GenericParameterConstraintCount;
		GenericParameterConstraintData* GenericParameterConstraints;
		uint32_t MethodSpecificationCount;
		MethodSpecificationData* MethodSpecifications;

		CLIFile();
		~CLIFile();

		bool Load(uint8_t* pData, uint32_t pLength, const char* pFilename);
		static PESectionHeader* GetSection(PESectionHeader* pSections, uint16_t pSectionCount, uint32_t pVirtualAddress);
		static uint8_t* GetCompressedUnsigned(uint8_t* pData, uint32_t* pValue);
		static int32_t RotateRight(int32_t pValue, uint8_t pBits);
		static uint8_t* GetCompressedSigned(uint8_t* pData, int32_t* pValue);

		MetadataToken* ExpandTypeDefRefOrSpecToken(uint32_t pToken);
		MetadataToken* ExpandMetadataToken(uint32_t pToken);

	private:
		typedef uint8_t*(CLIFile::*Handler)(uint8_t*);
		class HandlerEntry
		{
		public:
			Handler Initializer;
			Handler Loader;
		};

		static HandlerEntry sHandlers[];

		uint8_t* ModuleDefinitionInitializer(uint8_t* pTableData);
		uint8_t* TypeReferenceInitializer(uint8_t* pTableData);
		uint8_t* TypeDefinitionInitializer(uint8_t* pTableData);
		uint8_t* FieldInitializer(uint8_t* pTableData);
		uint8_t* MethodDefinitionInitializer(uint8_t* pTableData);
		uint8_t* ParameterInitializer(uint8_t* pTableData);
		uint8_t* InterfaceImplementationInitializer(uint8_t* pTableData);
		uint8_t* MemberReferenceInitializer(uint8_t* pTableData);
		uint8_t* ConstantInitializer(uint8_t* pTableData);
		uint8_t* CustomAttributeInitializer(uint8_t* pTableData);
		uint8_t* FieldMarshalInitializer(uint8_t* pTableData);
		uint8_t* DeclSecurityInitializer(uint8_t* pTableData);
		uint8_t* ClassLayoutInitializer(uint8_t* pTableData);
		uint8_t* FieldLayoutInitializer(uint8_t* pTableData);
		uint8_t* StandAloneSignatureInitializer(uint8_t* pTableData);
		uint8_t* EventMapInitializer(uint8_t* pTableData);
		uint8_t* EventInitializer(uint8_t* pTableData);
		uint8_t* PropertyMapInitializer(uint8_t* pTableData);
		uint8_t* PropertyInitializer(uint8_t* pTableData);
		uint8_t* MethodSemanticsInitializer(uint8_t* pTableData);
		uint8_t* MethodImplementationInitializer(uint8_t* pTableData);
		uint8_t* ModuleReferenceInitializer(uint8_t* pTableData);
		uint8_t* TypeSpecificationInitializer(uint8_t* pTableData);
		uint8_t* ImplementationMapInitializer(uint8_t* pTableData);
		uint8_t* FieldRVAInitializer(uint8_t* pTableData);
		uint8_t* AssemblyDefinitionInitializer(uint8_t* pTableData);
		uint8_t* AssemblyProcessorInitializer(uint8_t* pTableData);
		uint8_t* AssemblyOperatingSystemInitializer(uint8_t* pTableData);
		uint8_t* AssemblyReferenceInitializer(uint8_t* pTableData);
		uint8_t* AssemblyReferenceProcessorInitializer(uint8_t* pTableData);
		uint8_t* AssemblyReferenceOperatingSystemInitializer(uint8_t* pTableData);
		uint8_t* FileInitializer(uint8_t* pTableData);
		uint8_t* ExportedTypeInitializer(uint8_t* pTableData);
		uint8_t* ManifestResourceInitializer(uint8_t* pTableData);
		uint8_t* NestedClassInitializer(uint8_t* pTableData);
		uint8_t* GenericParameterInitializer(uint8_t* pTableData);
		uint8_t* MethodSpecificationInitializer(uint8_t* pTableData);
		uint8_t* GenericParameterConstraintInitializer(uint8_t* pTableData);

		uint8_t* ModuleDefinitionLoader(uint8_t* pTableData);
		uint8_t* TypeReferenceLoader(uint8_t* pTableData);
		uint8_t* TypeDefinitionLoader(uint8_t* pTableData);
		uint8_t* FieldLoader(uint8_t* pTableData);
		uint8_t* MethodDefinitionLoader(uint8_t* pTableData);
		uint8_t* ParameterLoader(uint8_t* pTableData);
		uint8_t* InterfaceImplementationLoader(uint8_t* pTableData);
		uint8_t* MemberReferenceLoader(uint8_t* pTableData);
		uint8_t* ConstantLoader(uint8_t* pTableData);
		uint8_t* CustomAttributeLoader(uint8_t* pTableData);
		uint8_t* FieldMarshalLoader(uint8_t* pTableData);
		uint8_t* DeclSecurityLoader(uint8_t* pTableData);
		uint8_t* ClassLayoutLoader(uint8_t* pTableData);
		uint8_t* FieldLayoutLoader(uint8_t* pTableData);
		uint8_t* StandAloneSignatureLoader(uint8_t* pTableData);
		uint8_t* EventMapLoader(uint8_t* pTableData);
		uint8_t* EventLoader(uint8_t* pTableData);
		uint8_t* PropertyMapLoader(uint8_t* pTableData);
		uint8_t* PropertyLoader(uint8_t* pTableData);
		uint8_t* MethodSemanticsLoader(uint8_t* pTableData);
		uint8_t* MethodImplementationLoader(uint8_t* pTableData);
		uint8_t* ModuleReferenceLoader(uint8_t* pTableData);
		uint8_t* TypeSpecificationLoader(uint8_t* pTableData);
		uint8_t* ImplementationMapLoader(uint8_t* pTableData);
		uint8_t* FieldRVALoader(uint8_t* pTableData);
		uint8_t* AssemblyDefinitionLoader(uint8_t* pTableData);
		uint8_t* AssemblyProcessorLoader(uint8_t* pTableData);
		uint8_t* AssemblyOperatingSystemLoader(uint8_t* pTableData);
		uint8_t* AssemblyReferenceLoader(uint8_t* pTableData);
		uint8_t* AssemblyReferenceProcessorLoader(uint8_t* pTableData);
		uint8_t* AssemblyReferenceOperatingSystemLoader(uint8_t* pTableData);
		uint8_t* FileLoader(uint8_t* pTableData);
		uint8_t* ExportedTypeLoader(uint8_t* pTableData);
		uint8_t* ManifestResourceLoader(uint8_t* pTableData);
		uint8_t* NestedClassLoader(uint8_t* pTableData);
		uint8_t* GenericParameterLoader(uint8_t* pTableData);
		uint8_t* MethodSpecificationLoader(uint8_t* pTableData);
		uint8_t* GenericParameterConstraintLoader(uint8_t* pTableData);

		void Link();
	};
};