#pragma once

namespace CLR
{
	class CLIFile;

	class AssemblyDefinitionData;
	class AssemblyOperatingSystemData;
	class AssemblyProcessorData;
	class AssemblyReferenceData;
	class AssemblyReferenceOperatingSystemData;
	class AssemblyReferenceProcessorData;
	class ClassLayoutData;
	class ConstantData;
	class CustomAttributeData;
	class DeclSecurityData;
	class EventData;
	class EventMapData;
	class ExportedTypeData;
	class FieldData;
	class FieldLayoutData;
	class FieldMarshalData;
	class FieldRVAData;
	class FileData;
	class GenericParameterData;
	class GenericParameterConstraintData;
	class ImplementationMapData;
	class InterfaceImplementationData;
	class ManifestResourceData;
	class MemberReferenceData;
	class MethodDefinitionData;
	class MethodImplementationData;
	class MethodSemanticsData;
	class MethodSpecificationData;
	class ModuleDefinitionData;
	class ModuleReferenceData;
	class NestedClassData;
	class ParameterData;
	class PropertyData;
	class PropertyMapData;
	class StandAloneSignatureData;
	class TypeDefinitionData;
	class TypeReferenceData;
	class TypeSpecificationData;

	class MethodSignature;
	class FieldSignature;
	class PropertySignature;
	class LocalsSignature;

	class SignatureReturnType;
	class SignatureParameter;
	class SignatureCustomModifier;
	class SignatureType;
	class SignatureMethodSpecification;
	class SignatureArrayShape;
	class SignatureLocalVariable;

	namespace MetadataTable
	{
		typedef enum MetadataTable
		{
			ModuleDefinition						= 0x00,
			TypeReference							= 0x01,
			TypeDefinition							= 0x02,
			Field									= 0x04,
			MethodDefinition						= 0x06,
			Parameter								= 0x08,
			InterfaceImplementation					= 0x09,
			MemberReference							= 0x0A,
			Constant								= 0x0B,
			CustomAttribute							= 0x0C,
			FieldMarshal							= 0x0D,
			DeclSecurity							= 0x0E,
			ClassLayout								= 0x0F,
			FieldLayout								= 0x10,
			StandAloneSignature						= 0x11,
			EventMap								= 0x12,
			Event									= 0x14,
			PropertyMap								= 0x15,
			Property								= 0x17,
			MethodSemantics							= 0x18,
			MethodImplementation					= 0x19,
			ModuleReference							= 0x1A,
			TypeSpecification						= 0x1B,
			ImplementationMap						= 0x1C,
			FieldRVA								= 0x1D,
			AssemblyDefinition						= 0x20,
			AssemblyProcessor						= 0x21,
			AssemblyOperatingSystem					= 0x22,
			AssemblyReference						= 0x23,
			AssemblyReferenceProcessor				= 0x24,
			AssemblyReferenceOperatingSystem		= 0x25,
			File									= 0x26,
			ExportedType							= 0x27,
			ManifestResource						= 0x28,
			NestedClass								= 0x29,
			GenericParameter						= 0x2A,
			MethodSpecification						= 0x2B,
			GenericParameterConstraint				= 0x2C,

			UserStrings								= 0x70,
		} MetadataTable;
	};

	namespace HeapOffsetSize
	{
		typedef enum HeapOffsetSize
		{
			Strings32Bit = 0x01,
			GUIDs32Bit = 0x02,
			Blobs32Bit = 0x04,
		} HeapOffsetSize;
	};

