#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* TypeReference_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_TypeReference)) != 0)
    {
        pFile->TypeReferenceCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->TypeReferences = (TypeReference*)calloc(pFile->TypeReferenceCount + 1, sizeof(TypeReference));
    }
    return pTableData;
}

void TypeReference_Cleanup(CLIFile* pFile)
{
    if (pFile->TypeReferences)
    {
        for (uint32_t index = 1; index <= pFile->TypeReferenceCount; ++index)
        {
            if (pFile->TypeReferences[index].CustomAttributes) free(pFile->TypeReferences[index].CustomAttributes);
            if (pFile->TypeReferences[index].MemberReferences) free(pFile->TypeReferences[index].MemberReferences);
        }
        free(pFile->TypeReferences);
        pFile->TypeReferences = NULL;
    }
}

const uint8_t* TypeReference_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t resolutionScopeIndex = 0;
    uint32_t resolutionScopeRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->TypeReferenceCount; ++index)
    {
        pFile->TypeReferences[index].TableIndex = index;
        if (pFile->ModuleDefinitionCount > ResolutionScope_Type_MaxRows16Bit ||
            pFile->ModuleReferenceCount > ResolutionScope_Type_MaxRows16Bit ||
            pFile->AssemblyReferenceCount > ResolutionScope_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > ResolutionScope_Type_MaxRows16Bit) { resolutionScopeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { resolutionScopeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeReferences[index].TypeOfResolutionScope = resolutionScopeIndex & ResolutionScope_Type_Mask;
        resolutionScopeRow = resolutionScopeIndex >> ResolutionScope_Type_Bits;
        switch (pFile->TypeReferences[index].TypeOfResolutionScope)
        {
        case ResolutionScope_Type_ModuleDefinition:
            if (resolutionScopeRow == 0 || resolutionScopeRow > pFile->ModuleDefinitionCount) Panic("TypeReference_Load ModuleDefinition");
            pFile->TypeReferences[index].ResolutionScope.ModuleDefinition = &pFile->ModuleDefinitions[resolutionScopeRow];
            break;
        case ResolutionScope_Type_ModuleReference:
            if (resolutionScopeRow == 0 || resolutionScopeRow > pFile->ModuleReferenceCount) Panic("TypeReference_Load ModuleReference");
            pFile->TypeReferences[index].ResolutionScope.ModuleReference = &pFile->ModuleReferences[resolutionScopeRow];
            break;
        case ResolutionScope_Type_AssemblyReference:
            if (resolutionScopeRow == 0 || resolutionScopeRow > pFile->AssemblyReferenceCount) Panic("TypeReference_Load AssemblyReference");
            pFile->TypeReferences[index].ResolutionScope.AssemblyReference = &pFile->AssemblyReferences[resolutionScopeRow];
            break;
        case ResolutionScope_Type_TypeReference:
            if (resolutionScopeRow == 0 || resolutionScopeRow > pFile->TypeReferenceCount) Panic("TypeReference_Load TypeReference");
            pFile->TypeReferences[index].ResolutionScope.TypeReference = &pFile->TypeReferences[resolutionScopeRow];
            break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeReferences[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeReferences[index].Namespace = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}

void TypeReference_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_TypeReference) ++pFile->CustomAttributes[index].Parent.TypeReference->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->TypeReferenceCount; ++index)
    {
        if (pFile->TypeReferences[index].CustomAttributeCount > 0)
        {
            pFile->TypeReferences[index].CustomAttributes = (CustomAttribute**)calloc(pFile->TypeReferences[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_TypeReference &&
                    pFile->CustomAttributes[searchIndex].Parent.TypeReference == &pFile->TypeReferences[index])
                {
                    pFile->TypeReferences[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
    {
        if (pFile->MemberReferences[index].TypeOfParent == MemberRefParent_Type_TypeReference) ++pFile->MemberReferences[index].Parent.TypeReference->MemberReferenceCount;
    }
    for (uint32_t index = 1; index <= pFile->TypeReferenceCount; ++index)
    {
        if (pFile->TypeReferences[index].MemberReferenceCount > 0)
        {
            pFile->TypeReferences[index].MemberReferences = (MemberReference**)calloc(pFile->TypeReferences[index].MemberReferenceCount, sizeof(MemberReference*));
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= pFile->MemberReferenceCount; ++searchIndex)
            {
                if (pFile->MemberReferences[searchIndex].TypeOfParent == MemberRefParent_Type_TypeReference &&
                    pFile->MemberReferences[searchIndex].Parent.TypeReference == &pFile->TypeReferences[index])
                {
                    pFile->TypeReferences[index].MemberReferences[memberReferenceIndex] = &pFile->MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
    }
}
