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

