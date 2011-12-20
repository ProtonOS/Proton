#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* ClassLayout_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ClassLayout)) != 0)
    {
        pFile->ClassLayoutCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ClassLayouts = (ClassLayout*)calloc(pFile->ClassLayoutCount, sizeof(ClassLayout));
    }
    return pTableData;
}

void ClassLayout_Cleanup(CLIFile* pFile)
{
    if (pFile->ClassLayouts)
    {
        for (uint32_t index = 0; index < pFile->ClassLayoutCount; ++index)
        {
        }
        free(pFile->ClassLayouts);
        pFile->ClassLayouts = NULL;
    }
}

const uint8_t* ClassLayout_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    for (uint32_t index = 0; index < pFile->ClassLayoutCount; ++index)
    {
        pFile->ClassLayouts[index].PackingSize = *(uint16_t*)pTableData; pTableData += 2;
        pFile->ClassLayouts[index].ClassSize = *(uint32_t*)pTableData; pTableData += 4;
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        parentIndex -= 1;
        pFile->ClassLayouts[index].Parent = &pFile->TypeDefinitions[parentIndex];
        printf("ClassLayout Parent: %u/%u %s.%s, %u %u\n", (unsigned int)parentIndex, (unsigned int)pFile->TypeDefinitionCount, pFile->ClassLayouts[index].Parent->Namespace, pFile->ClassLayouts[index].Parent->Name, (unsigned int)pFile->ClassLayouts[index].PackingSize, (unsigned int)pFile->ClassLayouts[index].ClassSize);
    }
    return pTableData;
}

void ClassLayout_Link(CLIFile* pFile)
{
}
