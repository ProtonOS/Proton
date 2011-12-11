#pragma once

#define PEFile_DOS_Signature            0x5A4D
#define PEFile_NT_Signature             0x00004550

typedef struct
{
    uint16_t Signature;
    uint16_t LastPageByteCount;
    uint16_t PageCount;
    uint16_t RelocationCount;
    uint16_t HeaderParagraphCount;
    uint16_t MinimumParagraphCount;
    uint16_t MaximumParagraphCount;
    uint16_t RegisterSS;
    uint16_t RegisterSP;
    uint16_t Checksum;
    uint16_t RegisterIP;
    uint16_t RegisterCS;
    uint16_t RelocationOffset;
    uint16_t Overlay;
    uint16_t Reserved[4];
    uint16_t OEMIdentifier;
    uint16_t OEMInformation;
    uint16_t MoreReserved[10];
    uint32_t NextHeaderOffset;
} DOSHeader;

typedef struct
{
    uint32_t Signature;
    uint16_t Machine;
    uint16_t NumberOfSections;
    uint32_t Timestamp;
    uint32_t PointerToSymbolTable;
    uint32_t NumberOfSymbols;
    uint16_t SizeOfOptionalHeader;
    uint16_t Characteristics;
} PEHeader;

typedef struct
{
    uint32_t VirtualAddress;
    uint32_t Size;
} PEDataDirectory;

typedef struct
{
    uint16_t Magic;
    uint8_t MajorLinkerVersion;
    uint8_t MinorLinkerVersion;
    uint32_t SizeOfCode;
    uint32_t SizeOfInitializedData;
    uint32_t SizeOfUninitializedData;
    uint32_t AddressOfEntryPoint;
    uint32_t BaseOfCode;
    uint32_t BaseOfData;

    uint32_t ImageBase;
    uint32_t SectionAlignment;
    uint32_t FileAlignment;
    uint16_t MajorOperatingSystemVersion;
    uint16_t MinorOperatingSystemVersion;
    uint16_t MajorImageVersion;
    uint16_t MinorImageVersion;
    uint16_t MajorSubsystemVersion;
    uint16_t MinorSubsystemVersion;
    uint32_t Reserved;
    uint32_t SizeOfImage;
    uint32_t SizeOfHeaders;
    uint32_t Checksum;
    uint16_t Subsystem;
    uint16_t DllCharacteristics;
    uint32_t SizeOfStackReserve;
    uint32_t SizeOfStackCommit;
    uint32_t SizeOfHeapReserve;
    uint32_t SizeOfHeapCommit;
    uint32_t LoaderFlags;
    uint32_t NumberOfRvaAndSizes;
    PEDataDirectory DataDirectory[16];
} PEOptionalHeader;

typedef struct
{
    uint8_t Name[8];
    union
    {
        uint32_t PhysicalAddress;
        uint32_t VirtualSize;
    } Misc;
    uint32_t VirtualAddress;
    uint32_t SizeOfRawData;
    uint32_t PointerToRawData;
    uint32_t PointerToRelocations;
    uint32_t PointerToLineNumbers;
    uint16_t NumberOfRelocations;
    uint16_t NumberOfLineNumbers;
    uint32_t Characteristics;
} PESectionHeader;

typedef struct
{
    uint8_t* Data;
    uint32_t Length;
    DOSHeader* DOSHeader;
    PEHeader* PEHeader;
    PEOptionalHeader* OptionalHeader;
    PESectionHeader* SectionHeaders;
} PEFile;

PEFile* PEFile_Create(uint8_t* pData, uint32_t pLength);
void PEFile_Destroy(PEFile* pFile);
PESectionHeader* PEFile_GetSection(PESectionHeader* pSections, uint16_t pSectionCount, uint32_t pVirtualAddress);
