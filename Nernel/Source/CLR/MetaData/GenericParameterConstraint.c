#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* GenericParameterConstraint_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_GenericParameterConstraint)) != 0)
    {
        pFile->GenericParameterConstraintCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->GenericParameterConstraints = (GenericParameterConstraint*)calloc(pFile->GenericParameterConstraintCount, sizeof(GenericParameterConstraint));
    }
    return pTableData;
}

void GenericParameterConstraint_Cleanup(CLIFile* pFile)
{
    if (pFile->GenericParameterConstraints)
    {
        for (uint32_t index = 0; index < pFile->GenericParameterConstraintCount; ++index)
        {
        }
        free(pFile->GenericParameterConstraints);
        pFile->GenericParameterConstraints = NULL;
    }
}

const uint8_t* GenericParameterConstraint_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t ownerIndex = 0;
    uint32_t constraintIndex = 0;
    uint8_t constraintTable = 0;
    uint32_t constraintRow = 0;
    for (uint32_t index = 0; index < pFile->GenericParameterConstraintCount; ++index)
    {
        if (pFile->GenericParameterCount > 0xFFFF) { ownerIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { ownerIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->GenericParameterConstraints[index].Owner = &pFile->GenericParameters[ownerIndex];
        if (pFile->TypeDefinitionCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefOrRef_Type_MaxRows16Bit) { constraintIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { constraintIndex = *(uint16_t*)pTableData; pTableData += 2; }
        constraintTable = constraintIndex & TypeDefOrRef_Type_Mask;
        constraintRow = constraintIndex >> TypeDefOrRef_Type_Bits;
        switch (constraintTable)
        {
        case TypeDefOrRef_Type_TypeDefinition: pFile->GenericParameterConstraints[index].Constraint.TypeDefinition = &pFile->TypeDefinitions[constraintRow]; break;
        case TypeDefOrRef_Type_TypeReference: pFile->GenericParameterConstraints[index].Constraint.TypeReference = &pFile->TypeReferences[constraintRow]; break;
        case TypeDefOrRef_Type_TypeSpecification: pFile->GenericParameterConstraints[index].Constraint.TypeSpecification = &pFile->TypeSpecifications[constraintRow]; break;
        default: break;
        }
    }
    return pTableData;
}