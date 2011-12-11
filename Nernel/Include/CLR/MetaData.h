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

bool_t MetaData_IsValidSignature(uint8_t* pMetaDataHeader);
uint32_t MetaData_GetVersionLength(uint8_t* pMetaDataHeader);
const char* MetaData_GetVersion(uint8_t* pMetaDataHeader);
uint16_t MetaData_GetStreamCount(uint8_t* pMetaDataHeader);
uint8_t* MetaData_GetStream(uint8_t* pMetaDataHeader, uint16_t pStreamIndex);
uint32_t MetaData_GetStreamRVA(uint8_t* pMetaDataHeader, uint16_t pStreamIndex);
uint32_t MetaData_GetStreamSize(uint8_t* pMetaDataHeader, uint16_t pStreamIndex);
const char* MetaData_GetStreamName(uint8_t* pMetaDataHeader, uint16_t pStreamIndex);
uint8_t* MetaData_GetStreamData(uint8_t* pMetaDataHeader, uint16_t pStreamIndex);


struct ModuleDefinition;
struct TypeReference;
struct TypeDefinition;
struct Field;
struct MethodDefinition;
struct Parameter;
struct InterfaceImplementation;
struct MemberReference;
struct Constant;
struct CustomAttribute;
struct FieldMarshal;
struct DeclSecurity;
struct ClassLayout;
struct FieldLayout;
struct StandAloneSignature;
struct EventMap;
struct Event;
struct PropertyMap;
struct Property;
struct MethodSemantics;
struct MethodImplementation;
struct ModuleReference;
struct TypeSpecification;
struct ImplementationMap;
struct FieldRVA;
struct AssemblyDefinition;
struct AssemblyProcessor;
struct AssemblyOperatingSystem;
struct AssemblyReference;
struct AssemblyReferenceProcessor;
struct AssemblyReferenceOperatingSystem;
struct File;
struct ExportedType;
struct ManifestResource;
struct NestedClass;
struct GenericParameter;
struct MethodSpecification;
struct GenericParameterConstraint;


#define TypeDefOrRefUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct TypeDefinition* TypeDefinition; \
    struct TypeReference* TypeReference; \
    struct TypeSpecification* TypeSpecification; \
} n;

#define HasConstantUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct Field* Field; \
    struct Parameter* Parameter; \
    struct Property* Property; \
} n;

#define HasCustomAttributeUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct MethodDefinition* MethodDefinition; \
    struct Field* Field; \
    struct TypeReference* TypeReference; \
    struct TypeDefinition* TypeDefinition; \
    struct Parameter* Parameter; \
    struct InterfaceImplementation* InterfaceImplementation; \
    struct MemberReference* MemberReference; \
    struct ModuleDefinition* ModuleDefinition; \
    struct Permission* Permission; \
    struct Property* Property; \
    struct Event* Event; \
    struct StandAloneSignature* StandAloneSignature; \
    struct ModuleReference* ModuleReference; \
    struct TypeSpecification* TypeSpecification; \
    struct AssemblyDefinition* AssemblyDefinition; \
    struct AssemblyReference* AssemblyReference; \
    struct File* File; \
    struct ExportedType* ExportedType; \
    struct ManifestResource* ManifestResource; \
    struct GenericParameter* GenericParameter; \
    struct GenericParameterConstraint* GenericParameterConstraint; \
    struct MethodSpecification* MethodSpecification; \
} n;

#define HasFieldMarshalUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct Field* Field; \
    struct Parameter* Parameter; \
} n;

#define HasDeclSecurityUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct TypeDefinition* TypeDefinition; \
    struct MethodDefinition* MethodDefinition; \
    struct AssemblyDefinition* AssemblyDefinition; \
} n;

#define MemberRefParentUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct TypeDefinition* TypeDefinition; \
    struct TypeReference* TypeReference; \
    struct ModuleReference* ModuleReference; \
    struct MethodDefinition* MethodDefinition; \
    struct TypeSpecification* TypeSpecification; \
} n;

#define HasSemanticsUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct Event* Event; \
    struct Property* Property; \
} n;

#define MethodDefOrRefUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct MethodDefinition* MethodDefinition; \
    struct MemberReference* MemberReference; \
} n;

#define MemberForwardedUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct Field* Field; \
    struct MethodDefinition* MethodDefinition; \
} n;

#define ImplementationUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct File* File; \
    struct AssemblyReference* AssemblyReference; \
    struct ExportedType* ExportedType; \
} n;

#define CustomAttributeTypeUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct MethodDefinition* MethodDefinition; \
    struct MemberReference* MemberReference; \
} n;

#define ResolutionScopeUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct ModuleDefinition* ModuleDefinition; \
    struct ModuleReference* ModuleReference; \
    struct AssemblyReference* AssemblyReference; \
    struct TypeReference* TypeReference; \
} n;