	namespace TypeDefRefOrSpecType
	{
		typedef enum TypeDefRefOrSpecType
		{
			TypeDefinition							= 0x00,
			TypeReference							= 0x01,
			TypeSpecification						= 0x02,
		} TypeDefRefOrSpecType;
		const uint32_t BITS = 2;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace HasConstantType
	{
		typedef enum HasConstantType
		{
			Field									= 0x00,
			Parameter								= 0x01,
			Property								= 0x02,
		} HasConstantType;
		const uint32_t BITS = 2;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace HasCustomAttributeType
	{
		typedef enum HasCustomAttributeType
		{
			MethodDefinition						= 0x00,
			Field									= 0x01,
			TypeReference							= 0x02,
			TypeDefinition							= 0x03,
			Parameter								= 0x04,
			InterfaceImplementation					= 0x05,
			MemberReference							= 0x06,
			ModuleDefinition						= 0x07,
			DeclSecurity							= 0x08,
			Property								= 0x09,
			Event									= 0x0A,
			StandAloneSignature						= 0x0B,
			ModuleReference							= 0x0C,
			TypeSpecification						= 0x0D,
			AssemblyDefinition						= 0x0E,
			AssemblyReference						= 0x0F,
			File									= 0x10,
			ExportedType							= 0x11,
			ManifestResource						= 0x12,
			GenericParameter						= 0x13,
			GenericParameterConstraint				= 0x14,
			MethodSpecification						= 0x15,
		} HasCustomAttributeType;
		const uint32_t BITS = 5;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace HasFieldMarshalType
	{
		typedef enum HasFieldMarshalType
		{
			Field									= 0x00,
			Parameter								= 0x01,
		} HasFieldMarshalType;
		const uint32_t BITS = 1;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace HasDeclSecurityType
	{
		typedef enum HasDeclSecurityType
		{
			TypeDefinition							= 0x00,
			MethodDefinition						= 0x01,
			AssemblyDefinition						= 0x02,
		} HasDeclSecurityType;
		const uint32_t BITS = 2;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace MemberRefParentType
	{
		typedef enum MemberRefParentType
		{
			TypeDefinition							= 0x00,
			TypeReference							= 0x01,
			ModuleReference							= 0x02,
			MethodDefinition						= 0x03,
			TypeSpecification						= 0x04,
		} MemberRefParentType;
		const uint32_t BITS = 3;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace HasSemanticsType
	{
		typedef enum HasSemanticsType
		{
			Event									= 0x00,
			Property								= 0x01,
		} HasSemanticsType;
		const uint32_t BITS = 1;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace MethodDefOrRefType
	{
		typedef enum MethodDefOrRefType
		{
			MethodDefinition						= 0x00,
			MemberReference							= 0x01,
		} MethodDefOrRefType;
		const uint32_t BITS = 1;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace MemberForwardedType
	{
		typedef enum MemberForwardedType
		{
			Field									= 0x00,
			MethodDefinition						= 0x01,
		} MemberForwardedType;
		const uint32_t BITS = 1;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace ImplementationType
	{
		typedef enum ImplementationType
		{
			File									= 0x00,
			AssemblyReference						= 0x01,
			ExportedType							= 0x02,
		} ImplementationType;
		const uint32_t BITS = 2;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace CustomAttributeType
	{
		typedef enum CustomAttributeType
		{
			MethodDefinition						= 0x02,
			MemberReference							= 0x03,
		} CustomAttributeType;
		const uint32_t BITS = 3;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace ResolutionScopeType
	{
		typedef enum ResolutionScopeType
		{
			ModuleDefinition						= 0x00,
			ModuleReference							= 0x01,
			AssemblyReference						= 0x02,
			TypeReference							= 0x03,
		} ResolutionScopeType;
		const uint32_t BITS = 2;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace TypeOrMethodDefType
	{
		typedef enum TypeOrMethodDefType
		{
			TypeDefinition							= 0x00,
			MethodDefinition						= 0x01,
		} TypeOrMethodDefType;
		const uint32_t BITS = 1;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};

	namespace FieldOrMethodDefType
	{
		typedef enum FieldOrMethodDefType
		{
			Field									= 0x00,
			MethodDefinition						= 0x01,
		} FieldOrMethodDefType;
		const uint32_t BITS = 1;
		const uint32_t MASK = ((1 << BITS) - 1);
		const uint16_t MAXROWS16BIT = (0xFFFF >> BITS);
	};


	namespace AssemblyHashAlgorithm
	{
		typedef enum AssemblyHashAlgorithm
		{
			None									= 0x0000,
			MD5										= 0x8003,
			SHA1									= 0x8004,
		} AssemblyHashAlgorithm;
	};

	namespace AssemblyFlags
	{
		typedef enum AssemblyFlags
		{
			PublicKey								= 0x0001,
			Retargetable							= 0x0100,
			DisableJITCompileOptimizer				= 0x4000,
			DisableJITCompileTracking				= 0x8000,
		} AssemblyFlags;
	};

	namespace EventAttributes
	{
		typedef enum EventAttributes
		{
			SpecialName								= 0x0200,
			RTSpecialName							= 0x0400,
		} EventAttributes;
	};

	namespace FieldAttributes
	{
		typedef enum FieldAttributes
		{
			FieldAccessMask							= 0x0007,
			//CompilerControlled					= 0x0000,
			Private									= 0x0001,
			FamilyAndAssembly						= 0x0002,
			Assembly								= 0x0003,
			Family									= 0x0004,
			FamilyOrAssembly						= 0x0005,
			Public									= 0x0006,
			Static									= 0x0010,
			InitOnly								= 0x0020,
			Literal									= 0x0040,
			NotSerialized							= 0x0080,
			SpecialName								= 0x0200,
			PInvokeImplementation					= 0x2000,
			RTSpecialName							= 0x0400,
			HasFieldMarshal							= 0x1000,
			HasDefault								= 0x8000,
			HasFieldRVA								= 0x0100,
		} FieldAttributes;
	};

	namespace FileAttributes
	{
		typedef enum FileAttributes
		{
			//ContainsMetadata						= 0x0000
			ContainsNoMetadata						= 0x0001,
		} FileAttributes;
	};

	namespace GenericParameterAttributes
	{
		typedef enum GenericParameterAttributes
		{
			VarianceMask							= 0x0003,
			//None									= 0x0000,
			Covariant								= 0x0001,
			Contravariant							= 0x0002,
			SpecialConstraintMask					= 0x001C,
			ReferenceTypeConstraint					= 0x0004,
			NotNullableValueTypeConstraint			= 0x0008,
			DefaultConstructorConstraint			= 0x0010,
		} GenericParameterAttributes;
	};

	namespace PInvokeAttributes
	{
		typedef enum PInvokeAttributes
		{
			NoMangle								= 0x0001,
			CharacterSetMask						= 0x0006,
			//CharacterSetNotSpecified				= 0x0000,
			CharacterSetAnsi						= 0x0002,
			CharacterSetUnicode						= 0x0004,
			CharacterSetAuto						= 0x0006,
			SupportsLastError						= 0x0040,
			CallConventionMask						= 0x0700,
			CallConventionPlatformAPI				= 0x0100,
			CallConventionCDecl						= 0x0200,
			CallConventionSTD						= 0x0300,
			CallConventionThis						= 0x0400,
			CallConventionFast						= 0x0500,
		} PInvokeAttributes;
	};

	namespace ManifestResourceAttributes
	{
		typedef enum ManifestResourceAttributes
		{
			VisibilityMask							= 0x0007,
			Public									= 0x0001,
			Private									= 0x0002,
		} ManifestResourceAttributes;
	};

	namespace MethodAttributes
	{
		typedef enum MethodAttributes
		{
			MemberAccessMask						= 0x0007,
			//CompilerControlled					= 0x0000,
			Private									= 0x0001,
			FamilyAndAssembly						= 0x0002,
			Assembly								= 0x0003,
			Family									= 0x0004,
			FamilyOrAssembly						= 0x0005,
			Public									= 0x0006,
			Static									= 0x0010,
			Final									= 0x0020,
			Virtual									= 0x0040,
			HideBySignature							= 0x0080,
			VTableLayoutMask						= 0x0100,
			//ReuseSlot								= 0x0000,
			NewSlot									= 0x0100,
			Strict									= 0x0200,
			Abstract								= 0x0400,
			SpecialName								= 0x0800,
			PInvokeImplementation					= 0x2000,
			UnmanagedExport							= 0x0008,
			RTSpecialName							= 0x1000,
			HasSecurity								= 0x4000,
			RequireSecurityObject					= 0x8000,
		} MethodAttributes;
	};

	namespace MethodImplAttributes
	{
		typedef enum MethodImplAttributes
		{
			CodeTypeMask							= 0x0003,
			//IL									= 0x0000,
			Native									= 0x0001,
			OPTIL									= 0x0002,
			Runtime									= 0x0003,
			ManagedMask								= 0x0004,
			Unmanaged								= 0x0004,
			//Managed								= 0x0000,
			ForwardReference						= 0x0010,
			PreserveSignature						= 0x0080,
			InternalCall							= 0x1000,
			Synchronized							= 0x0020,
			NoInlining								= 0x0008,
			MaxMethodImplementationValue			= 0xFFFF,
			NoOptimization							= 0x0040,
		} MethodImplAttributes;
	};

	namespace MethodSemanticsAttributes
	{
		typedef enum MethodSemanticsAttributes
		{
			Setter									= 0x0001,
			Getter									= 0x0002,
			Other									= 0x0004,
			AddOn									= 0x0008,
			RemoveOn								= 0x0010,
			Fire									= 0x0020,
		} MethodSemanticsAttributes;
	};

	namespace ParameterAttributes
	{
		typedef enum ParameterAttributes
		{
			In										= 0x0001,
			Out										= 0x0002,
			Optional								= 0x0010,
			HasDefault								= 0x1000,
			HasFieldMarshal							= 0x2000,
			Unused									= 0xCFE0,
		} ParameterAttributes;
	};

	namespace PropertyAttributes
	{
		typedef enum PropertyAttributes
		{
			SpecialName								= 0x0200,
			RTSpecialName							= 0x0400,
			HasDefault								= 0x1000,
			Unused									= 0xE9FF,
		} PropertyAttributes;
	};

	namespace TypeAttributes
	{
		typedef enum TypeAttributes
		{
			VisibilityMask							= 0x00000007,
			NotPublic								= 0x00000000,
			Public									= 0x00000001,
			NestedPublic							= 0x00000002,
			NestedPrivate							= 0x00000003,
			NestedFamily							= 0x00000004,
			NestedAssembly							= 0x00000005,
			NestedFamilyAndAssembly					= 0x00000006,
			NestedFamilyOrAssembly					= 0x00000007,
			LayoutMask								= 0x00000018,
			//AutoLayout							= 0x00000000,
			SequentialLayout						= 0x00000008,
			ExplicitLayout							= 0x00000010,
			ClassSemanticsMask						= 0x00000020,
			//Class									= 0x00000000,
			Interface								= 0x00000020,
			Abstract								= 0x00000080,
			Sealed									= 0x00000100,
			SpecialName								= 0x00000400,
			Import									= 0x00001000,
			Serializable							= 0x00002000,
			StringFormatMask						= 0x00030000,
			//AnsiClass								= 0x00000000,
			UnicodeClass							= 0x00010000,
			AutoClass								= 0x00020000,
			CustomFormatClass						= 0x00030000,
			CustomStringFormatMask					= 0x00C00000,
			BeforeFieldInit							= 0x00100000,
			RTSpecialName							= 0x00000800,
			HasSecurity								= 0x00040000,
			IsTypeForwarder							= 0x00200000,
		} TypeAttributes;
	};

	#define TypeDefOrRefUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		TypeDefinitionData* TypeDefinition; \
		TypeReferenceData* TypeReference; \
		TypeSpecificationData* TypeSpecification; \
	} n;

	#define HasConstantUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		FieldData* Field; \
		ParameterData* Parameter; \
		PropertyData* Property; \
	} n;

	#define HasCustomAttributeUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		MethodDefinitionData* MethodDefinition; \
		FieldData* Field; \
		TypeReferenceData* TypeReference; \
		TypeDefinitionData* TypeDefinition; \
		ParameterData* Parameter; \
		InterfaceImplementationData* InterfaceImplementation; \
		MemberReferenceData* MemberReference; \
		ModuleDefinitionData* ModuleDefinition; \
		DeclSecurityData* DeclSecurity; \
		PropertyData* Property; \
		EventData* Event; \
		StandAloneSignatureData* StandAloneSignature; \
		ModuleReferenceData* ModuleReference; \
		TypeSpecificationData* TypeSpecification; \
		AssemblyDefinitionData* AssemblyDefinition; \
		AssemblyReferenceData* AssemblyReference; \
		FileData* File; \
		ExportedTypeData* ExportedType; \
		ManifestResourceData* ManifestResource; \
		GenericParameterData* GenericParameter; \
		GenericParameterConstraintData* GenericParameterConstraint; \
		MethodSpecificationData* MethodSpecification; \
	} n;

	#define HasFieldMarshalUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		FieldData* Field; \
		ParameterData* Parameter; \
	} n;

	#define HasDeclSecurityUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		TypeDefinitionData* TypeDefinition; \
		MethodDefinitionData* MethodDefinition; \
		AssemblyDefinitionData* AssemblyDefinition; \
	} n;

	#define MemberRefParentUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		TypeDefinitionData* TypeDefinition; \
		TypeReferenceData* TypeReference; \
		ModuleReferenceData* ModuleReference; \
		MethodDefinitionData* MethodDefinition; \
		TypeSpecificationData* TypeSpecification; \
	} n;

	#define HasSemanticsUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		EventData* Event; \
		PropertyData* Property; \
	} n;

	#define MethodDefOrRefUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		MethodDefinitionData* MethodDefinition; \
		MemberReferenceData* MemberReference; \
	} n;

	#define MemberForwardedUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		FieldData* Field; \
		MethodDefinitionData* MethodDefinition; \
	} n;

	#define ImplementationUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		FileData* File; \
		AssemblyReferenceData* AssemblyReference; \
		ExportedTypeData* ExportedType; \
	} n;

	#define CustomAttributeTypeUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		MethodDefinitionData* MethodDefinition; \
		MemberReferenceData* MemberReference; \
	} n;

