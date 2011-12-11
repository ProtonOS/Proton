#pragma once

#include <CLR/PEFile.h>
#include <CLR/MetaData.h>

#define CLIFile_Machine                             0x014C

typedef struct
{
    uint32_t SizeOfHeader;
    uint16_t MajorRuntimeVersion;
    uint16_t MinorRuntimeVersion;
    PEDataDirectory MetaData;
    uint32_t Flags;
    union
    {
        uint32_t EntryPointToken;
        uint32_t EntryPointRVA;
    };

    PEDataDirectory Resources;
    PEDataDirectory StrongNameSignature;

    PEDataDirectory CodeManagerTable;
    PEDataDirectory VTableFixups;
    PEDataDirectory ExportAddressTableJumps;

    PEDataDirectory ManagedNativeHeader;
} CLIHeader;

typedef struct
{
    PEFile* PEFile;
    CLIHeader* CLIHeader;
    uint8_t* MetaDataHeader;
    uint8_t* Tables;
    uint8_t* StringsHeap;
    uint8_t* UserStringsHeap;
    uint8_t* BlobsHeap;
    uint8_t* GUIDsHeap;
    
} CLIFile;

CLIFile* CLIFile_Create(PEFile* pFile);
void CLIFile_Destroy(CLIFile* pFile);
const char* CLIFile_GetString(CLIFile* pFile, uint32_t pVirtualAddress);
const uint8_t* CLIFile_GetUserString(CLIFile* pFile, uint32_t pVirtualAddress);
const uint8_t* CLIFile_GetBlob(CLIFile* pFile, uint32_t pVirtualAddress);
const uint8_t* CLIFile_GetGUID(CLIFile* pFile, uint32_t pVirtualAddress);
