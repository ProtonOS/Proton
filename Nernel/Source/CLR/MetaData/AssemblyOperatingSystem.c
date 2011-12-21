#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* AssemblyOperatingSystem_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_AssemblyOperatingSystem)) != 0)
    {
        pFile->AssemblyOperatingSystemCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->AssemblyOperatingSystems = (AssemblyOperatingSystem*)calloc(pFile->AssemblyOperatingSystemCount + 1, sizeof(AssemblyOperatingSystem));
    }
    return pTableData;
}

void AssemblyOperatingSystem_Cleanup(CLIFile* pFile)
{
    if (pFile->AssemblyOperatingSystems)
    {
        for (uint32_t index = 1; index <= pFile->AssemblyOperatingSystemCount; ++index)
        {
        }
        free(pFile->AssemblyOperatingSystems);
        pFile->AssemblyOperatingSystems = NULL;
    }
}

const uint8_t* AssemblyOperatingSystem_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 1; index <= pFile->AssemblyOperatingSystemCount; ++index)
    {
        pFile->AssemblyOperatingSystems[index].PlatformID = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyOperatingSystems[index].MajorVersion = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyOperatingSystems[index].MinorVersion = *(uint32_t* )pTableData; pTableData += 4;
    }
    return pTableData;
}

void AssemblyOperatingSystem_Link(CLIFile* pFile)
{
}
