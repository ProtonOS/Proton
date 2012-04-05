#pragma once

typedef struct _AssemblyDefinition AssemblyDefinition;
typedef struct _AssemblyOperatingSystem AssemblyOperatingSystem;
typedef struct _AssemblyProcessor AssemblyProcessor;
typedef struct _AssemblyReference AssemblyReference;
typedef struct _AssemblyReferenceOperatingSystem AssemblyReferenceOperatingSystem;
typedef struct _AssemblyReferenceProcessor AssemblyReferenceProcessor;
typedef struct _ClassLayout ClassLayout;
typedef struct _Constant Constant;
typedef struct _CustomAttribute CustomAttribute;
typedef struct _DeclSecurity DeclSecurity;
typedef struct _Event Event;
typedef struct _EventMap EventMap;
typedef struct _ExportedType ExportedType;
typedef struct _Field Field;
typedef struct _FieldLayout FieldLayout;
typedef struct _FieldMarshal FieldMarshal;
typedef struct _FieldRVA FieldRVA;
typedef struct _File File;
typedef struct _GenericParameter GenericParameter;
typedef struct _GenericParameterConstraint GenericParameterConstraint;
typedef struct _ImplementationMap ImplementationMap;
typedef struct _InterfaceImplementation InterfaceImplementation;
typedef struct _ManifestResource ManifestResource;
typedef struct _MemberReference MemberReference;
typedef struct _MethodDefinition MethodDefinition;
typedef struct _MethodDefinitionBody MethodDefinitionBody;
typedef struct _MethodDefinitionException MethodDefinitionException;
typedef struct _MethodDefinitionSignature MethodDefinitionSignature;
typedef struct _MethodImplementation MethodImplementation;
typedef struct _MethodSemantics MethodSemantics;
typedef struct _MethodSpecification MethodSpecification;
typedef struct _ModuleDefinition ModuleDefinition;
typedef struct _ModuleReference ModuleReference;
typedef struct _NestedClass NestedClass;
typedef struct _Parameter Parameter;
typedef struct _Property Property;
typedef struct _PropertyMap PropertyMap;
typedef struct _StandAloneSignature StandAloneSignature;
typedef struct _TypeDefinition TypeDefinition;
typedef struct _TypeReference TypeReference;
typedef struct _TypeSpecification TypeSpecification;

#include <CLR/CLIFile.h>

typedef enum TypeDefRefOrSpecType
{
	TypeDefRefOrSpecType_TypeDefinition								= 0x00,
	TypeDefRefOrSpecType_TypeReference								= 0x01,
	TypeDefRefOrSpecType_TypeSpecification							= 0x02,
} TypeDefRefOrSpecType;

#define TypeDefRefOrSpecType_Bits									2
#define TypeDefRefOrSpecType_Mask									((1 << TypeDefRefOrSpecType_Bits) - 1)
#define TypeDefRefOrSpecType_MaxRows16Bit							(0xFFFF >> TypeDefRefOrSpecType_Bits)

typedef enum HasConstantType
{
	HasConstantType_Field											= 0x00,
	HasConstantType_Parameter										= 0x01,
	HasConstantType_Property										= 0x02,
} HasConstantType;

#define HasConstantType_Bits										2
#define HasConstantType_Mask										((1 << HasConstantType_Bits) - 1)
#define HasConstantType_MaxRows16Bit								(0xFFFF >> HasConstantType_Bits)

typedef enum HasCustomAttributeType
{
	HasCustomAttributeType_MethodDefinition							= 0x00,
	HasCustomAttributeType_Field									= 0x01,
	HasCustomAttributeType_TypeReference							= 0x02,
	HasCustomAttributeType_TypeDefinition							= 0x03,
	HasCustomAttributeType_Parameter								= 0x04,
	HasCustomAttributeType_InterfaceImplementation					= 0x05,
	HasCustomAttributeType_MemberReference							= 0x06,
	HasCustomAttributeType_ModuleDefinition							= 0x07,
	HasCustomAttributeType_DeclSecurity								= 0x08,
	HasCustomAttributeType_Property									= 0x09,
	HasCustomAttributeType_Event									= 0x0A,
	HasCustomAttributeType_StandAloneSignature						= 0x0B,
	HasCustomAttributeType_ModuleReference							= 0x0C,
	HasCustomAttributeType_TypeSpecification						= 0x0D,
	HasCustomAttributeType_AssemblyDefinition						= 0x0E,
	HasCustomAttributeType_AssemblyReference						= 0x0F,
	HasCustomAttributeType_File										= 0x10,
	HasCustomAttributeType_ExportedType								= 0x11,
	HasCustomAttributeType_ManifestResource							= 0x12,
	HasCustomAttributeType_GenericParameter							= 0x13,
	HasCustomAttributeType_GenericParameterConstraint				= 0x14,
	HasCustomAttributeType_MethodSpecification						= 0x15,
} HasCustomAttributeType;

#define HasCustomAttributeType_Bits									5
#define HasCustomAttributeType_Mask									((1 << HasCustomAttributeType_Bits) - 1)
#define HasCustomAttributeType_MaxRows16Bit							(0xFFFF >> HasCustomAttributeType_Bits)

typedef enum HasFieldMarshalType
{
	HasFieldMarshalType_Field										= 0x00,
	HasFieldMarshalType_Parameter									= 0x01,
} HasFieldMarshalType;

#define HasFieldMarshalType_Bits									1
#define HasFieldMarshalType_Mask									((1 << HasFieldMarshalType_Bits) - 1)
#define HasFieldMarshalType_MaxRows16Bit							(0xFFFF >> HasFieldMarshalType_Bits)

typedef enum HasDeclSecurityType
{
	HasDeclSecurityType_TypeDefinition								= 0x00,
	HasDeclSecurityType_MethodDefinition							= 0x01,
	HasDeclSecurityType_AssemblyDefinition							= 0x02,
} HasDeclSecurityType;

#define HasDeclSecurityType_Bits									2
#define HasDeclSecurityType_Mask									((1 << HasDeclSecurityType_Bits) - 1)
#define HasDeclSecurityType_MaxRows16Bit							(0xFFFF >> HasDeclSecurityType_Bits)

typedef enum MemberRefParentType
{
	MemberRefParentType_TypeDefinition								= 0x00,
	MemberRefParentType_TypeReference								= 0x01,
	MemberRefParentType_ModuleReference								= 0x02,
	MemberRefParentType_MethodDefinition							= 0x03,
	MemberRefParentType_TypeSpecification							= 0x04,
} MemberRefParentType;

#define MemberRefParentType_Bits									3
#define MemberRefParentType_Mask									((1 << MemberRefParentType_Bits) - 1)
#define MemberRefParentType_MaxRows16Bit							(0xFFFF >> MemberRefParentType_Bits)

typedef enum HasSemanticsType
{
	HasSemanticsType_Event											= 0x00,
	HasSemanticsType_Property										= 0x01,
} HasSemanticsType;

#define HasSemanticsType_Bits										1
#define HasSemanticsType_Mask										((1 << HasSemanticsType_Bits) - 1)
#define HasSemanticsType_MaxRows16Bit								(0xFFFF >> HasSemanticsType_Bits)

typedef enum MethodDefOrRefType
{
	MethodDefOrRefType_MethodDefinition								= 0x00,
	MethodDefOrRefType_MemberReference								= 0x01,
} MethodDefOrRefType;

