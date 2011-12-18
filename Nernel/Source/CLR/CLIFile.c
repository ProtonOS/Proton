#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    tableData = ModuleDefinition_Initialize(cliFile, tableData);
    tableData = TypeReference_Initialize(cliFile, tableData);
    tableData = TypeDefinition_Initialize(cliFile, tableData);
    tableData = Field_Initialize(cliFile, tableData);
    tableData = MethodDefinition_Initialize(cliFile, tableData);
    tableData = Parameter_Initialize(cliFile, tableData);
    tableData = InterfaceImplementation_Initialize(cliFile, tableData);
    tableData = MemberReference_Initialize(cliFile, tableData);
    tableData = Constant_Initialize(cliFile, tableData);
    tableData = CustomAttribute_Initialize(cliFile, tableData);
    tableData = FieldMarshal_Initialize(cliFile, tableData);
    tableData = DeclSecurity_Initialize(cliFile, tableData);
    tableData = ClassLayout_Initialize(cliFile, tableData);
    tableData = FieldLayout_Initialize(cliFile, tableData);
    tableData = StandAloneSignature_Initialize(cliFile, tableData);
    tableData = EventMap_Initialize(cliFile, tableData);
    tableData = Event_Initialize(cliFile, tableData);
    tableData = PropertyMap_Initialize(cliFile, tableData);
    tableData = Property_Initialize(cliFile, tableData);
    tableData = MethodSemantics_Initialize(cliFile, tableData);
    tableData = MethodImplementation_Initialize(cliFile, tableData);
    tableData = ModuleReference_Initialize(cliFile, tableData);
    tableData = TypeSpecification_Initialize(cliFile, tableData);
    tableData = ImplementationMap_Initialize(cliFile, tableData);
    tableData = FieldRVA_Initialize(cliFile, tableData);
    tableData = AssemblyDefinition_Initialize(cliFile, tableData);
    tableData = AssemblyProcessor_Initialize(cliFile, tableData);
    tableData = AssemblyOperatingSystem_Initialize(cliFile, tableData);
    tableData = AssemblyReference_Initialize(cliFile, tableData);
    tableData = AssemblyReferenceProcessor_Initialize(cliFile, tableData);
    tableData = AssemblyReferenceOperatingSystem_Initialize(cliFile, tableData);
    tableData = File_Initialize(cliFile, tableData);
    tableData = ExportedType_Initialize(cliFile, tableData);
    tableData = ManifestResource_Initialize(cliFile, tableData);
    tableData = NestedClass_Initialize(cliFile, tableData);
    tableData = GenericParameter_Initialize(cliFile, tableData);
    tableData = MethodSpecification_Initialize(cliFile, tableData);
    tableData = GenericParameterConstraint_Initialize(cliFile, tableData);

	tableData = ModuleDefinition_Load(cliFile, tableData);
	tableData = TypeReference_Load(cliFile, tableData);
	tableData = TypeDefinition_Load(cliFile, tableData);
	tableData = Field_Load(cliFile, tableData);
	tableData = MethodDefinition_Load(cliFile, tableData);
	tableData = Parameter_Load(cliFile, tableData);
	tableData = InterfaceImplementation_Load(cliFile, tableData);
	tableData = MemberReference_Load(cliFile, tableData);
	tableData = Constant_Load(cliFile, tableData);
	tableData = CustomAttribute_Load(cliFile, tableData);
	tableData = FieldMarshal_Load(cliFile, tableData);
	tableData = DeclSecurity_Load(cliFile, tableData);
	tableData = ClassLayout_Load(cliFile, tableData);
	tableData = FieldLayout_Load(cliFile, tableData);
	tableData = StandAloneSignature_Load(cliFile, tableData);
	tableData = EventMap_Load(cliFile, tableData);
	tableData = Event_Load(cliFile, tableData);
	tableData = PropertyMap_Load(cliFile, tableData);
	tableData = Property_Load(cliFile, tableData);
	tableData = MethodSemantics_Load(cliFile, tableData);
	tableData = MethodImplementation_Load(cliFile, tableData);
	tableData = ModuleReference_Load(cliFile, tableData);
	tableData = TypeSpecification_Load(cliFile, tableData);
	tableData = ImplementationMap_Load(cliFile, tableData);
	tableData = FieldRVA_Load(cliFile, tableData);
	tableData = AssemblyDefinition_Load(cliFile, tableData);
    tableData = AssemblyProcessor_Load(cliFile, tableData);
    tableData = AssemblyOperatingSystem_Load(cliFile, tableData);
    tableData = AssemblyReference_Load(cliFile, tableData);
    tableData = AssemblyReferenceProcessor_Load(cliFile, tableData);
    tableData = AssemblyReferenceOperatingSystem_Load(cliFile, tableData);
    tableData = File_Load(cliFile, tableData);
    tableData = ExportedType_Load(cliFile, tableData);
    tableData = ManifestResource_Load(cliFile, tableData);
    tableData = NestedClass_Load(cliFile, tableData);
    tableData = GenericParameter_Load(cliFile, tableData);
    tableData = MethodSpecification_Load(cliFile, tableData);
    tableData = GenericParameterConstraint_Load(cliFile, tableData);

    printf("Loaded MetaData\n");
    return cliFile;
}

void CLIFile_Destroy(CLIFile* pFile)
{
    ModuleDefinition_Cleanup(pFile);
    TypeReference_Cleanup(pFile);
    TypeDefinition_Cleanup(pFile);
    Field_Cleanup(pFile);
    MethodDefinition_Cleanup(pFile);
    Parameter_Cleanup(pFile);
    InterfaceImplementation_Cleanup(pFile);
    MemberReference_Cleanup(pFile);
    Constant_Cleanup(pFile);
    CustomAttribute_Cleanup(pFile);
    FieldMarshal_Cleanup(pFile);
    DeclSecurity_Cleanup(pFile);
    ClassLayout_Cleanup(pFile);
    FieldLayout_Cleanup(pFile);
    StandAloneSignature_Cleanup(pFile);
    EventMap_Cleanup(pFile);
    Event_Cleanup(pFile);
    PropertyMap_Cleanup(pFile);
    Property_Cleanup(pFile);
    MethodSemantics_Cleanup(pFile);
    MethodImplementation_Cleanup(pFile);
    ModuleReference_Cleanup(pFile);
    TypeSpecification_Cleanup(pFile);
    ImplementationMap_Cleanup(pFile);
    FieldRVA_Cleanup(pFile);
    AssemblyDefinition_Cleanup(pFile);
    AssemblyProcessor_Cleanup(pFile);
    AssemblyOperatingSystem_Cleanup(pFile);
    AssemblyReference_Cleanup(pFile);
    AssemblyReferenceProcessor_Cleanup(pFile);
    AssemblyReferenceOperatingSystem_Cleanup(pFile);
    File_Cleanup(pFile);
    ExportedType_Cleanup(pFile);
    ManifestResource_Cleanup(pFile);
    NestedClass_Cleanup(pFile);
    GenericParameter_Cleanup(pFile);
    MethodSpecification_Cleanup(pFile);
    GenericParameterConstraint_Cleanup(pFile);
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
