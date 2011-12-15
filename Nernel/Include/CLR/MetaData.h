#pragma once

#define MetaData_Signature                  0x424A5342

#define MetaData_Offset_Signature           0x00
#define MetaData_Offset_MajorVersion        0x04
#define MetaData_Offset_MinorVersion        0x06
#define MetaData_Offset_Reserved            0x08
#define MetaData_Offset_VersionLength       0x0C
#define MetaData_Offset_Version             0x10

#define MetaData_Offset_Flags               0x00
#define MetaData_Offset_StreamCount         0x02
#define MetaData_Offset_Streams             0x04

#define MetaData_Stream_Offset_RVA          0x00;
#define MetaData_Stream_Offset_Size         0x04;
#define MetaData_Stream_Offset_Name         0x08;

bool_t MetaData_IsValidSignature(const uint8_t* pMetaDataHeader);
uint32_t MetaData_GetVersionLength(const uint8_t* pMetaDataHeader);
const char* MetaData_GetVersion(const uint8_t* pMetaDataHeader);
uint16_t MetaData_GetStreamCount(const uint8_t* pMetaDataHeader);
const uint8_t* MetaData_GetStream(const uint8_t* pMetaDataHeader, uint16_t pStreamIndex);
uint32_t MetaData_GetStreamRVA(const uint8_t* pMetaDataHeader, uint16_t pStreamIndex);
uint32_t MetaData_GetStreamSize(const uint8_t* pMetaDataHeader, uint16_t pStreamIndex);
const char* MetaData_GetStreamName(const uint8_t* pMetaDataHeader, uint16_t pStreamIndex);
const uint8_t* MetaData_GetStreamData(const uint8_t* pMetaDataHeader, uint16_t pStreamIndex);

#define MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit   0x01
#define MetaDataTablesHeader_HeapOffsetSizes_GUIDs32Bit     0x02
#define MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit     0x04

#define MetaData_Table_ModuleDefinition                     0x00
#define MetaData_Table_TypeReference                        0x01
#define MetaData_Table_TypeDefinition                       0x02
#define MetaData_Table_Field                                0x04
#define MetaData_Table_MethodDefinition                     0x06
#define MetaData_Table_Parameter                            0x08
#define MetaData_Table_InterfaceImplementation              0x09
#define MetaData_Table_MemberReference                      0x0A
#define MetaData_Table_Constant                             0x0B
#define MetaData_Table_CustomAttribute                      0x0C
#define MetaData_Table_FieldMarshal                         0x0D
#define MetaData_Table_DeclSecurity                         0x0E
#define MetaData_Table_ClassLayout                          0x0F
#define MetaData_Table_FieldLayout                          0x10
#define MetaData_Table_StandAloneSignature                  0x11
#define MetaData_Table_EventMap                             0x12
#define MetaData_Table_Event                                0x14
#define MetaData_Table_PropertyMap                          0x15
#define MetaData_Table_Property                             0x17
#define MetaData_Table_MethodSemantics                      0x18
#define MetaData_Table_MethodImplementation                 0x19
#define MetaData_Table_ModuleReference                      0x1A
#define MetaData_Table_TypeSpecification                    0x1B
#define MetaData_Table_ImplementationMap                    0x1C
#define MetaData_Table_FieldRVA                             0x1D
#define MetaData_Table_AssemblyDefinition                   0x20
#define MetaData_Table_AssemblyProcessor                    0x21
#define MetaData_Table_AssemblyOperatingSystem              0x22
#define MetaData_Table_AssemblyReference                    0x23
#define MetaData_Table_AssemblyReferenceProcessor           0x24
#define MetaData_Table_AssemblyReferenceOperatingSystem     0x25
#define MetaData_Table_File                                 0x26
#define MetaData_Table_ExportedType                         0x27
#define MetaData_Table_ManifestResource                     0x28
#define MetaData_Table_NestedClass                          0x29
#define MetaData_Table_GenericParameter                     0x2A
#define MetaData_Table_MethodSpecification                  0x2B
#define MetaData_Table_GenericParameterConstraint           0x2C

typedef struct
{
    uint32_t Reserved;
    uint8_t MajorVersion;
    uint8_t MinorVersion;
    uint8_t HeapOffsetSizes;
    uint8_t MoreReserved;
    uint64_t PresentTables;
    uint64_t SortedTables;
} MetaDataTablesHeader;


