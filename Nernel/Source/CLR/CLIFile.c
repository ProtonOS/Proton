#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CLR/CLIFile.h>

typedef const uint8_t* (*CLIFile_MetaDataInitialize_Handler)(CLIFile* pFile, const uint8_t* pTableData);
typedef const uint8_t* (*CLIFile_MetaDataLoad_Handler)(CLIFile* pFile, const uint8_t* pTableData);
typedef void (*CLIFile_MetaDataLink_Handler)(CLIFile* pFile);
typedef void (*CLIFile_MetaDataCleanup_Handler)(CLIFile* pFile);

struct
{
    CLIFile_MetaDataInitialize_Handler Initialize;
    CLIFile_MetaDataLoad_Handler Load;
    CLIFile_MetaDataLink_Handler Link;
    CLIFile_MetaDataCleanup_Handler Cleanup;
} CLIFile_MetaDataTables[] =
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

CLIFile* CLIFile_Create(PEFile* pFile)
{
    if (pFile->PEHeader->Machine != CLIFile_Machine) return NULL;

    PEDataDirectory* cliHeaderDataDirectory = &pFile->OptionalHeader->DataDirectory[14];
    PESectionHeader* cliHeaderSectionHeader = PEFile_GetSection(pFile->SectionHeaders, pFile->PEHeader->NumberOfSections, cliHeaderDataDirectory->VirtualAddress);
    CLIHeader* cliHeader = (CLIHeader*)(pFile->Data + cliHeaderSectionHeader->PointerToRawData + (cliHeaderDataDirectory->VirtualAddress - cliHeaderSectionHeader->VirtualAddress));

    PESectionHeader* cliMetaDataSectionHeader = PEFile_GetSection(pFile->SectionHeaders, pFile->PEHeader->NumberOfSections, cliHeader->MetaData.VirtualAddress);
    uint8_t* cliMetaDataHeader = pFile->Data + cliMetaDataSectionHeader->PointerToRawData + (cliHeader->MetaData.VirtualAddress - cliMetaDataSectionHeader->VirtualAddress);

    if (!MetaData_IsValidSignature(cliMetaDataHeader)) return NULL;

    CLIFile* cliFile = (CLIFile*)calloc(1, sizeof(CLIFile));

    cliFile->PEFile = pFile;
    cliFile->CLIHeader = cliHeader;
    cliFile->MetaDataHeader = cliMetaDataHeader;

    uint16_t streamCount = MetaData_GetStreamCount(cliFile->MetaDataHeader);
    for (uint16_t streamIndex = 0; streamIndex < streamCount; ++streamIndex)
    {
        const char* streamName = MetaData_GetStreamName(cliFile->MetaDataHeader, streamIndex);
        const uint8_t* streamData = MetaData_GetStreamData(cliFile->MetaDataHeader, streamIndex);

        if (!strcmp(streamName, "#~")) cliFile->Tables = streamData;
        else if (!strcmp(streamName, "#Strings")) cliFile->StringsHeap = streamData;
        else if (!strcmp(streamName, "#US")) cliFile->UserStringsHeap = streamData;
        else if (!strcmp(streamName, "#Blob")) cliFile->BlobsHeap = streamData;
        else if (!strcmp(streamName, "#GUID")) cliFile->GUIDsHeap = streamData;
    }

    cliFile->TablesHeader = (MetaDataTablesHeader*)cliFile->Tables;
    const uint8_t* tableData = cliFile->Tables + sizeof(MetaDataTablesHeader);
    uint32_t tableCount = 0;
	printf("Valid = 0x%llx, Sorted = 0x%llx\n", (unsigned long long)cliFile->TablesHeader->PresentTables, (unsigned long long)cliFile->TablesHeader->SortedTables);
    while (CLIFile_MetaDataTables[tableCount].Initialize) ++tableCount;
    for (uint32_t tableIndex = 0; tableIndex < tableCount; ++tableIndex) tableData = CLIFile_MetaDataTables[tableIndex].Initialize(cliFile, tableData);
    for (uint32_t tableIndex = 0; tableIndex < tableCount; ++tableIndex) tableData = CLIFile_MetaDataTables[tableIndex].Load(cliFile, tableData);
    for (uint32_t tableIndex = 0; tableIndex < tableCount; ++tableIndex) CLIFile_MetaDataTables[tableIndex].Link(cliFile);

    printf("Loaded MetaData\n");
    return cliFile;
}