	#define ResolutionScopeUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		ModuleDefinitionData* ModuleDefinition; \
		ModuleReferenceData* ModuleReference; \
		AssemblyReferenceData* AssemblyReference; \
		TypeReferenceData* TypeReference; \
	} n;

	#define TypeOrMethodDefUnion(n) \
	uint8_t TypeOf##n; \
	union \
	{ \
		TypeDefinitionData* TypeDefinition; \
		MethodDefinitionData* MethodDefinition; \
	} n;


	class AssemblyDefinitionData
	{
	public:
		uint32_t TableIndex;
		uint32_t HashAlgorithmID;
		uint16_t MajorVersion;
		uint16_t MinorVersion;
		uint16_t Build;
		uint16_t Revision;
		uint32_t Flags;
		uint32_t PublicKeyLength;
		const uint8_t* PublicKey;
		const char* Name;
		const char* Culture;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;
		DeclSecurityData* DeclSecurity;

		AssemblyDefinitionData();
		~AssemblyDefinitionData();
	};

	class AssemblyOperatingSystemData
	{
	public:
		uint32_t TableIndex;
		uint32_t PlatformID;
		uint32_t MajorVersion;
		uint32_t MinorVersion;

		AssemblyOperatingSystemData();
		~AssemblyOperatingSystemData();
	};