#define TypeDefOrRef_Type_TypeDefinition                    0x00
#define TypeDefOrRef_Type_TypeReference                     0x01
#define TypeDefOrRef_Type_TypeSpecification                 0x02
#define TypeDefOrRef_Type_Bits                              0x02
#define TypeDefOrRef_Type_Mask                              0x03
#define TypeDefOrRef_Type_MaxRows16Bit                      (0xFFFF >> TypeDefOrRef_Type_Bits)

#define HasConstant_Type_Field                              0x00
#define HasConstant_Type_Parameter                          0x01
#define HasConstant_Type_Property                           0x02
#define HasConstant_Type_Bits                               0x02
#define HasConstant_Type_Mask                               0x03
#define HasConstant_Type_MaxRows16Bit                       (0xFFFF >> HasConstant_Type_Bits)

#define HasCustomAttribute_Type_MethodDefinition            0x00
#define HasCustomAttribute_Type_Field                       0x01
#define HasCustomAttribute_Type_TypeReference               0x02
#define HasCustomAttribute_Type_TypeDefinition              0x03
#define HasCustomAttribute_Type_Parameter                   0x04
#define HasCustomAttribute_Type_InterfaceImplementation     0x05
#define HasCustomAttribute_Type_MemberReference             0x06
#define HasCustomAttribute_Type_ModuleDefinition            0x07
#define HasCustomAttribute_Type_DeclSecurity                0x08
#define HasCustomAttribute_Type_Property                    0x09
#define HasCustomAttribute_Type_Event                       0x0A
#define HasCustomAttribute_Type_StandAloneSignature         0x0B
#define HasCustomAttribute_Type_ModuleReference             0x0C
#define HasCustomAttribute_Type_TypeSpecification           0x0D
#define HasCustomAttribute_Type_AssemblyDefinition          0x0E
#define HasCustomAttribute_Type_AssemblyReference           0x0F
#define HasCustomAttribute_Type_File                        0x10
#define HasCustomAttribute_Type_ExportedType                0x11
#define HasCustomAttribute_Type_ManifestResource            0x12
#define HasCustomAttribute_Type_GenericParameter            0x13
#define HasCustomAttribute_Type_GenericParameterConstraint  0x14
#define HasCustomAttribute_Type_MethodSpecification         0x15
#define HasCustomAttribute_Type_Bits                        0x05
#define HasCustomAttribute_Type_Mask                        0x1F
#define HasCustomAttribute_Type_MaxRows16Bit                (0xFFFF >> HasCustomAttribute_Type_Bits)

#define HasFieldMarshal_Type_Field                          0x00
#define HasFieldMarshal_Type_Parameter                      0x01
#define HasFieldMarshal_Type_Bits                           0x01
#define HasFieldMarshal_Type_Mask                           0x01
#define HasFieldMarshal_Type_MaxRows16Bit                   (0xFFFF >> HasFieldMarshal_Type_Bits)

#define HasDeclSecurity_Type_TypeDefinition                 0x00
#define HasDeclSecurity_Type_MethodDefinition               0x01
#define HasDeclSecurity_Type_AssemblyDefinition             0x02
#define HasDeclSecurity_Type_Bits                           0x02
#define HasDeclSecurity_Type_Mask                           0x03
#define HasDeclSecurity_Type_MaxRows16Bit                   (0xFFFF >> HasDeclSecurity_Type_Bits)

#define MemberRefParent_Type_TypeDefinition                 0x00
#define MemberRefParent_Type_TypeReference                  0x01
#define MemberRefParent_Type_ModuleReference                0x02
#define MemberRefParent_Type_MethodDefinition               0x03
#define MemberRefParent_Type_TypeSpecification              0x04
#define MemberRefParent_Type_Bits                           0x03
#define MemberRefParent_Type_Mask                           0x07
#define MemberRefParent_Type_MaxRows16Bit                   (0xFFFF >> MemberRefParent_Type_Bits)

#define HasSemantics_Type_Event                             0x00
#define HasSemantics_Type_Property                          0x01
#define HasSemantics_Type_Bits                              0x01
#define HasSemantics_Type_Mask                              0x01
#define HasSemantics_Type_MaxRows16Bit                      (0xFFFF >> HasSemantics_Type_Bits)

#define MethodDefOrRef_Type_MethodDefinition                0x00
#define MethodDefOrRef_Type_MemberReference                 0x01
#define MethodDefOrRef_Type_Bits                            0x01
#define MethodDefOrRef_Type_Mask                            0x01
#define MethodDefOrRef_Type_MaxRows16Bit                    (0xFFFF >> MethodDefOrRef_Type_Bits)