#define TypeOrMethodDefUnion(n) \
uint8_t TypeOf##n; \
union \
{ \
    struct TypeDefinition* TypeDefinition; \
    struct MethodDefinition* MethodDefinition; \
} n;

#define TypeDefOrRef_Type_TypeDefinition                    0x00
#define TypeDefOrRef_Type_TypeReference                     0x01
#define TypeDefOrRef_Type_TypeSpecification                 0x02

#define HasConstant_Type_Field                              0x00
#define HasConstant_Type_Parameter                          0x01
#define HasConstant_Type_Property                           0x02

#define HasCustomAttribute_Type_MethodDefinition            0x00
#define HasCustomAttribute_Type_Field                       0x01
#define HasCustomAttribute_Type_TypeReference               0x02
#define HasCustomAttribute_Type_TypeDefinition              0x03
#define HasCustomAttribute_Type_Parameter                   0x04
#define HasCustomAttribute_Type_InterfaceImplementation     0x05
#define HasCustomAttribute_Type_MemberReference             0x06
#define HasCustomAttribute_Type_ModuleDefinition            0x07
#define HasCustomAttribute_Type_Permission                  0x08
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

#define HasFieldMarshal_Type_Field                          0x00
#define HasFieldMarshal_Type_Parameter                      0x01

#define HasDeclSecurity_Type_TypeDefinition                 0x00
#define HasDeclSecurity_Type_MethodDefinition               0x01
#define HasDeclSecurity_Type_AssemblyDefinition             0x02

#define MemberRefParent_Type_TypeDefinition                 0x00
#define MemberRefParent_Type_TypeReference                  0x01
#define MemberRefParent_Type_ModuleReference                0x02
#define MemberRefParent_Type_MethodDefinition               0x03
#define MemberRefParent_Type_TypeSpecification              0x04

#define HasSemantics_Type_Event                             0x00
#define HasSemantics_Type_Property                          0x01

#define MethodDefOrRef_Type_MethodDefinition                0x00
#define MethodDefOrRef_Type_MemberReference                 0x01

#define MemberForwarded_Type_Field                          0x00
#define MemberForwarded_Type_MethodDefinition               0x01

#define Implementation_Type_File                            0x00
#define Implementation_Type_AssemblyReference               0x01
#define Implementation_Type_ExportedType                    0x02

#define CustomAttributeType_Type_MethodDefinition           0x02
#define CustomAttributeType_Type_MemberReference            0x03

#define ResolutionScope_Type_ModuleDefinition               0x00
#define ResolutionScope_Type_ModuleReference                0x01
#define ResolutionScope_Type_AssemblyReference              0x02
#define ResolutionScope_Type_TypeReference                  0x03

#define TypeOrMethodDef_Type_TypeDefinition                 0x00
#define TypeOrMethodDef_Type_MethodDefinition               0x01


typedef struct
{
    uint16_t Generation;
    const char* Name;
    const uint8_t* ModuleVersionID;
    const uint8_t* EncID;
    const uint8_t* EncBaseID;
} ModuleDefinition;

typedef struct
{
    ResolutionScopeUnion(ResolutionScope)
    const char* Name;
    const char* Namespace;
} TypeReference;

#define TypeDefinition_Extends_TypeDefinition       0x00
#define TypeDefinition_Extends_TypeReference        0x01
#define TypeDefinition_Extends_TypeSpecification    0x02


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

#define MethodImplementationAttributes_CodeTypeMask                     0x0003
#define MethodImplementationAttributes_IL                               0x0000
#define MethodImplementationAttributes_Native                           0x0001
#define MethodImplementationAttributes_OPTIL                            0x0002
#define MethodImplementationAttributes_Runtime                          0x0003
#define MethodImplementationAttributes_ManagedMask                      0x0004
#define MethodImplementationAttributes_Unmanaged                        0x0004
#define MethodImplementationAttributes_Managed                          0x0000
#define MethodImplementationAttributes_ForwardReference                 0x0010
#define MethodImplementationAttributes_PreserveSignature                0x0080
#define MethodImplementationAttributes_InternalCall                     0x1000
#define MethodImplementationAttributes_Synchronized                     0x0020
#define MethodImplementationAttributes_NoInlining                       0x0008
#define MethodImplementationAttributes_MaxMethodImplementationValue     0xFFFF
#define MethodImplementationAttributes_NoOptimization                   0x0040

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


typedef struct
{
    uint32_t Flags;
    const char* Name;
    const char* Namespace;
    TypeDefOrRefUnion(Extends)
    const struct Field* FieldList;
    const struct MethodDefinition* MethodDefinitionList;
} TypeDefinition;

