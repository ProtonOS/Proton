#pragma once

#include <CLR/MetaData/MetaData.h>

#define MethodDefinitionBody_Offset_Flags                 0x00

#define MethodDefinitionBody_Flags_HeaderType_Bits        0x02
#define MethodDefinitionBody_Flags_HeaderType_Mask        0x03
#define MethodDefinitionBody_Flags_HeaderType_Fat         0x03

#define MethodDefinitionBody_Tiny_MaxStack                0x08

#define MethodDefinitionBody_Offset_Fat_Flags                           0x00
#define MethodDefinitionBody_Offset_Fat_MaxStack                        0x02
#define MethodDefinitionBody_Offset_Fat_CodeSize                        0x04
#define MethodDefinitionBody_Offset_Fat_LocalVariableSignatureToken     0x08

#define MethodDefinitionBody_Fat_Flags_HasDataSection     0x08
#define MethodDefinitionBody_Fat_Flags_InitializeLocals   0x10
#define MethodDefinitionBody_Fat_Flags_Bits               0x0C
#define MethodDefinitionBody_Fat_Flags_Mask               0x0FFF

#define MethodDefinitionBody_Fat_BodySize_Multiplier      0x04

struct _MethodDefinitionBody
{
    uint16_t Flags;
    bool_t IsFat;
    uint16_t MaxStack;
    uint32_t CodeSize;
    uint32_t LocalVariableSignatureToken;
    const uint8_t* Code;
};

#define MethodDefinitionData_ByteAlignment                      0x04

#define MethodDefinitionData_Offset_Flags                       0x00
#define MethodDefinitionData_Offset_Size                        0x01
#define MethodDefinitionData_HeaderLength                       0x04

#define MethodDefinitionData_Flags_ExceptionTable               0x01
#define MethodDefinitionData_Flags_OptionalILTable              0x02
#define MethodDefinitionData_Flags_Fat                          0x40
#define MethodDefinitionData_Flags_HasAnotherDataSection        0x80

#define MethodDefinitionData_Fat_Size_Mask                      0x00FFFFFF

#define MethodDefinitionException_Offset_Flags                      0x00
#define MethodDefinitionException_Offset_TryOffset                  0x02
#define MethodDefinitionException_Offset_TryLength                  0x04
#define MethodDefinitionException_Offset_HandlerOffset              0x05
#define MethodDefinitionException_Offset_HandlerLength              0x07
#define MethodDefinitionException_Offset_ClassTokenOrFilterOffset   0x08
#define MethodDefinitionException_HeaderLength                      0x0C

#define MethodDefinitionException_Offset_Fat_Flags                      0x00
#define MethodDefinitionException_Offset_Fat_TryOffset                  0x04
#define MethodDefinitionException_Offset_Fat_TryLength                  0x08
#define MethodDefinitionException_Offset_Fat_HandlerOffset              0x0C
#define MethodDefinitionException_Offset_Fat_HandlerLength              0x10
#define MethodDefinitionException_Offset_Fat_ClassTokenOrFilterOffset   0x14
#define MethodDefinitionException_Fat_HeaderLength                      0x18

struct _MethodDefinitionException
{
    uint32_t Flags;
    uint32_t TryOffset;
    uint32_t TryLength;
    uint32_t HandlerOffset;
    uint32_t HandlerLength;
    uint32_t ClassTokenOrFilterOffset;
};

struct _MethodDefinition
{
	uint32_t TableIndex;
    MethodDefinitionBody Body;
    uint32_t ExceptionCount;
    MethodDefinitionException* Exceptions;
    uint16_t ImplFlags;
    uint16_t Flags;
    const char* Name;
    uint32_t SignatureLength;
    const uint8_t* Signature;
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

};

const uint8_t* MethodDefinition_Initialize(CLIFile* pFile, const uint8_t* pTableData);
void MethodDefinition_Cleanup(CLIFile* pFile);
const uint8_t* MethodDefinition_Load(CLIFile* pFile, const uint8_t* pTableData);
void MethodDefinition_Link(CLIFile* pFile);