#define MemberForwarded_Type_Field                          0x00
#define MemberForwarded_Type_MethodDefinition               0x01
#define MemberForwarded_Type_Bits                           0x01
#define MemberForwarded_Type_Mask                           0x01
#define MemberForwarded_Type_MaxRows16Bit                   (0xFFFF >> MemberForwarded_Type_Bits)

#define Implementation_Type_File                            0x00
#define Implementation_Type_AssemblyReference               0x01
#define Implementation_Type_ExportedType                    0x02
#define Implementation_Type_Bits                            0x02
#define Implementation_Type_Mask                            0x03
#define Implementation_Type_MaxRows16Bit                    (0xFFFF >> Implementation_Type_Bits)

#define CustomAttributeType_Type_MethodDefinition           0x02
#define CustomAttributeType_Type_MemberReference            0x03
#define CustomAttributeType_Type_Bits                       0x03
#define CustomAttributeType_Type_Mask                       0x07
#define CustomAttributeType_Type_MaxRows16Bit               (0xFFFF >> CustomAttributeType_Type_Bits)

#define ResolutionScope_Type_ModuleDefinition               0x00
#define ResolutionScope_Type_ModuleReference                0x01
#define ResolutionScope_Type_AssemblyReference              0x02
#define ResolutionScope_Type_TypeReference                  0x03
#define ResolutionScope_Type_Bits                           0x02
#define ResolutionScope_Type_Mask                           0x03
#define ResolutionScope_Type_MaxRows16Bit                   (0xFFFF >> ResolutionScope_Type_Bits)

#define TypeOrMethodDef_Type_TypeDefinition                 0x00
#define TypeOrMethodDef_Type_MethodDefinition               0x01
#define TypeOrMethodDef_Type_Bits                           0x01
#define TypeOrMethodDef_Type_Mask                           0x01
#define TypeOrMethodDef_Type_MaxRows16Bit                   (0xFFFF >> TypeOrMethodDef_Type_Bits)


#define AssemblyHashAlgorithm_None                                      0x0000
#define AssemblyHashAlgorithm_MD5                                       0x8003
#define AssemblyHashAlgorithm_SHA1                                      0x8004

#define AssemblyFlags_PublicKey                                         0x0001
#define AssemblyFlags_Retargetable                                      0x0100
#define AssemblyFlags_DisableJITCompileOptimizer                        0x4000
#define AssemblyFlags_DisableJITCompileTracking                         0x8000

#define EventAttributes_SpecialName                                     0x0200
#define EventAttributes_RTSpecialName                                   0x0400

#define FieldAttributes_FieldAccessMask                                 0x0007
#define FieldAttributes_CompilerControlled                              0x0000
#define FieldAttributes_Private                                         0x0001
#define FieldAttributes_FamilyAndAssembly                               0x0002
#define FieldAttributes_Assembly                                        0x0003
#define FieldAttributes_Family                                          0x0004
#define FieldAttributes_FamilyOrAssembly                                0x0005
#define FieldAttributes_Public                                          0x0006
#define FieldAttributes_Static                                          0x0010
#define FieldAttributes_InitOnly                                        0x0020
#define FieldAttributes_Literal                                         0x0040
#define FieldAttributes_NotSerialized                                   0x0080
#define FieldAttributes_SpecialName                                     0x0200
#define FieldAttributes_PInvokeImplementation                           0x2000
#define FieldAttributes_RTSpecialName                                   0x0400
#define FieldAttributes_HasFieldMarshal                                 0x1000
#define FieldAttributes_HasDefault                                      0x8000
#define FieldAttributes_HasFieldRVA                                     0x0100

#define FileAttributes_ContainsMetaData                                 0x0000
#define FileAttributes_ContainsNoMetaData                               0x0001

#define GenericParameterAttributes_VarianceMask                         0x0003
#define GenericParameterAttributes_None                                 0x0000
#define GenericParameterAttributes_Covariant                            0x0001
#define GenericParameterAttributes_Contravariant                        0x0002
#define GenericParameterAttributes_SpecialConstraintMask                0x001C
#define GenericParameterAttributes_ReferenceTypeConstraint              0x0004
#define GenericParameterAttributes_NotNullableValueTypeConstraint       0x0008
#define GenericParameterAttributes_DefaultConstructorConstraint         0x0010