#define MethodDefOrRefType_Bits										1
#define MethodDefOrRefType_Mask										((1 << MethodDefOrRefType_Bits) - 1)
#define MethodDefOrRefType_MaxRows16Bit								(0xFFFF >> MethodDefOrRefType_Bits)

typedef enum MemberForwardedType
{
	MemberForwardedType_Field										= 0x00,
	MemberForwardedType_MethodDefinition							= 0x01,
} MemberForwardedType;

#define MemberForwardedType_Bits									1
#define MemberForwardedType_Mask									((1 << MemberForwardedType_Bits) - 1)
#define MemberForwardedType_MaxRows16Bit							(0xFFFF >> MemberForwardedType_Bits)

typedef enum ImplementationType
{
	ImplementationType_File											= 0x00,
	ImplementationType_AssemblyReference							= 0x01,
	ImplementationType_ExportedType									= 0x02,
} ImplementationType;

#define ImplementationType_Bits										2
#define ImplementationType_Mask										((1 << ImplementationType_Bits) - 1)
#define ImplementationType_MaxRows16Bit								(0xFFFF >> ImplementationType_Bits)

typedef enum CustomAttributeType
{
	CustomAttributeType_MethodDefinition							= 0x02,
	CustomAttributeType_MemberReference								= 0x03,
} CustomAttributeType;

#define CustomAttributeType_Bits									3
#define CustomAttributeType_Mask									((1 << CustomAttributeType_Bits) - 1)
#define CustomAttributeType_MaxRows16Bit							(0xFFFF >> CustomAttributeType_Bits)

typedef enum ResolutionScopeType
{
	ResolutionScopeType_ModuleDefinition							= 0x00,
	ResolutionScopeType_ModuleReference								= 0x01,
	ResolutionScopeType_AssemblyReference							= 0x02,
	ResolutionScopeType_TypeReference								= 0x03,
} ResolutionScopeType;

#define ResolutionScopeType_Bits									2
#define ResolutionScopeType_Mask									((1 << ResolutionScopeType_Bits) - 1)
#define ResolutionScopeType_MaxRows16Bit							(0xFFFF >> ResolutionScopeType_Bits)

typedef enum TypeOrMethodDefType
{
	TypeOrMethodDefType_TypeDefinition								= 0x00,
	TypeOrMethodDefType_MethodDefinition							= 0x01,
} TypeOrMethodDefType;

#define TypeOrMethodDefType_Bits									1
#define TypeOrMethodDefType_Mask									((1 << TypeOrMethodDefType_Bits) - 1)
#define TypeOrMethodDefType_MaxRows16Bit							(0xFFFF >> TypeOrMethodDefType_Bits)

typedef enum FieldOrMethodDefType
{
	FieldOrMethodDefType_Field										= 0x00,
	FieldOrMethodDefType_MethodDefinition							= 0x01,
} FieldOrMethodDefType;

#define FieldOrMethodDef_Type_Bits									1
#define FieldOrMethodDef_Type_Mask									((1 << FieldOrMethodDefType_Bits) - 1)
#define FieldOrMethodDef_Type_MaxRows16Bit							(0xFFFF >> FieldOrMethodDefType_Bits)

typedef enum AssemblyHashAlgorithm
{
	AssemblyHashAlgorithm_None										= 0x0000,
	AssemblyHashAlgorithm_MD5										= 0x8003,
	AssemblyHashAlgorithm_SHA1										= 0x8004,
} AssemblyHashAlgorithm;

typedef enum AssemblyFlags
{
	AssemblyFlags_PublicKey											= 0x0001,
	AssemblyFlags_Retargetable										= 0x0100,
	AssemblyFlags_DisableJITCompileOptimizer						= 0x4000,
	AssemblyFlags_DisableJITCompileTracking							= 0x8000,
} AssemblyFlags;

typedef enum EventAttributes
{
	EventAttributes_SpecialName										= 0x0200,
	EventAttributes_RTSpecialName									= 0x0400,
} EventAttributes;

typedef enum FieldAttributes
{
	FieldAttributes_FieldAccessMask									= 0x0007,
	//FieldAttributes_CompilerControlled							= 0x0000,
	FieldAttributes_Private											= 0x0001,
	FieldAttributes_FamilyAndAssembly								= 0x0002,
	FieldAttributes_Assembly										= 0x0003,
	FieldAttributes_Family											= 0x0004,
	FieldAttributes_FamilyOrAssembly								= 0x0005,
	FieldAttributes_Public											= 0x0006,
	FieldAttributes_Static											= 0x0010,
	FieldAttributes_InitOnly										= 0x0020,
	FieldAttributes_Literal											= 0x0040,
	FieldAttributes_NotSerialized									= 0x0080,
	FieldAttributes_SpecialName										= 0x0200,
	FieldAttributes_PInvokeImplementation							= 0x2000,
	FieldAttributes_RTSpecialName									= 0x0400,
	FieldAttributes_HasFieldMarshal									= 0x1000,
	FieldAttributes_HasDefault										= 0x8000,
	FieldAttributes_HasFieldRVA										= 0x0100,
} FieldAttributes;


typedef enum FileAttributes
{
	//FileAttributes_ContainsMetadata								= 0x0000
	FileAttributes_ContainsNoMetadata								= 0x0001,
} FileAttributes;

typedef enum GenericParameterAttributes
{
	GenericParameterAttributes_VarianceMask							= 0x0003,
	//GenericParameterAttributes_None								= 0x0000,
	GenericParameterAttributes_Covariant							= 0x0001,
	GenericParameterAttributes_Contravariant						= 0x0002,
	GenericParameterAttributes_SpecialConstraintMask				= 0x001C,
	GenericParameterAttributes_ReferenceTypeConstraint				= 0x0004,
	GenericParameterAttributes_NotNullableValueTypeConstraint		= 0x0008,
	GenericParameterAttributes_DefaultConstructorConstraint			= 0x0010,
} GenericParameterAttributes;

typedef enum PInvokeAttributes
{
	PInvokeAttributes_NoMangle										= 0x0001,
	PInvokeAttributes_CharacterSetMask								= 0x0006,
	//PInvokeAttributes_CharacterSetNotSpecified					= 0x0000,
	PInvokeAttributes_CharacterSetAnsi								= 0x0002,
	PInvokeAttributes_CharacterSetUnicode							= 0x0004,
	PInvokeAttributes_CharacterSetAuto								= 0x0006,
	PInvokeAttributes_SupportsLastError								= 0x0040,
	PInvokeAttributes_CallConventionMask							= 0x0700,
	PInvokeAttributes_CallConventionPlatformAPI						= 0x0100,
	PInvokeAttributes_CallConventionCDecl							= 0x0200,
	PInvokeAttributes_CallConventionSTD								= 0x0300,
	PInvokeAttributes_CallConventionThis							= 0x0400,
	PInvokeAttributes_CallConventionFast							= 0x0500,
} PInvokeAttributes;

typedef enum ManifestResourceAttributes
{
	ManifestResourceAttributes_VisibilityMask						= 0x0007,
	ManifestResourceAttributes_Public								= 0x0001,
	ManifestResourceAttributes_Private								= 0x0002,
} ManifestResourceAttributes;

