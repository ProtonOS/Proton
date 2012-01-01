#include <stdlib.h>

#include <CLR/CLIFile.h>

MethodSignature* MethodSignature_Create()
{
    return (MethodSignature*)calloc(1, sizeof(MethodSignature));
}

void MethodSignature_Destroy(MethodSignature* pMethodSignature)
{
    if (pMethodSignature->Parameters)
    {
        for (uint32_t index = 0; index < pMethodSignature->ParameterCount; ++index) SignatureParameter_Destroy(pMethodSignature->Parameters[index]);
        free(pMethodSignature->Parameters);
    }
    if (pMethodSignature->ReturnType) SignatureReturnType_Destroy(pMethodSignature->ReturnType);
    free(pMethodSignature);
}

MethodSignature* MethodSignature_Expand(const uint8_t* pSignature, uint32_t pSignatureLength)
{
    if (pSignatureLength == 0) Panic("MethodSignature_Expand pSignatureLength == 0");
    MethodSignature* methodSignature = NULL;
    const uint8_t* cursor = MethodSignature_Parse(pSignature, &methodSignature);
    if ((uint32_t)(cursor - pSignature) != pSignatureLength) Panic("MethodSignature_Expand used != pSignatureLength");
    return methodSignature;
}

const uint8_t* MethodSignature_Parse(const uint8_t* pCursor, MethodSignature** pMethodSignature)
{
    *pMethodSignature = MethodSignature_Create();
    MethodSignature* methodSignature = *pMethodSignature;
    uint8_t callingConvention = *pCursor; ++pCursor;
    methodSignature->HasThis = (callingConvention & Signature_CallConvention_HasThis) != 0;
    methodSignature->ExplicitThis = (callingConvention & Signature_CallConvention_ExplicitThis) != 0;
    callingConvention &= ~(Signature_CallConvention_HasThis | Signature_CallConvention_ExplicitThis);
    methodSignature->Default = callingConvention == Signature_CallConvention_Default;
    methodSignature->VarArgs = callingConvention == Signature_CallConvention_VarArgs;
    methodSignature->Generic = callingConvention == Signature_CallConvention_Generic;
    if (methodSignature->Generic) pCursor = MetaData_GetCompressedUnsigned(pCursor, &methodSignature->GenericParameterCount);
    pCursor = MetaData_GetCompressedUnsigned(pCursor, &methodSignature->ParameterCount);
    pCursor = SignatureReturnType_Parse(pCursor, &methodSignature->ReturnType);
    if (methodSignature->ParameterCount > 0)
    {
        methodSignature->Parameters = (SignatureParameter**)calloc(methodSignature->ParameterCount, sizeof(SignatureParameter*));
        for (uint32_t index = 0; index < methodSignature->ParameterCount; ++index)
        {
            if (*pCursor == Signature_ElementType_Sentinel)
            {
                methodSignature->HasSentinel = TRUE;
                methodSignature->SentinelIndex = index;
                ++pCursor;
            }
            pCursor = SignatureParameter_Parse(pCursor, &methodSignature->Parameters[index]);
        }
    }
    return pCursor;
}

SignatureReturnType* SignatureReturnType_Create()
{
    return (SignatureReturnType*)calloc(1, sizeof(SignatureReturnType));
}

void SignatureReturnType_Destroy(SignatureReturnType* pReturnType)
{
    if (pReturnType->CustomModifiers)
    {
        for (uint32_t index = 0; index < pReturnType->CustomModifierCount; ++index) SignatureCustomModifier_Destroy(pReturnType->CustomModifiers[index]);
        free(pReturnType->CustomModifiers);
    }
    if (pReturnType->Type) SignatureType_Destroy(pReturnType->Type);
    free(pReturnType);
}

const uint8_t* SignatureReturnType_Parse(const uint8_t* pCursor, SignatureReturnType** pReturnType)
{
    *pReturnType = SignatureReturnType_Create();
    SignatureReturnType* returnType = *pReturnType;
    while (*pCursor == Signature_ElementType_CustomModifier_Required ||
           *pCursor == Signature_ElementType_CustomModifier_Optional)
    {
        ++returnType->CustomModifierCount;
        returnType->CustomModifiers = (SignatureCustomModifier**)realloc(returnType->CustomModifiers, sizeof(SignatureCustomModifier*) * returnType->CustomModifierCount);
        pCursor = SignatureCustomModifier_Parse(pCursor, &returnType->CustomModifiers[returnType->CustomModifierCount - 1]);
    }
    if (*pCursor == Signature_ElementType_TypedByReference)
    {
        returnType->TypedByReference = TRUE;
        return pCursor + 1;
    }
    if (*pCursor == Signature_ElementType_Void)
    {
        returnType->Void = TRUE;
        return pCursor + 1;
    }
    if (*pCursor == Signature_ElementType_ByReference)
    {
        returnType->ByReference = TRUE;
        ++pCursor;
    }
    pCursor = SignatureType_Parse(pCursor, &returnType->Type);
    return pCursor;
}

