#include <Common.h>
#include <CLR/CLIFile.h>


typedef uint8_t* (*CLIFile_MetadataInitialize_Handler)(CLIFile* pFile, uint8_t* pTableData);
typedef uint8_t* (*CLIFile_MetadataLoad_Handler)(CLIFile* pFile, uint8_t* pTableData);
typedef void (*CLIFile_MetadataLink_Handler)(CLIFile* pFile);
typedef void (*CLIFile_MetadataCleanup_Handler)(CLIFile* pFile);

struct
{
    CLIFile_MetadataInitialize_Handler Initialize;
    CLIFile_MetadataLoad_Handler Load;
    CLIFile_MetadataLink_Handler Link;
    CLIFile_MetadataCleanup_Handler Cleanup;
} gCLIFile_MetadataHandler_Table[] =
{
    { &ModuleDefinition_Initialize,                     &ModuleDefinition_Load,                     &ModuleDefinition_Link,                     &ModuleDefinition_Cleanup },
    { &TypeReference_Initialize,                        &TypeReference_Load,                        &TypeReference_Link,                        &TypeReference_Cleanup },
    { &TypeDefinition_Initialize,                       &TypeDefinition_Load,                       &TypeDefinition_Link,                       &TypeDefinition_Cleanup },
    { &Field_Initialize,                                &Field_Load,                                &Field_Link,                                &Field_Cleanup },
    { &MethodDefinition_Initialize,                     &MethodDefinition_Load,                     &MethodDefinition_Link,                     &MethodDefinition_Cleanup },
    { &Parameter_Initialize,                            &Parameter_Load,                            &Parameter_Link,                            &Parameter_Cleanup },
    { &InterfaceImplementation_Initialize,              &InterfaceImplementation_Load,              &InterfaceImplementation_Link,              &InterfaceImplementation_Cleanup },
    { &MemberReference_Initialize,                      &MemberReference_Load,                      &MemberReference_Link,                      &MemberReference_Cleanup },
    { &Constant_Initialize,                             &Constant_Load,                             &Constant_Link,                             &Constant_Cleanup },
    { &CustomAttribute_Initialize,                      &CustomAttribute_Load,                      &CustomAttribute_Link,                      &CustomAttribute_Cleanup },
    { &FieldMarshal_Initialize,                         &FieldMarshal_Load,                         &FieldMarshal_Link,                         &FieldMarshal_Cleanup },
    { &DeclSecurity_Initialize,                         &DeclSecurity_Load,                         &DeclSecurity_Link,                         &DeclSecurity_Cleanup },
    { &ClassLayout_Initialize,                          &ClassLayout_Load,                          &ClassLayout_Link,                          &ClassLayout_Cleanup },
    { &FieldLayout_Initialize,                          &FieldLayout_Load,                          &FieldLayout_Link,                          &FieldLayout_Cleanup },
    { &StandAloneSignature_Initialize,                  &StandAloneSignature_Load,                  &StandAloneSignature_Link,                  &StandAloneSignature_Cleanup },
    { &EventMap_Initialize,                             &EventMap_Load,                             &EventMap_Link,                             &EventMap_Cleanup },
    { &Event_Initialize,                                &Event_Load,                                &Event_Link,                                &Event_Cleanup },
    { &PropertyMap_Initialize,                          &PropertyMap_Load,                          &PropertyMap_Link,                          &PropertyMap_Cleanup },
    { &Property_Initialize,                             &Property_Load,                             &Property_Link,                             &Property_Cleanup },
    { &MethodSemantics_Initialize,                      &MethodSemantics_Load,                      &MethodSemantics_Link,                      &MethodSemantics_Cleanup },
    { &MethodImplementation_Initialize,                 &MethodImplementation_Load,                 &MethodImplementation_Link,                 &MethodImplementation_Cleanup },
    { &ModuleReference_Initialize,                      &ModuleReference_Load,                      &ModuleReference_Link,                      &ModuleReference_Cleanup },
    { &TypeSpecification_Initialize,                    &TypeSpecification_Load,                    &TypeSpecification_Link,                    &TypeSpecification_Cleanup },
    { &ImplementationMap_Initialize,                    &ImplementationMap_Load,                    &ImplementationMap_Link,                    &ImplementationMap_Cleanup },
    { &FieldRVA_Initialize,                             &FieldRVA_Load,                             &FieldRVA_Link,                             &FieldRVA_Cleanup },
    { &AssemblyDefinition_Initialize,                   &AssemblyDefinition_Load,                   &AssemblyDefinition_Link,                   &AssemblyDefinition_Cleanup },
    { &AssemblyProcessor_Initialize,                    &AssemblyProcessor_Load,                    &AssemblyProcessor_Link,                    &AssemblyProcessor_Cleanup },
    { &AssemblyOperatingSystem_Initialize,              &AssemblyOperatingSystem_Load,              &AssemblyOperatingSystem_Link,              &AssemblyOperatingSystem_Cleanup },
    { &AssemblyReference_Initialize,                    &AssemblyReference_Load,                    &AssemblyReference_Link,                    &AssemblyReference_Cleanup },
    { &AssemblyReferenceProcessor_Initialize,           &AssemblyReferenceProcessor_Load,           &AssemblyReferenceProcessor_Link,           &AssemblyReferenceProcessor_Cleanup },
    { &AssemblyReferenceOperatingSystem_Initialize,     &AssemblyReferenceOperatingSystem_Load,     &AssemblyReferenceOperatingSystem_Link,     &AssemblyReferenceOperatingSystem_Cleanup },
    { &File_Initialize,                                 &File_Load,                                 &File_Link,                                 &File_Cleanup },
    { &ExportedType_Initialize,                         &ExportedType_Load,                         &ExportedType_Link,                         &ExportedType_Cleanup },
    { &ManifestResource_Initialize,                     &ManifestResource_Load,                     &ManifestResource_Link,                     &ManifestResource_Cleanup },
    { &NestedClass_Initialize,                          &NestedClass_Load,                          &NestedClass_Link,                          &NestedClass_Cleanup },
    { &GenericParameter_Initialize,                     &GenericParameter_Load,                     &GenericParameter_Link,                     &GenericParameter_Cleanup },
    { &MethodSpecification_Initialize,                  &MethodSpecification_Load,                  &MethodSpecification_Link,                  &MethodSpecification_Cleanup },
    { &GenericParameterConstraint_Initialize,           &GenericParameterConstraint_Load,           &GenericParameterConstraint_Link,           &GenericParameterConstraint_Cleanup },
    { NULL,                                             NULL,                                       NULL,                                       NULL }
};