typedef struct
{
    uint16_t Flags;
    const char* Name;
    uint8_t TypeOfSignature;
    union
    {
        const struct TypeDefinition* TypeDefinition;
    } Signature;
} Field;

typedef struct
{
    uint32_t VirtualAddress;
    uint16_t ImplFlags;
    uint16_t Flags;
    const char* Name;
    uint8_t TypeOfSignature;
    union
    {
        const TypeDefinition* TypeDefinition;
    } Signature;
    struct Parameter* ParameterList;
} MethodDefinition;

typedef struct
{
    uint16_t Flags;
    uint16_t Sequence;
    const char* Name;
} Parameter;

typedef struct
{
    struct TypeDefinition* Implementor;
    TypeDefOrRefUnion(Interface)
} InterfaceImplementation;

typedef struct
{
    MemberRefParentUnion(Parent)
    const char* Name;
    uint8_t TypeOfSignature;
    union
    {
        const struct TypeDefinition* TypeDefinition;
    } Signature;
} MemberReference;

typedef struct
{
    uint8_t Type;
    HasConstantUnion(Parent)
    const uint8_t* Value;
} Constant;

typedef struct
{
    HasCustomAttributeUnion(Parent)
    CustomAttributeTypeUnion(Type)
    const uint8_t* Value;
} CustomAttribute;

typedef struct
{
    HasFieldMarshalUnion(Parent)
    const uint8_t* NativeType;
} FieldMarshal;

typedef struct
{
    uint16_t Action;
    HasDeclSecurityUnion(Parent)
    const uint8_t* PermissionSet;
} DeclSecurity;

typedef struct
{
    uint16_t PackingSize;
    uint32_t ClassSize;
    struct TypeDefinition* Parent;
} ClassLayout;

typedef struct
{
    uint32_t Offset;
    struct Field* Field;
} FieldLayout;

typedef struct
{
    const uint8_t* Signature;
} StandAloneSignature;

typedef struct
{
    struct TypeDefinition* Parent;
    struct Event* EventList;
} EventMap;

typedef struct
{
    uint16_t Flags;
    const char* Name;
    TypeDefOrRef(EventType)
} Event;

typedef struct
{
    struct TypeDefinition* Parent;
    struct Property* PropertyList;
} PropertyMap;

typedef struct
{
    uint16_t Flags;
    const char* Name;
    const uint8_t* Signature;
} Property;

typedef struct
{
    uint16_t Semantics;
    struct MethodDefinition* Method;
    HasSemanticsUnion(Association)
} MethodSemantics;

typedef struct
{
    struct TypeDefinition* Parent;
    MethodDefOrRefUnion(MethodBody)
    MethodDefOrRefUnion(MethodDeclaration)
} MethodImplementation;

typedef struct
{
    const char* Name;
} ModuleReference;

typedef struct
{
    const uint8_t* Signature;
} TypeSpecification;

typedef struct
{
    uint16_t MappingFlags;
    MemberForwardedUnion(MemberForwarded)
    const char* ImportName;
    struct ModuleReference* ImportScope;
} ImplementationMap;

typedef struct
{
    uint32_t VirtualAddress;
    struct Field* Field;
} FieldRVA;

typedef struct
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
} AssemblyDefinition;

typedef struct
{
    uint32_t Processor;
} AssemblyProcessor;

typedef struct
{
    uint32_t PlatformID;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
} AssemblyOperatingSystem;

typedef struct
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
} AssemblyReference;

typedef struct
{
    uint32_t Processor;
    struct AssemblyReference* AssemblyReference;
} AssemblyReferenceProcessor;

typedef struct
{
    uint32_t PlatformID;
    uint32_t MajorVersion;
    uint32_t MinorVersion;
    struct AssemblyReference* AssemblyReference;
} AssemblyReferenceOperatingSystem;

typedef struct
{
    uint32_t Flags;
    const char* Name;
    const uint8_t* HashValue;
} File;

typedef struct
{
    uint32_t Flags;
    struct TypeDefinition* TypeDefinitionID;
    const char* Name;
    const char* Namespace;
    ImplementationUnion(Implementation)
} ExportedType;

typedef struct
{
    uint32_t Offset;
    uint32_t Flags;
    const char* Name;
    ImplementationUnion(Implementation)
} ManifestResource;

typedef struct
{
    struct TypeDefinition* Nested;
    struct TypeDefinition* Enclosing;
} NestedClass;

typedef struct
{
    uint16_t Index;
    uint16_t Flags;
    TypeOrMethodDefUnion(Owner)
    const char* Name;
} GenericParameter;

typedef struct
{
    MethodDefOrRefUnion(Method)
    const uint8_t* Instantiation;
} MethodSpecification;

typedef struct
{
    struct GenericParameter* Owner;
    TypeDefOrRefUnion(Constraint)
} GenericParameterConstraint;
