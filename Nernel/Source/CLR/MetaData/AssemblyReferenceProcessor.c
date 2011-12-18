#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* AssemblyReferenceProcessor_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_AssemblyReferenceProcessor)) != 0)
    {
        pFile->AssemblyReferenceProcessorCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->AssemblyReferenceProcessors = (AssemblyReferenceProcessor*)calloc(pFile->AssemblyReferenceProcessorCount, sizeof(AssemblyReferenceProcessor));
    }
    return pTableData;
}

void AssemblyReferenceProcessor_Cleanup(CLIFile* pFile)
{
    if (pFile->AssemblyReferenceProcessors)
    {
        for (uint32_t index = 0; index < pFile->AssemblyReferenceProcessorCount; ++index)
        {
        }
        free(pFile->AssemblyReferenceProcessors);
        pFile->AssemblyReferenceProcessors = NULL;
    }
}

const uint8_t* AssemblyReferenceProcessor_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t assemblyReferenceIndex = 0;
    for (uint32_t index = 0; index < pFile->AssemblyReferenceProcessorCount; ++index)
    {
        pFile->AssemblyReferenceProcessors[index].Processor = *(uint32_t* )pTableData; pTableData += 4;
        if (pFile->AssemblyReferenceCount > 0xFFFF) { assemblyReferenceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { assemblyReferenceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyReferenceProcessors[index].AssemblyReference = &pFile->AssemblyReferences[assemblyReferenceIndex];
    }
    return pTableData;
}