#define PInvokeAttributes_NoMangle                                      0x0001
#define PInvokeAttributes_CharacterSetMask                              0x0006
#define PInvokeAttributes_CharacterSetNotSpecified                      0x0000
#define PInvokeAttributes_CharacterSetAnsi                              0x0002
#define PInvokeAttributes_CharacterSetUnicode                           0x0004
#define PInvokeAttributes_CharacterSetAuto                              0x0006
#define PInvokeAttributes_SupportsLastError                             0x0040
#define PInvokeAttributes_CallConventionMask                            0x0700
#define PInvokeAttributes_CallConventionPlatformAPI                     0x0100
#define PInvokeAttributes_CallConventionCDecl                           0x0200
#define PInvokeAttributes_CallConventionSTD                             0x0300
#define PInvokeAttributes_CallConventionThis                            0x0400
#define PInvokeAttributes_CallConventionFast                            0x0500

#define ManifestResourceAttributes_VisibilityMask                       0x0007
#define ManifestResourceAttributes_Public                               0x0001
#define ManifestResourceAttributes_Private                              0x0002

#define MethodAttributes_MemberAccessMask                               0x0007
#define MethodAttributes_CompilerControlled                             0x0000
#define MethodAttributes_Private                                        0x0001
#define MethodAttributes_FamilyAndAssembly                              0x0002
#define MethodAttributes_Assembly                                       0x0003
#define MethodAttributes_Family                                         0x0004
#define MethodAttributes_FamilyOrAssembly                               0x0005
#define MethodAttributes_Public                                         0x0006
#define MethodAttributes_Static                                         0x0010
#define MethodAttributes_Final                                          0x0020
#define MethodAttributes_Virtual                                        0x0040
#define MethodAttributes_HideBySignature                                0x0080
#define MethodAttributes_VTableLayoutMask                               0x0100
#define MethodAttributes_ReuseSlot                                      0x0000
#define MethodAttributes_NewSlot                                        0x0100
#define MethodAttributes_Strict                                         0x0200
#define MethodAttributes_Abstract                                       0x0400
#define MethodAttributes_SpecialName                                    0x0800
#define MethodAttributes_PInvokeImplementation                          0x2000
#define MethodAttributes_UnmanagedExport                                0x0008
#define MethodAttributes_RTSpecialName                                  0x1000
#define MethodAttributes_HasSecurity                                    0x4000
#define MethodAttributes_RequireSecurityObject                          0x8000

#define MethodImplAttributes_CodeTypeMask                     0x0003
#define MethodImplAttributes_IL                               0x0000
#define MethodImplAttributes_Native                           0x0001
#define MethodImplAttributes_OPTIL                            0x0002
#define MethodImplAttributes_Runtime                          0x0003
#define MethodImplAttributes_ManagedMask                      0x0004
#define MethodImplAttributes_Unmanaged                        0x0004
#define MethodImplAttributes_Managed                          0x0000
#define MethodImplAttributes_ForwardReference                 0x0010
#define MethodImplAttributes_PreserveSignature                0x0080
#define MethodImplAttributes_InternalCall                     0x1000
#define MethodImplAttributes_Synchronized                     0x0020
#define MethodImplAttributes_NoInlining                       0x0008
#define MethodImplAttributes_MaxMethodImplementationValue     0xFFFF
#define MethodImplAttributes_NoOptimization                   0x0040

#define MethodSemanticsAttributes_Setter                                0x0001
#define MethodSemanticsAttributes_Getter                                0x0002
#define MethodSemanticsAttributes_Other                                 0x0004
#define MethodSemanticsAttributes_AddOn                                 0x0008
#define MethodSemanticsAttributes_RemoveOn                              0x0010
#define MethodSemanticsAttributes_Fire                                  0x0020

#define ParameterAttributes_In                                          0x0001
#define ParameterAttributes_Out                                         0x0002
#define ParameterAttributes_Optional                                    0x0010
#define ParameterAttributes_HasDefault                                  0x1000
#define ParameterAttributes_HasFieldMarshal                             0x2000
#define ParameterAttributes_Unused                                      0xCFE0

#define PropertyAttributes_SpecialName                                  0x0200
#define PropertyAttributes_RTSpecialName                                0x0400
#define PropertyAttributes_HasDefault                                   0x1000
#define PropertyAttributes_Unused                                       0xE9FF

