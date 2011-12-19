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

    CLIFile* cliFile = malloc(sizeof(CLIFile));
    memset(cliFile, 0x00, sizeof(CLIFile));

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
