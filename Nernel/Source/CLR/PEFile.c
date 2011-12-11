#include <stdlib.h>

#include <CLR/PEFile.h>

PEFile* PEFile_Create(uint8_t* pData, uint32_t pLength)
{
    DOSHeader* dosHeader = (DOSHeader*)pData;
    if (dosHeader->Signature != PEFile_DOS_Signature) return NULL;
    PEHeader* peHeader = (PEHeader*)(pData + dosHeader->NextHeaderOffset);
    if (peHeader->Signature != PEFile_NT_Signature) return NULL;
    PEOptionalHeader* peOptionalHeader = (PEOptionalHeader*)(pData + dosHeader->NextHeaderOffset + sizeof(PEHeader));
    PESectionHeader* peSectionHeaders = (PESectionHeader*)(pData + dosHeader->NextHeaderOffset + sizeof(PEHeader) + sizeof(PEOptionalHeader));

    PEFile* peFile = malloc(sizeof(PEFile));
    peFile->Data = pData;
    peFile->Length = pLength;
    peFile->DOSHeader = dosHeader;
    peFile->PEHeader = peHeader;
    peFile->OptionalHeader = peOptionalHeader;
    peFile->SectionHeaders = peSectionHeaders;

    return peFile;
}

void PEFile_Destroy(PEFile* pFile)
{
    free(pFile);
}

PESectionHeader* PEFile_GetSection(PESectionHeader* pSections, uint16_t pSectionCount, uint32_t pVirtualAddress)
{
    PESectionHeader* section = pSections;
    while (pSectionCount > 0)
    {
        if (pVirtualAddress >= section->VirtualAddress && pVirtualAddress < (section->VirtualAddress + section->Misc.VirtualSize)) return section;
        ++section;
        --pSectionCount;
    }
    return NULL;
}