typedef enum MethodAttributes
{
	MethodAttributes_MemberAccessMask								= 0x0007,
	//MethodAttributes_CompilerControlled							= 0x0000,
	MethodAttributes_Private										= 0x0001,
	MethodAttributes_FamilyAndAssembly								= 0x0002,
	MethodAttributes_Assembly										= 0x0003,
	MethodAttributes_Family											= 0x0004,
	MethodAttributes_FamilyOrAssembly								= 0x0005,
	MethodAttributes_Public											= 0x0006,
	MethodAttributes_Static											= 0x0010,
	MethodAttributes_Final											= 0x0020,
	MethodAttributes_Virtual										= 0x0040,
	MethodAttributes_HideBySignature								= 0x0080,
	MethodAttributes_VTableLayoutMask								= 0x0100,
	//MethodAttributes_ReuseSlot									= 0x0000,
	MethodAttributes_NewSlot										= 0x0100,
	MethodAttributes_Strict											= 0x0200,
	MethodAttributes_Abstract										= 0x0400,
	MethodAttributes_SpecialName									= 0x0800,
	MethodAttributes_PInvokeImplementation							= 0x2000,
	MethodAttributes_UnmanagedExport								= 0x0008,
	MethodAttributes_RTSpecialName									= 0x1000,
	MethodAttributes_HasSecurity									= 0x4000,
	MethodAttributes_RequireSecurityObject							= 0x8000,
} MethodAttributes;

typedef enum MethodImplAttributes
{
	MethodImplAttributes_CodeTypeMask								= 0x0003,
	//MethodImplAttributes_IL										= 0x0000,
	MethodImplAttributes_Native										= 0x0001,
	MethodImplAttributes_OPTIL										= 0x0002,
	MethodImplAttributes_Runtime									= 0x0003,
	MethodImplAttributes_ManagedMask								= 0x0004,
	MethodImplAttributes_Unmanaged									= 0x0004,
	//MethodImplAttributes_Managed									= 0x0000,
	MethodImplAttributes_ForwardReference							= 0x0010,
	MethodImplAttributes_PreserveSignature							= 0x0080,
	MethodImplAttributes_InternalCall								= 0x1000,
	MethodImplAttributes_Synchronized								= 0x0020,
	MethodImplAttributes_NoInlining									= 0x0008,
	MethodImplAttributes_MaxMethodImplementationValue				= 0xFFFF,
	MethodImplAttributes_NoOptimization								= 0x0040,
} MethodImplAttributes;

typedef enum MethodSemanticsAttributes
{
	MethodSemanticsAttributes_Setter								= 0x0001,
	MethodSemanticsAttributes_Getter								= 0x0002,
	MethodSemanticsAttributes_Other									= 0x0004,
	MethodSemanticsAttributes_AddOn									= 0x0008,
	MethodSemanticsAttributes_RemoveOn								= 0x0010,
	MethodSemanticsAttributes_Fire									= 0x0020,
} MethodSemanticsAttributes;

typedef enum ParameterAttributes
{
	ParameterAttributes_In											= 0x0001,
	ParameterAttributes_Out											= 0x0002,
	ParameterAttributes_Optional									= 0x0010,
	ParameterAttributes_HasDefault									= 0x1000,
	ParameterAttributes_HasFieldMarshal								= 0x2000,
	ParameterAttributes_Unused										= 0xCFE0,
} ParameterAttributes;

typedef enum PropertyAttributes
{
	PropertyAttributes_SpecialName									= 0x0200,
	PropertyAttributes_RTSpecialName								= 0x0400,
	PropertyAttributes_HasDefault									= 0x1000,
	PropertyAttributes_Unused										= 0xE9FF,
} PropertyAttributes;

typedef enum TypeAttributes
{
	TypeAttributes_VisibilityMask									= 0x00000007,
	TypeAttributes_NotPublic										= 0x00000000,
	TypeAttributes_Public											= 0x00000001,
	TypeAttributes_NestedPublic										= 0x00000002,
	TypeAttributes_NestedPrivate									= 0x00000003,
	TypeAttributes_NestedFamily										= 0x00000004,
	TypeAttributes_NestedAssembly									= 0x00000005,
	TypeAttributes_NestedFamilyAndAssembly							= 0x00000006,
	TypeAttributes_NestedFamilyOrAssembly							= 0x00000007,
	TypeAttributes_LayoutMask										= 0x00000018,
	//TypeAttributes_AutoLayout										= 0x00000000,
	TypeAttributes_SequentialLayout									= 0x00000008,
	TypeAttributes_ExplicitLayout									= 0x00000010,
	TypeAttributes_ClassSemanticsMask								= 0x00000020,
	//TypeAttributes_Class											= 0x00000000,
	TypeAttributes_Interface										= 0x00000020,
	TypeAttributes_Abstract											= 0x00000080,
	TypeAttributes_Sealed											= 0x00000100,
	TypeAttributes_SpecialName										= 0x00000400,
	TypeAttributes_Import											= 0x00001000,
	TypeAttributes_Serializable										= 0x00002000,
	TypeAttributes_StringFormatMask									= 0x00030000,
	//TypeAttributes_AnsiClass										= 0x00000000,
	TypeAttributes_UnicodeClass										= 0x00010000,
	TypeAttributes_AutoClass										= 0x00020000,
	TypeAttributes_CustomFormatClass								= 0x00030000,
	TypeAttributes_CustomStringFormatMask							= 0x00C00000,
	TypeAttributes_BeforeFieldInit									= 0x00100000,
	TypeAttributes_RTSpecialName									= 0x00000800,
	TypeAttributes_HasSecurity										= 0x00040000,
	TypeAttributes_IsTypeForwarder									= 0x00200000,
} TypeAttributes;

#define TypeDefOrRefUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    TypeDefinition* TypeDefinition; \
    TypeReference* TypeReference; \
    TypeSpecification* TypeSpecification; \
} n;

#define HasConstantUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    Field* Field; \
    Parameter* Parameter; \
    Property* Property; \
} n;

#define HasCustomAttributeUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    MethodDefinition* MethodDefinition; \
    Field* Field; \
    TypeReference* TypeReference; \
    TypeDefinition* TypeDefinition; \
    Parameter* Parameter; \
    InterfaceImplementation* InterfaceImplementation; \
    MemberReference* MemberReference; \
    ModuleDefinition* ModuleDefinition; \
    DeclSecurity* DeclSecurity; \
    Property* Property; \
    Event* Event; \
    StandAloneSignature* StandAloneSignature; \
    ModuleReference* ModuleReference; \
    TypeSpecification* TypeSpecification; \
    AssemblyDefinition* AssemblyDefinition; \
    AssemblyReference* AssemblyReference; \
    File* File; \
    ExportedType* ExportedType; \
    ManifestResource* ManifestResource; \
    GenericParameter* GenericParameter; \
    GenericParameterConstraint* GenericParameterConstraint; \
    MethodSpecification* MethodSpecification; \
} n;

#define HasFieldMarshalUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    Field* Field; \
    Parameter* Parameter; \
} n;

#define HasDeclSecurityUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    TypeDefinition* TypeDefinition; \
    MethodDefinition* MethodDefinition; \
    AssemblyDefinition* AssemblyDefinition; \
} n;

#define MemberRefParentUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    TypeDefinition* TypeDefinition; \
    TypeReference* TypeReference; \
    ModuleReference* ModuleReference; \
    MethodDefinition* MethodDefinition; \
    TypeSpecification* TypeSpecification; \
} n;

#define HasSemanticsUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    Event* Event; \
    Property* Property; \
} n;

