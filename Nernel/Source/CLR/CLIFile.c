#include <Common.h>
#include <CLR/CLIFile.h>
#include <System/SystemClock.h>


typedef uint8_t* (*CLIFile_MetadataInitialize_Handler)(CLIFile* pFile, uint8_t* pTableData);
typedef uint8_t* (*CLIFile_MetadataLoad_Handler)(CLIFile* pFile, uint8_t* pTableData);
typedef void (*CLIFile_MetadataCleanup_Handler)(CLIFile* pFile);

struct
{
    const CLIFile_MetadataInitialize_Handler Initialize;
    const CLIFile_MetadataLoad_Handler Load;
    const CLIFile_MetadataCleanup_Handler Cleanup;
} const gCLIFile_MetadataHandler_Table[] =
{
    { &ModuleDefinition_Initialize,                     &ModuleDefinition_Load,                     &ModuleDefinition_Cleanup },
    { &TypeReference_Initialize,                        &TypeReference_Load,                        &TypeReference_Cleanup },
    { &TypeDefinition_Initialize,                       &TypeDefinition_Load,                       &TypeDefinition_Cleanup },
    { &Field_Initialize,                                &Field_Load,                                &Field_Cleanup },
    { &MethodDefinition_Initialize,                     &MethodDefinition_Load,                     &MethodDefinition_Cleanup },
    { &Parameter_Initialize,                            &Parameter_Load,                            &Parameter_Cleanup },
    { &InterfaceImplementation_Initialize,              &InterfaceImplementation_Load,              &InterfaceImplementation_Cleanup },
    { &MemberReference_Initialize,                      &MemberReference_Load,                      &MemberReference_Cleanup },
    { &Constant_Initialize,                             &Constant_Load,                             &Constant_Cleanup },
    { &CustomAttribute_Initialize,                      &CustomAttribute_Load,                      &CustomAttribute_Cleanup },
    { &FieldMarshal_Initialize,                         &FieldMarshal_Load,                         &FieldMarshal_Cleanup },
    { &DeclSecurity_Initialize,                         &DeclSecurity_Load,                         &DeclSecurity_Cleanup },
    { &ClassLayout_Initialize,                          &ClassLayout_Load,                          &ClassLayout_Cleanup },
    { &FieldLayout_Initialize,                          &FieldLayout_Load,                          &FieldLayout_Cleanup },
    { &StandAloneSignature_Initialize,                  &StandAloneSignature_Load,                  &StandAloneSignature_Cleanup },
    { &EventMap_Initialize,                             &EventMap_Load,                             &EventMap_Cleanup },
    { &Event_Initialize,                                &Event_Load,                                &Event_Cleanup },
    { &PropertyMap_Initialize,                          &PropertyMap_Load,                          &PropertyMap_Cleanup },
    { &Property_Initialize,                             &Property_Load,                             &Property_Cleanup },
    { &MethodSemantics_Initialize,                      &MethodSemantics_Load,                      &MethodSemantics_Cleanup },
    { &MethodImplementation_Initialize,                 &MethodImplementation_Load,                 &MethodImplementation_Cleanup },
    { &ModuleReference_Initialize,                      &ModuleReference_Load,                      &ModuleReference_Cleanup },
    { &TypeSpecification_Initialize,                    &TypeSpecification_Load,                    &TypeSpecification_Cleanup },
    { &ImplementationMap_Initialize,                    &ImplementationMap_Load,                    &ImplementationMap_Cleanup },
    { &FieldRVA_Initialize,                             &FieldRVA_Load,                             &FieldRVA_Cleanup },
    { &AssemblyDefinition_Initialize,                   &AssemblyDefinition_Load,                   &AssemblyDefinition_Cleanup },
    { &AssemblyProcessor_Initialize,                    &AssemblyProcessor_Load,                    &AssemblyProcessor_Cleanup },
    { &AssemblyOperatingSystem_Initialize,              &AssemblyOperatingSystem_Load,              &AssemblyOperatingSystem_Cleanup },
    { &AssemblyReference_Initialize,                    &AssemblyReference_Load,                    &AssemblyReference_Cleanup },
    { &AssemblyReferenceProcessor_Initialize,           &AssemblyReferenceProcessor_Load,           &AssemblyReferenceProcessor_Cleanup },
    { &AssemblyReferenceOperatingSystem_Initialize,     &AssemblyReferenceOperatingSystem_Load,     &AssemblyReferenceOperatingSystem_Cleanup },
    { &File_Initialize,                                 &File_Load,                                 &File_Cleanup },
    { &ExportedType_Initialize,                         &ExportedType_Load,                         &ExportedType_Cleanup },
    { &ManifestResource_Initialize,                     &ManifestResource_Load,                     &ManifestResource_Cleanup },
    { &NestedClass_Initialize,                          &NestedClass_Load,                          &NestedClass_Cleanup },
    { &GenericParameter_Initialize,                     &GenericParameter_Load,                     &GenericParameter_Cleanup },
    { &MethodSpecification_Initialize,                  &MethodSpecification_Load,                  &MethodSpecification_Cleanup },
    { &GenericParameterConstraint_Initialize,           &GenericParameterConstraint_Load,           &GenericParameterConstraint_Cleanup },
    { NULL,                                             NULL,                                       NULL }
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

	uint64_t preinitTicks = SystemClock_GetTicks();
    for (uint32_t tableIndex = 0; tableIndex < tableCount; ++tableIndex) tableData = gCLIFile_MetadataHandler_Table[tableIndex].Initialize(file, tableData);
	uint64_t preloadTicks = SystemClock_GetTicks();
    for (uint32_t tableIndex = 0; tableIndex < tableCount; ++tableIndex) tableData = gCLIFile_MetadataHandler_Table[tableIndex].Load(file, tableData);

	uint64_t prelinkTicks = SystemClock_GetTicks();
	CLIFile_Link(file);

	uint64_t stoppedTicks = SystemClock_GetTicks();
	printf("CLIFile_Create Timings: preinit = %uMS, preload = %uMS, prelink = %uMS, total = %uMS\n", (unsigned int)(preinitTicks - startedTicks), (unsigned int)(preloadTicks - startedTicks), (unsigned int)(prelinkTicks - startedTicks), (unsigned int)(stoppedTicks - startedTicks));
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

void CLIFile_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_ModuleDefinition) ++pFile->CustomAttributes[index].Parent.ModuleDefinition->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->ModuleDefinitionCount; ++index)
    {
        if (pFile->ModuleDefinitions[index].CustomAttributeCount > 0)
        {
            pFile->ModuleDefinitions[index].CustomAttributes = (CustomAttribute**)calloc(pFile->ModuleDefinitions[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_ModuleDefinition &&
                    pFile->CustomAttributes[searchIndex].Parent.ModuleDefinition == &pFile->ModuleDefinitions[index])
                {
                    pFile->ModuleDefinitions[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_TypeReference) ++pFile->CustomAttributes[index].Parent.TypeReference->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
    {
        if (pFile->MemberReferences[index].TypeOfParent == MemberRefParentType_TypeReference) ++pFile->MemberReferences[index].Parent.TypeReference->MemberReferenceCount;
    }
    for (uint32_t index = 1; index <= pFile->TypeReferenceCount; ++index)
    {
        if (pFile->TypeReferences[index].CustomAttributeCount > 0)
        {
            pFile->TypeReferences[index].CustomAttributes = (CustomAttribute**)calloc(pFile->TypeReferences[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_TypeReference &&
                    pFile->CustomAttributes[searchIndex].Parent.TypeReference == &pFile->TypeReferences[index])
                {
                    pFile->TypeReferences[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
        if (pFile->TypeReferences[index].MemberReferenceCount > 0)
        {
            pFile->TypeReferences[index].MemberReferences = (MemberReference**)calloc(pFile->TypeReferences[index].MemberReferenceCount, sizeof(MemberReference*));
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= pFile->MemberReferenceCount; ++searchIndex)
            {
                if (pFile->MemberReferences[searchIndex].TypeOfParent == MemberRefParentType_TypeReference &&
                    pFile->MemberReferences[searchIndex].Parent.TypeReference == &pFile->TypeReferences[index])
                {
                    pFile->TypeReferences[index].MemberReferences[memberReferenceIndex] = &pFile->MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
		if (!pFile->TypeReferences[index].ResolutionScope.AssemblyReference)
		{
			for (uint32_t searchIndex = 1; searchIndex <= pFile->ExportedTypeCount; ++searchIndex)
			{
				if (!strcmp(pFile->ExportedTypes[searchIndex].Name, pFile->TypeReferences[index].Name) &&
					!strcmp(pFile->ExportedTypes[searchIndex].Namespace, pFile->TypeReferences[index].Namespace))
				{
					pFile->TypeReferences[index].ExportedType = &pFile->ExportedTypes[searchIndex];
				}
			}
		}
    }


    for (uint32_t index = 1; index <= pFile->ClassLayoutCount; ++index) pFile->ClassLayouts[index].Parent->ClassLayout = &pFile->ClassLayouts[index];
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_TypeDefinition)
        {
            ++pFile->CustomAttributes[index].Parent.TypeDefinition->CustomAttributeCount;
        }
    }
    for (uint32_t index = 1; index <= pFile->GenericParameterCount; ++index)
    {
        if (pFile->GenericParameters[index].TypeOfOwner == TypeOrMethodDefType_TypeDefinition)
        {
            ++pFile->GenericParameters[index].Owner.TypeDefinition->GenericParameterCount;
        }
    }
    for (uint32_t index = 1; index <= pFile->InterfaceImplementationCount; ++index) ++pFile->InterfaceImplementations[index].Implementor->InterfaceImplementationCount;
    for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
    {
        if (pFile->MemberReferences[index].TypeOfParent == MemberRefParentType_TypeDefinition) ++pFile->MemberReferences[index].Parent.TypeDefinition->MemberReferenceCount;
    }
    for (uint32_t index = 1; index <= pFile->MethodImplementationCount; ++index)
    {
        ++pFile->MethodImplementations[index].Parent->MethodImplementationCount;
    }
    for (uint32_t index = 1; index <= pFile->NestedClassCount; ++index)
    {
        ++pFile->NestedClasses[index].Enclosing->NestedClassCount;
    }

    for (uint32_t index = 1; index <= pFile->DeclSecurityCount; ++index)
    {
        if (pFile->DeclSecurities[index].TypeOfParent == HasDeclSecurityType_TypeDefinition) pFile->DeclSecurities[index].Parent.TypeDefinition->DeclSecurity = &pFile->DeclSecurities[index];
    }
    for (uint32_t index = 1; index <= pFile->EventMapCount; ++index) pFile->EventMaps[index].Parent->EventMap = &pFile->EventMaps[index];
    for (uint32_t index = 1; index <= pFile->PropertyMapCount; ++index) pFile->PropertyMaps[index].Parent->PropertyMap = &pFile->PropertyMaps[index];

	for (uint32_t index = 1; index <= pFile->TypeDefinitionCount; ++index)
    {
        if (pFile->TypeDefinitions[index].CustomAttributeCount > 0)
        {
            pFile->TypeDefinitions[index].CustomAttributes = (CustomAttribute**)calloc(pFile->TypeDefinitions[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_TypeDefinition &&
                    pFile->CustomAttributes[searchIndex].Parent.TypeDefinition == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
        if (pFile->TypeDefinitions[index].GenericParameterCount > 0)
        {
            pFile->TypeDefinitions[index].GenericParameters = (GenericParameter**)calloc(pFile->TypeDefinitions[index].GenericParameterCount, sizeof(GenericParameter*));
            for (uint32_t searchIndex = 1, genericParameterIndex = 0; searchIndex <= pFile->GenericParameterCount; ++searchIndex)
            {
                if (pFile->GenericParameters[searchIndex].TypeOfOwner == TypeOrMethodDefType_TypeDefinition &&
                    pFile->GenericParameters[searchIndex].Owner.TypeDefinition == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].GenericParameters[genericParameterIndex] = &pFile->GenericParameters[searchIndex];
                    ++genericParameterIndex;
                }
            }
        }
        if (pFile->TypeDefinitions[index].InterfaceImplementationCount > 0)
        {
            pFile->TypeDefinitions[index].InterfaceImplementations = (InterfaceImplementation**)calloc(pFile->TypeDefinitions[index].InterfaceImplementationCount, sizeof(InterfaceImplementation*));
            for (uint32_t searchIndex = 1, interfaceImplementationIndex = 0; searchIndex <= pFile->InterfaceImplementationCount; ++searchIndex)
            {
                if (pFile->InterfaceImplementations[searchIndex].Implementor == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].InterfaceImplementations[interfaceImplementationIndex] = &pFile->InterfaceImplementations[searchIndex];
                    ++interfaceImplementationIndex;
                }
            }
        }
        if (pFile->TypeDefinitions[index].MemberReferenceCount > 0)
        {
            pFile->TypeDefinitions[index].MemberReferences = (MemberReference**)calloc(pFile->TypeDefinitions[index].MemberReferenceCount, sizeof(MemberReference*));
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= pFile->MemberReferenceCount; ++searchIndex)
            {
                if (pFile->MemberReferences[searchIndex].TypeOfParent == MemberRefParentType_TypeDefinition &&
                    pFile->MemberReferences[searchIndex].Parent.TypeDefinition == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].MemberReferences[memberReferenceIndex] = &pFile->MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
        if (pFile->TypeDefinitions[index].MethodImplementationCount > 0)
        {
            //printf("MethodImplementations: %u %s.%s %u\n", (unsigned int)index, pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, (unsigned int)pFile->TypeDefinitions[index].MethodImplementationCount);
            pFile->TypeDefinitions[index].MethodImplementations = (MethodImplementation**)calloc(pFile->TypeDefinitions[index].MethodImplementationCount, sizeof(MethodImplementation*));
            for (uint32_t searchIndex = 1, methodImplementationIndex = 0; searchIndex <= pFile->MethodImplementationCount; ++searchIndex)
            {
                if (pFile->MethodImplementations[searchIndex].Parent == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].MethodImplementations[methodImplementationIndex] = &pFile->MethodImplementations[searchIndex];
                    ++methodImplementationIndex;
                }
            }
        }
        if (pFile->TypeDefinitions[index].NestedClassCount > 0)
        {
            pFile->TypeDefinitions[index].NestedClasses = (NestedClass**)calloc(pFile->TypeDefinitions[index].NestedClassCount, sizeof(NestedClass*));
            for (uint32_t searchIndex = 1, nestedClassIndex = 0; searchIndex <= pFile->NestedClassCount; ++searchIndex)
            {
                if (pFile->NestedClasses[searchIndex].Enclosing == &pFile->TypeDefinitions[index])
                {
                    pFile->TypeDefinitions[index].NestedClasses[nestedClassIndex] = &pFile->NestedClasses[searchIndex];
                    ++nestedClassIndex;
                }
            }
        }
        if (pFile->TypeDefinitions[index].MethodDefinitionListCount > 0)
        {
            //printf("TypeDefinition[%u]: %s.%s, %u methods\n", (unsigned int)index, pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, (unsigned int)pFile->TypeDefinitions[index].MethodDefinitionListCount);
            for (uint32_t searchIndex = 0; searchIndex < pFile->TypeDefinitions[index].MethodDefinitionListCount; ++searchIndex)
            {
                //printf("    %s\n", pFile->TypeDefinitions[index].MethodDefinitionList[searchIndex].Name);
                pFile->TypeDefinitions[index].MethodDefinitionList[searchIndex].TypeDefinition = &pFile->TypeDefinitions[index];
            }
        }
		//printf("Linking Fields for %s.%s, %u\n", pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, pFile->TypeDefinitions[index].FieldListCount);
		for (uint32_t index2 = 0; index2 < pFile->TypeDefinitions[index].FieldListCount; ++index2)
		{
			pFile->TypeDefinitions[index].FieldList[index2].TypeDefinition = &pFile->TypeDefinitions[index];
			//printf("Linking Field %s to TypeDef %s.%s @ 0x%x\n", pFile->TypeDefinitions[index].FieldList[index2].Name, pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, (unsigned int)pFile->TypeDefinitions[index].FieldList[index2].TypeDefinition);
		}
	}


    for (uint32_t index = 1; index <= pFile->ConstantCount; ++index)
    {
        if (pFile->Constants[index].TypeOfParent == HasConstantType_Field) pFile->Constants[index].Parent.Field->Constant = &pFile->Constants[index];
    }
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_Field) ++pFile->CustomAttributes[index].Parent.Field->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->FieldCount; ++index)
    {
        if (pFile->Fields[index].CustomAttributeCount > 0)
        {
            pFile->Fields[index].CustomAttributes = (CustomAttribute**)calloc(pFile->Fields[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_Field &&
                    pFile->CustomAttributes[searchIndex].Parent.Field == &pFile->Fields[index])
                {
                    pFile->Fields[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->FieldMarshalCount; ++index)
    {
        if (pFile->FieldMarshals[index].TypeOfParent == HasConstantType_Field) pFile->FieldMarshals[index].Parent.Field->FieldMarshal = &pFile->FieldMarshals[index];
    }
    for (uint32_t index = 1; index <= pFile->ImplementationMapCount; ++index)
    {
        if (pFile->ImplementationMaps[index].TypeOfMemberForwarded == MemberForwardedType_Field) pFile->ImplementationMaps[index].MemberForwarded.Field->ImplementationMap = &pFile->ImplementationMaps[index];
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_MethodDefinition) ++pFile->CustomAttributes[index].Parent.MethodDefinition->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->GenericParameterCount; ++index)
    {
        if (pFile->GenericParameters[index].TypeOfOwner == TypeOrMethodDefType_MethodDefinition) ++pFile->GenericParameters[index].Owner.MethodDefinition->GenericParameterCount;
    }
    for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
    {
        if (pFile->MemberReferences[index].TypeOfParent == MemberRefParentType_MethodDefinition) ++pFile->MemberReferences[index].Parent.MethodDefinition->MemberReferenceCount;
    }

    for (uint32_t index = 1; index <= pFile->DeclSecurityCount; ++index)
    {
        if (pFile->DeclSecurities[index].TypeOfParent == HasDeclSecurityType_MethodDefinition) pFile->DeclSecurities[index].Parent.MethodDefinition->DeclSecurity = &pFile->DeclSecurities[index];
    }
    for (uint32_t index = 1; index <= pFile->ImplementationMapCount; ++index)
    {
        if (pFile->ImplementationMaps[index].TypeOfMemberForwarded == MemberForwardedType_MethodDefinition) pFile->ImplementationMaps[index].MemberForwarded.MethodDefinition->ImplementationMap = &pFile->ImplementationMaps[index];
    }

    for (uint32_t index = 1; index <= pFile->MethodDefinitionCount; ++index)
    {
        if (pFile->MethodDefinitions[index].CustomAttributeCount > 0)
        {
            pFile->MethodDefinitions[index].CustomAttributes = (CustomAttribute**)calloc(pFile->MethodDefinitions[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_MethodDefinition &&
                    pFile->CustomAttributes[searchIndex].Parent.MethodDefinition == &pFile->MethodDefinitions[index])
                {
                    pFile->MethodDefinitions[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
        if (pFile->MethodDefinitions[index].GenericParameterCount > 0)
        {
            pFile->MethodDefinitions[index].GenericParameters = (GenericParameter**)calloc(pFile->MethodDefinitions[index].GenericParameterCount, sizeof(GenericParameter*));
            for (uint32_t searchIndex = 1, genericParameterIndex = 0; searchIndex <= pFile->GenericParameterCount; ++searchIndex)
            {
                if (pFile->GenericParameters[searchIndex].TypeOfOwner == TypeOrMethodDefType_MethodDefinition &&
                    pFile->GenericParameters[searchIndex].Owner.MethodDefinition == &pFile->MethodDefinitions[index])
                {
                    pFile->MethodDefinitions[index].GenericParameters[genericParameterIndex] = &pFile->GenericParameters[searchIndex];
                    ++genericParameterIndex;
                }
            }
        }
        if (pFile->MethodDefinitions[index].MemberReferenceCount > 0)
        {
            pFile->MethodDefinitions[index].MemberReferences = (MemberReference**)calloc(pFile->MethodDefinitions[index].MemberReferenceCount, sizeof(MemberReference*));
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= pFile->MemberReferenceCount; ++searchIndex)
            {
                if (pFile->MemberReferences[searchIndex].TypeOfParent == MemberRefParentType_MethodDefinition &&
                    pFile->MemberReferences[searchIndex].Parent.MethodDefinition == &pFile->MethodDefinitions[index])
                {
                    pFile->MethodDefinitions[index].MemberReferences[memberReferenceIndex] = &pFile->MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
		//if ((pFile->MethodDefinitions[index].ImplFlags & MethodImplAttributes_InternalCall) != 0) pFile->MethodDefinitions[index].InternalCall = ResolveInternalCall(&pFile->MethodDefinitions[index], pFile);
    }


    for (uint32_t index = 1; index <= pFile->ConstantCount; ++index)
    {
        if (pFile->Constants[index].TypeOfParent == HasConstantType_Parameter) pFile->Constants[index].Parent.Parameter->Constant = &pFile->Constants[index];
    }
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_Parameter) ++pFile->CustomAttributes[index].Parent.Parameter->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->ParameterCount; ++index)
    {
        if (pFile->Parameters[index].CustomAttributeCount > 0)
        {
            pFile->Parameters[index].CustomAttributes = (CustomAttribute**)calloc(pFile->Parameters[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_Parameter &&
                    pFile->CustomAttributes[searchIndex].Parent.Parameter == &pFile->Parameters[index])
                {
                    pFile->Parameters[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->FieldMarshalCount; ++index)
    {
        if (pFile->FieldMarshals[index].TypeOfParent == HasConstantType_Parameter) pFile->FieldMarshals[index].Parent.Parameter->FieldMarshal = &pFile->FieldMarshals[index];
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_InterfaceImplementation) ++pFile->CustomAttributes[index].Parent.InterfaceImplementation->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->InterfaceImplementationCount; ++index)
    {
        if (pFile->InterfaceImplementations[index].CustomAttributeCount > 0)
        {
            pFile->InterfaceImplementations[index].CustomAttributes = (CustomAttribute**)calloc(pFile->InterfaceImplementations[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_InterfaceImplementation &&
                    pFile->CustomAttributes[searchIndex].Parent.InterfaceImplementation == &pFile->InterfaceImplementations[index])
                {
                    pFile->InterfaceImplementations[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_MemberReference) ++pFile->CustomAttributes[index].Parent.MemberReference->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
    {
        if (pFile->MemberReferences[index].CustomAttributeCount > 0)
        {
            pFile->MemberReferences[index].CustomAttributes = (CustomAttribute**)calloc(pFile->MemberReferences[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_MemberReference &&
                    pFile->CustomAttributes[searchIndex].Parent.MemberReference == &pFile->MemberReferences[index])
                {
                    pFile->MemberReferences[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_DeclSecurity) ++pFile->CustomAttributes[index].Parent.DeclSecurity->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->DeclSecurityCount; ++index)
    {
        if (pFile->DeclSecurities[index].CustomAttributeCount > 0)
        {
            pFile->DeclSecurities[index].CustomAttributes = (CustomAttribute**)calloc(pFile->DeclSecurities[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_DeclSecurity &&
                    pFile->CustomAttributes[searchIndex].Parent.DeclSecurity == &pFile->DeclSecurities[index])
                {
                    pFile->DeclSecurities[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_StandAloneSignature) ++pFile->CustomAttributes[index].Parent.StandAloneSignature->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->StandAloneSignatureCount; ++index)
    {
        if (pFile->StandAloneSignatures[index].CustomAttributeCount > 0)
        {
            pFile->StandAloneSignatures[index].CustomAttributes = (CustomAttribute**)calloc(pFile->StandAloneSignatures[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_StandAloneSignature &&
                    pFile->CustomAttributes[searchIndex].Parent.StandAloneSignature == &pFile->StandAloneSignatures[index])
                {
                    pFile->StandAloneSignatures[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_Event) ++pFile->CustomAttributes[index].Parent.Event->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->EventCount; ++index)
    {
        if (pFile->Events[index].CustomAttributeCount > 0)
        {
            pFile->Events[index].CustomAttributes = (CustomAttribute**)calloc(pFile->Events[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_Event &&
                    pFile->CustomAttributes[searchIndex].Parent.Event == &pFile->Events[index])
                {
                    pFile->Events[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->MethodSemanticsCount; ++index)
    {
        if (pFile->MethodSemantics[index].TypeOfAssociation == HasSemanticsType_Event) pFile->MethodSemantics[index].Association.Event->MethodSemantics = &pFile->MethodSemantics[index];
    }


    for (uint32_t index = 1; index <= pFile->ConstantCount; ++index)
    {
        if (pFile->Constants[index].TypeOfParent == HasConstantType_Property) pFile->Constants[index].Parent.Property->Constant = &pFile->Constants[index];
    }
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_Property) ++pFile->CustomAttributes[index].Parent.Property->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->PropertyCount; ++index)
    {
        if (pFile->Properties[index].CustomAttributeCount > 0)
        {
            pFile->Properties[index].CustomAttributes = (CustomAttribute**)calloc(pFile->Properties[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_Property &&
                    pFile->CustomAttributes[searchIndex].Parent.Property == &pFile->Properties[index])
                {
                    pFile->Properties[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->MethodSemanticsCount; ++index)
    {
        if (pFile->MethodSemantics[index].TypeOfAssociation == HasSemanticsType_Property) pFile->MethodSemantics[index].Association.Property->MethodSemantics = &pFile->MethodSemantics[index];
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_ModuleReference) ++pFile->CustomAttributes[index].Parent.ModuleReference->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
    {
        if (pFile->MemberReferences[index].TypeOfParent == MemberRefParentType_ModuleReference) ++pFile->MemberReferences[index].Parent.ModuleReference->MemberReferenceCount;
    }
    for (uint32_t index = 1; index <= pFile->ModuleReferenceCount; ++index)
    {
        if (pFile->ModuleReferences[index].CustomAttributeCount > 0)
        {
            pFile->ModuleReferences[index].CustomAttributes = (CustomAttribute**)calloc(pFile->ModuleReferences[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_ModuleReference &&
                    pFile->CustomAttributes[searchIndex].Parent.ModuleReference == &pFile->ModuleReferences[index])
                {
                    pFile->ModuleReferences[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->ModuleReferenceCount; ++index)
    {
        if (pFile->ModuleReferences[index].MemberReferenceCount > 0)
        {
            pFile->ModuleReferences[index].MemberReferences = (MemberReference**)calloc(pFile->ModuleReferences[index].MemberReferenceCount, sizeof(MemberReference*));
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= pFile->MemberReferenceCount; ++searchIndex)
            {
                if (pFile->MemberReferences[searchIndex].TypeOfParent == MemberRefParentType_ModuleReference &&
                    pFile->MemberReferences[searchIndex].Parent.ModuleReference == &pFile->ModuleReferences[index])
                {
                    pFile->ModuleReferences[index].MemberReferences[memberReferenceIndex] = &pFile->MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_TypeSpecification) ++pFile->CustomAttributes[index].Parent.TypeSpecification->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
    {
        if (pFile->MemberReferences[index].TypeOfParent == MemberRefParentType_TypeSpecification) ++pFile->MemberReferences[index].Parent.TypeSpecification->MemberReferenceCount;
    }
    for (uint32_t index = 1; index <= pFile->TypeSpecificationCount; ++index)
    {
        if (pFile->TypeSpecifications[index].CustomAttributeCount > 0)
        {
            pFile->TypeSpecifications[index].CustomAttributes = (CustomAttribute**)calloc(pFile->TypeSpecifications[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_TypeSpecification &&
                    pFile->CustomAttributes[searchIndex].Parent.TypeSpecification == &pFile->TypeSpecifications[index])
                {
                    pFile->TypeSpecifications[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }

        if (pFile->TypeSpecifications[index].MemberReferenceCount > 0)
        {
            pFile->TypeSpecifications[index].MemberReferences = (MemberReference**)calloc(pFile->TypeSpecifications[index].MemberReferenceCount, sizeof(MemberReference*));
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= pFile->MemberReferenceCount; ++searchIndex)
            {
                if (pFile->MemberReferences[searchIndex].TypeOfParent == MemberRefParentType_TypeSpecification &&
                    pFile->MemberReferences[searchIndex].Parent.TypeSpecification == &pFile->TypeSpecifications[index])
                {
                    pFile->TypeSpecifications[index].MemberReferences[memberReferenceIndex] = &pFile->MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
	}


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_AssemblyDefinition) ++pFile->CustomAttributes[index].Parent.AssemblyDefinition->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->AssemblyDefinitionCount; ++index)
    {
        if (pFile->AssemblyDefinitions[index].CustomAttributeCount > 0)
        {
            pFile->AssemblyDefinitions[index].CustomAttributes = (CustomAttribute**)calloc(pFile->AssemblyDefinitions[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_AssemblyDefinition &&
                    pFile->CustomAttributes[searchIndex].Parent.AssemblyDefinition == &pFile->AssemblyDefinitions[index])
                {
                    pFile->AssemblyDefinitions[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->DeclSecurityCount; ++index)
    {
        if (pFile->DeclSecurities[index].TypeOfParent == HasDeclSecurityType_AssemblyDefinition) pFile->DeclSecurities[index].Parent.AssemblyDefinition->DeclSecurity = &pFile->DeclSecurities[index];
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_AssemblyReference) ++pFile->CustomAttributes[index].Parent.AssemblyReference->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->AssemblyReferenceCount; ++index)
    {
        if (pFile->AssemblyReferences[index].CustomAttributeCount > 0)
        {
            pFile->AssemblyReferences[index].CustomAttributes = (CustomAttribute**)calloc(pFile->AssemblyReferences[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_AssemblyReference &&
                    pFile->CustomAttributes[searchIndex].Parent.AssemblyReference == &pFile->AssemblyReferences[index])
                {
                    pFile->AssemblyReferences[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_File) ++pFile->CustomAttributes[index].Parent.File->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->FileCount; ++index)
    {
        if (pFile->Files[index].CustomAttributeCount > 0)
        {
            pFile->Files[index].CustomAttributes = (CustomAttribute**)calloc(pFile->Files[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_File &&
                    pFile->CustomAttributes[searchIndex].Parent.File == &pFile->Files[index])
                {
                    pFile->Files[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_ExportedType) ++pFile->CustomAttributes[index].Parent.ExportedType->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->ExportedTypeCount; ++index)
    {
        if (pFile->ExportedTypes[index].CustomAttributeCount > 0)
        {
            pFile->ExportedTypes[index].CustomAttributes = (CustomAttribute**)calloc(pFile->ExportedTypes[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_ExportedType &&
                    pFile->CustomAttributes[searchIndex].Parent.ExportedType == &pFile->ExportedTypes[index])
                {
                    pFile->ExportedTypes[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_ManifestResource) ++pFile->CustomAttributes[index].Parent.ManifestResource->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->ManifestResourceCount; ++index)
    {
        if (pFile->ManifestResources[index].CustomAttributeCount > 0)
        {
            pFile->ManifestResources[index].CustomAttributes = (CustomAttribute**)calloc(pFile->ManifestResources[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_ManifestResource &&
                    pFile->CustomAttributes[searchIndex].Parent.ManifestResource == &pFile->ManifestResources[index])
                {
                    pFile->ManifestResources[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_GenericParameter) ++pFile->CustomAttributes[index].Parent.GenericParameter->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->GenericParameterConstraintCount; ++index) ++pFile->GenericParameterConstraints[index].Owner->GenericParameterConstraintCount;
    for (uint32_t index = 1; index <= pFile->GenericParameterCount; ++index)
    {
        if (pFile->GenericParameters[index].CustomAttributeCount > 0)
        {
            pFile->GenericParameters[index].CustomAttributes = (CustomAttribute**)calloc(pFile->GenericParameters[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_GenericParameter &&
                    pFile->CustomAttributes[searchIndex].Parent.GenericParameter == &pFile->GenericParameters[index])
                {
                    pFile->GenericParameters[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
        if (pFile->GenericParameters[index].GenericParameterConstraintCount > 0)
        {
            pFile->GenericParameters[index].GenericParameterConstraints = (GenericParameterConstraint**)calloc(pFile->GenericParameters[index].GenericParameterConstraintCount, sizeof(GenericParameterConstraint*));
            for (uint32_t searchIndex = 1, genericParameterConstraintIndex = 0; searchIndex <= pFile->GenericParameterConstraintCount; ++searchIndex)
            {
                if (pFile->GenericParameterConstraints[searchIndex].Owner == &pFile->GenericParameters[index])
                {
                    pFile->GenericParameters[index].GenericParameterConstraints[genericParameterConstraintIndex] = &pFile->GenericParameterConstraints[searchIndex];
                    ++genericParameterConstraintIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_MethodSpecification) ++pFile->CustomAttributes[index].Parent.MethodSpecification->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->MethodSpecificationCount; ++index)
    {
        if (pFile->MethodSpecifications[index].CustomAttributeCount > 0)
        {
            pFile->MethodSpecifications[index].CustomAttributes = (CustomAttribute**)calloc(pFile->MethodSpecifications[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_MethodSpecification &&
                    pFile->CustomAttributes[searchIndex].Parent.MethodSpecification == &pFile->MethodSpecifications[index])
                {
                    pFile->MethodSpecifications[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }


    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttributeType_GenericParameterConstraint) ++pFile->CustomAttributes[index].Parent.GenericParameterConstraint->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->GenericParameterConstraintCount; ++index)
    {
        if (pFile->GenericParameterConstraints[index].CustomAttributeCount > 0)
        {
            pFile->GenericParameterConstraints[index].CustomAttributes = (CustomAttribute**)calloc(pFile->GenericParameterConstraints[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType_GenericParameterConstraint &&
                    pFile->CustomAttributes[searchIndex].Parent.GenericParameterConstraint == &pFile->GenericParameterConstraints[index])
                {
                    pFile->GenericParameterConstraints[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
