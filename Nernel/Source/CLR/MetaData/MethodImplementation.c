#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* MethodImplementation_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_MethodImplementation)) != 0)
    {
        pFile->MethodImplementationCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->MethodImplementations = (MethodImplementation*)calloc(pFile->MethodImplementationCount + 1, sizeof(MethodImplementation));
    }
    return pTableData;
}

void MethodImplementation_Cleanup(CLIFile* pFile)
{
    if (pFile->MethodImplementations)
    {
        for (uint32_t index = 1; index <= pFile->MethodImplementationCount; ++index)
        {
        }
        free(pFile->MethodImplementations);
        pFile->MethodImplementations = NULL;
    }
}

const uint8_t* MethodImplementation_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t methodIndex = 0;
    uint32_t methodRow = 0;
    for (uint32_t index = 1; index <= pFile->MethodImplementationCount; ++index)
    {
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (parentIndex == 0 || parentIndex > pFile->TypeDefinitionCount) Panic("MethodImplementation_Load TypeDefinition");
        pFile->MethodImplementations[index].Parent = &pFile->TypeDefinitions[parentIndex];

        if (pFile->MethodDefinitionCount > MethodDefOrRef_Type_MaxRows16Bit ||
            pFile->MemberReferenceCount > MethodDefOrRef_Type_MaxRows16Bit) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodImplementations[index].TypeOfMethodBody = methodIndex & MethodDefOrRef_Type_Mask;
        methodRow = methodIndex >> MethodDefOrRef_Type_Bits;
        switch (pFile->MethodImplementations[index].TypeOfMethodBody)
        {
        case MethodDefOrRef_Type_MethodDefinition:
            if (methodRow == 0 || methodRow > pFile->MethodDefinitionCount) Panic("MethodImplementation_Load MethodDefinition");
            pFile->MethodImplementations[index].MethodBody.MethodDefinition = &pFile->MethodDefinitions[methodRow];
            break;
        case MethodDefOrRef_Type_MemberReference:
            if (methodRow == 0 || methodRow > pFile->MemberReferenceCount) Panic("MethodImplementation_Load MemberReference");
            pFile->MethodImplementations[index].MethodBody.MemberReference = &pFile->MemberReferences[methodRow];
            break;
        default: break;
        }

        if (pFile->MethodDefinitionCount > MethodDefOrRef_Type_MaxRows16Bit ||
            pFile->MemberReferenceCount > MethodDefOrRef_Type_MaxRows16Bit) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodImplementations[index].TypeOfMethodDeclaration = methodIndex & MethodDefOrRef_Type_Mask;
        methodRow = methodIndex >> MethodDefOrRef_Type_Bits;
        switch (pFile->MethodImplementations[index].TypeOfMethodDeclaration)
        {
        case MethodDefOrRef_Type_MethodDefinition:
            if (methodRow == 0 || methodRow > pFile->MethodDefinitionCount) Panic("MethodImplementation_Load MethodDefinition");
            pFile->MethodImplementations[index].MethodDeclaration.MethodDefinition = &pFile->MethodDefinitions[methodRow];
            break;
        case MethodDefOrRef_Type_MemberReference:
            if (methodRow == 0 || methodRow > pFile->MemberReferenceCount) Panic("MethodImplementation_Load MemberReference");
            pFile->MethodImplementations[index].MethodDeclaration.MemberReference = &pFile->MemberReferences[methodRow];
            break;
        default: break;
        }
    }
    return pTableData;
}

void MethodImplementation_Link(CLIFile* pFile)
{
}
