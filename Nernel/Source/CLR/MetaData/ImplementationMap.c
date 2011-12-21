#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* ImplementationMap_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ImplementationMap)) != 0)
    {
        pFile->ImplementationMapCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ImplementationMaps = (ImplementationMap*)calloc(pFile->ImplementationMapCount + 1, sizeof(ImplementationMap));
    }
    return pTableData;
}

void ImplementationMap_Cleanup(CLIFile* pFile)
{
    if (pFile->ImplementationMaps)
    {
        for (uint32_t index = 1; index <= pFile->ImplementationMapCount; ++index)
        {
        }
        free(pFile->ImplementationMaps);
        pFile->ImplementationMaps = NULL;
    }
}

const uint8_t* ImplementationMap_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t memberForwardedIndex = 0;
    uint32_t memberForwardedRow = 0;
    uint32_t importScopeIndex = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ImplementationMapCount; ++index)
    {
        pFile->ImplementationMaps[index].MappingFlags = *(uint16_t*)pTableData; pTableData += 2;

        if (pFile->FieldCount > MemberForwarded_Type_MaxRows16Bit ||
            pFile->MethodDefinitionCount > MemberForwarded_Type_MaxRows16Bit) { memberForwardedIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { memberForwardedIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ImplementationMaps[index].TypeOfMemberForwarded = memberForwardedIndex & MemberForwarded_Type_Mask;
        memberForwardedRow = memberForwardedIndex >> MemberForwarded_Type_Bits;
        switch (pFile->ImplementationMaps[index].TypeOfMemberForwarded)
        {
        case MemberForwarded_Type_Field: pFile->ImplementationMaps[index].MemberForwarded.Field = &pFile->Fields[memberForwardedRow]; break;
        case MemberForwarded_Type_MethodDefinition: pFile->ImplementationMaps[index].MemberForwarded.MethodDefinition = &pFile->MethodDefinitions[memberForwardedRow]; break;
        default: break;
        }

        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ImplementationMaps[index].ImportName = (const char*)(pFile->StringsHeap + heapIndex);

        if (pFile->ModuleReferenceCount > 0xFFFF) { importScopeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { importScopeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ImplementationMaps[index].ImportScope = &pFile->ModuleReferences[importScopeIndex];
    }
    return pTableData;
}

void ImplementationMap_Link(CLIFile* pFile)
{
}