	class AssemblyProcessorData
	{
	public:
		uint32_t TableIndex;
		uint32_t Processor;

		AssemblyProcessorData();
		~AssemblyProcessorData();
	};

	class AssemblyReferenceData
	{
	public:
		uint32_t TableIndex;
		uint16_t MajorVersion;
		uint16_t MinorVersion;
		uint16_t Build;
		uint16_t Revision;
		uint32_t Flags;
		uint32_t PublicKeyOrTokenLength;
		const uint8_t* PublicKeyOrToken;
		const char* Name;
		const char* Culture;
		uint32_t HashValueLength;
		const uint8_t* HashValue;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;

		AssemblyReferenceData();
		~AssemblyReferenceData();
	};

	class AssemblyReferenceOperatingSystemData
	{
	public:
		uint32_t TableIndex;
		uint32_t PlatformID;
		uint32_t MajorVersion;
		uint32_t MinorVersion;
		AssemblyReferenceData* AssemblyReference;

		AssemblyReferenceOperatingSystemData();
		~AssemblyReferenceOperatingSystemData();
	};

	class AssemblyReferenceProcessorData
	{
	public:
		uint32_t TableIndex;
		uint32_t Processor;
		AssemblyReferenceData* AssemblyReference;

		AssemblyReferenceProcessorData();
		~AssemblyReferenceProcessorData();
	};

