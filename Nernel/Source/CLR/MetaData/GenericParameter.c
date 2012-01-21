#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* GenericParameter_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_GenericParameter)) != 0)
    {
        pFile->GenericParameterCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->GenericParameters = (GenericParameter*)calloc(pFile->GenericParameterCount + 1, sizeof(GenericParameter));
    }
    return pTableData;
}

void GenericParameter_Cleanup(CLIFile* pFile)
{
    if (pFile->GenericParameters)
    {
        for (uint32_t index = 1; index <= pFile->GenericParameterCount; ++index)
        {
            if (pFile->GenericParameters[index].CustomAttributes) free(pFile->GenericParameters[index].CustomAttributes);
            if (pFile->GenericParameters[index].GenericParameterConstraints) free(pFile->GenericParameters[index].GenericParameterConstraints);
        }
        free(pFile->GenericParameters);
        pFile->GenericParameters = NULL;
    }
}

const uint8_t* GenericParameter_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t ownerIndex = 0;
    uint32_t ownerRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->GenericParameterCount; ++index)
    {
        pFile->GenericParameters[index].TableIndex = index;
        pFile->GenericParameters[index].Index = *(uint16_t* )pTableData; pTableData += 2;
        pFile->GenericParameters[index].Flags = *(uint16_t* )pTableData; pTableData += 2;
        if (pFile->TypeDefinitionCount > TypeOrMethodDef_Type_MaxRows16Bit ||
            pFile->MethodDefinitionCount > TypeOrMethodDef_Type_MaxRows16Bit) { ownerIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { ownerIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->GenericParameters[index].TypeOfOwner = ownerIndex & TypeOrMethodDef_Type_Mask;
        ownerRow = ownerIndex >> TypeOrMethodDef_Type_Bits;
        switch (pFile->GenericParameters[index].TypeOfOwner)
        {
        case TypeOrMethodDef_Type_TypeDefinition:
            if (ownerRow == 0 || ownerRow > pFile->TypeDefinitionCount) Panic("GenericParameter_Load TypeDefinition");
            pFile->GenericParameters[index].Owner.TypeDefinition = &pFile->TypeDefinitions[ownerRow];
            break;
        case TypeOrMethodDef_Type_MethodDefinition:
            if (ownerRow == 0 || ownerRow > pFile->MethodDefinitionCount) Panic("GenericParameter_Load MethodDefinition");
            pFile->GenericParameters[index].Owner.MethodDefinition = &pFile->MethodDefinitions[ownerRow];
            break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->GenericParameters[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}

void GenericParameter_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_GenericParameter) ++pFile->CustomAttributes[index].Parent.GenericParameter->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->GenericParameterCount; ++index)
    {
        if (pFile->GenericParameters[index].CustomAttributeCount > 0)
        {
            pFile->GenericParameters[index].CustomAttributes = (CustomAttribute**)calloc(pFile->GenericParameters[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_GenericParameter &&
                    pFile->CustomAttributes[searchIndex].Parent.GenericParameter == &pFile->GenericParameters[index])
                {
                    pFile->GenericParameters[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->GenericParameterConstraintCount; ++index) ++pFile->GenericParameterConstraints[index].Owner->GenericParameterConstraintCount;
    for (uint32_t index = 1; index <= pFile->GenericParameterCount; ++index)
    {
        if (pFile->GenericParameters[index].GenericParameterConstraintCount > 0)
        {
            pFile->GenericParameters[index].GenericParameterConstraints = (GenericParameterConstraint**)calloc(pFile->GenericParameters[index].GenericParameterConstraintCount, sizeof(GenericParameterConstraint*));
            for (uint32_t searchIndex = 1, genericParameterConstraintIndex = 0; searchIndex <= pFile->GenericParameterConstraintCount; ++searchIndex)
            {
                if (pFile->GenericParameterConstraints[searchIndex].Owner == &pFile->GenericParameters[index])
                {
                    pFile->GenericParameters[index].GenericParameterConstraints[genericParameterConstraintIndex] = &pFile->GenericParameterConstraints[searchIndex];
                    ++genericParameterConstraintIndex;
                }
            }
        }
    }
}
