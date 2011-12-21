#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* Event_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Event)) != 0)
    {
        pFile->EventCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Events = (Event*)calloc(pFile->EventCount + 1, sizeof(Event));
    }
    return pTableData;
}

void Event_Cleanup(CLIFile* pFile)
{
    if (pFile->Events)
    {
        for (uint32_t index = 1; index <= pFile->EventCount; ++index)
        {
            if (pFile->Events[index].CustomAttributes) free(pFile->Events[index].CustomAttributes);
        }
        free(pFile->Events);
        pFile->Events = NULL;
    }
}

const uint8_t* Event_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t eventTypeIndex = 0;
    uint32_t eventTypeRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->EventCount; ++index)
    {
        pFile->Events[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Events[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if (pFile->TypeDefinitionCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefOrRef_Type_MaxRows16Bit) { eventTypeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { eventTypeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Events[index].TypeOfEventType = eventTypeIndex & TypeDefOrRef_Type_Mask;
        eventTypeRow = eventTypeIndex >> TypeDefOrRef_Type_Bits;
        switch (pFile->Events[index].TypeOfEventType)
        {
        case TypeDefOrRef_Type_TypeDefinition: pFile->Events[index].EventType.TypeDefinition = &pFile->TypeDefinitions[eventTypeRow]; break;
        case TypeDefOrRef_Type_TypeReference: pFile->Events[index].EventType.TypeReference = &pFile->TypeReferences[eventTypeRow]; break;
        case TypeDefOrRef_Type_TypeSpecification: pFile->Events[index].EventType.TypeSpecification = &pFile->TypeSpecifications[eventTypeRow]; break;
        default: break;
        }
    }
    return pTableData;
}

void Event_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_Event) ++pFile->CustomAttributes[index].Parent.Event->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->EventCount; ++index)
    {
        if (pFile->Events[index].CustomAttributeCount > 0)
        {
            pFile->Events[index].CustomAttributes = (CustomAttribute**)calloc(pFile->Events[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_Event &&
                    pFile->CustomAttributes[searchIndex].Parent.Event == &pFile->Events[index])
                {
                    pFile->Events[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