#define MethodDefOrRefUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    MethodDefinition* MethodDefinition; \
    MemberReference* MemberReference; \
} n;

#define MemberForwardedUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    Field* Field; \
    MethodDefinition* MethodDefinition; \
} n;

#define ImplementationUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    File* File; \
    AssemblyReference* AssemblyReference; \
    ExportedType* ExportedType; \
} n;

#define CustomAttributeTypeUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    MethodDefinition* MethodDefinition; \
    MemberReference* MemberReference; \
} n;

#define ResolutionScopeUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    ModuleDefinition* ModuleDefinition; \
    ModuleReference* ModuleReference; \
    AssemblyReference* AssemblyReference; \
    TypeReference* TypeReference; \
} n;

#define TypeOrMethodDefUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    TypeDefinition* TypeDefinition; \
    MethodDefinition* MethodDefinition; \
} n;


typedef enum MetadataTable
{
	MetadataTable_ModuleDefinition					= 0x00,
	MetadataTable_TypeReference						= 0x01,
	MetadataTable_TypeDefinition					= 0x02,
	MetadataTable_Field								= 0x04,
	MetadataTable_MethodDefinition					= 0x06,
	MetadataTable_Parameter							= 0x08,
	MetadataTable_InterfaceImplementation			= 0x09,
	MetadataTable_MemberReference					= 0x0A,
	MetadataTable_Constant							= 0x0B,
	MetadataTable_CustomAttribute					= 0x0C,
	MetadataTable_FieldMarshal						= 0x0D,
	MetadataTable_DeclSecurity						= 0x0E,
	MetadataTable_ClassLayout						= 0x0F,
	MetadataTable_FieldLayout						= 0x10,
	MetadataTable_StandAloneSignature				= 0x11,
	MetadataTable_EventMap							= 0x12,
	MetadataTable_Event								= 0x14,
	MetadataTable_PropertyMap						= 0x15,
	MetadataTable_Property							= 0x17,
	MetadataTable_MethodSemantics					= 0x18,
	MetadataTable_MethodImplementation				= 0x19,
	MetadataTable_ModuleReference					= 0x1A,
	MetadataTable_TypeSpecification					= 0x1B,
	MetadataTable_ImplementationMap					= 0x1C,
	MetadataTable_FieldRVA							= 0x1D,
	MetadataTable_AssemblyDefinition				= 0x20,
	MetadataTable_AssemblyProcessor					= 0x21,
	MetadataTable_AssemblyOperatingSystem			= 0x22,
	MetadataTable_AssemblyReference					= 0x23,
	MetadataTable_AssemblyReferenceProcessor		= 0x24,
	MetadataTable_AssemblyReferenceOperatingSystem	= 0x25,
	MetadataTable_File								= 0x26,
	MetadataTable_ExportedType						= 0x27,
	MetadataTable_ManifestResource					= 0x28,
	MetadataTable_NestedClass						= 0x29,
	MetadataTable_GenericParameter					= 0x2A,
	MetadataTable_MethodSpecification				= 0x2B,
	MetadataTable_GenericParameterConstraint		= 0x2C,

	MetadataTable_UserStrings						= 0x70,
} MetadataTable;

#define METADATAHEAP__OffsetSize_Strings32Bit		0x01
#define METADATAHEAP__OffsetSize_GUIDs32Bit			0x02
#define METADATAHEAP__OffsetSize_Blobs32Bit			0x04



struct _AssemblyDefinition
{
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
    CustomAttribute** CustomAttributes;
    DeclSecurity* DeclSecurity;
};

uint8_t* AssemblyDefinition_Initialize(CLIFile* pFile, uint8_t* pTableData);
void AssemblyDefinition_Cleanup(CLIFile* pFile);
uint8_t* AssemblyDefinition_Load(CLIFile* pFile, uint8_t* pTableData);
void AssemblyDefinition_Link(CLIFile* pFile);


struct _AssemblyOperatingSystem
{
	uint32_t TableIndex;
    uint32_t PlatformID;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
};

uint8_t* AssemblyOperatingSystem_Initialize(CLIFile* pFile, uint8_t* pTableData);
void AssemblyOperatingSystem_Cleanup(CLIFile* pFile);
uint8_t* AssemblyOperatingSystem_Load(CLIFile* pFile, uint8_t* pTableData);
void AssemblyOperatingSystem_Link(CLIFile* pFile);


struct _AssemblyProcessor
{
	uint32_t TableIndex;
    uint32_t Processor;
};

uint8_t* AssemblyProcessor_Initialize(CLIFile* pFile, uint8_t* pTableData);
void AssemblyProcessor_Cleanup(CLIFile* pFile);
uint8_t* AssemblyProcessor_Load(CLIFile* pFile, uint8_t* pTableData);
void AssemblyProcessor_Link(CLIFile* pFile);


struct _AssemblyReference
{
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
    CustomAttribute** CustomAttributes;
};

uint8_t* AssemblyReference_Initialize(CLIFile* pFile, uint8_t* pTableData);
void AssemblyReference_Cleanup(CLIFile* pFile);
uint8_t* AssemblyReference_Load(CLIFile* pFile, uint8_t* pTableData);
void AssemblyReference_Link(CLIFile* pFile);


struct _AssemblyReferenceOperatingSystem
{
	uint32_t TableIndex;
    uint32_t PlatformID;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
    AssemblyReference* AssemblyReference;
};

uint8_t* AssemblyReferenceOperatingSystem_Initialize(CLIFile* pFile, uint8_t* pTableData);
void AssemblyReferenceOperatingSystem_Cleanup(CLIFile* pFile);
uint8_t* AssemblyReferenceOperatingSystem_Load(CLIFile* pFile, uint8_t* pTableData);
void AssemblyReferenceOperatingSystem_Link(CLIFile* pFile);


struct _AssemblyReferenceProcessor
{
	uint32_t TableIndex;
    uint32_t Processor;
    AssemblyReference* AssemblyReference;
};

uint8_t* AssemblyReferenceProcessor_Initialize(CLIFile* pFile, uint8_t* pTableData);
void AssemblyReferenceProcessor_Cleanup(CLIFile* pFile);
uint8_t* AssemblyReferenceProcessor_Load(CLIFile* pFile, uint8_t* pTableData);
void AssemblyReferenceProcessor_Link(CLIFile* pFile);


struct _ClassLayout
{
	uint32_t TableIndex;
    uint16_t PackingSize;
    uint32_t ClassSize;
    TypeDefinition* Parent;
};

uint8_t* ClassLayout_Initialize(CLIFile* pFile, uint8_t* pTableData);
void ClassLayout_Cleanup(CLIFile* pFile);
uint8_t* ClassLayout_Load(CLIFile* pFile, uint8_t* pTableData);
void ClassLayout_Link(CLIFile* pFile);


struct _Constant
{
	uint32_t TableIndex;
    uint8_t Type;
    HasConstantUnion(Parent)
    uint32_t ValueLength;
    const uint8_t* Value;
};

uint8_t* Constant_Initialize(CLIFile* pFile, uint8_t* pTableData);
void Constant_Cleanup(CLIFile* pFile);
uint8_t* Constant_Load(CLIFile* pFile, uint8_t* pTableData);
void Constant_Link(CLIFile* pFile);


struct _CustomAttribute
{
	uint32_t TableIndex;
    HasCustomAttributeUnion(Parent)
    CustomAttributeTypeUnion(Type)
    uint32_t ValueLength;
    const uint8_t* Value;
};