	class ClassLayoutData
	{
	public:
		uint32_t TableIndex;
		uint16_t PackingSize;
		uint32_t ClassSize;
		TypeDefinitionData* Parent;

		ClassLayoutData();
		~ClassLayoutData();
	};

	class ConstantData
	{
	public:
		uint32_t TableIndex;
		uint8_t Type;
		HasConstantUnion(Parent)
		uint32_t ValueLength;
		const uint8_t* Value;

		ConstantData();
		~ConstantData();
	};

	class CustomAttributeData
	{
	public:
		uint32_t TableIndex;
		HasCustomAttributeUnion(Parent)
		CustomAttributeTypeUnion(Type)
		uint32_t ValueLength;
		const uint8_t* Value;

		CustomAttributeData();
		~CustomAttributeData();
	};

	class DeclSecurityData
	{
	public:
		uint32_t TableIndex;
		uint16_t Action;
		HasDeclSecurityUnion(Parent)
		uint32_t PermissionSetLength;
		const uint8_t* PermissionSet;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;

		DeclSecurityData();
		~DeclSecurityData();
	};

	class EventData
	{
	public:
		uint32_t TableIndex;
		uint16_t Flags;
		const char* Name;
		TypeDefOrRefUnion(EventType)

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;
		MethodSemanticsData* MethodSemantics;

		EventData();
		~EventData();
	};

	class EventMapData
	{
	public:
		uint32_t TableIndex;
		TypeDefinitionData* Parent;
		EventData* EventList;
		uint32_t EventListCount;

		EventMapData();
		~EventMapData();
	};

	class ExportedTypeData
	{
	public:
		uint32_t TableIndex;
		uint32_t Flags;
		TypeDefinitionData* TypeDefinitionID;
		const char* Name;
		const char* Namespace;
		ImplementationUnion(Implementation)

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;

		ExportedTypeData();
		~ExportedTypeData();
	};

	class FieldData
	{
	public:
		CLIFile* File;
		uint32_t TableIndex;
		uint16_t Flags;
		const char* Name;
		uint32_t SignatureLength;
		uint8_t* Signature;
		FieldSignature* SignatureCache;

		ConstantData* Constant;
		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;
		FieldMarshalData* FieldMarshal;
		ImplementationMapData* ImplementationMap;
		TypeDefinitionData* TypeDefinition;

		FieldData();
		~FieldData();
	};

	class FieldLayoutData
	{
	public:
		uint32_t TableIndex;
		uint32_t Offset;
		FieldData* Field;

		FieldLayoutData();
		~FieldLayoutData();
	};

	class FieldMarshalData
	{
	public:
		uint32_t TableIndex;
		HasFieldMarshalUnion(Parent)
		uint32_t NativeTypeLength;
		const uint8_t* NativeType;

		FieldMarshalData();
		~FieldMarshalData();
	};

	class FieldRVAData
	{
	public:
		uint32_t TableIndex;
		const uint8_t* InitialValue;
		FieldData* Field;

		FieldRVAData();
		~FieldRVAData();
	};

	class FileData
	{
	public:
		uint32_t TableIndex;
		uint32_t Flags;
		const char* Name;
		uint32_t HashValueLength;
		const uint8_t* HashValue;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;

		FileData();
		~FileData();
	};

	class GenericParameterData
	{
	public:
		uint32_t TableIndex;
		uint16_t Index;
		uint16_t Flags;
		TypeOrMethodDefUnion(Owner)
		const char* Name;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;
		uint32_t GenericParameterConstraintCount;
		GenericParameterConstraintData** GenericParameterConstraints;

		GenericParameterData();
		~GenericParameterData();
	};

	class GenericParameterConstraintData
	{
	public:
		uint32_t TableIndex;
		GenericParameterData* Owner;
		TypeDefOrRefUnion(Constraint)

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;

		GenericParameterConstraintData();
		~GenericParameterConstraintData();
	};

	class ImplementationMapData
	{
	public:
		uint32_t TableIndex;
		uint16_t MappingFlags;
		MemberForwardedUnion(MemberForwarded)
		const char* ImportName;
		ModuleReferenceData* ImportScope;

		ImplementationMapData();
		~ImplementationMapData();
	};

	class InterfaceImplementationData
	{
	public:
		uint32_t TableIndex;
		TypeDefinitionData* Implementor;
		TypeDefOrRefUnion(Interface)

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;

