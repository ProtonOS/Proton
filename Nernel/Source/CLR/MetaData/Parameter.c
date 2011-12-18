#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* Parameter_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Parameter)) != 0)
    {
        pFile->ParameterCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Parameters = (Parameter*)calloc(pFile->ParameterCount, sizeof(Parameter));
    }
    return pTableData;
}

void Parameter_Cleanup(CLIFile* pFile)
{
    if (pFile->Parameters)
    {
        for (uint32_t index = 0; index < pFile->ParameterCount; ++index)
        {
        }
        free(pFile->Parameters);
        pFile->Parameters = NULL;
    }
}

const uint8_t* Parameter_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ParameterCount; ++index)
    {
        pFile->Parameters[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        pFile->Parameters[index].Sequence = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Parameters[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}