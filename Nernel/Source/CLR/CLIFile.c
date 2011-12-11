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
    if ((cliFile->TablesHeader->PresentTables & (1ull << MetaData_Table_GenericParameterConstraint)) != 0)
    {
        cliFile->GenericParameterConstraintCount = *(uint32_t*)tableData;
        cliFile->GenericParameterConstraints = (GenericParameterConstraint*)calloc(cliFile->GenericParameterConstraintCount, sizeof(GenericParameterConstraint));
        tableData += 4;
    }

    printf("Test: %u\n", *(unsigned int*)tableData);

    tableData = CLIFile_LoadModuleDefinitions(cliFile, tableData);
    printf("Loaded %u ModuleDefinitions\n", (unsigned int)cliFile->ModuleDefinitionCount);
    tableData = CLIFile_LoadTypeReferences(cliFile, tableData);
    printf("Loaded %u TypeReferences\n", (unsigned int)cliFile->TypeReferenceCount);
    tableData = CLIFile_LoadTypeDefinitions(cliFile, tableData);
    printf("Loaded %u TypeDefinitions\n", (unsigned int)cliFile->TypeDefinitionCount);
    tableData = CLIFile_LoadFields(cliFile, tableData);
    printf("Loaded %u Fields\n", (unsigned int)cliFile->FieldCount);

    printf("Loaded Assembly\n");
    return cliFile;
}

void CLIFile_Destroy(CLIFile* pFile)
{
    free(pFile);
}

const char* CLIFile_GetString(CLIFile* pFile, uint32_t pVirtualAddress) { return (const char*)(pFile->StringsHeap + pVirtualAddress); }
const uint8_t* CLIFile_GetUserString(CLIFile* pFile, uint32_t pVirtualAddress) { return (const uint8_t*)(pFile->UserStringsHeap + pVirtualAddress); }
const uint8_t* CLIFile_GetBlob(CLIFile* pFile, uint32_t pVirtualAddress) { return (const uint8_t*)(pFile->BlobsHeap + pVirtualAddress); }
const uint8_t* CLIFile_GetGUID(CLIFile* pFile, uint32_t pVirtualAddress) { return (const uint8_t*)(pFile->GUIDsHeap + pVirtualAddress); }

const uint8_t* CLIFile_LoadModuleDefinitions(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 0, heapIndex = 0; index < pFile->ModuleDefinitionCount; ++index)
    {
        pFile->ModuleDefinitions[index].Generation = *(uint16_t*)pTableData; pTableData += 2;
        printf("%u\n", *(uint16_t*)pTableData);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ModuleDefinitions[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ModuleDefinitions[index].ModuleVersionID = pFile->GUIDsHeap + heapIndex;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
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
        resolutionScopeIndex = *(uint16_t*)pTableData; pTableData += 2;
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
        extendsIndex = *(uint16_t*)pTableData; pTableData += 2;
        extendsTable = extendsIndex & TypeDefOrRef_Type_Mask;
        extendsRow = extendsIndex >> TypeDefOrRef_Type_Bits;
        switch (extendsTable)
        {
        case TypeDefOrRef_Type_TypeDefinition: pFile->TypeDefinitions[index].Extends.TypeDefinition = &pFile->TypeDefinitions[extendsRow]; break;
        case TypeDefOrRef_Type_TypeReference: pFile->TypeDefinitions[index].Extends.TypeReference = &pFile->TypeReferences[extendsRow]; break;
        case TypeDefOrRef_Type_TypeSpecification: pFile->TypeDefinitions[index].Extends.TypeSpecification = &pFile->TypeSpecifications[extendsRow]; break;
        default: break;
        }
        fieldListIndex = *(uint16_t*)pTableData; pTableData += 2;
        pFile->TypeDefinitions[index].FieldList = &pFile->Fields[fieldListIndex];
        methodDefinitionListIndex = *(uint16_t*)pTableData; pTableData += 2;
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
        //if (index < 20) printf("%u, %s\n", (unsigned int)heapIndex, pFile->Fields[index].Name);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Fields[index].Signature = pFile->BlobsHeap + heapIndex;
    }
    return pTableData;
}
