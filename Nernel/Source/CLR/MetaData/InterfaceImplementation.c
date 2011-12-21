#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* InterfaceImplementation_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_InterfaceImplementation)) != 0)
    {
        pFile->InterfaceImplementationCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->InterfaceImplementations = (InterfaceImplementation*)calloc(pFile->InterfaceImplementationCount + 1, sizeof(InterfaceImplementation));
    }
    return pTableData;
}

void InterfaceImplementation_Cleanup(CLIFile* pFile)
{
    if (pFile->InterfaceImplementations)
    {
        for (uint32_t index = 1; index <= pFile->InterfaceImplementationCount; ++index)
        {
            if (pFile->InterfaceImplementations[index].CustomAttributes) free(pFile->InterfaceImplementations[index].CustomAttributes);
        }
        free(pFile->InterfaceImplementations);
        pFile->InterfaceImplementations = NULL;
    }
}

const uint8_t* InterfaceImplementation_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t implementorIndex = 0;
    uint32_t interfaceIndex = 0;
    uint32_t interfaceRow = 0;
    for (uint32_t index = 1; index <= pFile->InterfaceImplementationCount; ++index)
    {
        if (pFile->TypeDefinitionCount > 0xFFFF) { implementorIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementorIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->InterfaceImplementations[index].Implementor = &pFile->TypeDefinitions[implementorIndex];

        if (pFile->TypeDefinitionCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefOrRef_Type_MaxRows16Bit) { interfaceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { interfaceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->InterfaceImplementations[index].TypeOfInterface = interfaceIndex & TypeDefOrRef_Type_Mask;
        interfaceRow = interfaceIndex >> TypeDefOrRef_Type_Bits;
        switch (pFile->InterfaceImplementations[index].TypeOfInterface)
        {
        case TypeDefOrRef_Type_TypeDefinition: pFile->InterfaceImplementations[index].Interface.TypeDefinition = &pFile->TypeDefinitions[interfaceRow]; break;
        case TypeDefOrRef_Type_TypeReference: pFile->InterfaceImplementations[index].Interface.TypeReference = &pFile->TypeReferences[interfaceRow]; break;
        case TypeDefOrRef_Type_TypeSpecification: pFile->InterfaceImplementations[index].Interface.TypeSpecification = &pFile->TypeSpecifications[interfaceRow]; break;
        default: break;
        }
    }
    return pTableData;
}

void InterfaceImplementation_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_InterfaceImplementation) ++pFile->CustomAttributes[index].Parent.InterfaceImplementation->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->InterfaceImplementationCount; ++index)
    {
        if (pFile->InterfaceImplementations[index].CustomAttributeCount > 0)
        {
            pFile->InterfaceImplementations[index].CustomAttributes = (CustomAttribute**)calloc(pFile->InterfaceImplementations[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_InterfaceImplementation &&
                    pFile->CustomAttributes[searchIndex].Parent.InterfaceImplementation == &pFile->InterfaceImplementations[index])
                {
                    pFile->InterfaceImplementations[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