void CLIFile_Destroy(CLIFile* pFile)
{
    uint32_t tableCount = 0;
    while (CLIFile_MetaDataTables[tableCount].Initialize) ++tableCount;
    for (uint32_t tableIndex = 0; tableIndex < tableCount; ++tableIndex) CLIFile_MetaDataTables[tableIndex].Cleanup(pFile);

    free(pFile);
}

const char* CLIFile_GetString(CLIFile* pFile, uint32_t pVirtualAddress)
{
	return (const char*)(pFile->StringsHeap + pVirtualAddress); 
}
const uint8_t* CLIFile_GetUserString(CLIFile* pFile, uint32_t pVirtualAddress) 
{
	return (const uint8_t*)(pFile->UserStringsHeap + pVirtualAddress);
}
const uint8_t* CLIFile_GetBlob(CLIFile* pFile, uint32_t pVirtualAddress)
{
	return (const uint8_t*)(pFile->BlobsHeap + pVirtualAddress); 
}
const uint8_t* CLIFile_GetGUID(CLIFile* pFile, uint32_t pVirtualAddress)
{
	return (const uint8_t*)(pFile->GUIDsHeap + pVirtualAddress);
}

MetaDataToken* CLIFile_ResolveToken(CLIFile* pFile, uint32_t pToken)
{
    MetaDataToken* token = (MetaDataToken*)calloc(1, sizeof(MetaDataToken));
    token->Table = pToken >> 24;
    token->Data = NULL;
    uint32_t index = pToken & 0x00FFFFFF;
    if (index == 0) return token;
    if (token->Table == MetaData_Table_UserStrings)
    {
        token->IsUserString = TRUE;
        token->Data = pFile->UserStringsHeap + index;
        return token;
    }
    switch (token->Table)
    {
    case MetaData_Table_ModuleDefinition:
        if (index > pFile->ModuleDefinitionCount) Panic("CLIFile_ResolveToken ModuleDefinition");
        token->Data = &pFile->ModuleDefinitions[index];
        break;
    case MetaData_Table_TypeReference:
        if (index > pFile->TypeReferenceCount) Panic("CLIFile_ResolveToken TypeReference");
        token->Data = &pFile->TypeReferences[index];
        break;
    case MetaData_Table_TypeDefinition:
        if (index > pFile->TypeDefinitionCount) Panic("CLIFile_ResolveToken TypeDefinition");
        token->Data = &pFile->TypeDefinitions[index];
        break;
    case MetaData_Table_Field:
        if (index > pFile->FieldCount) Panic("CLIFile_ResolveToken Field");
        token->Data = &pFile->Fields[index];
        break;
    case MetaData_Table_MethodDefinition:
        if (index > pFile->MethodDefinitionCount) Panic("CLIFile_ResolveToken MethodDefinition");
        token->Data = &pFile->MethodDefinitions[index];
        break;
    case MetaData_Table_Parameter:
        if (index > pFile->ParameterCount) Panic("CLIFile_ResolveToken Parameter");
        token->Data = &pFile->Parameters[index];
        break;
    case MetaData_Table_InterfaceImplementation:
        if (index > pFile->InterfaceImplementationCount) Panic("CLIFile_ResolveToken InterfaceImplementation");
        token->Data = &pFile->InterfaceImplementations[index];
        break;
    case MetaData_Table_MemberReference:
        if (index > pFile->MemberReferenceCount) Panic("CLIFile_ResolveToken MemberReference");
        token->Data = &pFile->MemberReferences[index];
        break;
    case MetaData_Table_Constant:
        if (index > pFile->ConstantCount) Panic("CLIFile_ResolveToken Constant");
        token->Data = &pFile->Constants[index];
        break;
    case MetaData_Table_CustomAttribute:
        if (index > pFile->CustomAttributeCount) Panic("CLIFile_ResolveToken CustomAttribute");
        token->Data = &pFile->CustomAttributes[index];
        break;
    case MetaData_Table_FieldMarshal:
        if (index > pFile->FieldMarshalCount) Panic("CLIFile_ResolveToken FieldMarshal");
        token->Data = &pFile->FieldMarshals[index];
        break;
    case MetaData_Table_DeclSecurity:
        if (index > pFile->DeclSecurityCount) Panic("CLIFile_ResolveToken DeclSecurity");
        token->Data = &pFile->DeclSecurities[index];
        break;
    case MetaData_Table_ClassLayout:
        if (index > pFile->ClassLayoutCount) Panic("CLIFile_ResolveToken ClassLayout");
        token->Data = &pFile->ClassLayouts[index];
        break;
    case MetaData_Table_FieldLayout:
        if (index > pFile->FieldLayoutCount) Panic("CLIFile_ResolveToken FieldLayout");
        token->Data = &pFile->FieldLayouts[index];
        break;
    case MetaData_Table_StandAloneSignature:
        if (index > pFile->StandAloneSignatureCount) Panic("CLIFile_ResolveToken StandAloneSignature");
        token->Data = &pFile->StandAloneSignatures[index];
        break;
    case MetaData_Table_EventMap:
        if (index > pFile->EventMapCount) Panic("CLIFile_ResolveToken EventMap");
        token->Data = &pFile->EventMaps[index];
        break;
    case MetaData_Table_Event:
        if (index > pFile->EventCount) Panic("CLIFile_ResolveToken Event");
        token->Data = &pFile->Events[index];
        break;
    case MetaData_Table_PropertyMap:
        if (index > pFile->PropertyMapCount) Panic("CLIFile_ResolveToken PropertyMap");
        token->Data = &pFile->PropertyMaps[index];
        break;
    case MetaData_Table_Property:
        if (index > pFile->PropertyCount) Panic("CLIFile_ResolveToken Property");
        token->Data = &pFile->Properties[index];
        break;
    case MetaData_Table_MethodSemantics:
        if (index > pFile->MethodSemanticsCount) Panic("CLIFile_ResolveToken MethodSemantics");
        token->Data = &pFile->MethodSemantics[index];
        break;
    case MetaData_Table_MethodImplementation:
        if (index > pFile->MethodImplementationCount) Panic("CLIFile_ResolveToken MethodImplementation");
        token->Data = &pFile->MethodImplementations[index];
        break;
    case MetaData_Table_ModuleReference:
        if (index > pFile->ModuleReferenceCount) Panic("CLIFile_ResolveToken ModuleReference");
        token->Data = &pFile->ModuleReferences[index];
        break;
    case MetaData_Table_TypeSpecification:
        if (index > pFile->TypeSpecificationCount) Panic("CLIFile_ResolveToken TypeSpecification");
        token->Data = &pFile->TypeSpecifications[index];
        break;
    case MetaData_Table_ImplementationMap:
        if (index > pFile->ImplementationMapCount) Panic("CLIFile_ResolveToken ImplementationMap");
        token->Data = &pFile->ImplementationMaps[index];
        break;
    case MetaData_Table_FieldRVA:
        if (index > pFile->FieldRVACount) Panic("CLIFile_ResolveToken FieldRVA");
        token->Data = &pFile->FieldRVAs[index];
        break;
    case MetaData_Table_AssemblyDefinition:
        if (index > pFile->AssemblyDefinitionCount) Panic("CLIFile_ResolveToken AssemblyDefinition");
        token->Data = &pFile->AssemblyDefinitions[index];
        break;
    case MetaData_Table_AssemblyProcessor:
        if (index > pFile->AssemblyProcessorCount) Panic("CLIFile_ResolveToken AssemblyProcessor");
        token->Data = &pFile->AssemblyProcessors[index];
        break;
    case MetaData_Table_AssemblyOperatingSystem:
        if (index > pFile->AssemblyOperatingSystemCount) Panic("CLIFile_ResolveToken AssemblyOperatingSystem");
        token->Data = &pFile->AssemblyOperatingSystems[index];
        break;
    case MetaData_Table_AssemblyReference:
        if (index > pFile->AssemblyReferenceCount) Panic("CLIFile_ResolveToken AssemblyReference");
        token->Data = &pFile->AssemblyReferences[index];
        break;
    case MetaData_Table_AssemblyReferenceProcessor:
        if (index > pFile->AssemblyReferenceProcessorCount) Panic("CLIFile_ResolveToken AssemblyReferenceProcessor");
        token->Data = &pFile->AssemblyReferenceProcessors[index];
        break;
    case MetaData_Table_AssemblyReferenceOperatingSystem:
        if (index > pFile->AssemblyReferenceOperatingSystemCount) Panic("CLIFile_ResolveToken AssemblyReferenceOperatingSystem");
        token->Data = &pFile->AssemblyReferenceOperatingSystems[index];
        break;
    case MetaData_Table_File:
        if (index > pFile->FileCount) Panic("CLIFile_ResolveToken File");
        token->Data = &pFile->Files[index];
        break;
    case MetaData_Table_ExportedType:
        if (index > pFile->ExportedTypeCount) Panic("CLIFile_ResolveToken ExportedType");
        token->Data = &pFile->ExportedTypes[index];
        break;
    case MetaData_Table_ManifestResource:
        if (index > pFile->ManifestResourceCount) Panic("CLIFile_ResolveToken ManifestResource");
        token->Data = &pFile->ManifestResources[index];
        break;
    case MetaData_Table_NestedClass:
        if (index > pFile->NestedClassCount) Panic("CLIFile_ResolveToken NestedClass");
        token->Data = &pFile->NestedClasses[index];
        break;
    case MetaData_Table_GenericParameter:
        if (index > pFile->GenericParameterCount) Panic("CLIFile_ResolveToken GenericParameter");
        token->Data = &pFile->GenericParameters[index];
        break;
    case MetaData_Table_MethodSpecification:
        if (index > pFile->MethodSpecificationCount) Panic("CLIFile_ResolveToken MethodSpecification");
        token->Data = &pFile->MethodSpecifications[index];
        break;
    case MetaData_Table_GenericParameterConstraint:
        if (index > pFile->GenericParameterConstraintCount) Panic("CLIFile_ResolveToken GenericParameterConstraint");
        token->Data = &pFile->GenericParameterConstraints[index];
        break;
    default:
        Panic("CLIFile_ResolveToken Unknown Table Type");
        break;
    }
    return token;
}

