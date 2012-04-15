#include <Common.h>
#include <CLR/CLIFile.h>
#include <System/SystemClock.h>


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
	uint64_t startedTicks = SystemClock_GetTicks();

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

	uint64_t stoppedTicks = SystemClock_GetTicks();
	printf("CLIFile_Create took %uMS\n", (unsigned int)(stoppedTicks - startedTicks));
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

uint8_t* CLIFile_GetCompressedSigned(uint8_t* pData, int32_t* pValue)
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

MetadataToken* CLIFile_ExpandMetadataToken(CLIFile* pFile, uint32_t pToken)
{
    MetadataToken* token = (MetadataToken*)calloc(1, sizeof(MetadataToken));
    token->Table = pToken >> 24;
    token->Data = NULL;
    uint32_t index = pToken & 0x00FFFFFF;
    if (index == 0) return token;
    if (token->Table == MetadataTable_UserStrings)
    {
        token->IsUserString = TRUE;
        token->Data = pFile->UserStringsHeap + index;
        return token;
    }
    switch (token->Table)
    {
    case MetadataTable_ModuleDefinition:
        if (index > pFile->ModuleDefinitionCount) Panic("CLIFile_ExpandMetadataToken ModuleDefinition");
        token->Data = &pFile->ModuleDefinitions[index];
        break;
    case MetadataTable_TypeReference:
        if (index > pFile->TypeReferenceCount) Panic("CLIFile_ExpandMetadataToken TypeReference");
        token->Data = &pFile->TypeReferences[index];
        break;
    case MetadataTable_TypeDefinition:
        if (index > pFile->TypeDefinitionCount) Panic("CLIFile_ExpandMetadataToken TypeDefinition");
        token->Data = &pFile->TypeDefinitions[index];
        break;
    case MetadataTable_Field:
        if (index > pFile->FieldCount) Panic("CLIFile_ExpandMetadataToken Field");
        token->Data = &pFile->Fields[index];
        break;
    case MetadataTable_MethodDefinition:
        if (index > pFile->MethodDefinitionCount) Panic("CLIFile_ExpandMetadataToken MethodDefinition");
        token->Data = &pFile->MethodDefinitions[index];
        break;
    case MetadataTable_Parameter:
        if (index > pFile->ParameterCount) Panic("CLIFile_ExpandMetadataToken Parameter");
        token->Data = &pFile->Parameters[index];
        break;
    case MetadataTable_InterfaceImplementation:
        if (index > pFile->InterfaceImplementationCount) Panic("CLIFile_ExpandMetadataToken InterfaceImplementation");
        token->Data = &pFile->InterfaceImplementations[index];
        break;
    case MetadataTable_MemberReference:
        if (index > pFile->MemberReferenceCount) Panic("CLIFile_ExpandMetadataToken MemberReference");
        token->Data = &pFile->MemberReferences[index];
        break;
    case MetadataTable_Constant:
        if (index > pFile->ConstantCount) Panic("CLIFile_ExpandMetadataToken Constant");
        token->Data = &pFile->Constants[index];
        break;
    case MetadataTable_CustomAttribute:
        if (index > pFile->CustomAttributeCount) Panic("CLIFile_ExpandMetadataToken CustomAttribute");
        token->Data = &pFile->CustomAttributes[index];
        break;
    case MetadataTable_FieldMarshal:
        if (index > pFile->FieldMarshalCount) Panic("CLIFile_ExpandMetadataToken FieldMarshal");
        token->Data = &pFile->FieldMarshals[index];
        break;
    case MetadataTable_DeclSecurity:
        if (index > pFile->DeclSecurityCount) Panic("CLIFile_ExpandMetadataToken DeclSecurity");
        token->Data = &pFile->DeclSecurities[index];
        break;
    case MetadataTable_ClassLayout:
        if (index > pFile->ClassLayoutCount) Panic("CLIFile_ExpandMetadataToken ClassLayout");
        token->Data = &pFile->ClassLayouts[index];
        break;
    case MetadataTable_FieldLayout:
        if (index > pFile->FieldLayoutCount) Panic("CLIFile_ExpandMetadataToken FieldLayout");
        token->Data = &pFile->FieldLayouts[index];
        break;
    case MetadataTable_StandAloneSignature:
        if (index > pFile->StandAloneSignatureCount) Panic("CLIFile_ExpandMetadataToken StandAloneSignature");
        token->Data = &pFile->StandAloneSignatures[index];
        break;
    case MetadataTable_EventMap:
        if (index > pFile->EventMapCount) Panic("CLIFile_ExpandMetadataToken EventMap");
        token->Data = &pFile->EventMaps[index];
        break;
    case MetadataTable_Event:
        if (index > pFile->EventCount) Panic("CLIFile_ExpandMetadataToken Event");
        token->Data = &pFile->Events[index];
        break;
    case MetadataTable_PropertyMap:
        if (index > pFile->PropertyMapCount) Panic("CLIFile_ExpandMetadataToken PropertyMap");
        token->Data = &pFile->PropertyMaps[index];
        break;
    case MetadataTable_Property:
        if (index > pFile->PropertyCount) Panic("CLIFile_ExpandMetadataToken Property");
        token->Data = &pFile->Properties[index];
        break;
    case MetadataTable_MethodSemantics:
        if (index > pFile->MethodSemanticsCount) Panic("CLIFile_ExpandMetadataToken MethodSemantics");
        token->Data = &pFile->MethodSemantics[index];
        break;
    case MetadataTable_MethodImplementation:
        if (index > pFile->MethodImplementationCount) Panic("CLIFile_ExpandMetadataToken MethodImplementation");
        token->Data = &pFile->MethodImplementations[index];
        break;
    case MetadataTable_ModuleReference:
        if (index > pFile->ModuleReferenceCount) Panic("CLIFile_ExpandMetadataToken ModuleReference");
        token->Data = &pFile->ModuleReferences[index];
        break;
    case MetadataTable_TypeSpecification:
        if (index > pFile->TypeSpecificationCount) Panic("CLIFile_ExpandMetadataToken TypeSpecification");
        token->Data = &pFile->TypeSpecifications[index];
        break;
    case MetadataTable_ImplementationMap:
        if (index > pFile->ImplementationMapCount) Panic("CLIFile_ExpandMetadataToken ImplementationMap");
        token->Data = &pFile->ImplementationMaps[index];
        break;
    case MetadataTable_FieldRVA:
        if (index > pFile->FieldRVACount) Panic("CLIFile_ExpandMetadataToken FieldRVA");
        token->Data = &pFile->FieldRVAs[index];
        break;
    case MetadataTable_AssemblyDefinition:
        if (index > pFile->AssemblyDefinitionCount) Panic("CLIFile_ExpandMetadataToken AssemblyDefinition");
        token->Data = &pFile->AssemblyDefinitions[index];
        break;
    case MetadataTable_AssemblyProcessor:
        if (index > pFile->AssemblyProcessorCount) Panic("CLIFile_ExpandMetadataToken AssemblyProcessor");
        token->Data = &pFile->AssemblyProcessors[index];
        break;
    case MetadataTable_AssemblyOperatingSystem:
        if (index > pFile->AssemblyOperatingSystemCount) Panic("CLIFile_ExpandMetadataToken AssemblyOperatingSystem");
        token->Data = &pFile->AssemblyOperatingSystems[index];
        break;
    case MetadataTable_AssemblyReference:
        if (index > pFile->AssemblyReferenceCount) Panic("CLIFile_ExpandMetadataToken AssemblyReference");
        token->Data = &pFile->AssemblyReferences[index];
        break;
    case MetadataTable_AssemblyReferenceProcessor:
        if (index > pFile->AssemblyReferenceProcessorCount) Panic("CLIFile_ExpandMetadataToken AssemblyReferenceProcessor");
        token->Data = &pFile->AssemblyReferenceProcessors[index];
        break;
    case MetadataTable_AssemblyReferenceOperatingSystem:
        if (index > pFile->AssemblyReferenceOperatingSystemCount) Panic("CLIFile_ExpandMetadataToken AssemblyReferenceOperatingSystem");
        token->Data = &pFile->AssemblyReferenceOperatingSystems[index];
        break;
    case MetadataTable_File:
        if (index > pFile->FileCount) Panic("CLIFile_ExpandMetadataToken File");
        token->Data = &pFile->Files[index];
        break;
    case MetadataTable_ExportedType:
        if (index > pFile->ExportedTypeCount) Panic("CLIFile_ExpandMetadataToken ExportedType");
        token->Data = &pFile->ExportedTypes[index];
        break;
    case MetadataTable_ManifestResource:
        if (index > pFile->ManifestResourceCount) Panic("CLIFile_ExpandMetadataToken ManifestResource");
        token->Data = &pFile->ManifestResources[index];
        break;
    case MetadataTable_NestedClass:
        if (index > pFile->NestedClassCount) Panic("CLIFile_ExpandMetadataToken NestedClass");
        token->Data = &pFile->NestedClasses[index];
        break;
    case MetadataTable_GenericParameter:
        if (index > pFile->GenericParameterCount) Panic("CLIFile_ExpandMetadataToken GenericParameter");
        token->Data = &pFile->GenericParameters[index];
        break;
    case MetadataTable_MethodSpecification:
        if (index > pFile->MethodSpecificationCount) Panic("CLIFile_ExpandMetadataToken MethodSpecification");
        token->Data = &pFile->MethodSpecifications[index];
        break;
    case MetadataTable_GenericParameterConstraint:
        if (index > pFile->GenericParameterConstraintCount) Panic("CLIFile_ExpandMetadataToken GenericParameterConstraint");
        token->Data = &pFile->GenericParameterConstraints[index];
        break;
    default:
        Panic("CLIFile_ExpandMetadataToken Unknown Table Type");
        break;
    }
    return token;
}