		InterfaceImplementationData();
		~InterfaceImplementationData();
	};

	class ManifestResourceData
	{
	public:
		uint32_t TableIndex;
		uint32_t Offset;
		uint32_t Flags;
		const char* Name;
		ImplementationUnion(Implementation)

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;

		ManifestResourceData();
		~ManifestResourceData();
	};

	class MemberReferenceData
	{
	public:
		CLIFile* File;
		uint32_t TableIndex;
		MemberRefParentUnion(Parent)
		const char* Name;
		uint32_t SignatureLength;
		uint8_t* Signature;
		MethodSignature* MethodSignatureCache;
		FieldSignature* FieldSignatureCache;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;

		uint8_t TypeOfResolved;
		union
		{
			MethodDefinitionData* MethodDefinition;
			FieldData* Field;
		} Resolved;

		MemberReferenceData();
		~MemberReferenceData();
	};

	class MethodDefinitionBodyTinyHeader
	{
	public:
		uint8_t Flags : 2;
		uint8_t Size : 6;
	};

	class MethodDefinitionBodyFatHeader
	{
	public:
		uint16_t Flags : 12;
		uint8_t Size : 4;
		uint16_t MaxStack;
		uint32_t CodeSize;
		uint32_t LocalVariableSignatureToken;
	};

	namespace MethodDefinitionBodyFlags
	{
		typedef enum MethodDefinitionBodyFlags
		{
			Tiny									= 0x02,
			Fat										= 0x03,
			MoreSections							= 0x08,
			InitializeLocals						= 0x10,
		} MethodDefinitionBodyFlags;
	};

	namespace MethodDefinitionBodyDefaults
	{
		const uint16_t TinyMaxStack = 8;
		const uint32_t FatSizeMultiplier = 4;
	};

	class MethodDefinitionBody
	{
	public:
		uint16_t Flags;
		bool IsFat;
		uint16_t MaxStack;
		uint32_t CodeSize;
		uint32_t LocalVariableSignatureToken;
		uint8_t* Code;

		MethodDefinitionBody();
		~MethodDefinitionBody();
	};

	class MethodDefinitionExceptionTinyHeader
	{
	public:
		uint8_t Kind;
		uint8_t DataSize;
		uint16_t Reserved;
	};

	class MethodDefinitionExceptionFatHeader
	{
	public:
		uint8_t Kind;
		uint32_t DataSize : 24;
	};

	namespace MethodDefinitionDataFlags
	{
		typedef enum MethodDefinitionDataFlags
		{
			ExceptionTable							= 0x01,
			OptionalILTable							= 0x02,
			Fat										= 0x40,
			HasAnotherDataSection					= 0x80,
		} MethodDefinitionDataFlags;
	}

	class MethodDefinitionExceptionTinyData
	{
	public:
		uint16_t Flags;
		uint16_t TryOffset;
		uint8_t TryLength;
		uint16_t HandlerOffset;
		uint8_t HandlerLength;
		uint32_t ClassToken;
		uint32_t FilterOffset;
	};

	class MethodDefinitionExceptionFatData
	{
	public:
		uint32_t Flags;
		uint32_t TryOffset;
		uint32_t TryLength;
		uint32_t HandlerOffset;
		uint32_t HandlerLength;
		uint32_t ClassToken;
		uint32_t FilterOffset;
	};

	class MethodDefinitionException
	{
	public:
		uint32_t Flags;
		uint32_t TryOffset;
		uint32_t TryLength;
		uint32_t HandlerOffset;
		uint32_t HandlerLength;
		uint32_t ClassTokenOrFilterOffset;

		MethodDefinitionException();
		~MethodDefinitionException();
	};

	class MethodDefinitionData
	{
	public:
		CLIFile* File;
		uint32_t TableIndex;
		MethodDefinitionBody Body;
		uint32_t ExceptionCount;
		MethodDefinitionException* Exceptions;
		uint16_t ImplFlags;
		uint16_t Flags;
		const char* Name;
		uint32_t SignatureLength;
		uint8_t* Signature;
		MethodSignature* SignatureCache;
		ParameterData* ParameterList;
		uint32_t ParameterListCount;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;
		DeclSecurityData* DeclSecurity;
		uint32_t GenericParameterCount;
		GenericParameterData** GenericParameters;
		ImplementationMapData* ImplementationMap;
		uint32_t MemberReferenceCount;
		MemberReferenceData** MemberReferences;
		TypeDefinitionData* TypeDefinition;

		void* InternalCall;

		MethodDefinitionData();
		~MethodDefinitionData();
	};

	class MethodImplementationData
	{
	public:
		uint32_t TableIndex;
		TypeDefinitionData* Parent;
		MethodDefOrRefUnion(MethodBody)
		MethodDefOrRefUnion(MethodDeclaration)

		MethodImplementationData();
		~MethodImplementationData();
	};

	class MethodSemanticsData
	{
	public:
		uint32_t TableIndex;
		uint16_t Semantics;
		MethodDefinitionData* Method;
		HasSemanticsUnion(Association)