CLIFile* CLIFile_Create(uint8_t* pData, uint32_t pLength, const char* pFilename)
{
    PEDOSHeader* dosHeader = (PEDOSHeader*)pData;
    if (dosHeader->Signature != CLIFILE__PE__DOS_Signature) return NULL;
    PENTHeader* ntHeader = (PENTHeader*)(pData + dosHeader->NextHeaderOffset);
    if (ntHeader->Signature != CLIFILE__PE__NT_Signature) return NULL;
    if (ntHeader->Machine != CLIFILE__Machine) return NULL;
    PEOptionalHeader* peOptionalHeader = (PEOptionalHeader*)(pData + dosHeader->NextHeaderOffset + sizeof(PENTHeader));
    PESectionHeader* peSectionHeaders = (PESectionHeader*)(pData + dosHeader->NextHeaderOffset + sizeof(PENTHeader) + sizeof(PEOptionalHeader));

    PEDataDirectory* cliHeaderDataDirectory = &peOptionalHeader->DataDirectory[14];
    PESectionHeader* cliHeaderSectionHeader = CLIFile_GetSection(peSectionHeaders, ntHeader->NumberOfSections, cliHeaderDataDirectory->VirtualAddress);
    PECLIHeader* cliHeader = (PECLIHeader*)(pData + cliHeaderSectionHeader->PointerToRawData + (cliHeaderDataDirectory->VirtualAddress - cliHeaderSectionHeader->VirtualAddress));

    PESectionHeader* cliMetadataSectionHeader = CLIFile_GetSection(peSectionHeaders, ntHeader->NumberOfSections, cliHeader->Metadata.VirtualAddress);
    uint8_t* cliMetadataHeader = pData + cliMetadataSectionHeader->PointerToRawData + (cliHeader->Metadata.VirtualAddress - cliMetadataSectionHeader->VirtualAddress);

	if (*(uint32_t*)(cliMetadataHeader + CLIFILE__Metadata__HeaderOffset_Signature) != CLIFILE__Metadata__Signature) return NULL;

    CLIFile* file = (CLIFile*)calloc(1, sizeof(CLIFile));
    file->Data = pData;
    file->Length = pLength;
	file->Filename = calloc(1, strlen(pFilename) + 1);
	strcpy(file->Filename, pFilename);
    file->DOSHeader = dosHeader;
    file->NTHeader = ntHeader;
    file->OptionalHeader = peOptionalHeader;
    file->SectionHeaders = peSectionHeaders;
    file->CLIHeader = cliHeader;
    file->MetadataHeader = cliMetadataHeader;

	uint32_t versionLength = *(uint32_t*)(cliMetadataHeader + CLIFILE__Metadata__HeaderOffset_VersionLength);
	uint16_t streamCount = *(uint16_t*)(cliMetadataHeader + CLIFILE__Metadata__HeaderOffset_Version + versionLength + CLIFILE__Metadata__HeaderVersionOffset_StreamCount);
	uint8_t* streamStart  = cliMetadataHeader + CLIFILE__Metadata__HeaderOffset_Version + versionLength + CLIFILE__Metadata__HeaderVersionOffset_Streams;
	uint8_t* streamIterator = streamStart;
    for (uint16_t streamIndex = 0; streamIndex < streamCount; ++streamIndex)
    {
        char* streamName = (char*)(streamIterator + CLIFILE__Metadata__StreamOffset_Name);
        uint8_t* streamData = cliMetadataHeader + *(uint32_t*)(streamIterator + CLIFILE__Metadata__StreamOffset_RVA);

        if (!strcmp(streamName, "#~")) file->Tables = streamData;
        else if (!strcmp(streamName, "#Strings")) file->StringsHeap = streamData;
        else if (!strcmp(streamName, "#US")) file->UserStringsHeap = streamData;
        else if (!strcmp(streamName, "#Blob")) file->BlobsHeap = streamData;
        else if (!strcmp(streamName, "#GUID")) file->GUIDsHeap = streamData;

        streamIterator += CLIFILE__Metadata__StreamOffset_Name;
        while (*streamIterator) ++streamIterator;
		streamIterator += 4 - ((unsigned int)streamIterator & 0x03);
	}
    file->TablesHeader = (PECLIMetadataTablesHeader*)file->Tables;

    uint8_t* tableData = file->Tables + sizeof(PECLIMetadataTablesHeader);
    uint32_t tableCount = 0;
    while (gCLIFile_MetadataHandler_Table[tableCount].Initialize) ++tableCount;

    for (uint32_t tableIndex = 0; tableIndex < tableCount; ++tableIndex) tableData = gCLIFile_MetadataHandler_Table[tableIndex].Initialize(file, tableData);
    for (uint32_t tableIndex = 0; tableIndex < tableCount; ++tableIndex) tableData = gCLIFile_MetadataHandler_Table[tableIndex].Load(file, tableData);
    for (uint32_t tableIndex = 0; tableIndex < tableCount; ++tableIndex) gCLIFile_MetadataHandler_Table[tableIndex].Link(file);

	return file;
}

