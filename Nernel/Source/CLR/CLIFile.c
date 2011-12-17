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

    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ModuleDefinition)) != 0)
    {
        cliFile->ModuleDefinitionCount = *(uint32_t*)tableData;
        cliFile->ModuleDefinitions = (ModuleDefinition*)calloc(cliFile->ModuleDefinitionCount, sizeof(ModuleDefinition));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_TypeReference)) != 0)
    {
        cliFile->TypeReferenceCount = *(uint32_t*)tableData;
        cliFile->TypeReferences = (TypeReference*)calloc(cliFile->TypeReferenceCount, sizeof(TypeReference));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_TypeDefinition)) != 0)
    {
        cliFile->TypeDefinitionCount = *(uint32_t*)tableData;
        cliFile->TypeDefinitions = (TypeDefinition*)calloc(cliFile->TypeDefinitionCount, sizeof(TypeDefinition));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Field)) != 0)
    {
        cliFile->FieldCount = *(uint32_t*)tableData;
        cliFile->Fields = (Field*)calloc(cliFile->FieldCount, sizeof(Field));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_MethodDefinition)) != 0)
    {
        cliFile->MethodDefinitionCount = *(uint32_t*)tableData;
        cliFile->MethodDefinitions = (MethodDefinition*)calloc(cliFile->MethodDefinitionCount, sizeof(MethodDefinition));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Parameter)) != 0)
    {
        cliFile->ParameterCount = *(uint32_t*)tableData;
        cliFile->Parameters = (Parameter*)calloc(cliFile->ParameterCount, sizeof(Parameter));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_InterfaceImplementation)) != 0)
    {
        cliFile->InterfaceImplementationCount = *(uint32_t*)tableData;
        cliFile->InterfaceImplementations = (InterfaceImplementation*)calloc(cliFile->InterfaceImplementationCount, sizeof(InterfaceImplementation));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_MemberReference)) != 0)
    {
        cliFile->MemberReferenceCount = *(uint32_t*)tableData;
        cliFile->MemberReferences = (MemberReference*)calloc(cliFile->MemberReferenceCount, sizeof(MemberReference));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Constant)) != 0)
    {
        cliFile->ConstantCount = *(uint32_t*)tableData;
        cliFile->Constants = (Constant*)calloc(cliFile->ConstantCount, sizeof(Constant));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_CustomAttribute)) != 0)
    {
        cliFile->CustomAttributeCount = *(uint32_t*)tableData;
        cliFile->CustomAttributes = (CustomAttribute*)calloc(cliFile->CustomAttributeCount, sizeof(CustomAttribute));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_FieldMarshal)) != 0)
    {
        cliFile->FieldMarshalCount = *(uint32_t*)tableData;
        cliFile->FieldMarshals = (FieldMarshal*)calloc(cliFile->FieldMarshalCount, sizeof(FieldMarshal));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_DeclSecurity)) != 0)
    {
        cliFile->DeclSecurityCount = *(uint32_t*)tableData;
        cliFile->DeclSecurities = (DeclSecurity*)calloc(cliFile->DeclSecurityCount, sizeof(DeclSecurity));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ClassLayout)) != 0)
    {
        cliFile->ClassLayoutCount = *(uint32_t*)tableData;
        cliFile->ClassLayouts = (ClassLayout*)calloc(cliFile->ClassLayoutCount, sizeof(ClassLayout));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_FieldLayout)) != 0)
    {
        cliFile->FieldLayoutCount = *(uint32_t*)tableData;
        cliFile->FieldLayouts = (FieldLayout*)calloc(cliFile->FieldLayoutCount, sizeof(FieldLayout));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_StandAloneSignature)) != 0)
    {
        cliFile->StandAloneSignatureCount = *(uint32_t*)tableData;
        cliFile->StandAloneSignatures = (StandAloneSignature*)calloc(cliFile->StandAloneSignatureCount, sizeof(StandAloneSignature));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_EventMap)) != 0)
    {
        cliFile->EventMapCount = *(uint32_t*)tableData;
        cliFile->EventMaps = (EventMap*)calloc(cliFile->EventMapCount, sizeof(EventMap));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Event)) != 0)
    {
        cliFile->EventCount = *(uint32_t*)tableData;
        cliFile->Events = (Event*)calloc(cliFile->EventCount, sizeof(Event));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_PropertyMap)) != 0)
    {
        cliFile->PropertyMapCount = *(uint32_t*)tableData;
        cliFile->PropertyMaps = (PropertyMap*)calloc(cliFile->PropertyMapCount, sizeof(PropertyMap));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Property)) != 0)
    {
        cliFile->PropertyCount = *(uint32_t*)tableData;
        cliFile->Properties = (Property*)calloc(cliFile->PropertyCount, sizeof(Property));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_MethodSemantics)) != 0)
    {
        cliFile->MethodSemanticsCount = *(uint32_t*)tableData;
        cliFile->MethodSemantics = (MethodSemantics*)calloc(cliFile->MethodSemanticsCount, sizeof(MethodSemantics));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_MethodImplementation)) != 0)
    {
        cliFile->MethodImplementationCount = *(uint32_t*)tableData;
        cliFile->MethodImplementations = (MethodImplementation*)calloc(cliFile->MethodImplementationCount, sizeof(MethodImplementation));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ModuleReference)) != 0)
    {
        cliFile->ModuleReferenceCount = *(uint32_t*)tableData;
        cliFile->ModuleReferences = (ModuleReference*)calloc(cliFile->ModuleReferenceCount, sizeof(ModuleReference));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_TypeSpecification)) != 0)
    {
        cliFile->TypeSpecificationCount = *(uint32_t*)tableData;
        cliFile->TypeSpecifications = (TypeSpecification*)calloc(cliFile->TypeSpecificationCount, sizeof(TypeSpecification));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ImplementationMap)) != 0)
    {
        cliFile->ImplementationMapCount = *(uint32_t*)tableData;
        cliFile->ImplementationMaps = (ImplementationMap*)calloc(cliFile->ImplementationMapCount, sizeof(ImplementationMap));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_FieldRVA)) != 0)
    {
        cliFile->FieldRVACount = *(uint32_t*)tableData;
        cliFile->FieldRVAs = (FieldRVA*)calloc(cliFile->FieldRVACount, sizeof(FieldRVA));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_AssemblyDefinition)) != 0)
    {
        cliFile->AssemblyDefinitionCount = *(uint32_t*)tableData;
        cliFile->AssemblyDefinitions = (AssemblyDefinition*)calloc(cliFile->AssemblyDefinitionCount, sizeof(AssemblyDefinition));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_AssemblyProcessor)) != 0)
    {
        cliFile->AssemblyProcessorCount = *(uint32_t*)tableData;
        cliFile->AssemblyProcessors = (AssemblyProcessor*)calloc(cliFile->AssemblyProcessorCount, sizeof(AssemblyProcessor));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_AssemblyOperatingSystem)) != 0)
    {
        cliFile->AssemblyOperatingSystemCount = *(uint32_t*)tableData;
        cliFile->AssemblyOperatingSystems = (AssemblyOperatingSystem*)calloc(cliFile->AssemblyOperatingSystemCount, sizeof(AssemblyOperatingSystem));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_AssemblyReference)) != 0)
    {
        cliFile->AssemblyReferenceCount = *(uint32_t*)tableData;
        cliFile->AssemblyReferences = (AssemblyReference*)calloc(cliFile->AssemblyReferenceCount, sizeof(AssemblyReference));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_AssemblyReferenceProcessor)) != 0)
    {
        cliFile->AssemblyReferenceProcessorCount = *(uint32_t*)tableData;
        cliFile->AssemblyReferenceProcessors = (AssemblyReferenceProcessor*)calloc(cliFile->AssemblyReferenceProcessorCount, sizeof(AssemblyReferenceProcessor));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_AssemblyReferenceOperatingSystem)) != 0)
    {
        cliFile->AssemblyReferenceOperatingSystemCount = *(uint32_t*)tableData;
        cliFile->AssemblyReferenceOperatingSystems = (AssemblyReferenceOperatingSystem*)calloc(cliFile->AssemblyReferenceOperatingSystemCount, sizeof(AssemblyReferenceOperatingSystem));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_File)) != 0)
    {
        cliFile->FileCount = *(uint32_t*)tableData;
        cliFile->Files = (File*)calloc(cliFile->FileCount, sizeof(File));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ExportedType)) != 0)
    {
        cliFile->ExportedTypeCount = *(uint32_t*)tableData;
        cliFile->ExportedTypes = (ExportedType*)calloc(cliFile->ExportedTypeCount, sizeof(ExportedType));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_ManifestResource)) != 0)
    {
        cliFile->ManifestResourceCount = *(uint32_t*)tableData;
        cliFile->ManifestResources = (ManifestResource*)calloc(cliFile->ManifestResourceCount, sizeof(ManifestResource));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_NestedClass)) != 0)
    {
        cliFile->NestedClassCount = *(uint32_t*)tableData;
        cliFile->NestedClasses = (NestedClass*)calloc(cliFile->NestedClassCount, sizeof(NestedClass));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_GenericParameter)) != 0)
    {
        cliFile->GenericParameterCount = *(uint32_t*)tableData;
        cliFile->GenericParameters = (GenericParameter*)calloc(cliFile->GenericParameterCount, sizeof(GenericParameter));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_MethodSpecification)) != 0)
    {
        cliFile->MethodSpecificationCount = *(uint32_t*)tableData;
        cliFile->MethodSpecifications = (MethodSpecification*)calloc(cliFile->MethodSpecificationCount, sizeof(MethodSpecification));
        tableData += 4;
    }
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_GenericParameterConstraint)) != 0)
    {
        cliFile->GenericParameterConstraintCount = *(uint32_t*)tableData;
        cliFile->GenericParameterConstraints = (GenericParameterConstraint*)calloc(cliFile->GenericParameterConstraintCount, sizeof(GenericParameterConstraint));
        tableData += 4;
    }

	// Not quite sure why this is needed, but it's there,
	// so we have to account for it.
	//tableData += 4;


    tableData = CLIFile_LoadModuleDefinitions(cliFile, tableData);
    printf("Loaded %u ModuleDefinitions\n", (unsigned int)cliFile->ModuleDefinitionCount);

	tableData = CLIFile_LoadTypeReferences(cliFile, tableData);
    printf("Loaded %u TypeReferences\n", (unsigned int)cliFile->TypeReferenceCount);
    
	tableData = CLIFile_LoadTypeDefinitions(cliFile, tableData);
    printf("Loaded %u TypeDefinitions\n", (unsigned int)cliFile->TypeDefinitionCount);
    
	tableData = CLIFile_LoadFields(cliFile, tableData);
    printf("Loaded %u Fields\n", (unsigned int)cliFile->FieldCount);

	tableData = CLIFile_LoadMethodDefinitions(cliFile, tableData);
    printf("Loaded %u MethodDefinitions\n", (unsigned int)cliFile->MethodDefinitionCount);

	tableData = CLIFile_LoadParameters(cliFile, tableData);
    printf("Loaded %u Parameters\n", (unsigned int)cliFile->ParameterCount);

	tableData = CLIFile_LoadInterfaceImplementations(cliFile, tableData);
    printf("Loaded %u InterfaceImplementations\n", (unsigned int)cliFile->InterfaceImplementationCount);

	tableData = CLIFile_LoadMemberReferences(cliFile, tableData);
    printf("Loaded %u MemberReferences\n", (unsigned int)cliFile->MemberReferenceCount);

	tableData = CLIFile_LoadConstants(cliFile, tableData);
    printf("Loaded %u Constants\n", (unsigned int)cliFile->ConstantCount);

	tableData = CLIFile_LoadCustomAttributes(cliFile, tableData);
    printf("Loaded %u CustomAttributes\n", (unsigned int)cliFile->CustomAttributeCount);

	tableData = CLIFile_LoadFieldMarshals(cliFile, tableData);
    printf("Loaded %u FieldMarshals\n", (unsigned int)cliFile->FieldMarshalCount);

	tableData = CLIFile_LoadDeclSecurities(cliFile, tableData);
    printf("Loaded %u DeclSecurities\n", (unsigned int)cliFile->DeclSecurityCount);

	tableData = CLIFile_LoadClassLayouts(cliFile, tableData);
    printf("Loaded %u ClassLayouts\n", (unsigned int)cliFile->ClassLayoutCount);

	tableData = CLIFile_LoadFieldLayouts(cliFile, tableData);
    printf("Loaded %u FieldLayouts\n", (unsigned int)cliFile->FieldLayoutCount);

	tableData = CLIFile_LoadStandAloneSignatures(cliFile, tableData);
    printf("Loaded %u StandAloneSignatures\n", (unsigned int)cliFile->StandAloneSignatureCount);

	tableData = CLIFile_LoadEventMaps(cliFile, tableData);
    printf("Loaded %u EventMaps\n", (unsigned int)cliFile->EventMapCount);

	tableData = CLIFile_LoadEvents(cliFile, tableData);
    printf("Loaded %u Events\n", (unsigned int)cliFile->EventCount);

	tableData = CLIFile_LoadPropertyMaps(cliFile, tableData);
    printf("Loaded %u PropertyMaps\n", (unsigned int)cliFile->PropertyMapCount);

	tableData = CLIFile_LoadProperties(cliFile, tableData);
    printf("Loaded %u Properties\n", (unsigned int)cliFile->PropertyCount);

	tableData = CLIFile_LoadMethodSemantics(cliFile, tableData);
    printf("Loaded %u MethodSemantics\n", (unsigned int)cliFile->MethodSemanticsCount);

	tableData = CLIFile_LoadMethodImplementations(cliFile, tableData);
    printf("Loaded %u MethodImplementations\n", (unsigned int)cliFile->MethodImplementationCount);

	tableData = CLIFile_LoadModuleReferences(cliFile, tableData);
    printf("Loaded %u ModuleReferences\n", (unsigned int)cliFile->ModuleReferenceCount);

	tableData = CLIFile_LoadTypeSpecifications(cliFile, tableData);
    printf("Loaded %u TypeSpecifications\n", (unsigned int)cliFile->TypeSpecificationCount);

	tableData = CLIFile_LoadImplementationMaps(cliFile, tableData);
    printf("Loaded %u ImplementationMaps\n", (unsigned int)cliFile->ImplementationMapCount);

	tableData = CLIFile_LoadFieldRVAs(cliFile, tableData);
    printf("Loaded %u FieldRVAs\n", (unsigned int)cliFile->FieldRVACount);

	tableData = CLIFile_LoadAssemblyDefinitions(cliFile, tableData);
    printf("Loaded %u AssemblyDefinitions\n", (unsigned int)cliFile->AssemblyDefinitionCount);

    tableData = CLIFile_LoadAssemblyProcessors(cliFile, tableData);
    printf("Loaded %u AssemblyProcessors\n", (unsigned int)cliFile->AssemblyProcessorCount);

    tableData = CLIFile_LoadAssemblyOperatingSystems(cliFile, tableData);
    printf("Loaded %u AssemblyOperatingSystems\n", (unsigned int)cliFile->AssemblyOperatingSystemCount);

    tableData = CLIFile_LoadAssemblyReferences(cliFile, tableData);
    printf("Loaded %u AssemblyReferences\n", (unsigned int)cliFile->AssemblyReferenceCount);

    tableData = CLIFile_LoadAssemblyReferenceProcessors(cliFile, tableData);
    printf("Loaded %u AssemblyReferenceProcessors\n", (unsigned int)cliFile->AssemblyReferenceProcessorCount);

    tableData = CLIFile_LoadAssemblyReferenceOperatingSystems(cliFile, tableData);
    printf("Loaded %u AssemblyReferenceOperatingSystems\n", (unsigned int)cliFile->AssemblyReferenceOperatingSystemCount);

    tableData = CLIFile_LoadFiles(cliFile, tableData);
    printf("Loaded %u Files\n", (unsigned int)cliFile->FileCount);

    tableData = CLIFile_LoadExportedTypes(cliFile, tableData);
    printf("Loaded %u ExportedTypes\n", (unsigned int)cliFile->ExportedTypeCount);

    tableData = CLIFile_LoadManifestResources(cliFile, tableData);
    printf("Loaded %u ManifestResources\n", (unsigned int)cliFile->ManifestResourceCount);

    tableData = CLIFile_LoadNestedClasses(cliFile, tableData);
    printf("Loaded %u NestedClasses\n", (unsigned int)cliFile->NestedClassCount);

    tableData = CLIFile_LoadGenericParameters(cliFile, tableData);
    printf("Loaded %u GenericParameters\n", (unsigned int)cliFile->GenericParameterCount);

    tableData = CLIFile_LoadMethodSpecifications(cliFile, tableData);
    printf("Loaded %u MethodSpecifications\n", (unsigned int)cliFile->MethodSpecificationCount);

    tableData = CLIFile_LoadGenericParameterConstraints(cliFile, tableData);
    printf("Loaded %u GenericParameterConstraints\n", (unsigned int)cliFile->GenericParameterConstraintCount);

    printf("Loaded Assembly\n");
    return cliFile;
}

