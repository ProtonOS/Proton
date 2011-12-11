#include <stdlib.h>

#include <CLR/CLIFile.h>

CLIFile* CLIFile_Create(PEFile* pFile)
{
    if (pFile->PEHeader->Machine != CLIFile_Machine) return NULL;

    PEDataDirectory* cliHeaderDataDirectory = &pFile->OptionalHeader->DataDirectory[14];
    PESectionHeader* cliHeaderSectionHeader = PEFile_GetSection(pFile->SectionHeaders, pFile->PEHeader->NumberOfSections, cliHeaderDataDirectory->VirtualAddress);
    CLIHeader* cliHeader = (CLIHeader*)(pFile->Data + cliHeaderSectionHeader->PointerToRawData + (cliHeaderDataDirectory->VirtualAddress - cliHeaderSectionHeader->VirtualAddress));

    PESectionHeader* cliMetaDataSectionHeader = PEFile_GetSection(pFile->SectionHeaders, pFile->PEHeader->NumberOfSections, cliHeader->MetaData.VirtualAddress);
    uint8_t* cliMetaDataHeader = pFile->Data + cliMetaDataSectionHeader->PointerToRawData + (cliHeader->MetaData.VirtualAddress - cliMetaDataSectionHeader->VirtualAddress);

    if (!MetaData_IsValidSignature(cliMetaDataHeader)) return NULL;

    CLIFile* cliFile = malloc(sizeof(CLIFile));
    cliFile->PEFile = pFile;
    cliFile->CLIHeader = cliHeader;
    cliFile->MetaDataHeader = cliMetaDataHeader;

    return cliFile;
}

void CLIFile_Destroy(CLIFile* pFile)
{
    PEFile_Destroy(pFile->PEFile);
    free(pFile);
}

const char* CLIFile_GetString(CLIFile* pFile, uint32_t pVirtualAddress) { return (const char*)(pFile->StringsHeap + pVirtualAddress); }
const uint8_t* CLIFile_GetUserString(CLIFile* pFile, uint32_t pVirtualAddress) { return (const uint8_t*)(pFile->UserStringsHeap + pVirtualAddress); }
const uint8_t* CLIFile_GetBlob(CLIFile* pFile, uint32_t pVirtualAddress) { return (const uint8_t*)(pFile->BlobsHeap + pVirtualAddress); }
const uint8_t* CLIFile_GetGUID(CLIFile* pFile, uint32_t pVirtualAddress) { return (const uint8_t*)(pFile->GUIDsHeap + pVirtualAddress); }
