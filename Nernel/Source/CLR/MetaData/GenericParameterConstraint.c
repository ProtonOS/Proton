#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* GenericParameterConstraint_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_GenericParameterConstraint)) != 0)
    {
        pFile->GenericParameterConstraintCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->GenericParameterConstraints = (GenericParameterConstraint*)calloc(pFile->GenericParameterConstraintCount + 1, sizeof(GenericParameterConstraint));
    }
    return pTableData;
}

void GenericParameterConstraint_Cleanup(CLIFile* pFile)
{
    if (pFile->GenericParameterConstraints)
    {
        for (uint32_t index = 1; index <= pFile->GenericParameterConstraintCount; ++index)
        {
            if (pFile->GenericParameterConstraints[index].CustomAttributes) free(pFile->GenericParameterConstraints[index].CustomAttributes);
        }
        free(pFile->GenericParameterConstraints);
        pFile->GenericParameterConstraints = NULL;
    }
}

const uint8_t* GenericParameterConstraint_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t ownerIndex = 0;
    uint32_t constraintIndex = 0;
    uint32_t constraintRow = 0;
    for (uint32_t index = 1; index <= pFile->GenericParameterConstraintCount; ++index)
    {
        pFile->GenericParameterConstraints[index].TableIndex = index;
        if (pFile->GenericParameterCount > 0xFFFF) { ownerIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { ownerIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (ownerIndex == 0 || ownerIndex > pFile->GenericParameterCount) Panic("GenericParameterConstraint_Load GenericParameter");
        pFile->GenericParameterConstraints[index].Owner = &pFile->GenericParameters[ownerIndex];
        if (pFile->TypeDefinitionCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefOrRef_Type_MaxRows16Bit) { constraintIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { constraintIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->GenericParameterConstraints[index].TypeOfConstraint = constraintIndex & TypeDefOrRef_Type_Mask;
        constraintRow = constraintIndex >> TypeDefOrRef_Type_Bits;
        switch (pFile->GenericParameterConstraints[index].TypeOfConstraint)
        {
        case TypeDefOrRef_Type_TypeDefinition:
            if (constraintRow == 0 || constraintRow > pFile->TypeDefinitionCount) Panic("GenericParameterConstraint_Load TypeDefinition");
            pFile->GenericParameterConstraints[index].Constraint.TypeDefinition = &pFile->TypeDefinitions[constraintRow];
            break;
        case TypeDefOrRef_Type_TypeReference:
            if (constraintRow == 0 || constraintRow > pFile->TypeReferenceCount) Panic("GenericParameterConstraint_Load TypeReference");
            pFile->GenericParameterConstraints[index].Constraint.TypeReference = &pFile->TypeReferences[constraintRow];
            break;
        case TypeDefOrRef_Type_TypeSpecification:
            if (constraintRow == 0 || constraintRow > pFile->TypeSpecificationCount) Panic("GenericParameterConstraint_Load TypeSpecification");
            pFile->GenericParameterConstraints[index].Constraint.TypeSpecification = &pFile->TypeSpecifications[constraintRow];
            break;
        default: break;
        }
    }
    return pTableData;
}

void GenericParameterConstraint_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_GenericParameterConstraint) ++pFile->CustomAttributes[index].Parent.GenericParameterConstraint->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->GenericParameterConstraintCount; ++index)
    {
        if (pFile->GenericParameterConstraints[index].CustomAttributeCount > 0)
        {
            pFile->GenericParameterConstraints[index].CustomAttributes = (CustomAttribute**)calloc(pFile->GenericParameterConstraints[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_GenericParameterConstraint &&
                    pFile->CustomAttributes[searchIndex].Parent.GenericParameterConstraint == &pFile->GenericParameterConstraints[index])
                {
                    pFile->GenericParameterConstraints[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
