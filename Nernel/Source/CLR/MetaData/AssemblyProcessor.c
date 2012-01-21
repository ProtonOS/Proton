#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* AssemblyProcessor_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_AssemblyProcessor)) != 0)
    {
        pFile->AssemblyProcessorCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->AssemblyProcessors = (AssemblyProcessor*)calloc(pFile->AssemblyProcessorCount + 1, sizeof(AssemblyProcessor));
    }
    return pTableData;
}

void AssemblyProcessor_Cleanup(CLIFile* pFile)
{
    if (pFile->AssemblyProcessors)
    {
        for (uint32_t index = 1; index <= pFile->AssemblyProcessorCount; ++index)
        {
        }
        free(pFile->AssemblyProcessors);
        pFile->AssemblyProcessors = NULL;
    }
}

const uint8_t* AssemblyProcessor_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 1; index <= pFile->AssemblyProcessorCount; ++index)
    {
        pFile->AssemblyProcessors[index].TableIndex = index;
        pFile->AssemblyProcessors[index].Processor = *(uint32_t* )pTableData; pTableData += 4;
    }
    return pTableData;
}

void AssemblyProcessor_Link(CLIFile* pFile)
{
}