		MethodSemanticsData();
		~MethodSemanticsData();
	};

	class MethodSpecificationData
	{
	public:
		uint32_t TableIndex;
		MethodDefOrRefUnion(Method)
		uint32_t InstantiationLength;
		const uint8_t* Instantiation;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;

		MethodSpecificationData();
		~MethodSpecificationData();
	};

	class ModuleDefinitionData
	{
	public:
		uint32_t TableIndex;
		uint16_t Generation;
		const char* Name;
		const uint8_t* ModuleVersionID;
		const uint8_t* EncID;
		const uint8_t* EncBaseID;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;

		ModuleDefinitionData();
		~ModuleDefinitionData();
	};

	class ModuleReferenceData
	{
	public:
		uint32_t TableIndex;
		const char* Name;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;
		uint32_t MemberReferenceCount;
		MemberReferenceData** MemberReferences;

		ModuleReferenceData();
		~ModuleReferenceData();
	};

	class NestedClassData
	{
	public:
		uint32_t TableIndex;
		TypeDefinitionData* Nested;
		TypeDefinitionData* Enclosing;

		NestedClassData();
		~NestedClassData();
	};

	class ParameterData
	{
	public:
		uint32_t TableIndex;
		uint16_t Flags;
		uint16_t Sequence;
		const char* Name;

		ConstantData* Constant;
		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;
		FieldMarshalData* FieldMarshal;

		ParameterData();
		~ParameterData();
	};

	class PropertyData
	{
	public:
		uint32_t TableIndex;
		uint16_t Flags;
		const char* Name;
		uint32_t SignatureLength;
		uint8_t* Signature;

		ConstantData* Constant;
		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;
		MethodSemanticsData* MethodSemantics;

		PropertyData();
		~PropertyData();
	};

	class PropertyMapData
	{
	public:
		uint32_t TableIndex;
		TypeDefinitionData* Parent;
		PropertyData* PropertyList;
		uint32_t PropertyListCount;

		PropertyMapData();
		~PropertyMapData();
	};

	class StandAloneSignatureData
	{
	public:
		uint32_t TableIndex;
		uint32_t SignatureLength;
		uint8_t* Signature;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;

		StandAloneSignatureData();
		~StandAloneSignatureData();
	};

	class TypeDefinitionData
	{
	public:
		CLIFile* File;
		uint32_t TableIndex;
		uint32_t Flags;
		const char* Name;
		const char* Namespace;
		TypeDefOrRefUnion(Extends)
		FieldData* FieldList;
		uint32_t FieldListCount;
		MethodDefinitionData* MethodDefinitionList;
		uint32_t MethodDefinitionListCount;

		ClassLayoutData* ClassLayout;
		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;
		DeclSecurityData* DeclSecurity;
		EventMapData* EventMap;
		uint32_t GenericParameterCount;
		GenericParameterData** GenericParameters;
		uint32_t InterfaceImplementationCount;
		InterfaceImplementationData** InterfaceImplementations;
		uint32_t MemberReferenceCount;
		MemberReferenceData** MemberReferences;
		uint32_t MethodImplementationCount;
		MethodImplementationData** MethodImplementations;
		uint32_t NestedClassCount;
		NestedClassData** NestedClasses;
		PropertyMapData* PropertyMap;

		TypeDefinitionData();
		~TypeDefinitionData();
	};

	class TypeReferenceData
	{
	public:
		uint32_t TableIndex;
		ResolutionScopeUnion(ResolutionScope)
		ExportedTypeData* ExportedType;
		const char* Name;
		const char* Namespace;
		TypeDefinitionData* ResolvedType;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;
		uint32_t MemberReferenceCount;
		MemberReferenceData** MemberReferences;

		TypeReferenceData();
		~TypeReferenceData();
	};

	class TypeSpecificationData
	{
	public:
		CLIFile* File;
		uint32_t TableIndex;
		uint32_t SignatureLength;
		uint8_t* Signature;

		uint32_t CustomAttributeCount;
		CustomAttributeData** CustomAttributes;
		uint32_t MemberReferenceCount;
		MemberReferenceData** MemberReferences;

		TypeSpecificationData();
		~TypeSpecificationData();
	};

	namespace SignatureElementType
	{
		typedef enum SignatureElementType
		{
			End								= 0x00,
			Void							= 0x01,
			Boolean							= 0x02,
			Char							= 0x03,
			I1								= 0x04,
			U1								= 0x05,
			I2								= 0x06,
			U2								= 0x07,
			I4								= 0x08,
			U4								= 0x09,
			I8								= 0x0A,
			U8								= 0x0B,
			R4								= 0x0C,
			R8								= 0x0D,
			String							= 0x0E,
			Pointer							= 0x0F,
			ByReference						= 0x10,
			ValueType						= 0x11,
			Class							= 0x12,
			Var								= 0x13,
			Array							= 0x14,
			GenericInstantiation			= 0x15,
			TypedByReference				= 0x16,
			IPointer						= 0x18,
			UPointer						= 0x19,
			FunctionPointer					= 0x1B,
			Object							= 0x1C,
			SingleDimensionArray			= 0x1D,
			MethodVar						= 0x1E,
			CustomModifier_Required			= 0x1F,
			CustomModifier_Optional			= 0x20,
			Internal						= 0x21,
			Modifier						= 0x40,
			Sentinel						= 0x41,
			Pinned							= 0x45,
			Type							= 0x50,
			CustomAttribute_Boxed			= 0x51,
			CustomAttribute_Field			= 0x53,
			CustomAttribute_Property		= 0x54,
			CustomAttribute_Enum			= 0x55,
		} SignatureElementType;
	};