uint8_t* CustomAttribute_Initialize(CLIFile* pFile, uint8_t* pTableData);
void CustomAttribute_Cleanup(CLIFile* pFile);
uint8_t* CustomAttribute_Load(CLIFile* pFile, uint8_t* pTableData);
void CustomAttribute_Link(CLIFile* pFile);


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

uint8_t* DeclSecurity_Initialize(CLIFile* pFile, uint8_t* pTableData);
void DeclSecurity_Cleanup(CLIFile* pFile);
uint8_t* DeclSecurity_Load(CLIFile* pFile, uint8_t* pTableData);
void DeclSecurity_Link(CLIFile* pFile);


struct _Event
{
	uint32_t TableIndex;
    uint16_t Flags;
    const char* Name;
    TypeDefOrRefUnion(EventType)

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    MethodSemantics* MethodSemantics;
};

uint8_t* Event_Initialize(CLIFile* pFile, uint8_t* pTableData);
void Event_Cleanup(CLIFile* pFile);
uint8_t* Event_Load(CLIFile* pFile, uint8_t* pTableData);
void Event_Link(CLIFile* pFile);


struct _EventMap
{
	uint32_t TableIndex;
    TypeDefinition* Parent;
    Event* EventList;
    uint32_t EventListCount;
};

uint8_t* EventMap_Initialize(CLIFile* pFile, uint8_t* pTableData);
void EventMap_Cleanup(CLIFile* pFile);
uint8_t* EventMap_Load(CLIFile* pFile, uint8_t* pTableData);
void EventMap_Link(CLIFile* pFile);


struct _ExportedType
{
	uint32_t TableIndex;
    uint32_t Flags;
    TypeDefinition* TypeDefinitionID;
    const char* Name;
    const char* Namespace;
    ImplementationUnion(Implementation)

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

uint8_t* ExportedType_Initialize(CLIFile* pFile, uint8_t* pTableData);
void ExportedType_Cleanup(CLIFile* pFile);
uint8_t* ExportedType_Load(CLIFile* pFile, uint8_t* pTableData);
void ExportedType_Link(CLIFile* pFile);


struct _Field
{
	CLIFile* File;
	uint32_t TableIndex;
    uint16_t Flags;
    const char* Name;
    uint32_t SignatureLength;
    uint8_t* Signature;

    Constant* Constant;
    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    FieldMarshal* FieldMarshal;
    ImplementationMap* ImplementationMap;
	TypeDefinition* TypeDefinition;
};

uint8_t* Field_Initialize(CLIFile* pFile, uint8_t* pTableData);
void Field_Cleanup(CLIFile* pFile);
uint8_t* Field_Load(CLIFile* pFile, uint8_t* pTableData);
void Field_Link(CLIFile* pFile);


struct _FieldLayout
{
	uint32_t TableIndex;
    uint32_t Offset;
    Field* Field;
};

uint8_t* FieldLayout_Initialize(CLIFile* pFile, uint8_t* pTableData);
void FieldLayout_Cleanup(CLIFile* pFile);
uint8_t* FieldLayout_Load(CLIFile* pFile, uint8_t* pTableData);
void FieldLayout_Link(CLIFile* pFile);


struct _FieldMarshal
{
	uint32_t TableIndex;
    HasFieldMarshalUnion(Parent)
    uint32_t NativeTypeLength;
    const uint8_t* NativeType;
};

uint8_t* FieldMarshal_Initialize(CLIFile* pFile, uint8_t* pTableData);
void FieldMarshal_Cleanup(CLIFile* pFile);
uint8_t* FieldMarshal_Load(CLIFile* pFile, uint8_t* pTableData);
void FieldMarshal_Link(CLIFile* pFile);


struct _FieldRVA
{
	uint32_t TableIndex;
    const uint8_t* InitialValue;
    Field* Field;
};

uint8_t* FieldRVA_Initialize(CLIFile* pFile, uint8_t* pTableData);
void FieldRVA_Cleanup(CLIFile* pFile);
uint8_t* FieldRVA_Load(CLIFile* pFile, uint8_t* pTableData);
void FieldRVA_Link(CLIFile* pFile);


struct _File
{
	uint32_t TableIndex;
    uint32_t Flags;
    const char* Name;
    uint32_t HashValueLength;
    const uint8_t* HashValue;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

uint8_t* File_Initialize(CLIFile* pFile, uint8_t* pTableData);
void File_Cleanup(CLIFile* pFile);
uint8_t* File_Load(CLIFile* pFile, uint8_t* pTableData);
void File_Link(CLIFile* pFile);


struct _GenericParameter
{
	uint32_t TableIndex;
    uint16_t Index;
    uint16_t Flags;
    TypeOrMethodDefUnion(Owner)
    const char* Name;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    uint32_t GenericParameterConstraintCount;
    GenericParameterConstraint** GenericParameterConstraints;
};

uint8_t* GenericParameter_Initialize(CLIFile* pFile, uint8_t* pTableData);
void GenericParameter_Cleanup(CLIFile* pFile);
uint8_t* GenericParameter_Load(CLIFile* pFile, uint8_t* pTableData);
void GenericParameter_Link(CLIFile* pFile);


struct _GenericParameterConstraint
{
	uint32_t TableIndex;
    GenericParameter* Owner;
    TypeDefOrRefUnion(Constraint)

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

uint8_t* GenericParameterConstraint_Initialize(CLIFile* pFile, uint8_t* pTableData);
void GenericParameterConstraint_Cleanup(CLIFile* pFile);
uint8_t* GenericParameterConstraint_Load(CLIFile* pFile, uint8_t* pTableData);
void GenericParameterConstraint_Link(CLIFile* pFile);


struct _ImplementationMap
{
	uint32_t TableIndex;
    uint16_t MappingFlags;
    MemberForwardedUnion(MemberForwarded)
    const char* ImportName;
    ModuleReference* ImportScope;
};

uint8_t* ImplementationMap_Initialize(CLIFile* pFile, uint8_t* pTableData);
void ImplementationMap_Cleanup(CLIFile* pFile);
uint8_t* ImplementationMap_Load(CLIFile* pFile, uint8_t* pTableData);
void ImplementationMap_Link(CLIFile* pFile);


struct _InterfaceImplementation
{
	uint32_t TableIndex;
    TypeDefinition* Implementor;
    TypeDefOrRefUnion(Interface)

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

uint8_t* InterfaceImplementation_Initialize(CLIFile* pFile, uint8_t* pTableData);
void InterfaceImplementation_Cleanup(CLIFile* pFile);
uint8_t* InterfaceImplementation_Load(CLIFile* pFile, uint8_t* pTableData);
void InterfaceImplementation_Link(CLIFile* pFile);


struct _ManifestResource
{
	uint32_t TableIndex;
    uint32_t Offset;
    uint32_t Flags;
    const char* Name;
    ImplementationUnion(Implementation)

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

uint8_t* ManifestResource_Initialize(CLIFile* pFile, uint8_t* pTableData);
void ManifestResource_Cleanup(CLIFile* pFile);
uint8_t* ManifestResource_Load(CLIFile* pFile, uint8_t* pTableData);
void ManifestResource_Link(CLIFile* pFile);


struct _MemberReference
{
	uint32_t TableIndex;
    MemberRefParentUnion(Parent)
    const char* Name;
    uint32_t SignatureLength;
    uint8_t* Signature;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;

