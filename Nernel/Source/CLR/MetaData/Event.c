#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* Event_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Event)) != 0)
    {
        pFile->EventCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Events = (Event*)calloc(pFile->EventCount, sizeof(Event));
    }
    return pTableData;
}

void Event_Cleanup(CLIFile* pFile)
{
    if (pFile->Events)
    {
        for (uint32_t index = 0; index < pFile->EventCount; ++index)
        {
        }
        free(pFile->Events);
        pFile->Events = NULL;
    }
}

const uint8_t* Event_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t eventTypeIndex = 0;
    uint8_t eventTypeTable = 0;
    uint32_t eventTypeRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->EventCount; ++index)
    {
        pFile->Events[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Events[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if (pFile->TypeDefinitionCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefOrRef_Type_MaxRows16Bit) { eventTypeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { eventTypeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        eventTypeTable = eventTypeIndex & TypeDefOrRef_Type_Mask;
        eventTypeRow = eventTypeIndex >> TypeDefOrRef_Type_Bits;
        switch (eventTypeTable)
        {
        case TypeDefOrRef_Type_TypeDefinition: pFile->Events[index].EventType.TypeDefinition = &pFile->TypeDefinitions[eventTypeRow]; break;
        case TypeDefOrRef_Type_TypeReference: pFile->Events[index].EventType.TypeReference = &pFile->TypeReferences[eventTypeRow]; break;
        case TypeDefOrRef_Type_TypeSpecification: pFile->Events[index].EventType.TypeSpecification = &pFile->TypeSpecifications[eventTypeRow]; break;
        default: break;
        }
    }
    return pTableData;
}