#define TypeAttributes_VisibilityMask                                   0x00000007
#define TypeAttributes_NotPublic                                        0x00000000
#define TypeAttributes_Public                                           0x00000001
#define TypeAttributes_NestedPublic                                     0x00000002
#define TypeAttributes_NestedPrivate                                    0x00000003
#define TypeAttributes_NestedFamily                                     0x00000004
#define TypeAttributes_NestedAssembly                                   0x00000005
#define TypeAttributes_NestedFamilyAndAssembly                          0x00000006
#define TypeAttributes_NestedFamilyOrAssembly                           0x00000007
#define TypeAttributes_LayoutMask                                       0x00000018
#define TypeAttributes_AutoLayout                                       0x00000000
#define TypeAttributes_SequentialLayout                                 0x00000008
#define TypeAttributes_ExplicitLayout                                   0x00000010
#define TypeAttributes_ClassSemanticsMask                               0x00000020
#define TypeAttributes_Class                                            0x00000000
#define TypeAttributes_Interface                                        0x00000020
#define TypeAttributes_Abstract                                         0x00000080
#define TypeAttributes_Sealed                                           0x00000100
#define TypeAttributes_SpecialName                                      0x00000400
#define TypeAttributes_Import                                           0x00001000
#define TypeAttributes_Serializable                                     0x00002000
#define TypeAttributes_StringFormatMask                                 0x00030000
#define TypeAttributes_AnsiClass                                        0x00000000
#define TypeAttributes_UnicodeClass                                     0x00010000
#define TypeAttributes_AutoClass                                        0x00020000
#define TypeAttributes_CustomFormatClass                                0x00030000
#define TypeAttributes_CustomStringFormatMask                           0x00C00000
#define TypeAttributes_BeforeFieldInit                                  0x00100000
#define TypeAttributes_RTSpecialName                                    0x00000800
#define TypeAttributes_HasSecurity                                      0x00040000
#define TypeAttributes_IsTypeForwarder                                  0x00200000


typedef struct _ModuleDefinition ModuleDefinition;
typedef struct _TypeReference TypeReference;
typedef struct _TypeDefinition TypeDefinition;
typedef struct _Field Field;
typedef struct _MethodDefinition MethodDefinition;
typedef struct _Parameter Parameter;
typedef struct _InterfaceImplementation InterfaceImplementation;
typedef struct _MemberReference MemberReference;
typedef struct _Constant Constant;
typedef struct _CustomAttribute CustomAttribute;
typedef struct _FieldMarshal FieldMarshal;
typedef struct _DeclSecurity DeclSecurity;
typedef struct _ClassLayout ClassLayout;
typedef struct _FieldLayout FieldLayout;
typedef struct _StandAloneSignature StandAloneSignature;
typedef struct _EventMap EventMap;
typedef struct _Event Event;
typedef struct _PropertyMap PropertyMap;
typedef struct _Property Property;
typedef struct _MethodSemantics MethodSemantics;
typedef struct _MethodImplementation MethodImplementation;
typedef struct _ModuleReference ModuleReference;
typedef struct _TypeSpecification TypeSpecification;
typedef struct _ImplementationMap ImplementationMap;
typedef struct _FieldRVA FieldRVA;
typedef struct _AssemblyDefinition AssemblyDefinition;
typedef struct _AssemblyProcessor AssemblyProcessor;
typedef struct _AssemblyOperatingSystem AssemblyOperatingSystem;
typedef struct _AssemblyReference AssemblyReference;
typedef struct _AssemblyReferenceProcessor AssemblyReferenceProcessor;
typedef struct _AssemblyReferenceOperatingSystem AssemblyReferenceOperatingSystem;
typedef struct _File File;
typedef struct _ExportedType ExportedType;
typedef struct _ManifestResource ManifestResource;
typedef struct _NestedClass NestedClass;
typedef struct _GenericParameter GenericParameter;
typedef struct _MethodSpecification MethodSpecification;
typedef struct _GenericParameterConstraint GenericParameterConstraint;


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


struct _ModuleDefinition
{
    uint16_t Generation;
    const char* Name;
    const uint8_t* ModuleVersionID;
    const uint8_t* EncID;
    const uint8_t* EncBaseID;
};

struct _TypeReference
{
    ResolutionScopeUnion(ResolutionScope)
    const char* Name;
    const char* Namespace;
	bool_t IsResolved;
	TypeDefinition* ResolvedType;
};

struct _TypeDefinition
{
    uint32_t Flags;
    const char* Name;
    const char* Namespace;
    TypeDefOrRefUnion(Extends);
	uint32_t FieldCount;
    Field* FieldList;
    MethodDefinition* MethodDefinitionList;
	bool_t HasStaticConstructor;
	bool_t HasStaticConstructorBeenRun;
};

struct _Field
{
    uint16_t Flags;
    const char* Name;
    const uint8_t* Signature;
};

