#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* AssemblyReferenceOperatingSystem_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_AssemblyReferenceOperatingSystem)) != 0)
    {
        pFile->AssemblyReferenceOperatingSystemCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->AssemblyReferenceOperatingSystems = (AssemblyReferenceOperatingSystem*)calloc(pFile->AssemblyReferenceOperatingSystemCount + 1, sizeof(AssemblyReferenceOperatingSystem));
    }
    return pTableData;
}

void AssemblyReferenceOperatingSystem_Cleanup(CLIFile* pFile)
{
    if (pFile->AssemblyReferenceOperatingSystems)
    {
        for (uint32_t index = 1; index <= pFile->AssemblyReferenceOperatingSystemCount; ++index)
        {
        }
        free(pFile->AssemblyReferenceOperatingSystems);
        pFile->AssemblyReferenceOperatingSystems = NULL;
    }
}

const uint8_t* AssemblyReferenceOperatingSystem_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t assemblyReferenceIndex = 0;
    for (uint32_t index = 1; index <= pFile->AssemblyReferenceOperatingSystemCount; ++index)
    {
        pFile->AssemblyReferenceOperatingSystems[index].TableIndex = index;
        pFile->AssemblyReferenceOperatingSystems[index].PlatformID = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyReferenceOperatingSystems[index].MajorVersion = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyReferenceOperatingSystems[index].MinorVersion = *(uint32_t* )pTableData; pTableData += 4;
        if (pFile->AssemblyReferenceCount > 0xFFFF) { assemblyReferenceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { assemblyReferenceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (assemblyReferenceIndex == 0 || assemblyReferenceIndex > pFile->AssemblyReferenceCount) Panic("AssemblyReferenceOperatingSystem_Load");
        pFile->AssemblyReferenceOperatingSystems[index].AssemblyReference = &pFile->AssemblyReferences[assemblyReferenceIndex];
    }
    return pTableData;
}

void AssemblyReferenceOperatingSystem_Link(CLIFile* pFile)
{
}