	uint8_t TypeOfResolved;
	union
	{
		MethodDefinition* MethodDefinition;
		Field* Field;
	} Resolved;
};

uint8_t* MemberReference_Initialize(CLIFile* pFile, uint8_t* pTableData);
void MemberReference_Cleanup(CLIFile* pFile);
uint8_t* MemberReference_Load(CLIFile* pFile, uint8_t* pTableData);
void MemberReference_Link(CLIFile* pFile);


#define MethodDefinitionBody__Offset_Flags								0x00

#define MethodDefinitionBody__Offset_Fat_Flags							0x00
#define MethodDefinitionBody__Offset_Fat_MaxStack						0x02
#define MethodDefinitionBody__Offset_Fat_CodeSize						0x04
#define MethodDefinitionBody__Offset_Fat_LocalVariableSignatureToken	0x08

typedef enum MethodDefinitionBodyFlags
{
	MethodDefinitionBodyFlags_HeaderType_Fat							= 0x03,
} MethodDefinitionBodyFlags;

#define MethodDefinitionBodyFlags_HeaderType_Bits						2
#define MethodDefinitionBodyFlags_HeaderType_Mask						((1 << MethodDefinitionBodyFlags_HeaderType_Bits) - 1)

typedef enum MethodDefinitionBodyFatFlags
{
	MethodDefinitionBody_Fat_Flags_HasDataSection						= 0x08,
	MethodDefinitionBody_Fat_Flags_InitializeLocals						= 0x10,
} MethodDefinitionBodyFatFlags;

#define MethodDefinitionBody_Fat_Flags_Bits								12
#define MethodDefinitionBody_Fat_Flags_Mask								((1 << MethodDefinitionBodyFlags_HeaderType_Bits) - 1)

#define MethodDefinitionBody_Tiny_MaxStack								8
#define MethodDefinitionBody_Fat_BodySize_Multiplier					4

struct _MethodDefinitionBody
{
    uint16_t Flags;
    bool_t IsFat;
    uint16_t MaxStack;
    uint32_t CodeSize;
    uint32_t LocalVariableSignatureToken;
    const uint8_t* Code;
};

#define MethodDefinitionException__Offset_Flags							0x00
#define MethodDefinitionException__Offset_TryOffset						0x02
#define MethodDefinitionException__Offset_TryLength						0x04
#define MethodDefinitionException__Offset_HandlerOffset					0x05
#define MethodDefinitionException__Offset_HandlerLength					0x07
#define MethodDefinitionException__Offset_ClassTokenOrFilterOffset		0x08
#define MethodDefinitionException__HeaderLength							0x0C

#define MethodDefinitionException__Offset_Fat_Flags						0x00
#define MethodDefinitionException__Offset_Fat_TryOffset					0x04
#define MethodDefinitionException__Offset_Fat_TryLength					0x08
#define MethodDefinitionException__Offset_Fat_HandlerOffset				0x0C
#define MethodDefinitionException__Offset_Fat_HandlerLength				0x10
#define MethodDefinitionException__Offset_Fat_ClassTokenOrFilterOffset	0x14
#define MethodDefinitionException__Fat_HeaderLength						0x18

struct _MethodDefinitionException
{
    uint32_t Flags;
    uint32_t TryOffset;
    uint32_t TryLength;
    uint32_t HandlerOffset;
    uint32_t HandlerLength;
    uint32_t ClassTokenOrFilterOffset;
};

#define MethodDefinitionData__ByteAlignment								0x04

#define MethodDefinitionData__Offset_Flags								0x00
#define MethodDefinitionData__Offset_Size								0x01
#define MethodDefinitionData__HeaderLength								0x04

typedef enum MethodDefinitionDataFlags
{
	MethodDefinitionDataFlags_ExceptionTable							= 0x01,
	MethodDefinitionDataFlags_OptionalILTable							= 0x02,
	MethodDefinitionDataFlags_Fat										= 0x40,
	MethodDefinitionDataFlags_HasAnotherDataSection						= 0x80,
} MethodDefinitionDataFlags;

#define MethodDefinitionData_Fat_Size_Mask								0x00FFFFFF

struct _MethodDefinition
{
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
    Parameter* ParameterList;
    uint32_t ParameterListCount;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    DeclSecurity* DeclSecurity;
    uint32_t GenericParameterCount;
    GenericParameter** GenericParameters;
    ImplementationMap* ImplementationMap;
    uint32_t MemberReferenceCount;
    MemberReference** MemberReferences;
    TypeDefinition* TypeDefinition;

	void* InternalCall;
};

uint8_t* MethodDefinition_Initialize(CLIFile* pFile, uint8_t* pTableData);
void MethodDefinition_Cleanup(CLIFile* pFile);
uint8_t* MethodDefinition_Load(CLIFile* pFile, uint8_t* pTableData);
void MethodDefinition_Link(CLIFile* pFile);


struct _MethodImplementation
{
	uint32_t TableIndex;
    TypeDefinition* Parent;
    MethodDefOrRefUnion(MethodBody)
    MethodDefOrRefUnion(MethodDeclaration)
};

uint8_t* MethodImplementation_Initialize(CLIFile* pFile, uint8_t* pTableData);
void MethodImplementation_Cleanup(CLIFile* pFile);
uint8_t* MethodImplementation_Load(CLIFile* pFile, uint8_t* pTableData);
void MethodImplementation_Link(CLIFile* pFile);


struct _MethodSemantics
{
	uint32_t TableIndex;
    uint16_t Semantics;
    MethodDefinition* Method;
    HasSemanticsUnion(Association)
};

uint8_t* MethodSemantics_Initialize(CLIFile* pFile, uint8_t* pTableData);
void MethodSemantics_Cleanup(CLIFile* pFile);
uint8_t* MethodSemantics_Load(CLIFile* pFile, uint8_t* pTableData);
void MethodSemantics_Link(CLIFile* pFile);


struct _MethodSpecification
{
	uint32_t TableIndex;
    MethodDefOrRefUnion(Method)
    uint32_t InstantiationLength;
    const uint8_t* Instantiation;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

uint8_t* MethodSpecification_Initialize(CLIFile* pFile, uint8_t* pTableData);
void MethodSpecification_Cleanup(CLIFile* pFile);
uint8_t* MethodSpecification_Load(CLIFile* pFile, uint8_t* pTableData);
void MethodSpecification_Link(CLIFile* pFile);


struct _ModuleDefinition
{
	uint32_t TableIndex;
    uint16_t Generation;
    const char* Name;
    const uint8_t* ModuleVersionID;
    const uint8_t* EncID;
    const uint8_t* EncBaseID;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

uint8_t* ModuleDefinition_Initialize(CLIFile* pFile, uint8_t* pTableData);
void ModuleDefinition_Cleanup(CLIFile* pFile);
uint8_t* ModuleDefinition_Load(CLIFile* pFile, uint8_t* pTableData);
void ModuleDefinition_Link(CLIFile* pFile);


struct _ModuleReference
{
	uint32_t TableIndex;
    const char* Name;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    uint32_t MemberReferenceCount;
    MemberReference** MemberReferences;
};

uint8_t* ModuleReference_Initialize(CLIFile* pFile, uint8_t* pTableData);
void ModuleReference_Cleanup(CLIFile* pFile);
uint8_t* ModuleReference_Load(CLIFile* pFile, uint8_t* pTableData);
void ModuleReference_Link(CLIFile* pFile);


struct _NestedClass
{
	uint32_t TableIndex;
    TypeDefinition* Nested;
    TypeDefinition* Enclosing;
};

uint8_t* NestedClass_Initialize(CLIFile* pFile, uint8_t* pTableData);
void NestedClass_Cleanup(CLIFile* pFile);
uint8_t* NestedClass_Load(CLIFile* pFile, uint8_t* pTableData);
void NestedClass_Link(CLIFile* pFile);


struct _Parameter
{
	uint32_t TableIndex;
    uint16_t Flags;
    uint16_t Sequence;
    const char* Name;