MetadataToken* CLIFile_ExpandTypeDefRefOrSpecToken(CLIFile* pFile, uint32_t pToken)
{
    MetadataToken* token = (MetadataToken*)calloc(1, sizeof(MetadataToken));
	uint8_t typeDefRefOrSpec = pToken & 0x03;
	switch (typeDefRefOrSpec)
	{
		case 0: token->Table = MetadataTable_TypeDefinition; break;
		case 1: token->Table = MetadataTable_TypeReference; break;
		case 2: token->Table = MetadataTable_TypeSpecification; break;
		default: Panic("CLIFile_ExpandTypeDefRefOrSpecToken Unknown Table Type"); break;
	}
    token->Data = NULL;
    uint32_t index = pToken >> 2;
    if (index == 0) return token;
    if (token->Table == MetadataTable_UserStrings)
    {
        token->IsUserString = TRUE;
        token->Data = pFile->UserStringsHeap + index;
        return token;
    }
    switch (token->Table)
    {
    case MetadataTable_TypeReference:
        if (index > pFile->TypeReferenceCount) Panic("CLIFile_ExpandTypeDefRefOrSpecToken TypeReference");
        token->Data = &pFile->TypeReferences[index];
        break;
    case MetadataTable_TypeDefinition:
        if (index > pFile->TypeDefinitionCount) Panic("CLIFile_ExpandTypeDefRefOrSpecToken TypeDefinition");
        token->Data = &pFile->TypeDefinitions[index];
        break;
    case MetadataTable_TypeSpecification:
        if (index > pFile->TypeSpecificationCount) Panic("CLIFile_ExpandTypeDefRefOrSpecToken TypeSpecification");
        token->Data = &pFile->TypeSpecifications[index];
        break;
    default:
        Panic("CLIFile_ExpandTypeDefRefOrSpecToken Unknown Table Type");
        break;
    }
    return token;
}

void CLIFile_DestroyMetadataToken(MetadataToken* pToken)
{
	free(pToken);
}