struct _MethodDefinition
{
    uint32_t VirtualAddress;
    uint16_t ImplFlags;
    uint16_t Flags;
    const char* Name;
    const uint8_t* Signature;
    Parameter* ParameterList;
    uint8_t ParameterListCount;
};

struct _Parameter
{
    uint16_t Flags;
    uint16_t Sequence;
    const char* Name;
};

struct _InterfaceImplementation
{
    TypeDefinition* Implementor;
    TypeDefOrRefUnion(Interface)
};

struct _MemberReference
{
    MemberRefParentUnion(Parent)
    const char* Name;
    const uint8_t* Signature;
};

struct _Constant
{
    uint8_t Type;
    HasConstantUnion(Parent)
    const uint8_t* Value;
};

struct _CustomAttribute
{
    HasCustomAttributeUnion(Parent)
    CustomAttributeTypeUnion(Type)
    const uint8_t* Value;
};

struct _FieldMarshal
{
    HasFieldMarshalUnion(Parent)
    const uint8_t* NativeType;
};

struct _DeclSecurity
{
    uint16_t Action;
    HasDeclSecurityUnion(Parent)
    const uint8_t* PermissionSet;
};

struct _ClassLayout
{
    uint16_t PackingSize;
    uint32_t ClassSize;
    TypeDefinition* Parent;
};

struct _FieldLayout
{
    uint32_t Offset;
    Field* Field;
};

struct _StandAloneSignature
{
    const uint8_t* Signature;
};

struct _EventMap
{
    TypeDefinition* Parent;
    Event* EventList;
};

struct _Event
{
    uint16_t Flags;
    const char* Name;
    TypeDefOrRefUnion(EventType)
};

struct _PropertyMap
{
    TypeDefinition* Parent;
    Property* PropertyList;
};

struct _Property
{
    uint16_t Flags;
    const char* Name;
    const uint8_t* Signature;
};

struct _MethodSemantics
{
    uint16_t Semantics;
    MethodDefinition* Method;
    HasSemanticsUnion(Association)
};

struct _MethodImplementation
{
    TypeDefinition* Parent;
    MethodDefOrRefUnion(MethodBody)
    MethodDefOrRefUnion(MethodDeclaration)
};

struct _ModuleReference
{
    const char* Name;
};

struct _TypeSpecification
{
    const uint8_t* Signature;
};

struct _ImplementationMap
{
    uint16_t MappingFlags;
    MemberForwardedUnion(MemberForwarded)
    const char* ImportName;
    ModuleReference* ImportScope;
};

struct _FieldRVA
{
    uint32_t VirtualAddress;
    Field* Field;
};

struct _AssemblyDefinition
{
    uint32_t HashAlgorithmID;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint16_t Build;
    uint16_t Revision;
    uint32_t Flags;
    const uint8_t* PublicKey;
    const char* Name;
    const char* Culture;
};

struct _AssemblyProcessor
{
    uint32_t Processor;
};

struct _AssemblyOperatingSystem
{
    uint32_t PlatformID;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
};

struct _AssemblyReference
{
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    uint16_t Build;
    uint16_t Revision;
    uint32_t Flags;
    const uint8_t* PublicKeyOrToken;
    const char* Name;
    const char* Culture;
    const uint8_t* HashValue;
};

struct _AssemblyReferenceProcessor
{
    uint32_t Processor;
    AssemblyReference* AssemblyReference;
};

struct _AssemblyReferenceOperatingSystem
{
    uint32_t PlatformID;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
    AssemblyReference* AssemblyReference;
};

struct _File
{
    uint32_t Flags;
    const char* Name;
    const uint8_t* HashValue;
};

struct _ExportedType
{
    uint32_t Flags;
    TypeDefinition* TypeDefinitionID;
    const char* Name;
    const char* Namespace;
    ImplementationUnion(Implementation)
};

struct _ManifestResource
{
    uint32_t Offset;
    uint32_t Flags;
    const char* Name;
    ImplementationUnion(Implementation)
};

struct _NestedClass
{
    TypeDefinition* Nested;
    TypeDefinition* Enclosing;
};

struct _GenericParameter
{
    uint16_t Index;
    uint16_t Flags;
    TypeOrMethodDefUnion(Owner)
    const char* Name;
};

struct _MethodSpecification
{
    MethodDefOrRefUnion(Method)
    const uint8_t* Instantiation;
};

struct _GenericParameterConstraint
{
    GenericParameter* Owner;
    TypeDefOrRefUnion(Constraint)
};