MetaDataToken* CLIFile_ResolveTypeDefOrRefOrSpecToken(CLIFile* pFile, uint32_t pToken)
{
    MetaDataToken* token = (MetaDataToken*)calloc(1, sizeof(MetaDataToken));
	uint8_t typeDefOrRefOrSpec = pToken & 0x03;
	switch (typeDefOrRefOrSpec)
	{
		case 0: token->Table = MetaData_Table_TypeDefinition; break;
		case 1: token->Table = MetaData_Table_TypeReference; break;
		case 2: token->Table = MetaData_Table_TypeSpecification; break;
		default: Panic("CLIFile_ResolveTypeDefOrRefOrSpecToken: Unknown Table Type"); break;
	}
    token->Data = NULL;
    uint32_t index = pToken >> 2;
    if (index == 0) return token;
    if (token->Table == MetaData_Table_UserStrings)
    {
        token->IsUserString = TRUE;
        token->Data = pFile->UserStringsHeap + index;
        return token;
    }
    switch (token->Table)
    {
    case MetaData_Table_TypeReference:
        if (index > pFile->TypeReferenceCount) Panic("CLIFile_ResolveTypeDefOrRefOrSpecToken: TypeReference");
        token->Data = &pFile->TypeReferences[index];
        break;
    case MetaData_Table_TypeDefinition:
        if (index > pFile->TypeDefinitionCount) Panic("CLIFile_ResolveTypeDefOrRefOrSpecToken: TypeDefinition");
        token->Data = &pFile->TypeDefinitions[index];
        break;
    case MetaData_Table_TypeSpecification:
        if (index > pFile->TypeSpecificationCount) Panic("CLIFile_ResolveTypeDefOrRefOrSpecToken: TypeSpecification");
        token->Data = &pFile->TypeSpecifications[index];
        break;
    default:
        Panic("CLIFile_ResolveTypeDefOrRefOrSpecToken: Unknown Table Type");
        break;
    }
    return token;
}