SignatureParameter* SignatureParameter_Create()
{
    return (SignatureParameter*)calloc(1, sizeof(SignatureParameter));
}

void SignatureParameter_Destroy(SignatureParameter* pParameter)
{
    if (pParameter->CustomModifiers)
    {
        for (uint32_t index = 0; index < pParameter->CustomModifierCount; ++index) SignatureCustomModifier_Destroy(pParameter->CustomModifiers[index]);
        free(pParameter->CustomModifiers);
    }
    if (pParameter->Type) SignatureType_Destroy(pParameter->Type);
    free(pParameter);
}

const uint8_t* SignatureParameter_Parse(const uint8_t* pCursor, SignatureParameter** pParameter)
{
    *pParameter = SignatureParameter_Create();
    SignatureParameter* parameter = *pParameter;
    while (*pCursor == Signature_ElementType_CustomModifier_Required ||
           *pCursor == Signature_ElementType_CustomModifier_Optional)
    {
        ++parameter->CustomModifierCount;
        parameter->CustomModifiers = (SignatureCustomModifier**)realloc(parameter->CustomModifiers, sizeof(SignatureCustomModifier*) * parameter->CustomModifierCount);
        pCursor = SignatureCustomModifier_Parse(pCursor, &parameter->CustomModifiers[parameter->CustomModifierCount - 1]);
    }
    if (*pCursor == Signature_ElementType_TypedByReference)
    {
        parameter->TypedByReference = TRUE;
        return pCursor + 1;
    }
    if (*pCursor == Signature_ElementType_ByReference)
    {
        parameter->ByReference = TRUE;
        ++pCursor;
    }
    pCursor = SignatureType_Parse(pCursor, &parameter->Type);
    return pCursor;
}

SignatureCustomModifier* SignatureCustomModifier_Create()
{
    return (SignatureCustomModifier*)calloc(1, sizeof(SignatureCustomModifier));
}

void SignatureCustomModifier_Destroy(SignatureCustomModifier* pCustomModifier)
{
    free(pCustomModifier);
}

const uint8_t* SignatureCustomModifier_Parse(const uint8_t* pCursor, SignatureCustomModifier** pCustomModifier)
{
    *pCustomModifier = SignatureCustomModifier_Create();
    SignatureCustomModifier* customModifier = *pCustomModifier;
    
    customModifier->Optional = *pCursor == Signature_ElementType_CustomModifier_Optional; ++pCursor;
    pCursor = MetaData_GetCompressedUnsigned(pCursor, &customModifier->TypeDefOrRefOrSpecToken);
    return pCursor;
}

SignatureType* SignatureType_Create()
{
    return (SignatureType*)calloc(1, sizeof(SignatureType));
}

void SignatureType_Destroy(SignatureType* pType)
{
    if (pType->ArrayType) SignatureType_Destroy(pType->ArrayType);
    if (pType->ArrayShape) SignatureArrayShape_Destroy(pType->ArrayShape);
    if (pType->FnPtrMethodSignature) MethodSignature_Destroy(pType->FnPtrMethodSignature);
    if (pType->GenericInstGenericArguments)
    {
        for (uint32_t index = 0; index < pType->GenericInstGenericArgumentCount; ++index) SignatureType_Destroy(pType->GenericInstGenericArguments[index]);
        free(pType->GenericInstGenericArguments);
    }
    if (pType->PtrCustomModifiers)
    {
        for (uint32_t index = 0; index < pType->PtrCustomModifierCount; ++index) SignatureCustomModifier_Destroy(pType->PtrCustomModifiers[index]);
        free(pType->PtrCustomModifiers);
    }
    if (pType->PtrType) SignatureType_Destroy(pType->PtrType);
    if (pType->SZArrayCustomModifiers)
    {
        for (uint32_t index = 0; index < pType->SZArrayCustomModifierCount; ++index) SignatureCustomModifier_Destroy(pType->SZArrayCustomModifiers[index]);
        free(pType->SZArrayCustomModifiers);
    }
    if (pType->SZArrayType) SignatureType_Destroy(pType->SZArrayType);
    free(pType);
}