    Constant* Constant;
    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    FieldMarshal* FieldMarshal;
};

uint8_t* Parameter_Initialize(CLIFile* pFile, uint8_t* pTableData);
void Parameter_Cleanup(CLIFile* pFile);
uint8_t* Parameter_Load(CLIFile* pFile, uint8_t* pTableData);
void Parameter_Link(CLIFile* pFile);


struct _Property
{
	uint32_t TableIndex;
    uint16_t Flags;
    const char* Name;
    uint32_t SignatureLength;
    uint8_t* Signature;

    Constant* Constant;
    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    MethodSemantics* MethodSemantics;
};

uint8_t* Property_Initialize(CLIFile* pFile, uint8_t* pTableData);
void Property_Cleanup(CLIFile* pFile);
uint8_t* Property_Load(CLIFile* pFile, uint8_t* pTableData);
void Property_Link(CLIFile* pFile);


struct _PropertyMap
{
	uint32_t TableIndex;
    TypeDefinition* Parent;
    Property* PropertyList;
    uint32_t PropertyListCount;
};

uint8_t* PropertyMap_Initialize(CLIFile* pFile, uint8_t* pTableData);
void PropertyMap_Cleanup(CLIFile* pFile);
uint8_t* PropertyMap_Load(CLIFile* pFile, uint8_t* pTableData);
void PropertyMap_Link(CLIFile* pFile);


struct _StandAloneSignature
{
	uint32_t TableIndex;
    uint32_t SignatureLength;
    uint8_t* Signature;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
};

uint8_t* StandAloneSignature_Initialize(CLIFile* pFile, uint8_t* pTableData);
void StandAloneSignature_Cleanup(CLIFile* pFile);
uint8_t* StandAloneSignature_Load(CLIFile* pFile, uint8_t* pTableData);
void StandAloneSignature_Link(CLIFile* pFile);


struct _TypeDefinition
{
	CLIFile* File;
	uint32_t TableIndex;
    uint32_t Flags;
    const char* Name;
    const char* Namespace;
    TypeDefOrRefUnion(Extends)
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
};

uint8_t* TypeDefinition_Initialize(CLIFile* pFile, uint8_t* pTableData);
void TypeDefinition_Cleanup(CLIFile* pFile);
uint8_t* TypeDefinition_Load(CLIFile* pFile, uint8_t* pTableData);
void TypeDefinition_Link(CLIFile* pFile);


struct _TypeReference
{
	uint32_t TableIndex;
    ResolutionScopeUnion(ResolutionScope)
	ExportedType* ExportedType;
    const char* Name;
    const char* Namespace;
	TypeDefinition* ResolvedType;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    uint32_t MemberReferenceCount;
    MemberReference** MemberReferences;
};

uint8_t* TypeReference_Initialize(CLIFile* pFile, uint8_t* pTableData);
void TypeReference_Cleanup(CLIFile* pFile);
uint8_t* TypeReference_Load(CLIFile* pFile, uint8_t* pTableData);
void TypeReference_Link(CLIFile* pFile);


struct _TypeSpecification
{
	CLIFile* File;
	uint32_t TableIndex;
    uint32_t SignatureLength;
    uint8_t* Signature;

