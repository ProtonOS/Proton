#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* PropertyMap_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_PropertyMap)) != 0)
    {
        pFile->PropertyMapCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->PropertyMaps = (PropertyMap*)calloc(pFile->PropertyMapCount, sizeof(PropertyMap));
    }
    return pTableData;
}

void PropertyMap_Cleanup(CLIFile* pFile)
{
    if (pFile->PropertyMaps)
    {
        for (uint32_t index = 0; index < pFile->PropertyMapCount; ++index)
        {
        }
        free(pFile->PropertyMaps);
        pFile->PropertyMaps = NULL;
    }
}

const uint8_t* PropertyMap_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t propertyListIndex = 0;
    uint32_t* propertyListIndexes = (uint32_t*)calloc(pFile->PropertyMapCount, sizeof(uint32_t));
    for (uint32_t index = 0; index < pFile->PropertyMapCount; ++index)
    {
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->PropertyMaps[index].Parent = &pFile->TypeDefinitions[parentIndex];
        if (pFile->PropertyCount > 0xFFFF) { propertyListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { propertyListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->PropertyMaps[index].PropertyList = &pFile->Properties[propertyListIndex];
        propertyListIndexes[index] = propertyListIndex;
    }
    uint32_t propertyListCount = 0;
    for (uint32_t index = 0, used = 0; index < pFile->PropertyMapCount; ++index, used += propertyListCount)
    {
        if (index == (pFile->PropertyMapCount - 1)) propertyListCount = pFile->PropertyCount - used;
        else propertyListCount = propertyListIndexes[index + 1] - propertyListIndexes[index];
        pFile->PropertyMaps[index].PropertyListCount = propertyListCount;
    }
    free(propertyListIndexes);
    return pTableData;
}