	class MethodSignature
	{
	public:
		CLIFile* File;

		bool HasThis;
		bool ExplicitThis;
		bool Default;
		bool VarArgs;
		bool Generic;
		bool CCall;
		bool STDCall;
		bool ThisCall;
		bool FastCall;
		uint32_t GenericParameterCount;
		SignatureReturnType* ReturnType;
		uint32_t ParameterCount;
		SignatureParameter** Parameters;
		bool HasSentinel;
		uint32_t SentinelIndex;

		MethodSignature(uint8_t* pSignature, CLIFile* pFile);
		~MethodSignature();

		bool Equals(MethodSignature* pSignature);
	};

	class FieldSignature
	{
	public:
		uint32_t CustomModifierCount;
		SignatureCustomModifier** CustomModifiers;
		SignatureType* Type;

		FieldSignature();
		~FieldSignature();
	};

	class PropertySignature
	{
	public:
		bool HasThis;
		uint32_t CustomModifierCount;
		SignatureCustomModifier** CustomModifiers;
		SignatureType* Type;
		uint32_t ParameterCount;
		SignatureParameter** Parameters;

		PropertySignature();
		~PropertySignature();
	};

	class LocalsSignature
	{
	public:
		uint32_t LocalVariableCount;
		SignatureLocalVariable** LocalVariables;

		LocalsSignature();
		~LocalsSignature();
	};

	class SignatureReturnType
	{
	public:
		uint32_t CustomModifierCount;
		SignatureCustomModifier** CustomModifiers;
		bool ByReference;
		SignatureType* Type;
		bool TypedByReference;
		bool Void;

		SignatureReturnType();
		~SignatureReturnType();
	};

	class SignatureParameter
	{
	public:
		uint32_t CustomModifierCount;
		SignatureCustomModifier** CustomModifiers;
		bool ByReference;
		SignatureType* Type;
		bool TypedByReference;

		SignatureParameter();
		~SignatureParameter();
	};

	class SignatureCustomModifier
	{
	public:
		bool Optional;
		uint32_t TypeDefOrRefOrSpecToken;

		SignatureCustomModifier();
		~SignatureCustomModifier();
	};

	class SignatureType
	{
	public:
		uint8_t ElementType;
		union
		{
			struct
			{
				SignatureType* ArrayType;
				SignatureArrayShape* ArrayShape;
			};
			uint32_t ClassTypeDefOrRefOrSpecToken;
			MethodSignature* FnPtrMethodSignature;
			struct
			{
				uint32_t GenericInstTypeDefOrRefOrSpecToken;
				uint32_t GenericInstGenericArgumentCount;
				SignatureType** GenericInstGenericArguments;
				bool GenericInstClass;
				bool GenericInstValue;
			};
			uint32_t MVarNumber;
			struct
			{
				uint32_t PtrCustomModifierCount;
				SignatureCustomModifier** PtrCustomModifiers;
				SignatureType* PtrType;
				bool PtrVoid;
			};
			struct
			{
				uint32_t SZArrayCustomModifierCount;
				SignatureCustomModifier** SZArrayCustomModifiers;
				SignatureType* SZArrayType;
			};
			uint32_t ValueTypeDefOrRefOrSpecToken;
			uint32_t VarNumber;
		};

		SignatureType(uint8_t* pSignature, CLIFile* pFile);
		~SignatureType();
	};

	class SignatureMethodSpecification
	{
	public:
		uint32_t GenericInstGenericArgumentCount;
		SignatureType** GenericInstGenericArguments;

		SignatureMethodSpecification();
		~SignatureMethodSpecification();
	};

	class SignatureArrayShape
	{
	public:
		uint32_t Rank;
		uint32_t SizeCount;
		uint32_t* Sizes;
		uint32_t LowerBoundCount;
		int32_t* LowerBounds;

		SignatureArrayShape();
		~SignatureArrayShape();
	};

	class SignatureLocalVariable
	{
	public:
		uint32_t CustomModifierCount;
		SignatureCustomModifier** CustomModifiers;
		bool ByReference;
		SignatureType* Type;
		bool TypedByReference;
		bool IsPinned;

		SignatureLocalVariable();
		~SignatureLocalVariable();
	};

	class MetadataToken
	{
	public:
		uint8_t Table;
		bool IsUserString;
		void* Data;
	};
};