    uint32_t CustomAttributeCount;
    CustomAttribute** CustomAttributes;
    uint32_t MemberReferenceCount;
    MemberReference** MemberReferences;
};

uint8_t* TypeSpecification_Initialize(CLIFile* pFile, uint8_t* pTableData);
void TypeSpecification_Cleanup(CLIFile* pFile);
uint8_t* TypeSpecification_Load(CLIFile* pFile, uint8_t* pTableData);
void TypeSpecification_Link(CLIFile* pFile);


// Signatures

typedef enum SignatureElementType
{
	SignatureElementType_End							= 0x00,
	SignatureElementType_Void							= 0x01,
	SignatureElementType_Boolean						= 0x02,
	SignatureElementType_Char							= 0x03,
	SignatureElementType_I1								= 0x04,
	SignatureElementType_U1								= 0x05,
	SignatureElementType_I2								= 0x06,
	SignatureElementType_U2								= 0x07,
	SignatureElementType_I4								= 0x08,
	SignatureElementType_U4								= 0x09,
	SignatureElementType_I8								= 0x0A,
	SignatureElementType_U8								= 0x0B,
	SignatureElementType_R4								= 0x0C,
	SignatureElementType_R8								= 0x0D,
	SignatureElementType_String							= 0x0E,
	SignatureElementType_Pointer						= 0x0F,
	SignatureElementType_ByReference					= 0x10,
	SignatureElementType_ValueType						= 0x11,
	SignatureElementType_Class							= 0x12,
	SignatureElementType_Var							= 0x13,
	SignatureElementType_Array							= 0x14,
	SignatureElementType_GenericInstantiation			= 0x15,
	SignatureElementType_TypedByReference				= 0x16,
	SignatureElementType_IPointer						= 0x18,
	SignatureElementType_UPointer						= 0x19,
	SignatureElementType_FunctionPointer				= 0x1B,
	SignatureElementType_Object							= 0x1C,
	SignatureElementType_SingleDimensionArray			= 0x1D,
	SignatureElementType_MethodVar						= 0x1E,
	SignatureElementType_CustomModifier_Required		= 0x1F,
	SignatureElementType_CustomModifier_Optional		= 0x20,
	SignatureElementType_Internal						= 0x21,
	SignatureElementType_Modifier						= 0x40,
	SignatureElementType_Sentinel						= 0x41,
	SignatureElementType_Pinned							= 0x45,
	SignatureElementType_Type							= 0x50,
	SignatureElementType_CustomAttribute_Boxed			= 0x51,
	SignatureElementType_CustomAttribute_Field			= 0x53,
	SignatureElementType_CustomAttribute_Property		= 0x54,
	SignatureElementType_CustomAttribute_Enum			= 0x55,
} SignatureElementType;

typedef enum SignatureCallConvention
{
	SignatureCallConvention_Default						= 0x00,
	SignatureCallConvention_C							= 0x01,
	SignatureCallConvention_STDCall						= 0x02,
	SignatureCallConvention_ThisCall					= 0x03,
	SignatureCallConvention_FastCall					= 0x04,
	SignatureCallConvention_VarArgs						= 0x05,
	SignatureCallConvention_Generic						= 0x10,
	SignatureCallConvention_HasThis						= 0x20,
	SignatureCallConvention_ExplicitThis				= 0x40,
} SignatureCallConvention;

typedef struct _MethodSignature MethodSignature;
typedef struct _FieldSignature FieldSignature;
typedef struct _PropertySignature PropertySignature;
typedef struct _LocalsSignature LocalsSignature;

typedef struct _SignatureReturnType SignatureReturnType;
typedef struct _SignatureParameter SignatureParameter;
typedef struct _SignatureCustomModifier SignatureCustomModifier;
typedef struct _SignatureType SignatureType;
typedef struct _SignatureMethodSpecification SignatureMethodSpecification;
typedef struct _SignatureArrayShape SignatureArrayShape;
typedef struct _SignatureLocalVariable SignatureLocalVariable;

bool_t Signature_Equals(uint8_t* pSignature1, uint32_t pSignature1Length, uint8_t* pSignature2, uint32_t pSignature2Length);

struct _MethodSignature
{
    bool_t HasThis;
    bool_t ExplicitThis;
    bool_t Default;
    bool_t VarArgs;
    bool_t Generic;
    bool_t CCall;
    bool_t STDCall;
    bool_t ThisCall;
    bool_t FastCall;
    uint32_t GenericParameterCount;
    SignatureReturnType* ReturnType;
    uint32_t ParameterCount;
    SignatureParameter** Parameters;
    bool_t HasSentinel;
    uint32_t SentinelIndex;
};

MethodSignature* MethodSignature_Create();
void MethodSignature_Destroy(MethodSignature* pMethodSignature);
uint8_t* MethodSignature_Parse(uint8_t* pCursor, MethodSignature** pMethodSignature, CLIFile* pCLIFile);
MethodSignature* MethodSignature_Expand(uint8_t* pSignature, CLIFile* pCLIFile);

struct _FieldSignature
{
    uint32_t CustomModifierCount;
    SignatureCustomModifier** CustomModifiers;
    SignatureType* Type;
};

FieldSignature* FieldSignature_Create();
void FieldSignature_Destroy(FieldSignature* pFieldSignature);
uint8_t* FieldSignature_Parse(uint8_t* pCursor, FieldSignature** pFieldSignature, CLIFile* pCLIFile);
FieldSignature* FieldSignature_Expand(uint8_t* pSignature, CLIFile* pCLIFile);

struct _PropertySignature
{
    bool_t HasThis;
    uint32_t CustomModifierCount;
    SignatureCustomModifier** CustomModifiers;
    SignatureType* Type;
    uint32_t ParameterCount;
    SignatureParameter** Parameters;
};

PropertySignature* PropertySignature_Create();
void PropertySignature_Destroy(PropertySignature* pPropertySignature);
uint8_t* PropertySignature_Parse(uint8_t* pCursor, PropertySignature** pPropertySignature, CLIFile* pCLIFile);
PropertySignature* PropertySignature_Expand(uint8_t* pSignature, CLIFile* pCLIFile);

struct _LocalsSignature
{
    uint32_t LocalVariableCount;
    SignatureLocalVariable** LocalVariables;
};

LocalsSignature* LocalsSignature_Create();
void LocalsSignature_Destroy(LocalsSignature* pLocalsSignature);
uint8_t* LocalsSignature_Parse(uint8_t* pCursor, LocalsSignature** pLocalsSignature, CLIFile* pCLIFile);
LocalsSignature* LocalsSignature_Expand(uint8_t* pSignature, CLIFile* pCLIFile);

struct _SignatureReturnType
{
    uint32_t CustomModifierCount;
    SignatureCustomModifier** CustomModifiers;
    bool_t ByReference;
    SignatureType* Type;
    bool_t TypedByReference;
    bool_t Void;
};

SignatureReturnType* SignatureReturnType_Create();
void SignatureReturnType_Destroy(SignatureReturnType* pReturnType);
uint8_t* SignatureReturnType_Parse(uint8_t* pCursor, SignatureReturnType** pReturnType, CLIFile* pCLIFile);

struct _SignatureParameter
{
    uint32_t CustomModifierCount;
    SignatureCustomModifier** CustomModifiers;
    bool_t ByReference;
    SignatureType* Type;
    bool_t TypedByReference;
};

SignatureParameter* SignatureParameter_Create();
void SignatureParameter_Destroy(SignatureParameter* pParameter);
uint8_t* SignatureParameter_Parse(uint8_t* pCursor, SignatureParameter** pParameter, CLIFile* pCLIFile);

struct _SignatureCustomModifier
{
    bool_t Optional;
    uint32_t TypeDefOrRefOrSpecToken;
    bool_t Pinned;
};

SignatureCustomModifier* SignatureCustomModifier_Create();
void SignatureCustomModifier_Destroy(SignatureCustomModifier* pCustomModifier);
uint8_t* SignatureCustomModifier_Parse(uint8_t* pCursor, SignatureCustomModifier** pCustomModifier, CLIFile* pCLIFile);

struct _SignatureType
{
    uint8_t ElementType;
    SignatureType* ArrayType;
    SignatureArrayShape* ArrayShape;
    uint32_t ClassTypeDefOrRefOrSpecToken;
    MethodSignature* FnPtrMethodSignature;
    bool_t GenericInstClass;
    bool_t GenericInstValue;
    uint32_t GenericInstTypeDefOrRefOrSpecToken;
    uint32_t GenericInstGenericArgumentCount;
    SignatureType** GenericInstGenericArguments;
    uint32_t MVarNumber;
    uint32_t PtrCustomModifierCount;
    SignatureCustomModifier** PtrCustomModifiers;
    SignatureType* PtrType;
    bool_t PtrVoid;
    uint32_t SZArrayCustomModifierCount;
    SignatureCustomModifier** SZArrayCustomModifiers;
    SignatureType* SZArrayType;
    uint32_t ValueTypeDefOrRefOrSpecToken;
    uint32_t VarNumber;
};

SignatureType* SignatureType_Create();
void SignatureType_Destroy(SignatureType* pType);
uint8_t* SignatureType_Parse(uint8_t* pCursor, SignatureType** pType, CLIFile* pCLIFile);
SignatureType* SignatureType_Expand(uint8_t* pSignature, CLIFile* pCLIFile);

struct _SignatureMethodSpecification
{
    uint32_t GenericInstGenericArgumentCount;
    SignatureType** GenericInstGenericArguments;
};

SignatureMethodSpecification* SignatureMethodSpecification_Create();
void SignatureMethodSpecification_Destroy(SignatureMethodSpecification* pMethodSpecification);
uint8_t* SignatureMethodSpecification_Parse(uint8_t* pCursor, SignatureMethodSpecification** pMethodSpecification, CLIFile* pCLIFile);
SignatureMethodSpecification* SignatureMethodSpecification_Expand(uint8_t* pSignature, CLIFile* pCLIFile);

struct _SignatureArrayShape
{
    uint32_t Rank;
    uint32_t SizeCount;
    uint32_t* Sizes;
    uint32_t LowerBoundCount;
    int32_t* LowerBounds;
};

SignatureArrayShape* SignatureArrayShape_Create();
void SignatureArrayShape_Destroy(SignatureArrayShape* pArrayShape);
uint8_t* SignatureArrayShape_Parse(uint8_t* pCursor, SignatureArrayShape** pArrayShape, CLIFile* pCLIFile);

struct _SignatureLocalVariable
{
    uint32_t CustomModifierCount;
    SignatureCustomModifier** CustomModifiers;
    bool_t ByReference;
    SignatureType* Type;
    bool_t TypedByReference;
};

SignatureLocalVariable* SignatureLocalVariable_Create();
void SignatureLocalVariable_Destroy(SignatureLocalVariable* pLocalVariable);
uint8_t* SignatureLocalVariable_Parse(uint8_t* pCursor, SignatureLocalVariable** pLocalVariable, CLIFile* pCLIFile);
