#pragma once

#define Signature_ElementType_End                       0x00
#define Signature_ElementType_Void                      0x01
#define Signature_ElementType_Boolean                   0x02
#define Signature_ElementType_Char                      0x03
#define Signature_ElementType_I1                        0x04
#define Signature_ElementType_U1                        0x05
#define Signature_ElementType_I2                        0x06
#define Signature_ElementType_U2                        0x07
#define Signature_ElementType_I4                        0x08
#define Signature_ElementType_U4                        0x09
#define Signature_ElementType_I8                        0x0A
#define Signature_ElementType_U8                        0x0B
#define Signature_ElementType_R4                        0x0C
#define Signature_ElementType_R8                        0x0D
#define Signature_ElementType_String                    0x0E
#define Signature_ElementType_Pointer                   0x0F
#define Signature_ElementType_ByReference               0x10
#define Signature_ElementType_ValueType                 0x11
#define Signature_ElementType_Class                     0x12
#define Signature_ElementType_Var                       0x13
#define Signature_ElementType_Array                     0x14
#define Signature_ElementType_GenericInstantiation      0x15
#define Signature_ElementType_TypedByReference          0x16
#define Signature_ElementType_IPointer                  0x18
#define Signature_ElementType_UPointer                  0x19
#define Signature_ElementType_FunctionPointer           0x1B
#define Signature_ElementType_Object                    0x1C
#define Signature_ElementType_SingleDimensionArray      0x1D
#define Signature_ElementType_MethodVar                 0x1E
#define Signature_ElementType_CustomModifier_Required   0x1F
#define Signature_ElementType_CustomModifier_Optional   0x20
#define Signature_ElementType_Internal                  0x21
#define Signature_ElementType_Modifier                  0x40
#define Signature_ElementType_Sentinel                  0x41
#define Signature_ElementType_Pinned                    0x45
#define Signature_ElementType_Type                      0x50
#define Signature_ElementType_CustomAttribute_Boxed     0x51
#define Signature_ElementType_CustomAttribute_Field     0x53
#define Signature_ElementType_CustomAttribute_Property  0x54
#define Signature_ElementType_CustomAttribute_Enum      0x55


#define Signature_CallConvention_Default                0x00
#define Signature_CallConvention_C                      0x01
#define Signature_CallConvention_STDCall                0x02
#define Signature_CallConvention_ThisCall               0x03
#define Signature_CallConvention_FastCall               0x04
#define Signature_CallConvention_VarArgs                0x05
#define Signature_CallConvention_Generic                0x10
#define Signature_CallConvention_HasThis                0x20
#define Signature_CallConvention_ExplicitThis           0x40

typedef struct _MethodSignature MethodSignature;

typedef struct _SignatureReturnType SignatureReturnType;
typedef struct _SignatureParameter SignatureParameter;
typedef struct _SignatureCustomModifier SignatureCustomModifier;
typedef struct _SignatureType SignatureType;
typedef struct _SignatureArrayShape SignatureArrayShape;

struct _MethodSignature
{
    bool_t HasThis;
    bool_t ExplicitThis;
    bool_t Default;
    bool_t VarArgs;
    bool_t Generic;
    uint32_t GenericParameterCount;
    SignatureReturnType* ReturnType;
    uint32_t ParameterCount;
    SignatureParameter** Parameters;
    bool_t HasSentinel;
    uint32_t SentinelIndex;
};

struct _SignatureReturnType
{
    uint32_t CustomModifierCount;
    SignatureCustomModifier** CustomModifiers;
    bool_t ByReference;
    SignatureType* Type;
    bool_t TypedByReference;
    bool_t Void;
};

struct _SignatureParameter
{
    uint32_t CustomModifierCount;
    SignatureCustomModifier** CustomModifiers;
    bool_t ByReference;
    SignatureType* Type;
    bool_t TypedByReference;
};

struct _SignatureCustomModifier
{
    bool_t Optional;
    uint32_t TypeDefOrRefOrSpecToken;
};

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

struct _SignatureArrayShape
{
    uint32_t Rank;
    uint32_t SizeCount;
    uint32_t* Sizes;
    uint32_t LowerBoundCount;
    int32_t* LowerBounds;
};

MethodSignature* MethodSignature_Create();
void MethodSignature_Destroy(MethodSignature* pMethodSignature);
const uint8_t* MethodSignature_Parse(const uint8_t* pCursor, MethodSignature** pMethodSignature);
MethodSignature* MethodSignature_Expand(const uint8_t* pSignature, uint32_t pSignatureLength);

SignatureReturnType* SignatureReturnType_Create();
void SignatureReturnType_Destroy(SignatureReturnType* pReturnType);
const uint8_t* SignatureReturnType_Parse(const uint8_t* pCursor, SignatureReturnType** pReturnType);

SignatureParameter* SignatureParameter_Create();
void SignatureParameter_Destroy(SignatureParameter* pParameter);
const uint8_t* SignatureParameter_Parse(const uint8_t* pCursor, SignatureParameter** pParameter);

SignatureCustomModifier* SignatureCustomModifier_Create();
void SignatureCustomModifier_Destroy(SignatureCustomModifier* pCustomModifier);
const uint8_t* SignatureCustomModifier_Parse(const uint8_t* pCursor, SignatureCustomModifier** pCustomModifier);

SignatureType* SignatureType_Create();
void SignatureType_Destroy(SignatureType* pType);
const uint8_t* SignatureType_Parse(const uint8_t* pCursor, SignatureType** pType);

SignatureArrayShape* SignatureArrayShape_Create();
void SignatureArrayShape_Destroy(SignatureArrayShape* pArrayShape);
const uint8_t* SignatureArrayShape_Parse(const uint8_t* pCursor, SignatureArrayShape** pArrayShape);