void CLIFile_Destroy(CLIFile* pFile)
{
    uint32_t tableCount = 0;
    while (gCLIFile_MetadataHandler_Table[tableCount].Initialize) ++tableCount;
    for (uint32_t tableIndex = 0; tableIndex < tableCount; ++tableIndex) gCLIFile_MetadataHandler_Table[tableIndex].Cleanup(pFile);
	free(pFile->Filename);
	free(pFile);
}

PESectionHeader* CLIFile_GetSection(PESectionHeader* pSections, uint16_t pSectionCount, uint32_t pVirtualAddress)
{
    PESectionHeader* section = pSections;
	uint16_t sectionCount = pSectionCount;
    while (sectionCount > 0)
    {
        if (pVirtualAddress >= section->VirtualAddress && pVirtualAddress < (section->VirtualAddress + section->Misc.VirtualSize)) return section;
        ++section;
        --sectionCount;
    }
    return NULL;
}

uint8_t* CLIFile_GetCompressedUnsigned(uint8_t* pData, uint32_t* pValue)
{
    if ((*pData & 0x80) == 0)
    {
        *pValue = pData[0] & 0x7F;
        return pData + 1;
    }
    if ((*pData & 0xC0) == 0x80)
    {
        *pValue = ((pData[0] & 0x3F) << 8) + pData[1];
        return pData + 2;
    }
    if ((*pData & 0xE0) == 0xC0)
    {
        *pValue = ((pData[0] & 0x1F) << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        return pData + 4;
    }
    *pValue = 0;
    return pData;
}

int32_t CLIFile_RotateRight(int32_t pValue, uint8_t pBits)
{
    bool_t bit = (pValue & 0x01) != 0;
    pValue >>= 1;
    if (bit) pValue |= (1 << (pBits - 1));
    return pValue;
}

uint8_t* CLI_GetCompressedSigned(uint8_t* pData, int32_t* pValue)
{
    if ((*pData & 0x80) == 0)
    {
        *pValue = pData[0] & 0x7F;
        *pValue = CLIFile_RotateRight(*pValue, 7);
        if ((*pValue & 0x40) != 0) *pValue |= 0xFFFFFF80;
        return pData + 1;
    }
    if ((*pData & 0xC0) == 0x80)
    {
        *pValue = ((pData[0] & 0x3F) << 8) + pData[1];
        *pValue = CLIFile_RotateRight(*pValue, 14);
        if ((*pValue & 0x2000) != 0) *pValue |= 0xFFFFC000;
        return pData + 2;
    }
    if ((*pData & 0xE0) == 0xC0)
    {
        *pValue = ((pData[0] & 0x1F) << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        *pValue = CLIFile_RotateRight(*pValue, 29);
        if ((*pValue & 0x10000000) != 0) *pValue |= 0xE0000000;
        return pData + 4;
    }
    *pValue = 0;
    return pData;
}