const uint8_t* SignatureType_Parse(const uint8_t* pCursor, SignatureType** pType)
{
    *pType = SignatureType_Create();
    SignatureType* type = *pType;
    type->ElementType = *pCursor; ++pCursor;
    switch (type->ElementType)
    {
    case Signature_ElementType_Array:
        pCursor = SignatureType_Parse(pCursor, &type->ArrayType);
        pCursor = SignatureArrayShape_Parse(pCursor, &type->ArrayShape);
        break;
    case Signature_ElementType_Class:
        pCursor = MetaData_GetCompressedUnsigned(pCursor, &type->ClassTypeDefOrRefOrSpecToken);
        break;
    case Signature_ElementType_FunctionPointer:
        pCursor = MethodSignature_Parse(pCursor, &type->FnPtrMethodSignature);
        break;
    case Signature_ElementType_GenericInstantiation:
        type->GenericInstClass = *pCursor == Signature_ElementType_Class;
        type->GenericInstValue = *pCursor == Signature_ElementType_ValueType;
        ++pCursor;
        pCursor = MetaData_GetCompressedUnsigned(pCursor, &type->GenericInstTypeDefOrRefOrSpecToken);
        pCursor = MetaData_GetCompressedUnsigned(pCursor, &type->GenericInstGenericArgumentCount);
        if (type->GenericInstGenericArgumentCount > 0)
        {
            type->GenericInstGenericArguments = (SignatureType**)calloc(type->GenericInstGenericArgumentCount, sizeof(SignatureCustomModifier*) * type->GenericInstGenericArgumentCount);
            for (uint32_t index = 0; index < type->GenericInstGenericArgumentCount; ++index) pCursor = SignatureType_Parse(pCursor, &type->GenericInstGenericArguments[index]);
        }
        break;
    case Signature_ElementType_MethodVar:
        pCursor = MetaData_GetCompressedUnsigned(pCursor, &type->MVarNumber);
        break;
    case Signature_ElementType_Pointer:
        while (*pCursor == Signature_ElementType_CustomModifier_Required ||
               *pCursor == Signature_ElementType_CustomModifier_Optional)
        {
            ++type->PtrCustomModifierCount;
            type->PtrCustomModifiers = (SignatureCustomModifier**)realloc(type->PtrCustomModifiers, sizeof(SignatureCustomModifier*) * type->PtrCustomModifierCount);
            pCursor = SignatureCustomModifier_Parse(pCursor, &type->PtrCustomModifiers[type->PtrCustomModifierCount - 1]);
        }
        if (*pCursor == Signature_ElementType_Void)
        {
            type->PtrVoid = TRUE;
            ++pCursor;
        }
        else pCursor = SignatureType_Parse(pCursor, &type->PtrType);
        break;
    case Signature_ElementType_SingleDimensionArray:
        while (*pCursor == Signature_ElementType_CustomModifier_Required ||
               *pCursor == Signature_ElementType_CustomModifier_Optional)
        {
            ++type->SZArrayCustomModifierCount;
            type->SZArrayCustomModifiers = (SignatureCustomModifier**)realloc(type->SZArrayCustomModifiers, sizeof(SignatureCustomModifier*) * type->SZArrayCustomModifierCount);
            pCursor = SignatureCustomModifier_Parse(pCursor, &type->SZArrayCustomModifiers[type->SZArrayCustomModifierCount - 1]);
        }
        pCursor = SignatureType_Parse(pCursor, &type->SZArrayType);
        break;
    case Signature_ElementType_ValueType:
        pCursor = MetaData_GetCompressedUnsigned(pCursor, &type->ValueTypeDefOrRefOrSpecToken);
        break;
    case Signature_ElementType_Var:
        pCursor = MetaData_GetCompressedUnsigned(pCursor, &type->VarNumber);
        break;
    default: break;
    }
    return pCursor;
}

SignatureArrayShape* SignatureArrayShape_Create()
{
    return (SignatureArrayShape*)calloc(1, sizeof(SignatureArrayShape));
}

void SignatureArrayShape_Destroy(SignatureArrayShape* pArrayShape)
{
    if (pArrayShape->Sizes) free(pArrayShape->Sizes);
    if (pArrayShape->LowerBounds) free(pArrayShape->LowerBounds);
    free(pArrayShape);
}

const uint8_t* SignatureArrayShape_Parse(const uint8_t* pCursor, SignatureArrayShape** pArrayShape)
{
    *pArrayShape = SignatureArrayShape_Create();
    SignatureArrayShape* arrayShape = *pArrayShape;
    pCursor = MetaData_GetCompressedUnsigned(pCursor, &arrayShape->Rank);
    pCursor = MetaData_GetCompressedUnsigned(pCursor, &arrayShape->SizeCount);
    if (arrayShape->SizeCount > 0)
    {
        arrayShape->Sizes = (uint32_t*)calloc(arrayShape->SizeCount, sizeof(uint32_t));
        for (uint32_t index = 0; index < arrayShape->SizeCount; ++index) pCursor = MetaData_GetCompressedUnsigned(pCursor, &arrayShape->Sizes[index]);
    }
    pCursor = MetaData_GetCompressedUnsigned(pCursor, &arrayShape->LowerBoundCount);
    if (arrayShape->LowerBoundCount > 0)
    {
        arrayShape->LowerBounds = (int32_t*)calloc(arrayShape->LowerBoundCount, sizeof(int32_t));
        for (uint32_t index = 0; index < arrayShape->LowerBoundCount; ++index) pCursor = MetaData_GetCompressedSigned(pCursor, &arrayShape->LowerBounds[index]);
    }
    return pCursor;
}