void CLIFile_Destroy(CLIFile* pFile)
{
    if (pFile->ModuleDefinitions) free(pFile->ModuleDefinitions);
    if (pFile->TypeReferences) free(pFile->TypeReferences);
    if (pFile->TypeDefinitions) free(pFile->TypeDefinitions);
    if (pFile->Fields) free(pFile->Fields);
    if (pFile->MethodDefinitions) free(pFile->MethodDefinitions);
    if (pFile->Parameters) free(pFile->Parameters);
    if (pFile->InterfaceImplementations) free(pFile->InterfaceImplementations);
    if (pFile->MemberReferences) free(pFile->MemberReferences);
    if (pFile->Constants) free(pFile->Constants);
    if (pFile->CustomAttributes) free(pFile->CustomAttributes);
    if (pFile->FieldMarshals) free(pFile->FieldMarshals);
    if (pFile->DeclSecurities) free(pFile->DeclSecurities);
    if (pFile->ClassLayouts) free(pFile->ClassLayouts);
    if (pFile->FieldLayouts) free(pFile->FieldLayouts);
    if (pFile->StandAloneSignatures) free(pFile->StandAloneSignatures);
    if (pFile->EventMaps) free(pFile->EventMaps);
    if (pFile->Events) free(pFile->Events);
    if (pFile->PropertyMaps) free(pFile->PropertyMaps);
    if (pFile->Properties) free(pFile->Properties);
    if (pFile->MethodSemantics) free(pFile->MethodSemantics);
    if (pFile->MethodImplementations) free(pFile->MethodImplementations);
    if (pFile->ModuleReferences) free(pFile->ModuleReferences);
    if (pFile->TypeSpecifications) free(pFile->TypeSpecifications);
    if (pFile->ImplementationMaps) free(pFile->ImplementationMaps);
    if (pFile->FieldRVAs) free(pFile->FieldRVAs);
    if (pFile->AssemblyDefinitions) free(pFile->AssemblyDefinitions);
    if (pFile->AssemblyProcessors) free(pFile->AssemblyProcessors);
    if (pFile->AssemblyOperatingSystems) free(pFile->AssemblyOperatingSystems);
    if (pFile->AssemblyReferences) free(pFile->AssemblyReferences);
    if (pFile->AssemblyReferenceProcessors) free(pFile->AssemblyReferenceProcessors);
    if (pFile->AssemblyReferenceOperatingSystems) free(pFile->AssemblyReferenceOperatingSystems);
    if (pFile->Files) free(pFile->Files);
    if (pFile->ExportedTypes) free(pFile->ExportedTypes);
    if (pFile->ManifestResources) free(pFile->ManifestResources);
    if (pFile->NestedClasses) free(pFile->NestedClasses);
    if (pFile->GenericParameters) free(pFile->GenericParameters);
    if (pFile->MethodSpecifications) free(pFile->MethodSpecifications);
    if (pFile->GenericParameterConstraints) free(pFile->GenericParameterConstraints);
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

const uint8_t* CLIFile_LoadModuleDefinitions(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ModuleDefinitionCount; ++index)
    {
        pFile->ModuleDefinitions[index].Generation = *(uint16_t*)pTableData; 
		pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0)  { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else  { heapIndex = *((uint16_t*)pTableData); pTableData += 2; }
        pFile->ModuleDefinitions[index].Name = CLIFile_GetString(pFile, heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2;  }
        pFile->ModuleDefinitions[index].ModuleVersionID = pFile->GUIDsHeap + heapIndex;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2;  }
        pFile->ModuleDefinitions[index].EncID = pFile->GUIDsHeap + heapIndex;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ModuleDefinitions[index].EncBaseID = pFile->GUIDsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadTypeReferences(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t resolutionScopeIndex = 0;
    uint8_t resolutionScopeTable = 0;
    uint32_t resolutionScopeRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->TypeReferenceCount; ++index)
    {
        if (pFile->ModuleDefinitionCount > ResolutionScope_Type_MaxRows16Bit ||
            pFile->ModuleReferenceCount > ResolutionScope_Type_MaxRows16Bit ||
            pFile->AssemblyReferenceCount > ResolutionScope_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > ResolutionScope_Type_MaxRows16Bit) { resolutionScopeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { resolutionScopeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        resolutionScopeTable = resolutionScopeIndex & ResolutionScope_Type_Mask;
        resolutionScopeRow = resolutionScopeIndex >> ResolutionScope_Type_Bits;
        switch (resolutionScopeTable)
        {
        case ResolutionScope_Type_ModuleDefinition: pFile->TypeReferences[index].ResolutionScope.ModuleDefinition = &pFile->ModuleDefinitions[resolutionScopeRow]; break;
        case ResolutionScope_Type_ModuleReference: pFile->TypeReferences[index].ResolutionScope.ModuleReference = &pFile->ModuleReferences[resolutionScopeRow]; break;
        case ResolutionScope_Type_AssemblyReference: pFile->TypeReferences[index].ResolutionScope.AssemblyReference = &pFile->AssemblyReferences[resolutionScopeRow]; break;
        case ResolutionScope_Type_TypeReference: pFile->TypeReferences[index].ResolutionScope.TypeReference = &pFile->TypeReferences[resolutionScopeRow]; break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeReferences[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeReferences[index].Namespace = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadTypeDefinitions(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t extendsIndex = 0;
    uint8_t extendsTable = 0;
    uint32_t extendsRow = 0;
    uint32_t fieldListIndex = 0;
    uint32_t methodDefinitionListIndex = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->TypeDefinitionCount; ++index)
    {
        pFile->TypeDefinitions[index].Flags = *(uint32_t*)pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].Namespace = (const char*)(pFile->StringsHeap + heapIndex);
        if (pFile->TypeDefinitionCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefOrRef_Type_MaxRows16Bit) { extendsIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { extendsIndex = *(uint16_t*)pTableData; pTableData += 2; }
        extendsTable = extendsIndex & TypeDefOrRef_Type_Mask;
        extendsRow = extendsIndex >> TypeDefOrRef_Type_Bits;
        switch (extendsTable)
        {
        case TypeDefOrRef_Type_TypeDefinition: pFile->TypeDefinitions[index].Extends.TypeDefinition = &pFile->TypeDefinitions[extendsRow]; break;
        case TypeDefOrRef_Type_TypeReference: pFile->TypeDefinitions[index].Extends.TypeReference = &pFile->TypeReferences[extendsRow]; break;
        case TypeDefOrRef_Type_TypeSpecification: pFile->TypeDefinitions[index].Extends.TypeSpecification = &pFile->TypeSpecifications[extendsRow]; break;
        default: break;
        }
        if (pFile->FieldCount > 0xFFFF) { fieldListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].FieldList = &pFile->Fields[fieldListIndex];
        if (pFile->MethodDefinitionCount > 0xFFFF) { methodDefinitionListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodDefinitionListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].MethodDefinitionList = &pFile->MethodDefinitions[methodDefinitionListIndex];
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadFields(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->FieldCount; ++index)
    {
        pFile->Fields[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Fields[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Fields[index].Signature = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* ParseHeapEncodedLength(const uint8_t* pData, uint32_t* pLength)
{
    if ((pData[0] & 0x80) == 0)
    {
        *pLength = pData[0] & 0x7F;
        return pData + 1;
    }
    if ((pData[0] & 0xC0) == 0x80)
    {
        *pLength = ((pData[0] & 0x3F) << 8) + pData[1];
        return pData + 2;
    }
    if ((pData[0] & 0xE0) == 0xC0)
    {
        *pLength = ((pData[0] & 0x1F) << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        return pData + 4;
    }
    *pLength = 0;
    return pData;
}

const uint8_t* CLIFile_LoadMethodDefinitions(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t methodBodyVirtualAddress = 0;
    PESectionHeader* methodBodySectionHeader = NULL;
    const uint8_t* methodBodyHeader = NULL;
    uint8_t methodBodyHeaderSize = 0;
    uint32_t parameterListIndex = 0;
    uint32_t* parameterListIndexes = (uint32_t*)calloc(pFile->MethodDefinitionCount, sizeof(uint32_t));
    for (uint32_t index = 0, heapIndex = 0; index < pFile->MethodDefinitionCount; ++index)
    {
        methodBodyVirtualAddress = *(uint32_t*)pTableData; pTableData += 4;
        if (methodBodyVirtualAddress != 0)
        {
            methodBodySectionHeader = PEFile_GetSection(pFile->PEFile->SectionHeaders, pFile->PEFile->PEHeader->NumberOfSections, methodBodyVirtualAddress);
            methodBodyHeader = pFile->PEFile->Data + methodBodySectionHeader->PointerToRawData + (methodBodyVirtualAddress - methodBodySectionHeader->VirtualAddress);
            methodBodyHeaderSize = 1;
            pFile->MethodDefinitions[index].BodyHeader.Flags = methodBodyHeader[0] & MethodDefinitionBodyHeader_Flags_HeaderType_Mask;
            pFile->MethodDefinitions[index].BodyHeader.MaxStack = MethodDefinitionBodyHeader_Tiny_MaxStack;
            pFile->MethodDefinitions[index].BodyHeader.CodeSize = methodBodyHeader[0] >> MethodDefinitionBodyHeader_Flags_HeaderType_Bits;
            pFile->MethodDefinitions[index].BodyHeader.LocalVariableSignatureToken = 0;
            if (pFile->MethodDefinitions[index].BodyHeader.Flags == MethodDefinitionBodyHeader_Flags_HeaderType_Fat)
            {
                pFile->MethodDefinitions[index].BodyHeader.Flags = (*(uint16_t*)(methodBodyHeader + 0)) & MethodDefinitionBodyHeader_Fat_Flags_Mask;
                methodBodyHeaderSize = ((*(uint16_t*)(methodBodyHeader + 0)) >> MethodDefinitionBodyHeader_Fat_Flags_Bits) * 4;
                pFile->MethodDefinitions[index].BodyHeader.MaxStack = *(uint16_t*)(methodBodyHeader + 2);
                pFile->MethodDefinitions[index].BodyHeader.CodeSize = *(uint32_t*)(methodBodyHeader + 4);
                pFile->MethodDefinitions[index].BodyHeader.LocalVariableSignatureToken = *(uint32_t*)(methodBodyHeader + 8);
            }
            pFile->MethodDefinitions[index].BodyCode = methodBodyHeader + methodBodyHeaderSize;
        }
        pFile->MethodDefinitions[index].ImplFlags = *(uint16_t*)pTableData; pTableData += 2;
        pFile->MethodDefinitions[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodDefinitions[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodDefinitions[index].Signature = pFile->BlobsHeap + heapIndex;
        if (pFile->ParameterCount > 0xFFFF) { parameterListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parameterListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodDefinitions[index].ParameterList = &pFile->Parameters[parameterListIndex];
        parameterListIndexes[index] = parameterListIndex;
        //if (methodBodyVirtualAddress != 0) printf("Method: %s, RVA: %u, HeaderSize: %u, CodeSize: %u\n", pFile->MethodDefinitions[index].Name, (unsigned int)methodBodyVirtualAddress, (unsigned int)methodBodyHeaderSize, (unsigned int)pFile->MethodDefinitions[index].BodyHeader.CodeSize);
    }
    uint8_t parameterListCount = 0;
    for (uint32_t index = 0, used = 0; index < pFile->MethodDefinitionCount; ++index, used += parameterListCount)
    {
        if (index == (pFile->MethodDefinitionCount - 1)) parameterListCount = pFile->ParameterCount - used;
        else parameterListCount = parameterListIndexes[index + 1] - parameterListIndexes[index];
    }
    free(parameterListIndexes);
    return pTableData;
}

const uint8_t* CLIFile_LoadParameters(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ParameterCount; ++index)
    {
        pFile->Parameters[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        pFile->Parameters[index].Sequence = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Parameters[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadInterfaceImplementations(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t implementorIndex = 0;
    uint32_t interfaceIndex = 0;
    uint8_t interfaceTable = 0;
    uint32_t interfaceRow = 0;
    for (uint32_t index = 0; index < pFile->InterfaceImplementationCount; ++index)
    {
        if (pFile->TypeDefinitionCount > 0xFFFF) { implementorIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementorIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->InterfaceImplementations[index].Implementor = &pFile->TypeDefinitions[implementorIndex];

        if (pFile->TypeDefinitionCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefOrRef_Type_MaxRows16Bit) { interfaceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { interfaceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        interfaceTable = interfaceIndex & TypeDefOrRef_Type_Mask;
        interfaceRow = interfaceIndex >> TypeDefOrRef_Type_Bits;
        switch (interfaceTable)
        {
        case TypeDefOrRef_Type_TypeDefinition: pFile->InterfaceImplementations[index].Interface.TypeDefinition = &pFile->TypeDefinitions[interfaceRow]; break;
        case TypeDefOrRef_Type_TypeReference: pFile->InterfaceImplementations[index].Interface.TypeReference = &pFile->TypeReferences[interfaceRow]; break;
        case TypeDefOrRef_Type_TypeSpecification: pFile->InterfaceImplementations[index].Interface.TypeSpecification = &pFile->TypeSpecifications[interfaceRow]; break;
        default: break;
        }
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadMemberReferences(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint8_t parentTable = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->MemberReferenceCount; ++index)
    {
        if (pFile->TypeDefinitionCount > MemberRefParent_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > MemberRefParent_Type_MaxRows16Bit ||
            pFile->ModuleReferenceCount > MemberRefParent_Type_MaxRows16Bit ||
            pFile->MethodDefinitionCount > MemberRefParent_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > MemberRefParent_Type_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        parentTable = parentIndex & MemberRefParent_Type_Mask;
        parentRow = parentIndex >> MemberRefParent_Type_Bits;
        switch (parentTable)
        {
        case MemberRefParent_Type_TypeDefinition: pFile->MemberReferences[index].Parent.TypeDefinition = &pFile->TypeDefinitions[parentRow]; break;
        case MemberRefParent_Type_TypeReference: pFile->MemberReferences[index].Parent.TypeReference = &pFile->TypeReferences[parentRow]; break;
        case MemberRefParent_Type_ModuleReference: pFile->MemberReferences[index].Parent.ModuleReference = &pFile->ModuleReferences[parentRow]; break;
        case MemberRefParent_Type_MethodDefinition: pFile->MemberReferences[index].Parent.MethodDefinition = &pFile->MethodDefinitions[parentRow]; break;
        case MemberRefParent_Type_TypeSpecification: pFile->MemberReferences[index].Parent.TypeSpecification = &pFile->TypeSpecifications[parentRow]; break;
        default: break;
        }

        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MemberReferences[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MemberReferences[index].Signature = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadConstants(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint8_t parentTable = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ConstantCount; ++index)
    {
        pFile->Constants[index].Type = *pTableData; pTableData += 2; // 1 unused padding byte
        if (pFile->FieldCount > HasConstant_Type_MaxRows16Bit ||
            pFile->ParameterCount > HasConstant_Type_MaxRows16Bit ||
            pFile->PropertyCount > HasConstant_Type_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        parentTable = parentIndex & HasConstant_Type_Mask;
        parentRow = parentIndex >> HasConstant_Type_Bits;
        switch (parentTable)
        {
        case HasConstant_Type_Field: pFile->Constants[index].Parent.Field = &pFile->Fields[parentRow]; break;
        case HasConstant_Type_Parameter: pFile->Constants[index].Parent.Parameter = &pFile->Parameters[parentRow]; break;
        case HasConstant_Type_Property: pFile->Constants[index].Parent.Property = &pFile->Properties[parentRow]; break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Constants[index].Value = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadCustomAttributes(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint8_t parentTable = 0;
    uint32_t parentRow = 0;
    uint32_t typeIndex = 0;
    uint8_t typeTable = 0;
    uint32_t typeRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->CustomAttributeCount; ++index)
    {
        if (pFile->MethodDefinitionCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->FieldCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->TypeDefinitionCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->ParameterCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->InterfaceImplementationCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->MemberReferenceCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->ModuleDefinitionCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->DeclSecurityCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->PropertyCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->EventCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->StandAloneSignatureCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->ModuleReferenceCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->AssemblyDefinitionCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->AssemblyReferenceCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->FileCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->ExportedTypeCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->ManifestResourceCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->GenericParameterCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->GenericParameterConstraintCount > HasCustomAttribute_Type_MaxRows16Bit ||
            pFile->MethodSpecificationCount > HasCustomAttribute_Type_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        parentTable = parentIndex & HasCustomAttribute_Type_Mask;
        parentRow = parentIndex >> HasCustomAttribute_Type_Bits;
        switch (parentTable)
        {
        case HasCustomAttribute_Type_MethodDefinition: pFile->CustomAttributes[index].Parent.MethodDefinition = &pFile->MethodDefinitions[parentRow]; break;
        case HasCustomAttribute_Type_Field: pFile->CustomAttributes[index].Parent.Field = &pFile->Fields[parentRow]; break;
        case HasCustomAttribute_Type_TypeReference: pFile->CustomAttributes[index].Parent.TypeReference = &pFile->TypeReferences[parentRow]; break;
        case HasCustomAttribute_Type_TypeDefinition: pFile->CustomAttributes[index].Parent.TypeDefinition = &pFile->TypeDefinitions[parentRow]; break;
        case HasCustomAttribute_Type_Parameter: pFile->CustomAttributes[index].Parent.Parameter = &pFile->Parameters[parentRow]; break;
        case HasCustomAttribute_Type_InterfaceImplementation: pFile->CustomAttributes[index].Parent.InterfaceImplementation = &pFile->InterfaceImplementations[parentRow]; break;
        case HasCustomAttribute_Type_MemberReference: pFile->CustomAttributes[index].Parent.MemberReference = &pFile->MemberReferences[parentRow]; break;
        case HasCustomAttribute_Type_ModuleDefinition: pFile->CustomAttributes[index].Parent.ModuleDefinition = &pFile->ModuleDefinitions[parentRow]; break;
        case HasCustomAttribute_Type_DeclSecurity: pFile->CustomAttributes[index].Parent.DeclSecurity = &pFile->DeclSecurities[parentRow]; break;
        case HasCustomAttribute_Type_Property: pFile->CustomAttributes[index].Parent.Property = &pFile->Properties[parentRow]; break;
        case HasCustomAttribute_Type_Event: pFile->CustomAttributes[index].Parent.Event = &pFile->Events[parentRow]; break;
        case HasCustomAttribute_Type_StandAloneSignature: pFile->CustomAttributes[index].Parent.StandAloneSignature = &pFile->StandAloneSignatures[parentRow]; break;
        case HasCustomAttribute_Type_ModuleReference: pFile->CustomAttributes[index].Parent.ModuleReference = &pFile->ModuleReferences[parentRow]; break;
        case HasCustomAttribute_Type_TypeSpecification: pFile->CustomAttributes[index].Parent.TypeSpecification = &pFile->TypeSpecifications[parentRow]; break;
        case HasCustomAttribute_Type_AssemblyDefinition: pFile->CustomAttributes[index].Parent.AssemblyDefinition = &pFile->AssemblyDefinitions[parentRow]; break;
        case HasCustomAttribute_Type_AssemblyReference: pFile->CustomAttributes[index].Parent.AssemblyReference = &pFile->AssemblyReferences[parentRow]; break;
        case HasCustomAttribute_Type_File: pFile->CustomAttributes[index].Parent.File = &pFile->Files[parentRow]; break;
        case HasCustomAttribute_Type_ExportedType: pFile->CustomAttributes[index].Parent.ExportedType = &pFile->ExportedTypes[parentRow]; break;
        case HasCustomAttribute_Type_ManifestResource: pFile->CustomAttributes[index].Parent.ManifestResource = &pFile->ManifestResources[parentRow]; break;
        case HasCustomAttribute_Type_GenericParameter: pFile->CustomAttributes[index].Parent.GenericParameter = &pFile->GenericParameters[parentRow]; break;
        case HasCustomAttribute_Type_GenericParameterConstraint: pFile->CustomAttributes[index].Parent.GenericParameterConstraint = &pFile->GenericParameterConstraints[parentRow]; break;
        case HasCustomAttribute_Type_MethodSpecification: pFile->CustomAttributes[index].Parent.MethodSpecification = &pFile->MethodSpecifications[parentRow]; break;
        default: break;
        }
        if (pFile->MethodDefinitionCount > CustomAttributeType_Type_MaxRows16Bit ||
            pFile->MemberReferenceCount > CustomAttributeType_Type_MaxRows16Bit) { typeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { typeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        typeTable = typeIndex & CustomAttributeType_Type_Mask;
        typeRow = typeIndex >> CustomAttributeType_Type_Bits;
        switch (typeTable)
        {
        case CustomAttributeType_Type_MethodDefinition: pFile->CustomAttributes[index].Type.MethodDefinition = &pFile->MethodDefinitions[typeRow]; break;
        case CustomAttributeType_Type_MemberReference: pFile->CustomAttributes[index].Type.MemberReference = &pFile->MemberReferences[typeRow]; break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->CustomAttributes[index].Value = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadFieldMarshals(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint8_t parentTable = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->FieldMarshalCount; ++index)
    {
        if (pFile->FieldCount > HasFieldMarshal_Type_MaxRows16Bit ||
            pFile->ParameterCount > HasFieldMarshal_Type_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        parentTable = parentIndex & HasFieldMarshal_Type_Mask;
        parentRow = parentIndex >> HasFieldMarshal_Type_Bits;
        switch (parentTable)
        {
        case HasFieldMarshal_Type_Field: pFile->FieldMarshals[index].Parent.Field = &pFile->Fields[parentRow]; break;
        case HasFieldMarshal_Type_Parameter: pFile->FieldMarshals[index].Parent.Parameter = &pFile->Parameters[parentRow]; break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->FieldMarshals[index].NativeType = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadDeclSecurities(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint8_t parentTable = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->DeclSecurityCount; ++index)
    {
        pFile->DeclSecurities[index].Action = *(uint16_t*)pTableData; pTableData += 2;

        if (pFile->TypeDefinitionCount > HasDeclSecurity_Type_MaxRows16Bit ||
            pFile->MethodDefinitionCount > HasDeclSecurity_Type_MaxRows16Bit ||
            pFile->AssemblyDefinitionCount > HasDeclSecurity_Type_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        parentTable = parentIndex & HasDeclSecurity_Type_Mask;
        parentRow = parentIndex >> HasDeclSecurity_Type_Bits;
        switch (parentTable)
        {
        case HasDeclSecurity_Type_TypeDefinition: pFile->DeclSecurities[index].Parent.TypeDefinition = &pFile->TypeDefinitions[parentRow]; break;
        case HasDeclSecurity_Type_MethodDefinition: pFile->DeclSecurities[index].Parent.MethodDefinition = &pFile->MethodDefinitions[parentRow]; break;
        case HasDeclSecurity_Type_AssemblyDefinition: pFile->DeclSecurities[index].Parent.AssemblyDefinition = &pFile->AssemblyDefinitions[parentRow]; break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->DeclSecurities[index].PermissionSet = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadClassLayouts(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    for (uint32_t index = 0; index < pFile->ClassLayoutCount; ++index)
    {
        pFile->ClassLayouts[index].PackingSize = *(uint16_t*)pTableData; pTableData += 2;
        pFile->ClassLayouts[index].ClassSize = *(uint32_t*)pTableData; pTableData += 4;
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ClassLayouts[index].Parent = &pFile->TypeDefinitions[parentIndex];
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadFieldLayouts(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t fieldIndex = 0;
    for (uint32_t index = 0; index < pFile->FieldLayoutCount; ++index)
    {
        pFile->FieldLayouts[index].Offset = *(uint32_t*)pTableData; pTableData += 4;
        if (pFile->FieldCount > 0xFFFF) { fieldIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->FieldLayouts[index].Field = &pFile->Fields[fieldIndex];
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadStandAloneSignatures(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->StandAloneSignatureCount; ++index)
    {
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->StandAloneSignatures[index].Signature = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadEventMaps(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t eventListIndex = 0;
    for (uint32_t index = 0; index < pFile->EventMapCount; ++index)
    {
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->EventMaps[index].Parent = &pFile->TypeDefinitions[parentIndex];
        if (pFile->EventCount > 0xFFFF) { eventListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { eventListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->EventMaps[index].EventList = &pFile->Events[eventListIndex];
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadEvents(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t eventTypeIndex = 0;
    uint8_t eventTypeTable = 0;
    uint32_t eventTypeRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->EventCount; ++index)
    {
        pFile->Events[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Events[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if (pFile->TypeDefinitionCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefOrRef_Type_MaxRows16Bit) { eventTypeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { eventTypeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        eventTypeTable = eventTypeIndex & TypeDefOrRef_Type_Mask;
        eventTypeRow = eventTypeIndex >> TypeDefOrRef_Type_Bits;
        switch (eventTypeTable)
        {
        case TypeDefOrRef_Type_TypeDefinition: pFile->Events[index].EventType.TypeDefinition = &pFile->TypeDefinitions[eventTypeRow]; break;
        case TypeDefOrRef_Type_TypeReference: pFile->Events[index].EventType.TypeReference = &pFile->TypeReferences[eventTypeRow]; break;
        case TypeDefOrRef_Type_TypeSpecification: pFile->Events[index].EventType.TypeSpecification = &pFile->TypeSpecifications[eventTypeRow]; break;
        default: break;
        }
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadPropertyMaps(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t propertyListIndex = 0;
    for (uint32_t index = 0; index < pFile->PropertyMapCount; ++index)
    {
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->PropertyMaps[index].Parent = &pFile->TypeDefinitions[parentIndex];
        if (pFile->PropertyCount > 0xFFFF) { propertyListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { propertyListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->PropertyMaps[index].PropertyList = &pFile->Properties[propertyListIndex];
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadProperties(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->PropertyCount; ++index)
    {
        pFile->Properties[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Properties[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Properties[index].Signature = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadMethodSemantics(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t methodIndex = 0;
    uint32_t associationIndex = 0;
    uint8_t associationTable = 0;
    uint32_t associationRow = 0;
    for (uint32_t index = 0; index < pFile->MethodSemanticsCount; ++index)
    {
        pFile->MethodSemantics[index].Semantics = *(uint16_t*)pTableData; pTableData += 2;
        if (pFile->MethodDefinitionCount > 0xFFFF) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodSemantics[index].Method = &pFile->MethodDefinitions[methodIndex];

        if (pFile->EventCount > HasSemantics_Type_MaxRows16Bit ||
            pFile->PropertyCount > HasSemantics_Type_MaxRows16Bit) { associationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { associationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        associationTable = associationIndex & HasSemantics_Type_Mask;
        associationRow = associationIndex >> HasSemantics_Type_Bits;
        switch (associationTable)
        {
        case HasSemantics_Type_Event: pFile->MethodSemantics[index].Association.Event = &pFile->Events[associationRow]; break;
        case HasSemantics_Type_Property: pFile->MethodSemantics[index].Association.Property = &pFile->Properties[associationRow]; break;
        default: break;
        }
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadMethodImplementations(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t methodIndex = 0;
    uint8_t methodTable = 0;
    uint32_t methodRow = 0;
    for (uint32_t index = 0; index < pFile->MethodImplementationCount; ++index)
    {
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodImplementations[index].Parent = &pFile->TypeDefinitions[parentIndex];

        if (pFile->MethodDefinitionCount > MethodDefOrRef_Type_MaxRows16Bit ||
            pFile->MemberReferenceCount > MethodDefOrRef_Type_MaxRows16Bit) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        methodTable = methodIndex & MethodDefOrRef_Type_Mask;
        methodRow = methodIndex >> MethodDefOrRef_Type_Bits;
        switch (methodTable)
        {
        case MethodDefOrRef_Type_MethodDefinition: pFile->MethodImplementations[index].MethodBody.MethodDefinition = &pFile->MethodDefinitions[methodRow]; break;
        case MethodDefOrRef_Type_MemberReference: pFile->MethodImplementations[index].MethodBody.MemberReference = &pFile->MemberReferences[methodRow]; break;
        default: break;
        }

        if (pFile->MethodDefinitionCount > MethodDefOrRef_Type_MaxRows16Bit ||
            pFile->MemberReferenceCount > MethodDefOrRef_Type_MaxRows16Bit) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        methodTable = methodIndex & MethodDefOrRef_Type_Mask;
        methodRow = methodIndex >> MethodDefOrRef_Type_Bits;
        switch (methodTable)
        {
        case MethodDefOrRef_Type_MethodDefinition: pFile->MethodImplementations[index].MethodDeclaration.MethodDefinition = &pFile->MethodDefinitions[methodRow]; break;
        case MethodDefOrRef_Type_MemberReference: pFile->MethodImplementations[index].MethodDeclaration.MemberReference = &pFile->MemberReferences[methodRow]; break;
        default: break;
        }
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadModuleReferences(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ModuleReferenceCount; ++index)
    {
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ModuleReferences[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadTypeSpecifications(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->TypeSpecificationCount; ++index)
    {
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeSpecifications[index].Signature = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadImplementationMaps(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t memberForwardedIndex = 0;
    uint8_t memberForwardedTable = 0;
    uint32_t memberForwardedRow = 0;
    uint32_t importScopeIndex = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ImplementationMapCount; ++index)
    {
        pFile->ImplementationMaps[index].MappingFlags = *(uint16_t*)pTableData; pTableData += 2;

        if (pFile->FieldCount > MemberForwarded_Type_MaxRows16Bit ||
            pFile->MethodDefinitionCount > MemberForwarded_Type_MaxRows16Bit) { memberForwardedIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { memberForwardedIndex = *(uint16_t*)pTableData; pTableData += 2; }
        memberForwardedTable = memberForwardedIndex & MemberForwarded_Type_Mask;
        memberForwardedRow = memberForwardedIndex >> MemberForwarded_Type_Bits;
        switch (memberForwardedTable)
        {
        case MemberForwarded_Type_Field: pFile->ImplementationMaps[index].MemberForwarded.Field = &pFile->Fields[memberForwardedRow]; break;
        case MemberForwarded_Type_MethodDefinition: pFile->ImplementationMaps[index].MemberForwarded.MethodDefinition = &pFile->MethodDefinitions[memberForwardedRow]; break;
        default: break;
        }

        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ImplementationMaps[index].ImportName = (const char*)(pFile->StringsHeap + heapIndex);

        if (pFile->ModuleReferenceCount > 0xFFFF) { importScopeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { importScopeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ImplementationMaps[index].ImportScope = &pFile->ModuleReferences[importScopeIndex];
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadFieldRVAs(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t fieldIndex = 0;
    for (uint32_t index = 0; index < pFile->FieldRVACount; ++index)
    {
        pFile->FieldRVAs[index].VirtualAddress = *(uint32_t* )pTableData; pTableData += 4;

        if (pFile->FieldCount > 0xFFFF) { fieldIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->FieldRVAs[index].Field = &pFile->Fields[fieldIndex];
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadAssemblyDefinitions(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->AssemblyDefinitionCount; ++index)
    {
        pFile->AssemblyDefinitions[index].HashAlgorithmID = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyDefinitions[index].MajorVersion = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].MinorVersion = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].Build = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].Revision = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyDefinitions[index].PublicKey = pFile->BlobsHeap + heapIndex;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyDefinitions[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyDefinitions[index].Culture = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadAssemblyProcessors(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0; index < pFile->AssemblyProcessorCount; ++index)
    {
        pFile->AssemblyProcessors[index].Processor = *(uint32_t* )pTableData; pTableData += 4;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadAssemblyOperatingSystems(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0; index < pFile->AssemblyOperatingSystemCount; ++index)
    {
        pFile->AssemblyOperatingSystems[index].PlatformID = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyOperatingSystems[index].MajorVersion = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyOperatingSystems[index].MinorVersion = *(uint32_t* )pTableData; pTableData += 4;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadAssemblyReferences(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->AssemblyReferenceCount; ++index)
    {
        pFile->AssemblyReferences[index].MajorVersion = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyReferences[index].MinorVersion = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyReferences[index].Build = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyReferences[index].Revision = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyReferences[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyReferences[index].PublicKeyOrToken = pFile->BlobsHeap + heapIndex;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyReferences[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyReferences[index].Culture = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyReferences[index].HashValue = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadAssemblyReferenceProcessors(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t assemblyReferenceIndex = 0;
    for (uint32_t index = 0; index < pFile->AssemblyReferenceProcessorCount; ++index)
    {
        pFile->AssemblyReferenceProcessors[index].Processor = *(uint32_t* )pTableData; pTableData += 4;
        if (pFile->AssemblyReferenceCount > 0xFFFF) { assemblyReferenceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { assemblyReferenceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyReferenceProcessors[index].AssemblyReference = &pFile->AssemblyReferences[assemblyReferenceIndex];
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadAssemblyReferenceOperatingSystems(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t assemblyReferenceIndex = 0;
    for (uint32_t index = 0; index < pFile->AssemblyReferenceOperatingSystemCount; ++index)
    {
        pFile->AssemblyReferenceOperatingSystems[index].PlatformID = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyReferenceOperatingSystems[index].MajorVersion = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyReferenceOperatingSystems[index].MinorVersion = *(uint32_t* )pTableData; pTableData += 4;
        if (pFile->AssemblyReferenceCount > 0xFFFF) { assemblyReferenceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { assemblyReferenceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyReferenceOperatingSystems[index].AssemblyReference = &pFile->AssemblyReferences[assemblyReferenceIndex];
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadFiles(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->FileCount; ++index)
    {
        pFile->Files[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Files[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Files[index].HashValue = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadExportedTypes(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t typeDefinitionIndex = 0;
    uint32_t implementationIndex = 0;
    uint8_t implementationTable = 0;
    uint32_t implementationRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ExportedTypeCount; ++index)
    {
        pFile->ExportedTypes[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if (pFile->TypeDefinitionCount > 0xFFFF) { typeDefinitionIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { typeDefinitionIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ExportedTypes[index].TypeDefinitionID = &pFile->TypeDefinitions[typeDefinitionIndex];
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ExportedTypes[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ExportedTypes[index].Namespace = (const char*)(pFile->StringsHeap + heapIndex);

        if (pFile->FileCount > Implementation_Type_MaxRows16Bit ||
            pFile->AssemblyReferenceCount > Implementation_Type_MaxRows16Bit ||
            pFile->ExportedTypeCount > Implementation_Type_MaxRows16Bit) { implementationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        implementationTable = implementationIndex & Implementation_Type_Mask;
        implementationRow = implementationIndex >> Implementation_Type_Bits;
        switch (implementationTable)
        {
        case Implementation_Type_File: pFile->ExportedTypes[index].Implementation.File = &pFile->Files[implementationRow]; break;
        case Implementation_Type_AssemblyReference: pFile->ExportedTypes[index].Implementation.AssemblyReference = &pFile->AssemblyReferences[implementationRow]; break;
        case Implementation_Type_ExportedType: pFile->ExportedTypes[index].Implementation.ExportedType = &pFile->ExportedTypes[implementationRow]; break;
        default: break;
        }
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadManifestResources(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t implementationIndex = 0;
    uint8_t implementationTable = 0;
    uint32_t implementationRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ManifestResourceCount; ++index)
    {
        pFile->ManifestResources[index].Offset = *(uint32_t* )pTableData; pTableData += 4;
        pFile->ManifestResources[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ManifestResources[index].Name = (const char*)(pFile->StringsHeap + heapIndex);

        if (pFile->FileCount > Implementation_Type_MaxRows16Bit ||
            pFile->AssemblyReferenceCount > Implementation_Type_MaxRows16Bit ||
            pFile->ExportedTypeCount > Implementation_Type_MaxRows16Bit) { implementationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        implementationTable = implementationIndex & Implementation_Type_Mask;
        implementationRow = implementationIndex >> Implementation_Type_Bits;
        switch (implementationTable)
        {
        case Implementation_Type_File: pFile->ExportedTypes[index].Implementation.File = &pFile->Files[implementationRow]; break;
        case Implementation_Type_AssemblyReference: pFile->ExportedTypes[index].Implementation.AssemblyReference = &pFile->AssemblyReferences[implementationRow]; break;
        case Implementation_Type_ExportedType: pFile->ExportedTypes[index].Implementation.ExportedType = &pFile->ExportedTypes[implementationRow]; break;
        default: break;
        }
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadNestedClasses(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t nestedIndex = 0;
    uint32_t enclosingIndex = 0;
    for (uint32_t index = 0; index < pFile->NestedClassCount; ++index)
    {
        if (pFile->TypeDefinitionCount > 0xFFFF) { nestedIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { nestedIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->NestedClasses[index].Nested = &pFile->TypeDefinitions[nestedIndex];
        if (pFile->TypeDefinitionCount > 0xFFFF) { enclosingIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { enclosingIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->NestedClasses[index].Enclosing = &pFile->TypeDefinitions[enclosingIndex];
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadGenericParameters(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t ownerIndex = 0;
    uint8_t ownerTable = 0;
    uint32_t ownerRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->GenericParameterCount; ++index)
    {
        pFile->GenericParameters[index].Index = *(uint16_t* )pTableData; pTableData += 2;
        pFile->GenericParameters[index].Flags = *(uint16_t* )pTableData; pTableData += 2;
        if (pFile->TypeDefinitionCount > TypeOrMethodDef_Type_MaxRows16Bit ||
            pFile->MethodDefinitionCount > TypeOrMethodDef_Type_MaxRows16Bit) { ownerIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { ownerIndex = *(uint16_t*)pTableData; pTableData += 2; }
        ownerTable = ownerIndex & TypeOrMethodDef_Type_Mask;
        ownerRow = ownerIndex >> TypeOrMethodDef_Type_Bits;
        switch (ownerTable)
        {
        case TypeOrMethodDef_Type_TypeDefinition: pFile->GenericParameters[index].Owner.TypeDefinition = &pFile->TypeDefinitions[ownerRow]; break;
        case TypeOrMethodDef_Type_MethodDefinition: pFile->GenericParameters[index].Owner.MethodDefinition = &pFile->MethodDefinitions[ownerRow]; break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->GenericParameters[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadMethodSpecifications(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t methodIndex = 0;
    uint8_t methodTable = 0;
    uint32_t methodRow = 0;
    for (uint32_t index = 0, heapIndex = 0; index < pFile->MethodSpecificationCount; ++index)
    {
        if (pFile->MethodDefinitionCount > MethodDefOrRef_Type_MaxRows16Bit ||
            pFile->MemberReferenceCount > MethodDefOrRef_Type_MaxRows16Bit) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        methodTable = methodIndex & MethodDefOrRef_Type_Mask;
        methodRow = methodIndex >> MethodDefOrRef_Type_Bits;
        switch (methodTable)
        {
        case MethodDefOrRef_Type_MethodDefinition: pFile->MethodSpecifications[index].Method.MethodDefinition = &pFile->MethodDefinitions[methodRow]; break;
        case MethodDefOrRef_Type_MemberReference: pFile->MethodSpecifications[index].Method.MemberReference = &pFile->MemberReferences[methodRow]; break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodSpecifications[index].Instantiation = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}

const uint8_t* CLIFile_LoadGenericParameterConstraints(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t ownerIndex = 0;
    uint32_t constraintIndex = 0;
    uint8_t constraintTable = 0;
    uint32_t constraintRow = 0;
    for (uint32_t index = 0; index < pFile->GenericParameterConstraintCount; ++index)
    {
        if (pFile->GenericParameterCount > 0xFFFF) { ownerIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { ownerIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->GenericParameterConstraints[index].Owner = &pFile->GenericParameters[ownerIndex];
        if (pFile->TypeDefinitionCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefOrRef_Type_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefOrRef_Type_MaxRows16Bit) { constraintIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { constraintIndex = *(uint16_t*)pTableData; pTableData += 2; }
        constraintTable = constraintIndex & TypeDefOrRef_Type_Mask;
        constraintRow = constraintIndex >> TypeDefOrRef_Type_Bits;
        switch (constraintTable)
        {
        case TypeDefOrRef_Type_TypeDefinition: pFile->GenericParameterConstraints[index].Constraint.TypeDefinition = &pFile->TypeDefinitions[constraintRow]; break;
        case TypeDefOrRef_Type_TypeReference: pFile->GenericParameterConstraints[index].Constraint.TypeReference = &pFile->TypeReferences[constraintRow]; break;
        case TypeDefOrRef_Type_TypeSpecification: pFile->GenericParameterConstraints[index].Constraint.TypeSpecification = &pFile->TypeSpecifications[constraintRow]; break;
        default: break;
        }
    }
    return pTableData;
}
