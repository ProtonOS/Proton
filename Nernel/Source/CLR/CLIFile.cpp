#include "CLIFile.h"
#include "Assembly.h"

CLR::CLIFile::HandlerEntry CLR::CLIFile::sHandlers[] =
{
	{ &CLIFile::ModuleDefinitionInitializer, &CLIFile::ModuleDefinitionLoader },
	{ &CLIFile::TypeReferenceInitializer, &CLIFile::TypeReferenceLoader },
	{ &CLIFile::TypeDefinitionInitializer, &CLIFile::TypeDefinitionLoader },
	{ &CLIFile::FieldInitializer, &CLIFile::FieldLoader },
	{ &CLIFile::MethodDefinitionInitializer, &CLIFile::MethodDefinitionLoader },
	{ &CLIFile::ParameterInitializer, &CLIFile::ParameterLoader },
	{ &CLIFile::InterfaceImplementationInitializer, &CLIFile::InterfaceImplementationLoader },
	{ &CLIFile::MemberReferenceInitializer, &CLIFile::MemberReferenceLoader },
	{ &CLIFile::ConstantInitializer, &CLIFile::ConstantLoader },
	{ &CLIFile::CustomAttributeInitializer, &CLIFile::CustomAttributeLoader },
	{ &CLIFile::FieldMarshalInitializer, &CLIFile::FieldMarshalLoader },
	{ &CLIFile::DeclSecurityInitializer, &CLIFile::DeclSecurityLoader },
	{ &CLIFile::ClassLayoutInitializer, &CLIFile::ClassLayoutLoader },
	{ &CLIFile::FieldLayoutInitializer, &CLIFile::FieldLayoutLoader },
	{ &CLIFile::StandAloneSignatureInitializer, &CLIFile::StandAloneSignatureLoader },
	{ &CLIFile::EventMapInitializer, &CLIFile::EventMapLoader },
	{ &CLIFile::EventInitializer, &CLIFile::EventLoader },
	{ &CLIFile::PropertyMapInitializer, &CLIFile::PropertyMapLoader },
	{ &CLIFile::PropertyInitializer, &CLIFile::PropertyLoader },
	{ &CLIFile::MethodSemanticsInitializer, &CLIFile::MethodSemanticsLoader },
	{ &CLIFile::MethodImplementationInitializer, &CLIFile::MethodImplementationLoader },
	{ &CLIFile::ModuleReferenceInitializer, &CLIFile::ModuleReferenceLoader },
	{ &CLIFile::TypeSpecificationInitializer, &CLIFile::TypeSpecificationLoader },
	{ &CLIFile::ImplementationMapInitializer, &CLIFile::ImplementationMapLoader },
	{ &CLIFile::FieldRVAInitializer, &CLIFile::FieldRVALoader },
	{ &CLIFile::AssemblyDefinitionInitializer, &CLIFile::AssemblyDefinitionLoader },
	{ &CLIFile::AssemblyProcessorInitializer, &CLIFile::AssemblyProcessorLoader },
	{ &CLIFile::AssemblyOperatingSystemInitializer, &CLIFile::AssemblyOperatingSystemLoader },
	{ &CLIFile::AssemblyReferenceInitializer, &CLIFile::AssemblyReferenceLoader },
	{ &CLIFile::AssemblyReferenceProcessorInitializer, &CLIFile::AssemblyReferenceProcessorLoader },
	{ &CLIFile::AssemblyReferenceOperatingSystemInitializer, &CLIFile::AssemblyReferenceOperatingSystemLoader },
	{ &CLIFile::FileInitializer, &CLIFile::FileLoader },
	{ &CLIFile::ExportedTypeInitializer, &CLIFile::ExportedTypeLoader },
	{ &CLIFile::ManifestResourceInitializer, &CLIFile::ManifestResourceLoader },
	{ &CLIFile::NestedClassInitializer, &CLIFile::NestedClassLoader },
	{ &CLIFile::GenericParameterInitializer, &CLIFile::GenericParameterLoader },
	{ &CLIFile::MethodSpecificationInitializer, &CLIFile::MethodSpecificationLoader },
	{ &CLIFile::GenericParameterConstraintInitializer, &CLIFile::GenericParameterConstraintLoader }
};

CLR::CLIFile::CLIFile()
{
	Data = nullptr;
	Length = 0;
	Filename = nullptr;

	DOSHeader = nullptr;
	NTHeader = nullptr;
	OptionalHeader = nullptr;
	SectionHeaders = nullptr;
	CLIHeader = nullptr;
	MetadataHeader = nullptr;
	Tables = nullptr;
	StringsHeap = nullptr;
	UserStringsHeap = nullptr;
	BlobsHeap = nullptr;
	GUIDsHeap = nullptr;
	TablesHeader = nullptr;

	LoadedAssembly = nullptr;

	ModuleDefinitionCount = 0;
	ModuleDefinitions = nullptr;
	TypeReferenceCount = 0;
	TypeReferences = nullptr;
	TypeDefinitionCount = 0;
	TypeDefinitions = nullptr;
	FieldCount = 0;
	Fields = nullptr;
	MethodDefinitionCount = 0;
	MethodDefinitions = nullptr;
	ParameterCount = 0;
	Parameters = nullptr;
	InterfaceImplementationCount = 0;
	InterfaceImplementations = nullptr;
	MemberReferenceCount = 0;
	MemberReferences = nullptr;
	ConstantCount = 0;
	Constants = nullptr;
	CustomAttributeCount = 0;
	CustomAttributes = nullptr;
	FieldMarshalCount = 0;
	FieldMarshals = nullptr;
	DeclSecurityCount = 0;
	DeclSecurities = nullptr;
	ClassLayoutCount = 0;
	ClassLayouts = nullptr;
	FieldLayoutCount = 0;
	FieldLayouts = nullptr;
	StandAloneSignatureCount = 0;
	StandAloneSignatures = nullptr;
	EventMapCount = 0;
	EventMaps = nullptr;
	EventCount = 0;
	Events = nullptr;
	PropertyMapCount = 0;
	PropertyMaps = nullptr;
	PropertyCount = 0;
	Properties = nullptr;
	MethodSemanticsCount = 0;
	MethodSemantics = nullptr;
	MethodImplementationCount = 0;
	MethodImplementations = nullptr;
	ModuleReferenceCount = 0;
	ModuleReferences = nullptr;
	TypeSpecificationCount = 0;
	TypeSpecifications = nullptr;
	ImplementationMapCount = 0;
	ImplementationMaps = nullptr;
	FieldRVACount = 0;
	FieldRVAs = nullptr;
	AssemblyDefinitionCount = 0;
	AssemblyDefinitions = nullptr;
	AssemblyProcessorCount = 0;
	AssemblyProcessors = nullptr;
	AssemblyOperatingSystemCount = 0;
	AssemblyOperatingSystems = nullptr;
	AssemblyReferenceCount = 0;
	AssemblyReferences = nullptr;
	AssemblyReferenceProcessorCount = 0;
	AssemblyReferenceProcessors = nullptr;
	AssemblyReferenceOperatingSystemCount = 0;
	AssemblyReferenceOperatingSystems = nullptr;
	FileCount = 0;
	Files = nullptr;
	ExportedTypeCount = 0;
	ExportedTypes = nullptr;
	ManifestResourceCount = 0;
	ManifestResources = nullptr;
	NestedClassCount = 0;
	NestedClasses = nullptr;
	GenericParameterCount = 0;
	GenericParameters = nullptr;
	GenericParameterConstraintCount = 0;
	GenericParameterConstraints = nullptr;
	MethodSpecificationCount = 0;
	MethodSpecifications = nullptr;
}

CLR::CLIFile::~CLIFile()
{
	if (Filename) delete [] Filename;
	if (LoadedAssembly) delete LoadedAssembly;

	if (ModuleDefinitions) delete [] ModuleDefinitions;
	if (TypeReferences) delete [] TypeReferences;
	if (TypeDefinitions) delete [] TypeDefinitions;
	if (Fields) delete [] Fields;
	if (MethodDefinitions) delete [] MethodDefinitions;
	if (Parameters) delete [] Parameters;
	if (InterfaceImplementations) delete [] InterfaceImplementations;
	if (MemberReferences) delete [] MemberReferences;
	if (Constants) delete [] Constants;
	if (CustomAttributes) delete [] CustomAttributes;
	if (FieldMarshals) delete [] FieldMarshals;
	if (DeclSecurities) delete [] DeclSecurities;
	if (ClassLayouts) delete [] ClassLayouts;
	if (FieldLayouts) delete [] FieldLayouts;
	if (StandAloneSignatures) delete [] StandAloneSignatures;
	if (EventMaps) delete [] EventMaps;
	if (Events) delete [] Events;
	if (PropertyMaps) delete [] PropertyMaps;
	if (Properties) delete [] Properties;
	if (MethodSemantics) delete [] MethodSemantics;
	if (MethodImplementations) delete [] MethodImplementations;
	if (ModuleReferences) delete [] ModuleReferences;
	if (TypeSpecifications) delete [] TypeSpecifications;
	if (ImplementationMaps) delete [] ImplementationMaps;
	if (FieldRVAs) delete [] FieldRVAs;
	if (AssemblyDefinitions) delete [] AssemblyDefinitions;
	if (AssemblyProcessors) delete [] AssemblyProcessors;
	if (AssemblyOperatingSystems) delete [] AssemblyOperatingSystems;
	if (AssemblyReferences) delete [] AssemblyReferences;
	if (AssemblyReferenceProcessors) delete [] AssemblyReferenceProcessors;
	if (AssemblyReferenceOperatingSystems) delete [] AssemblyReferenceOperatingSystems;
	if (Files) delete [] Files;
	if (ExportedTypes) delete [] ExportedTypes;
	if (ManifestResources) delete [] ManifestResources;
	if (NestedClasses) delete [] NestedClasses;
	if (GenericParameters) delete [] GenericParameters;
	if (GenericParameterConstraints) delete [] GenericParameterConstraints;
	if (MethodSpecifications) delete [] MethodSpecifications;
}

bool CLR::CLIFile::Load(uint8_t* pData, uint32_t pLength, const char* pFilename)
{
    PEDOSHeader* dosHeader = (PEDOSHeader*)pData;
    if (dosHeader->Signature != SIGNATURE_DOS) return false;
    PENTHeader* ntHeader = (PENTHeader*)(pData + dosHeader->NextHeaderOffset);
    if (ntHeader->Signature != SIGNATURE_NT) return false;
    if (ntHeader->Machine != SIGNATURE_MACHINE) return false;
    PEOptionalHeader* peOptionalHeader = (PEOptionalHeader*)(pData + dosHeader->NextHeaderOffset + sizeof(PENTHeader));
    PESectionHeader* peSectionHeaders = (PESectionHeader*)(pData + dosHeader->NextHeaderOffset + sizeof(PENTHeader) + sizeof(PEOptionalHeader));

    PEDataDirectory* cliHeaderDataDirectory = &peOptionalHeader->DataDirectory[14];
    PESectionHeader* cliHeaderSectionHeader = GetSection(peSectionHeaders, ntHeader->NumberOfSections, cliHeaderDataDirectory->VirtualAddress);
    PECLIHeader* cliHeader = (PECLIHeader*)(pData + cliHeaderSectionHeader->PointerToRawData + (cliHeaderDataDirectory->VirtualAddress - cliHeaderSectionHeader->VirtualAddress));

    PESectionHeader* cliMetadataSectionHeader = GetSection(peSectionHeaders, ntHeader->NumberOfSections, cliHeader->Metadata.VirtualAddress);
    uint8_t* cliMetadataHeader = pData + cliMetadataSectionHeader->PointerToRawData + (cliHeader->Metadata.VirtualAddress - cliMetadataSectionHeader->VirtualAddress);

	if (*(uint32_t*)(cliMetadataHeader + METADATA_HEADEROFFSET_SIGNATURE) != SIGNATURE_METADATA) return false;

	Data = pData;
	Length = pLength;
	Filename = new char[strlen(pFilename) + 1];
	strcpy(Filename, pFilename);
	DOSHeader = dosHeader;
	NTHeader = ntHeader;
	OptionalHeader = peOptionalHeader;
	SectionHeaders = peSectionHeaders;
	CLIHeader = cliHeader;
	MetadataHeader = cliMetadataHeader;

	uint32_t versionLength = *(uint32_t*)(cliMetadataHeader + METADATA_HEADEROFFSET_VERSIONLENGTH);
	uint16_t streamCount = *(uint16_t*)(cliMetadataHeader + METADATA_HEADEROFFSET_VERSION + versionLength + METADATA_HEADERVERSIONOFFSET_STREAMCOUNT);
	uint8_t* streamStart  = cliMetadataHeader + METADATA_HEADEROFFSET_VERSION + versionLength + METADATA_HEADERVERSIONOFFSET_STREAMS;
	uint8_t* streamIterator = streamStart;
    for (uint16_t streamIndex = 0; streamIndex < streamCount; ++streamIndex)
    {
        char* streamName = (char*)(streamIterator + METADATA_STREAMOFFSET_NAME);
        uint8_t* streamData = cliMetadataHeader + *(uint32_t*)(streamIterator + METADATA_STREAMOFFSET_RVA);

        if (!strcmp(streamName, "#~")) Tables = streamData;
        else if (!strcmp(streamName, "#Strings")) StringsHeap = streamData;
        else if (!strcmp(streamName, "#US")) UserStringsHeap = streamData;
        else if (!strcmp(streamName, "#Blob")) BlobsHeap = streamData;
        else if (!strcmp(streamName, "#GUID")) GUIDsHeap = streamData;

        streamIterator += METADATA_STREAMOFFSET_NAME;
        while (*streamIterator) ++streamIterator;
		streamIterator += 4 - ((unsigned int)streamIterator & 0x03);
	}

    TablesHeader = (PECLIMetadataTablesHeader*)Tables;

	printf("CLIFile TablesHeader->PresentTables = 0x%llX\n", (unsigned long long)TablesHeader->PresentTables);
    uint8_t* tableData = Tables + sizeof(PECLIMetadataTablesHeader);
	for (HandlerEntry entry : sHandlers)
	{
		tableData = (this->*entry.Initializer)(tableData);
	}
	for (HandlerEntry entry : sHandlers)
	{
		tableData = (this->*entry.Loader)(tableData);
	}

	Link();
	return true;
}

CLR::PESectionHeader* CLR::CLIFile::GetSection(PESectionHeader* pSections, uint16_t pSectionCount, uint32_t pVirtualAddress)
{
    PESectionHeader* section = pSections;
	uint16_t sectionCount = pSectionCount;
    while (sectionCount > 0)
    {
        if (pVirtualAddress >= section->VirtualAddress && pVirtualAddress < (section->VirtualAddress + section->Misc.VirtualSize)) return section;
        ++section;
        --sectionCount;
    }
    return nullptr;
}

uint8_t* CLR::CLIFile::GetCompressedUnsigned(uint8_t* pData, uint32_t* pValue)
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

int32_t CLR::CLIFile::RotateRight(int32_t pValue, uint8_t pBits)
{
    bool bit = (pValue & 0x01) != 0;
    pValue >>= 1;
    if (bit) pValue |= (1 << (pBits - 1));
    return pValue;
}

uint8_t* CLR::CLIFile::GetCompressedSigned(uint8_t* pData, int32_t* pValue)
{
    if ((*pData & 0x80) == 0)
    {
        *pValue = pData[0] & 0x7F;
        *pValue = RotateRight(*pValue, 7);
        if ((*pValue & 0x40) != 0) *pValue |= 0xFFFFFF80;
        return pData + 1;
    }
    if ((*pData & 0xC0) == 0x80)
    {
        *pValue = ((pData[0] & 0x3F) << 8) + pData[1];
        *pValue = RotateRight(*pValue, 14);
        if ((*pValue & 0x2000) != 0) *pValue |= 0xFFFFC000;
        return pData + 2;
    }
    if ((*pData & 0xE0) == 0xC0)
    {
        *pValue = ((pData[0] & 0x1F) << 24) + (pData[1] << 16) + (pData[2] << 8) + pData[3];
        *pValue = RotateRight(*pValue, 29);
        if ((*pValue & 0x10000000) != 0) *pValue |= 0xE0000000;
        return pData + 4;
    }
    *pValue = 0;
    return pData;
}

uint8_t* CLR::CLIFile::ModuleDefinitionInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::ModuleDefinition)) != 0)
    {
        ModuleDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        ModuleDefinitions = new ModuleDefinitionData[ModuleDefinitionCount + 1];
    }
	printf("CLIFile ModuleDefinitionCount = %u\n", (unsigned int)ModuleDefinitionCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::ModuleDefinitionLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= ModuleDefinitionCount; ++index)
    {
        ModuleDefinitions[index].TableIndex = index;
        ModuleDefinitions[index].Generation = *(uint16_t*)pTableData; 
		pTableData += 2;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0)  { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else  { heapIndex = *((uint16_t*)pTableData); pTableData += 2; }
        ModuleDefinitions[index].Name = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2;  }
        ModuleDefinitions[index].ModuleVersionID = GUIDsHeap + heapIndex;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2;  }
        ModuleDefinitions[index].EncID = GUIDsHeap + heapIndex;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        ModuleDefinitions[index].EncBaseID = GUIDsHeap + heapIndex;
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::TypeReferenceInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::TypeReference)) != 0)
    {
        TypeReferenceCount = *(uint32_t*)pTableData; pTableData += 4;
        TypeReferences = new TypeReferenceData[TypeReferenceCount + 1];
    }
	printf("CLIFile TypeReferenceCount = %u\n", (unsigned int)TypeReferenceCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::TypeReferenceLoader(uint8_t* pTableData)
{
    uint32_t resolutionScopeIndex = 0;
    uint32_t resolutionScopeRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= TypeReferenceCount; ++index)
    {
        TypeReferences[index].TableIndex = index;
        if (ModuleDefinitionCount > ResolutionScopeType::MAXROWS16BIT ||
            ModuleReferenceCount > ResolutionScopeType::MAXROWS16BIT ||
            AssemblyReferenceCount > ResolutionScopeType::MAXROWS16BIT ||
            TypeReferenceCount > ResolutionScopeType::MAXROWS16BIT) { resolutionScopeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { resolutionScopeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        TypeReferences[index].TypeOfResolutionScope = resolutionScopeIndex & ResolutionScopeType::MASK;
        resolutionScopeRow = resolutionScopeIndex >> ResolutionScopeType::BITS;
        switch (TypeReferences[index].TypeOfResolutionScope)
        {
		case ResolutionScopeType::ModuleDefinition:
            if (resolutionScopeRow == 0 || resolutionScopeRow > ModuleDefinitionCount) Panic("TypeReferenceLoader ModuleDefinition");
            TypeReferences[index].ResolutionScope.ModuleDefinition = &ModuleDefinitions[resolutionScopeRow];
            break;
		case ResolutionScopeType::ModuleReference:
            if (resolutionScopeRow == 0 || resolutionScopeRow > ModuleReferenceCount) Panic("TypeReferenceLoader ModuleReference");
            TypeReferences[index].ResolutionScope.ModuleReference = &ModuleReferences[resolutionScopeRow];
            break;
		case ResolutionScopeType::AssemblyReference:
            if (resolutionScopeRow == 0 || resolutionScopeRow > AssemblyReferenceCount) Panic("TypeReferenceLoader AssemblyReference");
            TypeReferences[index].ResolutionScope.AssemblyReference = &AssemblyReferences[resolutionScopeRow];
            break;
		case ResolutionScopeType::TypeReference:
            if (resolutionScopeRow == 0 || resolutionScopeRow > TypeReferenceCount) Panic("TypeReferenceLoader TypeReference");
            TypeReferences[index].ResolutionScope.TypeReference = &TypeReferences[resolutionScopeRow];
            break;
        default: break;
        }
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        TypeReferences[index].Name = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        TypeReferences[index].Namespace = (const char*)(StringsHeap + heapIndex);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::TypeDefinitionInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::TypeDefinition)) != 0)
    {
        TypeDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        TypeDefinitions = new TypeDefinitionData[TypeDefinitionCount + 1];
    }
	printf("CLIFile TypeDefinitionCount = %u\n", (unsigned int)TypeDefinitionCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::TypeDefinitionLoader(uint8_t* pTableData)
{
    uint32_t extendsIndex = 0;
    uint32_t extendsRow = 0;
    uint32_t fieldListIndex = 0;
    uint32_t methodDefinitionListIndex = 0;
    uint32_t* fieldListIndexes = new uint32_t[TypeDefinitionCount + 1];
    uint32_t* methodDefinitionListIndexes = new uint32_t[TypeDefinitionCount + 1];
    for (uint32_t index = 1, heapIndex = 0; index <= TypeDefinitionCount; ++index)
    {
		TypeDefinitions[index].File = this;
        TypeDefinitions[index].TableIndex = index;
        TypeDefinitions[index].Flags = *(uint32_t*)pTableData; pTableData += 4;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        TypeDefinitions[index].Name = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        TypeDefinitions[index].Namespace = (const char*)(StringsHeap + heapIndex);
        if (TypeDefinitionCount > TypeDefRefOrSpecType::MAXROWS16BIT ||
            TypeReferenceCount > TypeDefRefOrSpecType::MAXROWS16BIT ||
            TypeSpecificationCount > TypeDefRefOrSpecType::MAXROWS16BIT) { extendsIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { extendsIndex = *(uint16_t*)pTableData; pTableData += 2; }
        TypeDefinitions[index].TypeOfExtends = extendsIndex & TypeDefRefOrSpecType::MASK;
        extendsRow = extendsIndex >> TypeDefRefOrSpecType::BITS;
        switch (TypeDefinitions[index].TypeOfExtends)
        {
		case TypeDefRefOrSpecType::TypeDefinition:
            if (extendsRow > TypeDefinitionCount) Panic("TypeDefinitionLoader TypeDefinition");
            if (extendsRow > 0) TypeDefinitions[index].Extends.TypeDefinition = &TypeDefinitions[extendsRow];
            break;
		case TypeDefRefOrSpecType::TypeReference:
            if (extendsRow > TypeReferenceCount) Panic("TypeDefinitionLoader TypeReference");
            if (extendsRow > 0) TypeDefinitions[index].Extends.TypeReference = &TypeReferences[extendsRow];
            break;
		case TypeDefRefOrSpecType::TypeSpecification:
            if (extendsRow > TypeSpecificationCount) Panic("TypeDefinitionLoader TypeSpecification");
            if (extendsRow > 0) TypeDefinitions[index].Extends.TypeSpecification = &TypeSpecifications[extendsRow];
            break;
        default: break;
        }
        if (FieldCount > 0xFFFF) { fieldListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (fieldListIndex == 0 || fieldListIndex > FieldCount + 1) Panic("TypeDefinitionLoader Field");
        if (fieldListIndex <= FieldCount)
        {
			//printf("TypeDef Field Link: %s.%s, start @ %u\n", pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, fieldListIndex);
			//printf("Field List: 0x%x\n", (unsigned int)&pFile->Fields[fieldListIndex]);
            TypeDefinitions[index].FieldList = &Fields[fieldListIndex];
            fieldListIndexes[index] = fieldListIndex;
        }
        if (MethodDefinitionCount > 0xFFFF) { methodDefinitionListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodDefinitionListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (methodDefinitionListIndex == 0 || methodDefinitionListIndex > MethodDefinitionCount + 1) Panic("TypeDefinitionLoader MethodDefinition");
        if (methodDefinitionListIndex <= MethodDefinitionCount)
        {
            TypeDefinitions[index].MethodDefinitionList = &MethodDefinitions[methodDefinitionListIndex];
            methodDefinitionListIndexes[index] = methodDefinitionListIndex;
        }
    }
    uint32_t fieldListCount = 0;
    for (uint32_t index = 1, used = 0; index <= TypeDefinitionCount; ++index, used += fieldListCount)
    {
        if (index == TypeDefinitionCount || fieldListIndexes[index + 1] == 0) fieldListCount = FieldCount - used;
        else fieldListCount = fieldListIndexes[index + 1] - fieldListIndexes[index];
        TypeDefinitions[index].FieldListCount = fieldListCount;
		//printf("TypeDef: %s.%s, Owns %u Fields starting @ %u\n", pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, fieldListCount, fieldListIndexes[index]);
    }
	delete [] fieldListIndexes;
    uint32_t methodDefinitionListCount = 0;
    for (uint32_t index = 1, used = 0; index <= TypeDefinitionCount; ++index, used += methodDefinitionListCount)
    {
        if (index == TypeDefinitionCount || methodDefinitionListIndexes[index + 1] == 0) methodDefinitionListCount = MethodDefinitionCount - used;
        else methodDefinitionListCount = methodDefinitionListIndexes[index + 1] - methodDefinitionListIndexes[index];
        TypeDefinitions[index].MethodDefinitionListCount = methodDefinitionListCount;
    }
	delete [] methodDefinitionListIndexes;
	return pTableData;
}

uint8_t* CLR::CLIFile::FieldInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::Field)) != 0)
    {
        FieldCount = *(uint32_t*)pTableData; pTableData += 4;
        Fields = new FieldData[FieldCount + 1];
    }
	printf("CLIFile FieldCount = %u\n", (unsigned int)FieldCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::FieldLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= FieldCount; ++index)
    {
		Fields[index].File = this;
        Fields[index].TableIndex = index;
        Fields[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        Fields[index].Name = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        Fields[index].Signature = GetCompressedUnsigned(BlobsHeap + heapIndex, &Fields[index].SignatureLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::MethodDefinitionInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::MethodDefinition)) != 0)
    {
        MethodDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        MethodDefinitions = new MethodDefinitionData[MethodDefinitionCount + 1];
    }
	printf("CLIFile MethodDefinitionCount = %u\n", (unsigned int)MethodDefinitionCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::MethodDefinitionLoader(uint8_t* pTableData)
{
    uint32_t methodBodyVirtualAddress = 0;
    PESectionHeader* methodBodySectionHeader = nullptr;
    uint8_t* methodBody = nullptr;
    uint32_t methodBodySize = 0;
    uint32_t parameterListIndex = 0;
    uint32_t* parameterListIndexes = new uint32_t[MethodDefinitionCount + 1];
    for (uint32_t index = 1, heapIndex = 0; index <= MethodDefinitionCount; ++index)
    {
		MethodDefinitions[index].File = this;
        MethodDefinitions[index].TableIndex = index;
        methodBodyVirtualAddress = *(uint32_t*)pTableData; pTableData += 4;

        MethodDefinitions[index].ImplFlags = *(uint16_t*)pTableData; pTableData += 2;
        MethodDefinitions[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        MethodDefinitions[index].Name = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        MethodDefinitions[index].Signature = GetCompressedUnsigned(BlobsHeap + heapIndex, &MethodDefinitions[index].SignatureLength);
        if (ParameterCount > 0xFFFF) { parameterListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parameterListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (parameterListIndex == 0 || parameterListIndex > ParameterCount + 1) Panic("MethodDefinitionLoader Parameter");
        if (parameterListIndex <= ParameterCount)
        {
            MethodDefinitions[index].ParameterList = &Parameters[parameterListIndex];
            parameterListIndexes[index] = parameterListIndex;
        }

        if (methodBodyVirtualAddress != 0)
        {
            methodBodySectionHeader = GetSection(SectionHeaders, NTHeader->NumberOfSections, methodBodyVirtualAddress);
            methodBody = Data + methodBodySectionHeader->PointerToRawData + (methodBodyVirtualAddress - methodBodySectionHeader->VirtualAddress);
			MethodDefinitionBodyTinyHeader* bodyTinyHeader = (MethodDefinitionBodyTinyHeader*)methodBody;
            methodBodySize = bodyTinyHeader->Size;
            MethodDefinitions[index].Body.Flags = bodyTinyHeader->Flags;
            MethodDefinitions[index].Body.MaxStack = MethodDefinitionBodyDefaults::TinyMaxStack;
            MethodDefinitions[index].Body.CodeSize = bodyTinyHeader->Size;
            if ((MethodDefinitions[index].Body.Flags & MethodDefinitionBodyFlags::Fat) == MethodDefinitionBodyFlags::Fat)
            {
				MethodDefinitionBodyFatHeader* bodyFatHeader = (MethodDefinitionBodyFatHeader*)methodBody;
                MethodDefinitions[index].Body.Flags = bodyFatHeader->Flags;
                methodBodySize = bodyFatHeader->Size * MethodDefinitionBodyDefaults::FatSizeMultiplier;
                MethodDefinitions[index].Body.IsFat = true;
                MethodDefinitions[index].Body.MaxStack = bodyFatHeader->MaxStack;
                MethodDefinitions[index].Body.CodeSize = bodyFatHeader->CodeSize;
                MethodDefinitions[index].Body.LocalVariableSignatureToken = bodyFatHeader->LocalVariableSignatureToken;
            }
            MethodDefinitions[index].Body.Code = methodBody + methodBodySize;
            //printf("Method: %s, RVA: %u, HeaderSize: %u, CodeSize: %u\n", pFile->MethodDefinitions[index].Name, (unsigned int)methodBodyVirtualAddress, (unsigned int)methodBodySize, (unsigned int)pFile->MethodDefinitions[index].Body.CodeSize);

            if (MethodDefinitions[index].Body.IsFat && (MethodDefinitions[index].Body.Flags & MethodDefinitionBodyFlags::MoreSections) != 0)
            {
                uint32_t methodDataVirtualAddress = methodBodyVirtualAddress + methodBodySize + MethodDefinitions[index].Body.CodeSize;
                while ((methodDataVirtualAddress % 4) != 0) ++methodDataVirtualAddress;
                const uint8_t* methodDataStart = Data + methodBodySectionHeader->PointerToRawData + (methodDataVirtualAddress - methodBodySectionHeader->VirtualAddress);
                const uint8_t* methodData = methodDataStart;
                bool hasData = true;
                while (hasData)
                {
					MethodDefinitionExceptionTinyHeader* exceptionTinyHeader = (MethodDefinitionExceptionTinyHeader*)methodData;
                    uint8_t methodDataFlags = exceptionTinyHeader->Kind;
                    uint32_t methodDataSize = exceptionTinyHeader->DataSize;
                    bool methodDataFlagsExceptions =  (methodDataFlags & MethodDefinitionDataFlags::ExceptionTable) != 0;
                    bool methodDataFlagsFat = (methodDataFlags & MethodDefinitionDataFlags::Fat) != 0;
                    bool methodDataFlagsHasData = (methodDataFlags & MethodDefinitionDataFlags::HasAnotherDataSection) != 0;
                    if (methodDataFlagsFat)
					{
						MethodDefinitionExceptionFatHeader* exceptionFatHeader = (MethodDefinitionExceptionFatHeader*)methodData;
						methodDataSize = exceptionFatHeader->DataSize;
					}

                    if (methodDataFlagsExceptions)
                    {
						uint32_t headerLength = sizeof(MethodDefinitionExceptionTinyHeader);
						if (methodDataFlagsFat) headerLength = sizeof(MethodDefinitionExceptionFatHeader);
                        uint32_t methodDataExceptionCount = (methodDataSize - headerLength) / headerLength;
                        MethodDefinitions[index].ExceptionCount += methodDataExceptionCount;

                        //printf("  HasExceptions, RVA: %u, Flags: 0x%x, Size: %u, Count: %u\n", (unsigned int)methodDataVirtualAddress, (unsigned int)methodDataFlags, (unsigned int)methodDataSize, (unsigned int)pFile->MethodDefinitions[index].ExceptionCount);
                    }

                    hasData = methodDataFlagsHasData;
                    if (hasData) methodData += methodDataSize;
                }
                if (MethodDefinitions[index].ExceptionCount > 0)
                {
                    uint32_t methodExceptionIndex = 0;
                    MethodDefinitions[index].Exceptions = new MethodDefinitionException[MethodDefinitions[index].ExceptionCount];

                    methodData = methodDataStart;
                    hasData = true;
                    while (hasData)
                    {
						MethodDefinitionExceptionTinyHeader* exceptionTinyHeader = (MethodDefinitionExceptionTinyHeader*)methodData;
						uint8_t methodDataFlags = exceptionTinyHeader->Kind;
						uint32_t methodDataSize = exceptionTinyHeader->DataSize;
						bool methodDataFlagsExceptions =  (methodDataFlags & MethodDefinitionDataFlags::ExceptionTable) != 0;
						bool methodDataFlagsFat = (methodDataFlags & MethodDefinitionDataFlags::Fat) != 0;
						bool methodDataFlagsHasData = (methodDataFlags & MethodDefinitionDataFlags::HasAnotherDataSection) != 0;
						if (methodDataFlagsFat)
						{
							MethodDefinitionExceptionFatHeader* exceptionFatHeader = (MethodDefinitionExceptionFatHeader*)methodData;
							methodDataSize = exceptionFatHeader->DataSize;
						}

                        if (methodDataFlagsExceptions)
                        {
							uint32_t headerLength = sizeof(MethodDefinitionExceptionTinyHeader);
							if (methodDataFlagsFat) headerLength = sizeof(MethodDefinitionExceptionFatHeader);
							uint32_t methodDataExceptionCount = (methodDataSize - headerLength) / headerLength;
                            const uint8_t* methodDataException = methodData + headerLength;
                            for (uint32_t methodDataExceptionIndex = 0; methodDataExceptionIndex < methodDataExceptionCount; ++methodDataExceptionIndex)
                            {
                                uint32_t methodDataExceptionOffset = methodDataExceptionIndex * sizeof(MethodDefinitionExceptionTinyData);
								MethodDefinitionExceptionTinyData* exceptionTinyData = (MethodDefinitionExceptionTinyData*)(methodDataException + methodDataExceptionOffset);
                                MethodDefinitions[index].Exceptions[methodExceptionIndex].Flags = exceptionTinyData->Flags;
                                MethodDefinitions[index].Exceptions[methodExceptionIndex].TryOffset = exceptionTinyData->TryOffset;
                                MethodDefinitions[index].Exceptions[methodExceptionIndex].TryLength = exceptionTinyData->TryLength;
                                MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerOffset = exceptionTinyData->HandlerOffset;
                                MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerLength = exceptionTinyData->HandlerLength;
                                MethodDefinitions[index].Exceptions[methodExceptionIndex].ClassTokenOrFilterOffset = exceptionTinyData->FilterOffset;
                                if (methodDataFlagsFat)
                                {
                                    methodDataExceptionOffset = methodDataExceptionIndex * sizeof(MethodDefinitionExceptionFatData);
									MethodDefinitionExceptionFatData* exceptionFatData = (MethodDefinitionExceptionFatData*)(methodDataException + methodDataExceptionOffset);
									MethodDefinitions[index].Exceptions[methodExceptionIndex].Flags = exceptionFatData->Flags;
									MethodDefinitions[index].Exceptions[methodExceptionIndex].TryOffset = exceptionFatData->TryOffset;
									MethodDefinitions[index].Exceptions[methodExceptionIndex].TryLength = exceptionFatData->TryLength;
									MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerOffset = exceptionFatData->HandlerOffset;
									MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerLength = exceptionFatData->HandlerLength;
									MethodDefinitions[index].Exceptions[methodExceptionIndex].ClassTokenOrFilterOffset = exceptionFatData->FilterOffset;
                                }
                                //printf("    Exception, Flags: 0x%x, TryOffset: %u, TryLength: %u, HandlerOffset: %u, HandlerLength: %u, ClassTokenOrFilterOffset: %u\n", (unsigned int)pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].Flags, (unsigned int)pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].TryOffset, (unsigned int)pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].TryLength, (unsigned int)pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerOffset, (unsigned int)pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerLength, (unsigned int)pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].ClassTokenOrFilterOffset);
                                ++methodExceptionIndex;
                            }
                        }

                        hasData = methodDataFlagsHasData;
                        if (hasData) methodData += methodDataSize;
                    }

                    //printf("  TotalExceptions: %u\n", (unsigned int)methodExceptionIndex);
                }
            }
        }
    }
    uint32_t parameterListCount = 0;
    for (uint32_t index = 1, used = 0; index <= MethodDefinitionCount; ++index, used += parameterListCount)
    {
        if (index == MethodDefinitionCount || parameterListIndexes[index + 1] == 0) parameterListCount = ParameterCount - used;
        else parameterListCount = parameterListIndexes[index + 1] - parameterListIndexes[index];
        MethodDefinitions[index].ParameterListCount = parameterListCount;
    }
	delete [] parameterListIndexes;
	return pTableData;
}

uint8_t* CLR::CLIFile::ParameterInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::Parameter)) != 0)
    {
        ParameterCount = *(uint32_t*)pTableData; pTableData += 4;
        Parameters = new ParameterData[ParameterCount + 1];
    }
	printf("CLIFile ParameterCount = %u\n", (unsigned int)ParameterCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::ParameterLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= ParameterCount; ++index)
    {
        Parameters[index].TableIndex = index;
        Parameters[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        Parameters[index].Sequence = *(uint16_t*)pTableData; pTableData += 2;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        Parameters[index].Name = (const char*)(StringsHeap + heapIndex);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::InterfaceImplementationInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::InterfaceImplementation)) != 0)
    {
        InterfaceImplementationCount = *(uint32_t*)pTableData; pTableData += 4;
        InterfaceImplementations = new InterfaceImplementationData[InterfaceImplementationCount + 1];
    }
	printf("CLIFile InterfaceImplementationCount = %u\n", (unsigned int)InterfaceImplementationCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::InterfaceImplementationLoader(uint8_t* pTableData)
{
    uint32_t implementorIndex = 0;
    uint32_t interfaceIndex = 0;
    uint32_t interfaceRow = 0;
    for (uint32_t index = 1; index <= InterfaceImplementationCount; ++index)
    {
        InterfaceImplementations[index].TableIndex = index;
        if (TypeDefinitionCount > 0xFFFF) { implementorIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementorIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (implementorIndex == 0 || implementorIndex > TypeDefinitionCount) Panic("InterfaceImplementationLoader TypeDefinition");
        InterfaceImplementations[index].Implementor = &TypeDefinitions[implementorIndex];

        if (TypeDefinitionCount > TypeDefRefOrSpecType::MAXROWS16BIT ||
            TypeReferenceCount > TypeDefRefOrSpecType::MAXROWS16BIT ||
            TypeSpecificationCount > TypeDefRefOrSpecType::MAXROWS16BIT) { interfaceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { interfaceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        InterfaceImplementations[index].TypeOfInterface = interfaceIndex & TypeDefRefOrSpecType::MASK;
        interfaceRow = interfaceIndex >> TypeDefRefOrSpecType::BITS;
        switch (InterfaceImplementations[index].TypeOfInterface)
        {
		case TypeDefRefOrSpecType::TypeDefinition:
            if (interfaceRow == 0 || interfaceRow > TypeDefinitionCount) Panic("InterfaceImplementationLoader TypeDefinition");
            InterfaceImplementations[index].Interface.TypeDefinition = &TypeDefinitions[interfaceRow];
            break;
		case TypeDefRefOrSpecType::TypeReference:
            if (interfaceRow == 0 || interfaceRow > TypeReferenceCount) Panic("InterfaceImplementationLoader TypeReference");
            InterfaceImplementations[index].Interface.TypeReference = &TypeReferences[interfaceRow];
            break;
		case TypeDefRefOrSpecType::TypeSpecification:
            if (interfaceRow == 0 || interfaceRow > TypeSpecificationCount) Panic("InterfaceImplementationLoader TypeSpecification");
            InterfaceImplementations[index].Interface.TypeSpecification = &TypeSpecifications[interfaceRow];
            break;
        default: break;
        }
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::MemberReferenceInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::MemberReference)) != 0)
    {
        MemberReferenceCount = *(uint32_t*)pTableData; pTableData += 4;
        MemberReferences = new MemberReferenceData[MemberReferenceCount + 1];
    }
	printf("CLIFile MemberReferenceCount = %u\n", (unsigned int)MemberReferenceCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::MemberReferenceLoader(uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= MemberReferenceCount; ++index)
    {
        MemberReferences[index].TableIndex = index;
        if (TypeDefinitionCount > MemberRefParentType::MAXROWS16BIT ||
            TypeReferenceCount > MemberRefParentType::MAXROWS16BIT ||
            ModuleReferenceCount > MemberRefParentType::MAXROWS16BIT ||
            MethodDefinitionCount > MemberRefParentType::MAXROWS16BIT ||
            TypeSpecificationCount > MemberRefParentType::MAXROWS16BIT) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        MemberReferences[index].TypeOfParent = parentIndex & MemberRefParentType::MASK;
        parentRow = parentIndex >> MemberRefParentType::BITS;
        switch (MemberReferences[index].TypeOfParent)
        {
		case MemberRefParentType::TypeDefinition:
            if (parentRow == 0 || parentRow > TypeDefinitionCount) Panic("MemberReferenceLoader TypeDefinition");
            MemberReferences[index].Parent.TypeDefinition = &TypeDefinitions[parentRow];
            break;
		case MemberRefParentType::TypeReference:
            if (parentRow == 0 || parentRow > TypeReferenceCount) Panic("MemberReferenceLoader TypeReference");
            MemberReferences[index].Parent.TypeReference = &TypeReferences[parentRow];
            break;
		case MemberRefParentType::ModuleReference:
            if (parentRow == 0 || parentRow > ModuleReferenceCount) Panic("MemberReferenceLoader ModuleReference");
            MemberReferences[index].Parent.ModuleReference = &ModuleReferences[parentRow];
            break;
		case MemberRefParentType::MethodDefinition:
            if (parentRow == 0 || parentRow > MethodDefinitionCount) Panic("MemberReferenceLoader MethodDefinition");
            MemberReferences[index].Parent.MethodDefinition = &MethodDefinitions[parentRow];
            break;
		case MemberRefParentType::TypeSpecification:
            if (parentRow == 0 || parentRow > TypeSpecificationCount) Panic("MemberReferenceLoader TypeSpecification");
            MemberReferences[index].Parent.TypeSpecification = &TypeSpecifications[parentRow];
            break;
        default: break;
        }

        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        MemberReferences[index].Name = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        MemberReferences[index].Signature = GetCompressedUnsigned(BlobsHeap + heapIndex, &MemberReferences[index].SignatureLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::ConstantInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::Constant)) != 0)
    {
        ConstantCount = *(uint32_t*)pTableData; pTableData += 4;
        Constants = new ConstantData[ConstantCount + 1];
    }
	printf("CLIFile ConstantCount = %u\n", (unsigned int)ConstantCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::ConstantLoader(uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= ConstantCount; ++index)
    {
        Constants[index].TableIndex = index;
        Constants[index].Type = *pTableData; pTableData += 2; // 1 unused padding byte
        if (FieldCount > HasConstantType::MAXROWS16BIT ||
            ParameterCount > HasConstantType::MAXROWS16BIT ||
            PropertyCount > HasConstantType::MAXROWS16BIT) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        Constants[index].TypeOfParent = parentIndex & HasConstantType::MASK;
        parentRow = parentIndex >> HasConstantType::BITS;
        switch (Constants[index].TypeOfParent)
        {
		case HasConstantType::Field:
            if (parentRow == 0 || parentRow > FieldCount) Panic("ConstantLoader Field");
            Constants[index].Parent.Field = &Fields[parentRow];
            break;
		case HasConstantType::Parameter:
            if (parentRow == 0 || parentRow > ParameterCount) Panic("ConstantLoader Parameter");
            Constants[index].Parent.Parameter = &Parameters[parentRow];
            break;
		case HasConstantType::Property:
            if (parentRow == 0 || parentRow > PropertyCount) Panic("ConstantLoader Property");
            Constants[index].Parent.Property = &Properties[parentRow];
            break;
        default: break;
        }
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        Constants[index].Value = GetCompressedUnsigned(BlobsHeap + heapIndex, &Constants[index].ValueLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::CustomAttributeInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::CustomAttribute)) != 0)
    {
        CustomAttributeCount = *(uint32_t*)pTableData; pTableData += 4;
        CustomAttributes = new CustomAttributeData[CustomAttributeCount + 1];
    }
	printf("CLIFile CustomAttributeCount = %u\n", (unsigned int)CustomAttributeCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::CustomAttributeLoader(uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    uint32_t typeIndex = 0;
    uint32_t typeRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= CustomAttributeCount; ++index)
    {
        CustomAttributes[index].TableIndex = index;
        if (MethodDefinitionCount > HasCustomAttributeType::MAXROWS16BIT ||
            FieldCount > HasCustomAttributeType::MAXROWS16BIT ||
            TypeReferenceCount > HasCustomAttributeType::MAXROWS16BIT ||
            TypeDefinitionCount > HasCustomAttributeType::MAXROWS16BIT ||
            ParameterCount > HasCustomAttributeType::MAXROWS16BIT ||
            InterfaceImplementationCount > HasCustomAttributeType::MAXROWS16BIT ||
            MemberReferenceCount > HasCustomAttributeType::MAXROWS16BIT ||
            ModuleDefinitionCount > HasCustomAttributeType::MAXROWS16BIT ||
            DeclSecurityCount > HasCustomAttributeType::MAXROWS16BIT ||
            PropertyCount > HasCustomAttributeType::MAXROWS16BIT ||
            EventCount > HasCustomAttributeType::MAXROWS16BIT ||
            StandAloneSignatureCount > HasCustomAttributeType::MAXROWS16BIT ||
            ModuleReferenceCount > HasCustomAttributeType::MAXROWS16BIT ||
            TypeSpecificationCount > HasCustomAttributeType::MAXROWS16BIT ||
            AssemblyDefinitionCount > HasCustomAttributeType::MAXROWS16BIT ||
            AssemblyReferenceCount > HasCustomAttributeType::MAXROWS16BIT ||
            FileCount > HasCustomAttributeType::MAXROWS16BIT ||
            ExportedTypeCount > HasCustomAttributeType::MAXROWS16BIT ||
            ManifestResourceCount > HasCustomAttributeType::MAXROWS16BIT ||
            GenericParameterCount > HasCustomAttributeType::MAXROWS16BIT ||
            GenericParameterConstraintCount > HasCustomAttributeType::MAXROWS16BIT ||
            MethodSpecificationCount > HasCustomAttributeType::MAXROWS16BIT) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        CustomAttributes[index].TypeOfParent = parentIndex & HasCustomAttributeType::MASK;
        parentRow = parentIndex >> HasCustomAttributeType::BITS;
        switch (CustomAttributes[index].TypeOfParent)
        {
		case HasCustomAttributeType::MethodDefinition:
            if (parentRow == 0 || parentRow > MethodDefinitionCount) Panic("CustomAttributeLoader MethodDefinition");
            CustomAttributes[index].Parent.MethodDefinition = &MethodDefinitions[parentRow];
            break;
		case HasCustomAttributeType::Field:
            if (parentRow == 0 || parentRow > FieldCount) Panic("CustomAttributeLoader Field");
            CustomAttributes[index].Parent.Field = &Fields[parentRow];
            break;
		case HasCustomAttributeType::TypeReference:
            if (parentRow == 0 || parentRow > TypeReferenceCount) Panic("CustomAttributeLoader TypeReference");
            CustomAttributes[index].Parent.TypeReference = &TypeReferences[parentRow];
            break;
		case HasCustomAttributeType::TypeDefinition:
            if (parentRow == 0 || parentRow > TypeDefinitionCount) Panic("CustomAttributeLoader TypeDefinition");
            CustomAttributes[index].Parent.TypeDefinition = &TypeDefinitions[parentRow];
            break;
		case HasCustomAttributeType::Parameter:
            if (parentRow == 0 || parentRow > ParameterCount) Panic("CustomAttributeLoader Parameter");
            CustomAttributes[index].Parent.Parameter = &Parameters[parentRow];
            break;
		case HasCustomAttributeType::InterfaceImplementation:
            if (parentRow == 0 || parentRow > InterfaceImplementationCount) Panic("CustomAttributeLoader InterfaceImplementation");
            CustomAttributes[index].Parent.InterfaceImplementation = &InterfaceImplementations[parentRow];
            break;
		case HasCustomAttributeType::MemberReference:
            if (parentRow == 0 || parentRow > MemberReferenceCount) Panic("CustomAttributeLoader MemberReference");
            CustomAttributes[index].Parent.MemberReference = &MemberReferences[parentRow];
            break;
		case HasCustomAttributeType::ModuleDefinition:
            if (parentRow == 0 || parentRow > ModuleDefinitionCount) Panic("CustomAttributeLoader ModuleDefinition");
            CustomAttributes[index].Parent.ModuleDefinition = &ModuleDefinitions[parentRow];
            break;
		case HasCustomAttributeType::DeclSecurity:
            if (parentRow == 0 || parentRow > DeclSecurityCount) Panic("CustomAttributeLoader DeclSecurity");
            CustomAttributes[index].Parent.DeclSecurity = &DeclSecurities[parentRow];
            break;
		case HasCustomAttributeType::Property:
            if (parentRow == 0 || parentRow > PropertyCount) Panic("CustomAttributeLoader Property");
            CustomAttributes[index].Parent.Property = &Properties[parentRow];
            break;
		case HasCustomAttributeType::Event:
            if (parentRow == 0 || parentRow > EventCount) Panic("CustomAttributeLoader Event");
            CustomAttributes[index].Parent.Event = &Events[parentRow];
            break;
		case HasCustomAttributeType::StandAloneSignature:
            if (parentRow == 0 || parentRow > StandAloneSignatureCount) Panic("CustomAttributeLoader StandAloneSignature");
            CustomAttributes[index].Parent.StandAloneSignature = &StandAloneSignatures[parentRow];
            break;
		case HasCustomAttributeType::ModuleReference:
            if (parentRow == 0 || parentRow > ModuleReferenceCount) Panic("CustomAttributeLoader ModuleReference");
            CustomAttributes[index].Parent.ModuleReference = &ModuleReferences[parentRow];
            break;
		case HasCustomAttributeType::TypeSpecification:
            if (parentRow == 0 || parentRow > TypeSpecificationCount) Panic("CustomAttributeLoader TypeSpecification");
            CustomAttributes[index].Parent.TypeSpecification = &TypeSpecifications[parentRow];
            break;
		case HasCustomAttributeType::AssemblyDefinition:
            if (parentRow == 0 || parentRow > AssemblyDefinitionCount) Panic("CustomAttributeLoader AssemblyDefinition");
            CustomAttributes[index].Parent.AssemblyDefinition = &AssemblyDefinitions[parentRow];
            break;
		case HasCustomAttributeType::AssemblyReference:
            if (parentRow == 0 || parentRow > AssemblyReferenceCount) Panic("CustomAttributeLoader AssemblyReference");
            CustomAttributes[index].Parent.AssemblyReference = &AssemblyReferences[parentRow];
            break;
		case HasCustomAttributeType::File:
            if (parentRow == 0 || parentRow > FileCount) Panic("CustomAttributeLoader File");
            CustomAttributes[index].Parent.File = &Files[parentRow];
            break;
		case HasCustomAttributeType::ExportedType:
            if (parentRow == 0 || parentRow > ExportedTypeCount) Panic("CustomAttributeLoader ExportedType");
            CustomAttributes[index].Parent.ExportedType = &ExportedTypes[parentRow];
            break;
		case HasCustomAttributeType::ManifestResource:
            if (parentRow == 0 || parentRow > ManifestResourceCount) Panic("CustomAttributeLoader ManifestResource");
            CustomAttributes[index].Parent.ManifestResource = &ManifestResources[parentRow];
            break;
		case HasCustomAttributeType::GenericParameter:
            if (parentRow == 0 || parentRow > GenericParameterCount) Panic("CustomAttributeLoader GenericParameter");
            CustomAttributes[index].Parent.GenericParameter = &GenericParameters[parentRow];
            break;
		case HasCustomAttributeType::GenericParameterConstraint:
            if (parentRow == 0 || parentRow > GenericParameterConstraintCount) Panic("CustomAttributeLoader GenericParameterConstraint");
            CustomAttributes[index].Parent.GenericParameterConstraint = &GenericParameterConstraints[parentRow];
            break;
		case HasCustomAttributeType::MethodSpecification:
            if (parentRow == 0 || parentRow > MethodSpecificationCount) Panic("CustomAttributeLoader MethodSpecification");
            CustomAttributes[index].Parent.MethodSpecification = &MethodSpecifications[parentRow];
            break;
        default: break;
        }
        if (MethodDefinitionCount > CustomAttributeType::MAXROWS16BIT ||
            MemberReferenceCount > CustomAttributeType::MAXROWS16BIT) { typeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { typeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        CustomAttributes[index].TypeOfType = typeIndex & CustomAttributeType::MASK;
        typeRow = typeIndex >> CustomAttributeType::BITS;
        switch (CustomAttributes[index].TypeOfType)
        {
		case CustomAttributeType::MethodDefinition:
            if (typeRow == 0 || typeRow > MethodDefinitionCount) Panic("CustomAttributeLoader Type MethodDefinition");
            CustomAttributes[index].Type.MethodDefinition = &MethodDefinitions[typeRow];
            break;
		case CustomAttributeType::MemberReference:
            if (typeRow == 0 || typeRow > MemberReferenceCount) Panic("CustomAttributeLoader Type MemberReference");
            CustomAttributes[index].Type.MemberReference = &MemberReferences[typeRow];
            break;
        default: break;
        }
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        CustomAttributes[index].Value = GetCompressedUnsigned(BlobsHeap + heapIndex, &CustomAttributes[index].ValueLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::FieldMarshalInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::FieldMarshal)) != 0)
    {
        FieldMarshalCount = *(uint32_t*)pTableData; pTableData += 4;
        FieldMarshals = new FieldMarshalData[FieldMarshalCount + 1];
    }
	printf("CLIFile FieldMarshalCount = %u\n", (unsigned int)FieldMarshalCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::FieldMarshalLoader(uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= FieldMarshalCount; ++index)
    {
        FieldMarshals[index].TableIndex = index;
        if (FieldCount > HasFieldMarshalType::MAXROWS16BIT ||
            ParameterCount > HasFieldMarshalType::MAXROWS16BIT) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        FieldMarshals[index].TypeOfParent = parentIndex & HasFieldMarshalType::MASK;
        parentRow = parentIndex >> HasFieldMarshalType::BITS;
        switch (FieldMarshals[index].TypeOfParent)
        {
		case HasFieldMarshalType::Field:
            if (parentRow == 0 || parentRow > FieldCount) Panic("FieldMarshalLoader Field");
            FieldMarshals[index].Parent.Field = &Fields[parentRow];
            break;
		case HasFieldMarshalType::Parameter:
            if (parentRow == 0 || parentRow > ParameterCount) Panic("FieldMarshalLoader Parameter");
            FieldMarshals[index].Parent.Parameter = &Parameters[parentRow];
            break;
        default: break;
        }
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        FieldMarshals[index].NativeType = GetCompressedUnsigned(BlobsHeap + heapIndex, &FieldMarshals[index].NativeTypeLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::DeclSecurityInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::DeclSecurity)) != 0)
    {
        DeclSecurityCount = *(uint32_t*)pTableData; pTableData += 4;
        DeclSecurities = new DeclSecurityData[DeclSecurityCount + 1];
    }
	printf("CLIFile DeclSecurityCount = %u\n", (unsigned int)DeclSecurityCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::DeclSecurityLoader(uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= DeclSecurityCount; ++index)
    {
        DeclSecurities[index].TableIndex = index;
        DeclSecurities[index].Action = *(uint16_t*)pTableData; pTableData += 2;

        if (TypeDefinitionCount > HasDeclSecurityType::MAXROWS16BIT ||
            MethodDefinitionCount > HasDeclSecurityType::MAXROWS16BIT ||
            AssemblyDefinitionCount > HasDeclSecurityType::MAXROWS16BIT) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        DeclSecurities[index].TypeOfParent = parentIndex & HasDeclSecurityType::MASK;
        parentRow = parentIndex >> HasDeclSecurityType::BITS;
        switch (DeclSecurities[index].TypeOfParent)
        {
		case HasDeclSecurityType::TypeDefinition:
            if (parentRow == 0 || parentRow > TypeDefinitionCount) Panic("DeclSecurityLoader TypeDefinition");
            DeclSecurities[index].Parent.TypeDefinition = &TypeDefinitions[parentRow];
            break;
		case HasDeclSecurityType::MethodDefinition:
            if (parentRow == 0 || parentRow > MethodDefinitionCount) Panic("DeclSecurityLoader MethodDefinition");
            DeclSecurities[index].Parent.MethodDefinition = &MethodDefinitions[parentRow];
            break;
		case HasDeclSecurityType::AssemblyDefinition:
            if (parentRow == 0 || parentRow > AssemblyDefinitionCount) Panic("DeclSecurityLoader AssemblyDefinition");
            DeclSecurities[index].Parent.AssemblyDefinition = &AssemblyDefinitions[parentRow];
            break;
        default: break;
        }
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        DeclSecurities[index].PermissionSet = GetCompressedUnsigned(BlobsHeap + heapIndex, &DeclSecurities[index].PermissionSetLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::ClassLayoutInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::ClassLayout)) != 0)
    {
        ClassLayoutCount = *(uint32_t*)pTableData; pTableData += 4;
        ClassLayouts = new ClassLayoutData[ClassLayoutCount + 1];
    }
	printf("CLIFile ClassLayoutCount = %u\n", (unsigned int)ClassLayoutCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::ClassLayoutLoader(uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    for (uint32_t index = 1; index <= ClassLayoutCount; ++index)
    {
        ClassLayouts[index].TableIndex = index;
        ClassLayouts[index].PackingSize = *(uint16_t*)pTableData; pTableData += 2;
        ClassLayouts[index].ClassSize = *(uint32_t*)pTableData; pTableData += 4;
        if (TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (parentIndex == 0 || parentIndex > TypeDefinitionCount) Panic("ClassLayoutLoader");
        ClassLayouts[index].Parent = &TypeDefinitions[parentIndex];
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::FieldLayoutInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::FieldLayout)) != 0)
    {
        FieldLayoutCount = *(uint32_t*)pTableData; pTableData += 4;
        FieldLayouts = new FieldLayoutData[FieldLayoutCount + 1];
    }
	printf("CLIFile FieldLayoutCount = %u\n", (unsigned int)FieldLayoutCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::FieldLayoutLoader(uint8_t* pTableData)
{
    uint32_t fieldIndex = 0;
    for (uint32_t index = 1; index <= FieldLayoutCount; ++index)
    {
        FieldLayouts[index].TableIndex = index;
        FieldLayouts[index].Offset = *(uint32_t*)pTableData; pTableData += 4;
        if (FieldCount > 0xFFFF) { fieldIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (fieldIndex == 0 || fieldIndex > FieldCount) Panic("FieldLayoutLoader Field");
        FieldLayouts[index].Field = &Fields[fieldIndex];
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::StandAloneSignatureInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::StandAloneSignature)) != 0)
    {
        StandAloneSignatureCount = *(uint32_t*)pTableData; pTableData += 4;
        StandAloneSignatures = new StandAloneSignatureData[StandAloneSignatureCount + 1];
    }
	printf("CLIFile StandAloneSignatureCount = %u\n", (unsigned int)StandAloneSignatureCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::StandAloneSignatureLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= StandAloneSignatureCount; ++index)
    {
        StandAloneSignatures[index].TableIndex = index;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        StandAloneSignatures[index].Signature = GetCompressedUnsigned(BlobsHeap + heapIndex, &StandAloneSignatures[index].SignatureLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::EventMapInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::EventMap)) != 0)
    {
        EventMapCount = *(uint32_t*)pTableData; pTableData += 4;
        EventMaps = new EventMapData[EventMapCount + 1];
    }
	printf("CLIFile EventMapCount = %u\n", (unsigned int)EventMapCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::EventMapLoader(uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t eventListIndex = 0;
    uint32_t* eventListIndexes = new uint32_t[EventMapCount + 1];
    for (uint32_t index = 1; index <= EventMapCount; ++index)
    {
        EventMaps[index].TableIndex = index;
        if (TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (parentIndex == 0 || parentIndex > TypeDefinitionCount) Panic("EventMapLoader TypeDefinition");
        EventMaps[index].Parent = &TypeDefinitions[parentIndex];
        if (EventCount > 0xFFFF) { eventListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { eventListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (eventListIndex == 0 || eventListIndex > EventCount + 1) Panic("EventMapLoader Event");
        if (eventListIndex <= EventCount)
        {
            EventMaps[index].EventList = &Events[eventListIndex];
            eventListIndexes[index] = eventListIndex;
        }
    }
    uint32_t eventListCount = 0;
    for (uint32_t index = 1, used = 0; index <= EventMapCount; ++index, used += eventListCount)
    {
        if (index == EventMapCount || eventListIndexes[index + 1] == 0) eventListCount = EventCount - used;
        else eventListCount = eventListIndexes[index + 1] - eventListIndexes[index];
        EventMaps[index].EventListCount = eventListCount;
    }
	delete [] eventListIndexes;
    return pTableData;
}

uint8_t* CLR::CLIFile::EventInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::Event)) != 0)
    {
        EventCount = *(uint32_t*)pTableData; pTableData += 4;
        Events = new EventData[EventCount + 1];
    }
	printf("CLIFile EventCount = %u\n", (unsigned int)EventCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::EventLoader(uint8_t* pTableData)
{
    uint32_t eventTypeIndex = 0;
    uint32_t eventTypeRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= EventCount; ++index)
    {
        Events[index].TableIndex = index;
        Events[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        Events[index].Name = (const char*)(StringsHeap + heapIndex);
        if (TypeDefinitionCount > TypeDefRefOrSpecType::MAXROWS16BIT ||
            TypeReferenceCount > TypeDefRefOrSpecType::MAXROWS16BIT ||
            TypeSpecificationCount > TypeDefRefOrSpecType::MAXROWS16BIT) { eventTypeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { eventTypeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        Events[index].TypeOfEventType = eventTypeIndex & TypeDefRefOrSpecType::MASK;
        eventTypeRow = eventTypeIndex >> TypeDefRefOrSpecType::BITS;
        switch (Events[index].TypeOfEventType)
        {
		case TypeDefRefOrSpecType::TypeDefinition:
            if (eventTypeRow == 0 || eventTypeRow > TypeDefinitionCount) Panic("EventLoader TypeDefinition");
            Events[index].EventType.TypeDefinition = &TypeDefinitions[eventTypeRow];
            break;
		case TypeDefRefOrSpecType::TypeReference:
            if (eventTypeRow == 0 || eventTypeRow > TypeReferenceCount) Panic("EventLoader TypeReference");
            Events[index].EventType.TypeReference = &TypeReferences[eventTypeRow];
            break;
		case TypeDefRefOrSpecType::TypeSpecification:
            if (eventTypeRow == 0 || eventTypeRow > TypeSpecificationCount) Panic("EventLoader TypeSpecification");
            Events[index].EventType.TypeSpecification = &TypeSpecifications[eventTypeRow];
            break;
        default: break;
        }
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::PropertyMapInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::PropertyMap)) != 0)
    {
        PropertyMapCount = *(uint32_t*)pTableData; pTableData += 4;
        PropertyMaps = new PropertyMapData[PropertyMapCount + 1];
    }
	printf("CLIFile PropertyMapCount = %u\n", (unsigned int)PropertyMapCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::PropertyMapLoader(uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t propertyListIndex = 0;
    uint32_t* propertyListIndexes = new uint32_t[PropertyMapCount + 1];
    for (uint32_t index = 1; index <= PropertyMapCount; ++index)
    {
        PropertyMaps[index].TableIndex = index;
        if (TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (parentIndex == 0 || parentIndex > TypeDefinitionCount) Panic("PropertyMapLoader TypeDefinition");
        PropertyMaps[index].Parent = &TypeDefinitions[parentIndex];
        if (PropertyCount > 0xFFFF) { propertyListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { propertyListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (propertyListIndex == 0 || propertyListIndex > PropertyCount + 1) Panic("PropertyMapLoader Property");
        if (propertyListIndex <= PropertyCount)
        {
            PropertyMaps[index].PropertyList = &Properties[propertyListIndex];
            propertyListIndexes[index] = propertyListIndex;
        }
    }
    uint32_t propertyListCount = 0;
    for (uint32_t index = 1, used = 0; index <= PropertyMapCount; ++index, used += propertyListCount)
    {
        if (index == PropertyMapCount || propertyListIndexes[index + 1] == 0) propertyListCount = PropertyCount - used;
        else propertyListCount = propertyListIndexes[index + 1] - propertyListIndexes[index];
        PropertyMaps[index].PropertyListCount = propertyListCount;
    }
	delete [] propertyListIndexes;
	return pTableData;
}

uint8_t* CLR::CLIFile::PropertyInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::Property)) != 0)
    {
        PropertyCount = *(uint32_t*)pTableData; pTableData += 4;
        Properties = new PropertyData[PropertyCount + 1];
    }
	printf("CLIFile PropertyCount = %u\n", (unsigned int)PropertyCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::PropertyLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= PropertyCount; ++index)
    {
        Properties[index].TableIndex = index;
        Properties[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        Properties[index].Name = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        Properties[index].Signature = GetCompressedUnsigned(BlobsHeap + heapIndex, &Properties[index].SignatureLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::MethodSemanticsInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::MethodSemantics)) != 0)
    {
        MethodSemanticsCount = *(uint32_t*)pTableData; pTableData += 4;
        MethodSemantics = new MethodSemanticsData[MethodSemanticsCount + 1];
    }
	printf("CLIFile MethodSemanticsCount = %u\n", (unsigned int)MethodSemanticsCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::MethodSemanticsLoader(uint8_t* pTableData)
{
    uint32_t methodIndex = 0;
    uint32_t associationIndex = 0;
    uint32_t associationRow = 0;
    for (uint32_t index = 1; index <= MethodSemanticsCount; ++index)
    {
        MethodSemantics[index].TableIndex = index;
        MethodSemantics[index].Semantics = *(uint16_t*)pTableData; pTableData += 2;
        if (MethodDefinitionCount > 0xFFFF) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (methodIndex == 0 || methodIndex > MethodDefinitionCount) Panic("MethodSemanticsLoader MethodDefinition");
        MethodSemantics[index].Method = &MethodDefinitions[methodIndex];

        if (EventCount > HasSemanticsType::MAXROWS16BIT ||
            PropertyCount > HasSemanticsType::MAXROWS16BIT) { associationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { associationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        MethodSemantics[index].TypeOfAssociation = associationIndex & HasSemanticsType::MASK;
        associationRow = associationIndex >> HasSemanticsType::BITS;
        switch (MethodSemantics[index].TypeOfAssociation)
        {
		case HasSemanticsType::Event:
            if (associationRow == 0 || associationRow > EventCount) Panic("MethodSemanticsLoader Event");
            MethodSemantics[index].Association.Event = &Events[associationRow];
            break;
		case HasSemanticsType::Property:
            if (associationRow == 0 || associationRow > PropertyCount) Panic("MethodSemanticsLoader Property");
            MethodSemantics[index].Association.Property = &Properties[associationRow];
            break;
        default: break;
        }
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::MethodImplementationInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::MethodImplementation)) != 0)
    {
        MethodImplementationCount = *(uint32_t*)pTableData; pTableData += 4;
        MethodImplementations = new MethodImplementationData[MethodImplementationCount + 1];
    }
	printf("CLIFile MethodImplementationCount = %u\n", (unsigned int)MethodImplementationCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::MethodImplementationLoader(uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t methodIndex = 0;
    uint32_t methodRow = 0;
    for (uint32_t index = 1; index <= MethodImplementationCount; ++index)
    {
        MethodImplementations[index].TableIndex = index;
        if (TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (parentIndex == 0 || parentIndex > TypeDefinitionCount) Panic("MethodImplementationLoader TypeDefinition");
        MethodImplementations[index].Parent = &TypeDefinitions[parentIndex];

        if (MethodDefinitionCount > MethodDefOrRefType::MAXROWS16BIT ||
            MemberReferenceCount > MethodDefOrRefType::MAXROWS16BIT) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        MethodImplementations[index].TypeOfMethodBody = methodIndex & MethodDefOrRefType::MASK;
        methodRow = methodIndex >> MethodDefOrRefType::BITS;
        switch (MethodImplementations[index].TypeOfMethodBody)
        {
		case MethodDefOrRefType::MethodDefinition:
            if (methodRow == 0 || methodRow > MethodDefinitionCount) Panic("MethodImplementationLoader MethodDefinition");
            MethodImplementations[index].MethodBody.MethodDefinition = &MethodDefinitions[methodRow];
            break;
		case MethodDefOrRefType::MemberReference:
            if (methodRow == 0 || methodRow > MemberReferenceCount) Panic("MethodImplementationLoader MemberReference");
            MethodImplementations[index].MethodBody.MemberReference = &MemberReferences[methodRow];
            break;
        default: break;
        }

        if (MethodDefinitionCount > MethodDefOrRefType::MAXROWS16BIT ||
            MemberReferenceCount > MethodDefOrRefType::MAXROWS16BIT) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        MethodImplementations[index].TypeOfMethodDeclaration = methodIndex & MethodDefOrRefType::MASK;
        methodRow = methodIndex >> MethodDefOrRefType::BITS;
        switch (MethodImplementations[index].TypeOfMethodDeclaration)
        {
		case MethodDefOrRefType::MethodDefinition:
            if (methodRow == 0 || methodRow > MethodDefinitionCount) Panic("MethodImplementationLoader MethodDefinition");
            MethodImplementations[index].MethodDeclaration.MethodDefinition = &MethodDefinitions[methodRow];
            break;
		case MethodDefOrRefType::MemberReference:
            if (methodRow == 0 || methodRow > MemberReferenceCount) Panic("MethodImplementationLoader MemberReference");
            MethodImplementations[index].MethodDeclaration.MemberReference = &MemberReferences[methodRow];
            break;
        default: break;
        }
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::ModuleReferenceInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::ModuleReference)) != 0)
    {
        ModuleReferenceCount = *(uint32_t*)pTableData; pTableData += 4;
        ModuleReferences = new ModuleReferenceData[ModuleReferenceCount + 1];
    }
	printf("CLIFile ModuleReferenceCount = %u\n", (unsigned int)ModuleReferenceCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::ModuleReferenceLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= ModuleReferenceCount; ++index)
    {
        ModuleReferences[index].TableIndex = index;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        ModuleReferences[index].Name = (const char*)(StringsHeap + heapIndex);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::TypeSpecificationInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::TypeSpecification)) != 0)
    {
        TypeSpecificationCount = *(uint32_t*)pTableData; pTableData += 4;
        TypeSpecifications = new TypeSpecificationData[TypeSpecificationCount + 1];
    }
	printf("CLIFile TypeSpecificationCount = %u\n", (unsigned int)TypeSpecificationCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::TypeSpecificationLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= TypeSpecificationCount; ++index)
    {
		TypeSpecifications[index].File = this;
        TypeSpecifications[index].TableIndex = index;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        TypeSpecifications[index].Signature = GetCompressedUnsigned(BlobsHeap + heapIndex, &TypeSpecifications[index].SignatureLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::ImplementationMapInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::ImplementationMap)) != 0)
    {
        ImplementationMapCount = *(uint32_t*)pTableData; pTableData += 4;
        ImplementationMaps = new ImplementationMapData[ImplementationMapCount + 1];
    }
	printf("CLIFile ImplementationMapCount = %u\n", (unsigned int)ImplementationMapCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::ImplementationMapLoader(uint8_t* pTableData)
{
    uint32_t memberForwardedIndex = 0;
    uint32_t memberForwardedRow = 0;
    uint32_t importScopeIndex = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= ImplementationMapCount; ++index)
    {
        ImplementationMaps[index].TableIndex = index;
        ImplementationMaps[index].MappingFlags = *(uint16_t*)pTableData; pTableData += 2;

        if (FieldCount > MemberForwardedType::MAXROWS16BIT ||
            MethodDefinitionCount > MemberForwardedType::MAXROWS16BIT) { memberForwardedIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { memberForwardedIndex = *(uint16_t*)pTableData; pTableData += 2; }
        ImplementationMaps[index].TypeOfMemberForwarded = memberForwardedIndex & MemberForwardedType::MASK;
        memberForwardedRow = memberForwardedIndex >> MemberForwardedType::BITS;
        switch (ImplementationMaps[index].TypeOfMemberForwarded)
        {
		case MemberForwardedType::Field:
            if (memberForwardedRow == 0 || memberForwardedRow > FieldCount) Panic("ImplementationMapLoader Field");
            ImplementationMaps[index].MemberForwarded.Field = &Fields[memberForwardedRow];
            break;
		case MemberForwardedType::MethodDefinition:
            if (memberForwardedRow == 0 || memberForwardedRow > MethodDefinitionCount) Panic("ImplementationMapLoader MethodDefinition");
            ImplementationMaps[index].MemberForwarded.MethodDefinition = &MethodDefinitions[memberForwardedRow];
            break;
        default: break;
        }

        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        ImplementationMaps[index].ImportName = (const char*)(StringsHeap + heapIndex);

        if (ModuleReferenceCount > 0xFFFF) { importScopeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { importScopeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (importScopeIndex == 0 || importScopeIndex > ModuleReferenceCount) Panic("ImplementationMapLoader ModuleReference");
        ImplementationMaps[index].ImportScope = &ModuleReferences[importScopeIndex];
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::FieldRVAInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::FieldRVA)) != 0)
    {
        FieldRVACount = *(uint32_t*)pTableData; pTableData += 4;
        FieldRVAs = new FieldRVAData[FieldRVACount + 1];
    }
	printf("CLIFile FieldRVACount = %u\n", (unsigned int)FieldRVACount);
	return pTableData;
}

uint8_t* CLR::CLIFile::FieldRVALoader(uint8_t* pTableData)
{
    uint32_t fieldInitialValueVirtualAddress = 0;
    PESectionHeader* fieldInitialValueSectionHeader = nullptr;
    const uint8_t* fieldInitialValue = nullptr;
    uint32_t fieldIndex = 0;
    for (uint32_t index = 1; index <= FieldRVACount; ++index)
    {
        FieldRVAs[index].TableIndex = index;
        fieldInitialValueVirtualAddress = *(uint32_t*)pTableData; pTableData += 4;
        fieldInitialValueSectionHeader = GetSection(SectionHeaders, NTHeader->NumberOfSections, fieldInitialValueVirtualAddress);
        fieldInitialValue = Data + fieldInitialValueSectionHeader->PointerToRawData + (fieldInitialValueVirtualAddress - fieldInitialValueSectionHeader->VirtualAddress);
        FieldRVAs[index].InitialValue = fieldInitialValue;

        if (FieldCount > 0xFFFF) { fieldIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (fieldIndex == 0 || fieldIndex > FieldCount) Panic("FieldRVALoader Field");
        FieldRVAs[index].Field = &Fields[fieldIndex];
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyDefinitionInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::AssemblyDefinition)) != 0)
    {
        AssemblyDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        AssemblyDefinitions = new AssemblyDefinitionData[AssemblyDefinitionCount + 1];
    }
	printf("CLIFile AssemblyDefinitionCount = %u\n", (unsigned int)AssemblyDefinitionCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyDefinitionLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= AssemblyDefinitionCount; ++index)
    {
        AssemblyDefinitions[index].TableIndex = index;
        AssemblyDefinitions[index].HashAlgorithmID = *(uint32_t* )pTableData; pTableData += 4;
        AssemblyDefinitions[index].MajorVersion = *(uint16_t* )pTableData; pTableData += 2;
        AssemblyDefinitions[index].MinorVersion = *(uint16_t* )pTableData; pTableData += 2;
        AssemblyDefinitions[index].Build = *(uint16_t* )pTableData; pTableData += 2;
        AssemblyDefinitions[index].Revision = *(uint16_t* )pTableData; pTableData += 2;
        AssemblyDefinitions[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        AssemblyDefinitions[index].PublicKey = GetCompressedUnsigned(BlobsHeap + heapIndex, &AssemblyDefinitions[index].PublicKeyLength);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        AssemblyDefinitions[index].Name = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        AssemblyDefinitions[index].Culture = (const char*)(StringsHeap + heapIndex);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyProcessorInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::AssemblyProcessor)) != 0)
    {
        AssemblyProcessorCount = *(uint32_t*)pTableData; pTableData += 4;
        AssemblyProcessors = new AssemblyProcessorData[AssemblyProcessorCount + 1];
    }
	printf("CLIFile AssemblyProcessorCount = %u\n", (unsigned int)AssemblyProcessorCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyProcessorLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1; index <= AssemblyProcessorCount; ++index)
    {
        AssemblyProcessors[index].TableIndex = index;
        AssemblyProcessors[index].Processor = *(uint32_t* )pTableData; pTableData += 4;
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyOperatingSystemInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::AssemblyOperatingSystem)) != 0)
    {
        AssemblyOperatingSystemCount = *(uint32_t*)pTableData; pTableData += 4;
        AssemblyOperatingSystems = new AssemblyOperatingSystemData[AssemblyOperatingSystemCount + 1];
    }
	printf("CLIFile AssemblyOperatingSystemCount = %u\n", (unsigned int)AssemblyOperatingSystemCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyOperatingSystemLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1; index <= AssemblyOperatingSystemCount; ++index)
    {
        AssemblyOperatingSystems[index].TableIndex = index;
        AssemblyOperatingSystems[index].PlatformID = *(uint32_t* )pTableData; pTableData += 4;
        AssemblyOperatingSystems[index].MajorVersion = *(uint32_t* )pTableData; pTableData += 4;
        AssemblyOperatingSystems[index].MinorVersion = *(uint32_t* )pTableData; pTableData += 4;
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyReferenceInitializer(uint8_t* pTableData)
{
    if (((unsigned long long)TablesHeader->PresentTables & (1ull << MetadataTable::AssemblyReference)) != 0)
    {
        AssemblyReferenceCount = *(uint32_t*)pTableData; pTableData += 4;
        AssemblyReferences = new AssemblyReferenceData[AssemblyReferenceCount + 1];
    }
	printf("CLIFile AssemblyReferenceCount = %u\n", (unsigned int)AssemblyReferenceCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyReferenceLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= AssemblyReferenceCount; ++index)
    {
        AssemblyReferences[index].TableIndex = index;
        AssemblyReferences[index].MajorVersion = *(uint16_t* )pTableData; pTableData += 2;
        AssemblyReferences[index].MinorVersion = *(uint16_t* )pTableData; pTableData += 2;
        AssemblyReferences[index].Build = *(uint16_t* )pTableData; pTableData += 2;
        AssemblyReferences[index].Revision = *(uint16_t* )pTableData; pTableData += 2;
        AssemblyReferences[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        AssemblyReferences[index].PublicKeyOrToken = GetCompressedUnsigned(BlobsHeap + heapIndex, &AssemblyReferences[index].PublicKeyOrTokenLength);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        AssemblyReferences[index].Name = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        AssemblyReferences[index].Culture = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        AssemblyReferences[index].HashValue = GetCompressedUnsigned(BlobsHeap + heapIndex, &AssemblyReferences[index].HashValueLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyReferenceProcessorInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::AssemblyReferenceProcessor)) != 0)
    {
        AssemblyReferenceProcessorCount = *(uint32_t*)pTableData; pTableData += 4;
        AssemblyReferenceProcessors = new AssemblyReferenceProcessorData[AssemblyReferenceProcessorCount + 1];
    }
	printf("CLIFile AssemblyReferenceProcessorCount = %u\n", (unsigned int)AssemblyReferenceProcessorCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyReferenceProcessorLoader(uint8_t* pTableData)
{
    uint32_t assemblyReferenceIndex = 0;
    for (uint32_t index = 1; index <= AssemblyReferenceProcessorCount; ++index)
    {
        AssemblyReferenceProcessors[index].TableIndex = index;
        AssemblyReferenceProcessors[index].Processor = *(uint32_t* )pTableData; pTableData += 4;
        if (AssemblyReferenceCount > 0xFFFF) { assemblyReferenceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { assemblyReferenceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (assemblyReferenceIndex == 0 || assemblyReferenceIndex > AssemblyReferenceCount) Panic("AssemblyReferenceProcessorLoader");
        AssemblyReferenceProcessors[index].AssemblyReference = &AssemblyReferences[assemblyReferenceIndex];
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyReferenceOperatingSystemInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::AssemblyReferenceOperatingSystem)) != 0)
    {
        AssemblyReferenceOperatingSystemCount = *(uint32_t*)pTableData; pTableData += 4;
        AssemblyReferenceOperatingSystems = new AssemblyReferenceOperatingSystemData[AssemblyReferenceOperatingSystemCount + 1];
    }
	printf("CLIFile AssemblyReferenceOperatingSystemCount = %u\n", (unsigned int)AssemblyReferenceOperatingSystemCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::AssemblyReferenceOperatingSystemLoader(uint8_t* pTableData)
{
    uint32_t assemblyReferenceIndex = 0;
    for (uint32_t index = 1; index <= AssemblyReferenceOperatingSystemCount; ++index)
    {
        AssemblyReferenceOperatingSystems[index].TableIndex = index;
        AssemblyReferenceOperatingSystems[index].PlatformID = *(uint32_t* )pTableData; pTableData += 4;
        AssemblyReferenceOperatingSystems[index].MajorVersion = *(uint32_t* )pTableData; pTableData += 4;
        AssemblyReferenceOperatingSystems[index].MinorVersion = *(uint32_t* )pTableData; pTableData += 4;
        if (AssemblyReferenceCount > 0xFFFF) { assemblyReferenceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { assemblyReferenceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (assemblyReferenceIndex == 0 || assemblyReferenceIndex > AssemblyReferenceCount) Panic("AssemblyReferenceOperatingSystemLoader");
        AssemblyReferenceOperatingSystems[index].AssemblyReference = &AssemblyReferences[assemblyReferenceIndex];
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::FileInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::File)) != 0)
    {
        FileCount = *(uint32_t*)pTableData; pTableData += 4;
        Files = new FileData[FileCount + 1];
    }
	printf("CLIFile FileCount = %u\n", (unsigned int)FileCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::FileLoader(uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= FileCount; ++index)
    {
        Files[index].TableIndex = index;
        Files[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        Files[index].Name = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        Files[index].HashValue = GetCompressedUnsigned(BlobsHeap + heapIndex, &Files[index].HashValueLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::ExportedTypeInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::ExportedType)) != 0)
    {
        ExportedTypeCount = *(uint32_t*)pTableData; pTableData += 4;
        ExportedTypes = new ExportedTypeData[ExportedTypeCount + 1];
    }
	printf("CLIFile ExportedTypeCount = %u\n", (unsigned int)ExportedTypeCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::ExportedTypeLoader(uint8_t* pTableData)
{
    uint32_t typeDefinitionIndex = 0;
    uint32_t implementationIndex = 0;
    uint32_t implementationRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= ExportedTypeCount; ++index)
    {
        ExportedTypes[index].TableIndex = index;
        ExportedTypes[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if (TypeDefinitionCount > 0xFFFF) { typeDefinitionIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { typeDefinitionIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (typeDefinitionIndex == 0 || typeDefinitionIndex > TypeDefinitionCount) Panic("ExportedTypeLoader TypeDefinition");
        ExportedTypes[index].TypeDefinitionID = &TypeDefinitions[typeDefinitionIndex];
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        ExportedTypes[index].Name = (const char*)(StringsHeap + heapIndex);
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        ExportedTypes[index].Namespace = (const char*)(StringsHeap + heapIndex);

        if (FileCount > ImplementationType::MAXROWS16BIT ||
            AssemblyReferenceCount > ImplementationType::MAXROWS16BIT ||
            ExportedTypeCount > ImplementationType::MAXROWS16BIT) { implementationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        ExportedTypes[index].TypeOfImplementation = implementationIndex & ImplementationType::MASK;
        implementationRow = implementationIndex >> ImplementationType::BITS;
        switch (ExportedTypes[index].TypeOfImplementation)
        {
		case ImplementationType::File:
            if (implementationRow == 0 || implementationRow > FileCount) Panic("ExportedTypeLoader File");
            ExportedTypes[index].Implementation.File = &Files[implementationRow];
            break;
		case ImplementationType::AssemblyReference:
            if (implementationRow == 0 || implementationRow > AssemblyReferenceCount) Panic("ExportedTypeLoader AssemblyReference");
            ExportedTypes[index].Implementation.AssemblyReference = &AssemblyReferences[implementationRow];
            break;
		case ImplementationType::ExportedType:
            if (implementationRow == 0 || implementationRow > ExportedTypeCount) Panic("ExportedTypeLoader ExportedType");
            ExportedTypes[index].Implementation.ExportedType = &ExportedTypes[implementationRow];
            break;
        default: break;
        }
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::ManifestResourceInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::ManifestResource)) != 0)
    {
        ManifestResourceCount = *(uint32_t*)pTableData; pTableData += 4;
        ManifestResources = new ManifestResourceData[ManifestResourceCount + 1];
    }
	printf("CLIFile ManifestResourceCount = %u\n", (unsigned int)ManifestResourceCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::ManifestResourceLoader(uint8_t* pTableData)
{
    uint32_t implementationIndex = 0;
    uint32_t implementationRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= ManifestResourceCount; ++index)
    {
        ManifestResources[index].TableIndex = index;
        ManifestResources[index].Offset = *(uint32_t* )pTableData; pTableData += 4;
        ManifestResources[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        ManifestResources[index].Name = (const char*)(StringsHeap + heapIndex);

        if (FileCount > ImplementationType::MAXROWS16BIT ||
            AssemblyReferenceCount > ImplementationType::MAXROWS16BIT ||
            ExportedTypeCount > ImplementationType::MAXROWS16BIT) { implementationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        ManifestResources[index].TypeOfImplementation = implementationIndex & ImplementationType::MASK;
        implementationRow = implementationIndex >> ImplementationType::BITS;
        switch (ManifestResources[index].TypeOfImplementation)
        {
		case ImplementationType::File:
            if (implementationRow > FileCount) Panic("ManifestResourceLoader File");
            ManifestResources[index].Implementation.File = &Files[implementationRow];
            break;
		case ImplementationType::AssemblyReference:
            if (implementationRow == 0 || implementationRow > AssemblyReferenceCount) Panic("ManifestResourceLoader AssemblyReference");
            ManifestResources[index].Implementation.AssemblyReference = &AssemblyReferences[implementationRow];
            break;
		case ImplementationType::ExportedType:
            if (implementationRow == 0 || implementationRow > ExportedTypeCount) Panic("ManifestResourceLoader ExportedType");
            ManifestResources[index].Implementation.ExportedType = &ExportedTypes[implementationRow];
            break;
        default: break;
        }
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::NestedClassInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::NestedClass)) != 0)
    {
        NestedClassCount = *(uint32_t*)pTableData; pTableData += 4;
        NestedClasses = new NestedClassData[NestedClassCount + 1];
    }
	printf("CLIFile NestedClassCount = %u\n", (unsigned int)NestedClassCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::NestedClassLoader(uint8_t* pTableData)
{
    uint32_t nestedIndex = 0;
    uint32_t enclosingIndex = 0;
    for (uint32_t index = 1; index <= NestedClassCount; ++index)
    {
        NestedClasses[index].TableIndex = index;
        if (TypeDefinitionCount > 0xFFFF) { nestedIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { nestedIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (nestedIndex == 0 || nestedIndex > TypeDefinitionCount) Panic("NestedClassLoader TypeDefinition");
        NestedClasses[index].Nested = &TypeDefinitions[nestedIndex];
        if (TypeDefinitionCount > 0xFFFF) { enclosingIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { enclosingIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (enclosingIndex == 0 || enclosingIndex > TypeDefinitionCount) Panic("NestedClassLoader TypeDefinition");
        NestedClasses[index].Enclosing = &TypeDefinitions[enclosingIndex];
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::GenericParameterInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::GenericParameter)) != 0)
    {
        GenericParameterCount = *(uint32_t*)pTableData; pTableData += 4;
        GenericParameters = new GenericParameterData[GenericParameterCount + 1];
    }
	printf("CLIFile GenericParameterCount = %u\n", (unsigned int)GenericParameterCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::GenericParameterLoader(uint8_t* pTableData)
{
    uint32_t ownerIndex = 0;
    uint32_t ownerRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= GenericParameterCount; ++index)
    {
        GenericParameters[index].TableIndex = index;
        GenericParameters[index].Index = *(uint16_t* )pTableData; pTableData += 2;
        GenericParameters[index].Flags = *(uint16_t* )pTableData; pTableData += 2;
        if (TypeDefinitionCount > TypeOrMethodDefType::MAXROWS16BIT ||
            MethodDefinitionCount > TypeOrMethodDefType::MAXROWS16BIT) { ownerIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { ownerIndex = *(uint16_t*)pTableData; pTableData += 2; }
        GenericParameters[index].TypeOfOwner = ownerIndex & TypeOrMethodDefType::MASK;
        ownerRow = ownerIndex >> TypeOrMethodDefType::BITS;
        switch (GenericParameters[index].TypeOfOwner)
        {
		case TypeOrMethodDefType::TypeDefinition:
            if (ownerRow == 0 || ownerRow > TypeDefinitionCount) Panic("GenericParameterLoader TypeDefinition");
            GenericParameters[index].Owner.TypeDefinition = &TypeDefinitions[ownerRow];
            break;
		case TypeOrMethodDefType::MethodDefinition:
            if (ownerRow == 0 || ownerRow > MethodDefinitionCount) Panic("GenericParameterLoader MethodDefinition");
            GenericParameters[index].Owner.MethodDefinition = &MethodDefinitions[ownerRow];
            break;
        default: break;
        }
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        GenericParameters[index].Name = (const char*)(StringsHeap + heapIndex);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::MethodSpecificationInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::MethodSpecification)) != 0)
    {
        MethodSpecificationCount = *(uint32_t*)pTableData; pTableData += 4;
        MethodSpecifications = new MethodSpecificationData[MethodSpecificationCount + 1];
    }
	printf("CLIFile MethodSpecificationCount = %u\n", (unsigned int)MethodSpecificationCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::MethodSpecificationLoader(uint8_t* pTableData)
{
    uint32_t methodIndex = 0;
    uint32_t methodRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= MethodSpecificationCount; ++index)
    {
        MethodSpecifications[index].TableIndex = index;
        if (MethodDefinitionCount > MethodDefOrRefType::MAXROWS16BIT ||
            MemberReferenceCount > MethodDefOrRefType::MAXROWS16BIT) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        MethodSpecifications[index].TypeOfMethod = methodIndex & MethodDefOrRefType::MASK;
        methodRow = methodIndex >> MethodDefOrRefType::BITS;
        switch (MethodSpecifications[index].TypeOfMethod)
        {
		case MethodDefOrRefType::MethodDefinition:
            if (methodRow == 0 || methodRow > MethodDefinitionCount) Panic("MethodSpecificationLoader MethodDefinition");
            MethodSpecifications[index].Method.MethodDefinition = &MethodDefinitions[methodRow];
            break;
		case MethodDefOrRefType::MemberReference:
            if (methodRow == 0 || methodRow > MemberReferenceCount) Panic("MethodSpecificationLoader MemberReference");
            MethodSpecifications[index].Method.MemberReference = &MemberReferences[methodRow];
            break;
        default: break;
        }
        if ((TablesHeader->HeapOffsetSizes & HeapOffsetSize::Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        MethodSpecifications[index].Instantiation = GetCompressedUnsigned(BlobsHeap + heapIndex, &MethodSpecifications[index].InstantiationLength);
    }
	return pTableData;
}

uint8_t* CLR::CLIFile::GenericParameterConstraintInitializer(uint8_t* pTableData)
{
    if ((TablesHeader->PresentTables & (1ull << MetadataTable::GenericParameterConstraint)) != 0)
    {
        GenericParameterConstraintCount = *(uint32_t*)pTableData; pTableData += 4;
        GenericParameterConstraints = new GenericParameterConstraintData[GenericParameterConstraintCount + 1];
    }
	printf("CLIFile GenericParameterConstraintCount = %u\n", (unsigned int)GenericParameterConstraintCount);
	return pTableData;
}

uint8_t* CLR::CLIFile::GenericParameterConstraintLoader(uint8_t* pTableData)
{
    uint32_t ownerIndex = 0;
    uint32_t constraintIndex = 0;
    uint32_t constraintRow = 0;
    for (uint32_t index = 1; index <= GenericParameterConstraintCount; ++index)
    {
        GenericParameterConstraints[index].TableIndex = index;
        if (GenericParameterCount > 0xFFFF) { ownerIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { ownerIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (ownerIndex == 0 || ownerIndex > GenericParameterCount) Panic("GenericParameterConstraintLoader GenericParameter");
        GenericParameterConstraints[index].Owner = &GenericParameters[ownerIndex];
        if (TypeDefinitionCount > TypeDefRefOrSpecType::MAXROWS16BIT ||
            TypeReferenceCount > TypeDefRefOrSpecType::MAXROWS16BIT ||
            TypeSpecificationCount > TypeDefRefOrSpecType::MAXROWS16BIT) { constraintIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { constraintIndex = *(uint16_t*)pTableData; pTableData += 2; }
        GenericParameterConstraints[index].TypeOfConstraint = constraintIndex & TypeDefRefOrSpecType::MASK;
        constraintRow = constraintIndex >> TypeDefRefOrSpecType::BITS;
        switch (GenericParameterConstraints[index].TypeOfConstraint)
        {
		case TypeDefRefOrSpecType::TypeDefinition:
            if (constraintRow == 0 || constraintRow > TypeDefinitionCount) Panic("GenericParameterConstraintLoader TypeDefinition");
            GenericParameterConstraints[index].Constraint.TypeDefinition = &TypeDefinitions[constraintRow];
            break;
		case TypeDefRefOrSpecType::TypeReference:
            if (constraintRow == 0 || constraintRow > TypeReferenceCount) Panic("GenericParameterConstraintLoader TypeReference");
            GenericParameterConstraints[index].Constraint.TypeReference = &TypeReferences[constraintRow];
            break;
		case TypeDefRefOrSpecType::TypeSpecification:
            if (constraintRow == 0 || constraintRow > TypeSpecificationCount) Panic("GenericParameterConstraintLoader TypeSpecification");
            GenericParameterConstraints[index].Constraint.TypeSpecification = &TypeSpecifications[constraintRow];
            break;
        default: break;
        }
    }
	return pTableData;
}

void CLR::CLIFile::Link()
{
    for (uint32_t index = 1; index <= CustomAttributeCount; ++index)
    {
		switch (CustomAttributes[index].TypeOfParent)
		{
		case HasCustomAttributeType::ModuleDefinition:
			++CustomAttributes[index].Parent.ModuleDefinition->CustomAttributeCount;
			break;
		case HasCustomAttributeType::TypeReference:
			++CustomAttributes[index].Parent.TypeReference->CustomAttributeCount;
			break;
		case HasCustomAttributeType::TypeDefinition:
			++CustomAttributes[index].Parent.TypeDefinition->CustomAttributeCount;
			break;
		case HasCustomAttributeType::Field:
			++CustomAttributes[index].Parent.Field->CustomAttributeCount;
			break;
		case HasCustomAttributeType::MethodDefinition:
			++CustomAttributes[index].Parent.MethodDefinition->CustomAttributeCount;
			break;
		case HasCustomAttributeType::Parameter:
			++CustomAttributes[index].Parent.Parameter->CustomAttributeCount;
			break;
		case HasCustomAttributeType::InterfaceImplementation:
			++CustomAttributes[index].Parent.InterfaceImplementation->CustomAttributeCount;
			break;
		case HasCustomAttributeType::MemberReference:
			++CustomAttributes[index].Parent.MemberReference->CustomAttributeCount;
			break;
		case HasCustomAttributeType::DeclSecurity:
			++CustomAttributes[index].Parent.DeclSecurity->CustomAttributeCount;
			break;
		case HasCustomAttributeType::StandAloneSignature:
			++CustomAttributes[index].Parent.StandAloneSignature->CustomAttributeCount;
			break;
		case HasCustomAttributeType::Event:
			++CustomAttributes[index].Parent.Event->CustomAttributeCount;
			break;
		case HasCustomAttributeType::Property:
			++CustomAttributes[index].Parent.Property->CustomAttributeCount;
			break;
		case HasCustomAttributeType::ModuleReference:
			++CustomAttributes[index].Parent.ModuleReference->CustomAttributeCount;
			break;
		case HasCustomAttributeType::TypeSpecification:
			++CustomAttributes[index].Parent.TypeSpecification->CustomAttributeCount;
			break;
		case HasCustomAttributeType::AssemblyDefinition:
			++CustomAttributes[index].Parent.AssemblyDefinition->CustomAttributeCount;
			break;
		case HasCustomAttributeType::AssemblyReference:
			++CustomAttributes[index].Parent.AssemblyReference->CustomAttributeCount;
			break;
		case HasCustomAttributeType::File:
			++CustomAttributes[index].Parent.File->CustomAttributeCount;
			break;
		case HasCustomAttributeType::ExportedType:
			++CustomAttributes[index].Parent.ExportedType->CustomAttributeCount;
			break;
		case HasCustomAttributeType::ManifestResource:
			++CustomAttributes[index].Parent.ManifestResource->CustomAttributeCount;
			break;
		case HasCustomAttributeType::GenericParameter:
			++CustomAttributes[index].Parent.GenericParameter->CustomAttributeCount;
			break;
		case HasCustomAttributeType::MethodSpecification:
			++CustomAttributes[index].Parent.MethodSpecification->CustomAttributeCount;
			break;
		case HasCustomAttributeType::GenericParameterConstraint:
			++CustomAttributes[index].Parent.GenericParameterConstraint->CustomAttributeCount;
			break;
		default:
			Panic("Unknown CustomAttribute Parent Table");
			break;
		}
    }
    for (uint32_t index = 1; index <= MemberReferenceCount; ++index)
    {
		switch (MemberReferences[index].TypeOfParent)
		{
		case MemberRefParentType::TypeDefinition:
			++MemberReferences[index].Parent.TypeDefinition->MemberReferenceCount;
			break;
		case MemberRefParentType::TypeReference:
			++MemberReferences[index].Parent.TypeReference->MemberReferenceCount;
			break;
		case MemberRefParentType::ModuleReference:
			++MemberReferences[index].Parent.ModuleReference->MemberReferenceCount;
			break;
		case MemberRefParentType::MethodDefinition:
			++MemberReferences[index].Parent.MethodDefinition->MemberReferenceCount;
			break;
		case MemberRefParentType::TypeSpecification:
			++MemberReferences[index].Parent.TypeSpecification->MemberReferenceCount;
			break;
		default:
			Panic("Unknown MemberReference Parent Table");
			break;
		}
    }
    for (uint32_t index = 1; index <= GenericParameterCount; ++index)
    {
		switch(GenericParameters[index].TypeOfOwner)
		{
		case TypeOrMethodDefType::TypeDefinition:
			++GenericParameters[index].Owner.TypeDefinition->GenericParameterCount;
			break;
		case TypeOrMethodDefType::MethodDefinition:
			++GenericParameters[index].Owner.MethodDefinition->GenericParameterCount;
			break;
		default:
			Panic("Unknown GenericParameter Owner Table");
			break;
		}
    }
    for (uint32_t index = 1; index <= InterfaceImplementationCount; ++index)
	{
		++InterfaceImplementations[index].Implementor->InterfaceImplementationCount;
	}
    for (uint32_t index = 1; index <= MethodImplementationCount; ++index)
    {
        ++MethodImplementations[index].Parent->MethodImplementationCount;
    }
    for (uint32_t index = 1; index <= NestedClassCount; ++index)
    {
        ++NestedClasses[index].Enclosing->NestedClassCount;
    }
    for (uint32_t index = 1; index <= GenericParameterConstraintCount; ++index)
	{
		++GenericParameterConstraints[index].Owner->GenericParameterConstraintCount;
	}


    for (uint32_t index = 1; index <= ModuleDefinitionCount; ++index)
    {
        if (ModuleDefinitions[index].CustomAttributeCount > 0)
        {
            ModuleDefinitions[index].CustomAttributes = new CustomAttributeData*[ModuleDefinitions[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::ModuleDefinition &&
                    CustomAttributes[searchIndex].Parent.ModuleDefinition == &ModuleDefinitions[index])
                {
                    ModuleDefinitions[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= TypeReferenceCount; ++index)
    {
        if (TypeReferences[index].CustomAttributeCount > 0)
        {
            TypeReferences[index].CustomAttributes = new CustomAttributeData*[TypeReferences[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::TypeReference &&
                    CustomAttributes[searchIndex].Parent.TypeReference == &TypeReferences[index])
                {
                    TypeReferences[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
        if (TypeReferences[index].MemberReferenceCount > 0)
        {
            TypeReferences[index].MemberReferences = new MemberReferenceData*[TypeReferences[index].MemberReferenceCount];
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= MemberReferenceCount; ++searchIndex)
            {
                if (MemberReferences[searchIndex].TypeOfParent == MemberRefParentType::TypeReference &&
                    MemberReferences[searchIndex].Parent.TypeReference == &TypeReferences[index])
                {
                    TypeReferences[index].MemberReferences[memberReferenceIndex] = &MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
		if (!TypeReferences[index].ResolutionScope.AssemblyReference)
		{
			for (uint32_t searchIndex = 1; searchIndex <= ExportedTypeCount; ++searchIndex)
			{
				if (!strcmp(ExportedTypes[searchIndex].Name, TypeReferences[index].Name) &&
					!strcmp(ExportedTypes[searchIndex].Namespace, TypeReferences[index].Namespace))
				{
					TypeReferences[index].ExportedType = &ExportedTypes[searchIndex];
				}
			}
		}
    }

    for (uint32_t index = 1; index <= ClassLayoutCount; ++index)
	{
		ClassLayouts[index].Parent->ClassLayout = &ClassLayouts[index];
	}

    for (uint32_t index = 1; index <= DeclSecurityCount; ++index)
    {
		switch(DeclSecurities[index].TypeOfParent)
		{
		case HasDeclSecurityType::TypeDefinition:
			DeclSecurities[index].Parent.TypeDefinition->DeclSecurity = &DeclSecurities[index];
			break;
		case HasDeclSecurityType::MethodDefinition:
			DeclSecurities[index].Parent.MethodDefinition->DeclSecurity = &DeclSecurities[index];
			break;
		case HasDeclSecurityType::AssemblyDefinition:
			DeclSecurities[index].Parent.AssemblyDefinition->DeclSecurity = &DeclSecurities[index];
			break;
		default:
			Panic("Unknown DeclSecurity Parent Table");
			break;
		}

        if (DeclSecurities[index].CustomAttributeCount > 0)
        {
            DeclSecurities[index].CustomAttributes = new CustomAttributeData*[DeclSecurities[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::DeclSecurity &&
                    CustomAttributes[searchIndex].Parent.DeclSecurity == &DeclSecurities[index])
                {
                    DeclSecurities[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= EventMapCount; ++index)
	{
		EventMaps[index].Parent->EventMap = &EventMaps[index];
	}

    for (uint32_t index = 1; index <= PropertyMapCount; ++index)
	{
		PropertyMaps[index].Parent->PropertyMap = &PropertyMaps[index];
	}

	for (uint32_t index = 1; index <= TypeDefinitionCount; ++index)
    {
        if (TypeDefinitions[index].CustomAttributeCount > 0)
        {
            TypeDefinitions[index].CustomAttributes = new CustomAttributeData*[TypeDefinitions[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::TypeDefinition &&
                    CustomAttributes[searchIndex].Parent.TypeDefinition == &TypeDefinitions[index])
                {
                    TypeDefinitions[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
        if (TypeDefinitions[index].GenericParameterCount > 0)
        {
            TypeDefinitions[index].GenericParameters = new GenericParameterData*[TypeDefinitions[index].GenericParameterCount];
            for (uint32_t searchIndex = 1, genericParameterIndex = 0; searchIndex <= GenericParameterCount; ++searchIndex)
            {
                if (GenericParameters[searchIndex].TypeOfOwner == TypeOrMethodDefType::TypeDefinition &&
                    GenericParameters[searchIndex].Owner.TypeDefinition == &TypeDefinitions[index])
                {
                    TypeDefinitions[index].GenericParameters[genericParameterIndex] = &GenericParameters[searchIndex];
                    ++genericParameterIndex;
                }
            }
        }
        if (TypeDefinitions[index].InterfaceImplementationCount > 0)
        {
            TypeDefinitions[index].InterfaceImplementations = new InterfaceImplementationData*[TypeDefinitions[index].InterfaceImplementationCount];
            for (uint32_t searchIndex = 1, interfaceImplementationIndex = 0; searchIndex <= InterfaceImplementationCount; ++searchIndex)
            {
                if (InterfaceImplementations[searchIndex].Implementor == &TypeDefinitions[index])
                {
                    TypeDefinitions[index].InterfaceImplementations[interfaceImplementationIndex] = &InterfaceImplementations[searchIndex];
                    ++interfaceImplementationIndex;
                }
            }
        }
        if (TypeDefinitions[index].MemberReferenceCount > 0)
        {
            TypeDefinitions[index].MemberReferences = new MemberReferenceData*[TypeDefinitions[index].MemberReferenceCount];
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= MemberReferenceCount; ++searchIndex)
            {
                if (MemberReferences[searchIndex].TypeOfParent == MemberRefParentType::TypeDefinition &&
                    MemberReferences[searchIndex].Parent.TypeDefinition == &TypeDefinitions[index])
                {
                    TypeDefinitions[index].MemberReferences[memberReferenceIndex] = &MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
        if (TypeDefinitions[index].MethodImplementationCount > 0)
        {
            //printf("MethodImplementations: %u %s.%s %u\n", (unsigned int)index, TypeDefinitions[index].Namespace, TypeDefinitions[index].Name, (unsigned int)TypeDefinitions[index].MethodImplementationCount);
            TypeDefinitions[index].MethodImplementations = new MethodImplementationData*[TypeDefinitions[index].MethodImplementationCount];
            for (uint32_t searchIndex = 1, methodImplementationIndex = 0; searchIndex <= MethodImplementationCount; ++searchIndex)
            {
                if (MethodImplementations[searchIndex].Parent == &TypeDefinitions[index])
                {
                    TypeDefinitions[index].MethodImplementations[methodImplementationIndex] = &MethodImplementations[searchIndex];
                    ++methodImplementationIndex;
                }
            }
        }
        if (TypeDefinitions[index].NestedClassCount > 0)
        {
            TypeDefinitions[index].NestedClasses = new NestedClassData*[TypeDefinitions[index].NestedClassCount];
            for (uint32_t searchIndex = 1, nestedClassIndex = 0; searchIndex <= NestedClassCount; ++searchIndex)
            {
                if (NestedClasses[searchIndex].Enclosing == &TypeDefinitions[index])
                {
                    TypeDefinitions[index].NestedClasses[nestedClassIndex] = &NestedClasses[searchIndex];
                    ++nestedClassIndex;
                }
            }
        }
        if (TypeDefinitions[index].MethodDefinitionListCount > 0)
        {
            //printf("TypeDefinition[%u]: %s.%s, %u methods\n", (unsigned int)index, TypeDefinitions[index].Namespace, TypeDefinitions[index].Name, (unsigned int)TypeDefinitions[index].MethodDefinitionListCount);
            for (uint32_t searchIndex = 0; searchIndex < TypeDefinitions[index].MethodDefinitionListCount; ++searchIndex)
            {
                //printf("    %s\n", TypeDefinitions[index].MethodDefinitionList[searchIndex].Name);
                TypeDefinitions[index].MethodDefinitionList[searchIndex].TypeDefinition = &TypeDefinitions[index];
            }
        }
		//printf("Linking Fields for %s.%s, %u\n", TypeDefinitions[index].Namespace, TypeDefinitions[index].Name, TypeDefinitions[index].FieldListCount);
		for (uint32_t index2 = 0; index2 < TypeDefinitions[index].FieldListCount; ++index2)
		{
			TypeDefinitions[index].FieldList[index2].TypeDefinition = &TypeDefinitions[index];
			//printf("Linking Field %s to TypeDef %s.%s @ 0x%x\n", TypeDefinitions[index].FieldList[index2].Name, TypeDefinitions[index].Namespace, TypeDefinitions[index].Name, (unsigned int)TypeDefinitions[index].FieldList[index2].TypeDefinition);
		}
	}

    for (uint32_t index = 1; index <= ConstantCount; ++index)
    {
		switch(Constants[index].TypeOfParent)
		{
		case HasConstantType::Field:
			Constants[index].Parent.Field->Constant = &Constants[index];
			break;
		case HasConstantType::Parameter:
			Constants[index].Parent.Parameter->Constant = &Constants[index];
			break;
		case HasConstantType::Property:
			Constants[index].Parent.Property->Constant = &Constants[index];
			break;
		default:
			Panic("Unknown Constant Parent Table");
			break;
		}
    }

    for (uint32_t index = 1; index <= FieldCount; ++index)
    {
        if (Fields[index].CustomAttributeCount > 0)
        {
            Fields[index].CustomAttributes = new CustomAttributeData*[Fields[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::Field &&
                    CustomAttributes[searchIndex].Parent.Field == &Fields[index])
                {
                    Fields[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= FieldMarshalCount; ++index)
    {
		switch(FieldMarshals[index].TypeOfParent)
		{
		case HasConstantType::Field:
			FieldMarshals[index].Parent.Field->FieldMarshal = &FieldMarshals[index];
			break;
		case HasConstantType::Parameter:
			FieldMarshals[index].Parent.Parameter->FieldMarshal = &FieldMarshals[index];
			break;
		default:
			Panic("Unknown FieldMarshal Parent Table");
			break;
		}
    }

    for (uint32_t index = 1; index <= ImplementationMapCount; ++index)
    {
		switch(ImplementationMaps[index].TypeOfMemberForwarded)
		{
		case MemberForwardedType::Field:
			ImplementationMaps[index].MemberForwarded.Field->ImplementationMap = &ImplementationMaps[index];
			break;
		case MemberForwardedType::MethodDefinition:
			ImplementationMaps[index].MemberForwarded.MethodDefinition->ImplementationMap = &ImplementationMaps[index];
			break;
		default:
			Panic("Unknown FieldMarshal MemberForwarded Table");
			break;
		}
    }

    for (uint32_t index = 1; index <= MethodDefinitionCount; ++index)
    {
        if (MethodDefinitions[index].CustomAttributeCount > 0)
        {
            MethodDefinitions[index].CustomAttributes = new CustomAttributeData*[MethodDefinitions[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::MethodDefinition &&
                    CustomAttributes[searchIndex].Parent.MethodDefinition == &MethodDefinitions[index])
                {
                    MethodDefinitions[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
        if (MethodDefinitions[index].GenericParameterCount > 0)
        {
            MethodDefinitions[index].GenericParameters = new GenericParameterData*[MethodDefinitions[index].GenericParameterCount];
            for (uint32_t searchIndex = 1, genericParameterIndex = 0; searchIndex <= GenericParameterCount; ++searchIndex)
            {
                if (GenericParameters[searchIndex].TypeOfOwner == TypeOrMethodDefType::MethodDefinition &&
                    GenericParameters[searchIndex].Owner.MethodDefinition == &MethodDefinitions[index])
                {
                    MethodDefinitions[index].GenericParameters[genericParameterIndex] = &GenericParameters[searchIndex];
                    ++genericParameterIndex;
                }
            }
        }
        if (MethodDefinitions[index].MemberReferenceCount > 0)
        {
            MethodDefinitions[index].MemberReferences = new MemberReferenceData*[MethodDefinitions[index].MemberReferenceCount];
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= MemberReferenceCount; ++searchIndex)
            {
                if (MemberReferences[searchIndex].TypeOfParent == MemberRefParentType::MethodDefinition &&
                    MemberReferences[searchIndex].Parent.MethodDefinition == &MethodDefinitions[index])
                {
                    MethodDefinitions[index].MemberReferences[memberReferenceIndex] = &MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
//		if ((MethodDefinitions[index].ImplFlags & MethodImplAttributes::InternalCall) != 0) MethodDefinitions[index].InternalCall = ILDecomposition_ResolveInternalCall(&MethodDefinitions[index], pFile);
    }

    for (uint32_t index = 1; index <= ParameterCount; ++index)
    {
        if (Parameters[index].CustomAttributeCount > 0)
        {
            Parameters[index].CustomAttributes = new CustomAttributeData*[Parameters[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::Parameter &&
                    CustomAttributes[searchIndex].Parent.Parameter == &Parameters[index])
                {
                    Parameters[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= InterfaceImplementationCount; ++index)
    {
        if (InterfaceImplementations[index].CustomAttributeCount > 0)
        {
            InterfaceImplementations[index].CustomAttributes = new CustomAttributeData*[InterfaceImplementations[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::InterfaceImplementation &&
                    CustomAttributes[searchIndex].Parent.InterfaceImplementation == &InterfaceImplementations[index])
                {
                    InterfaceImplementations[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= MemberReferenceCount; ++index)
    {
        if (MemberReferences[index].CustomAttributeCount > 0)
        {
            MemberReferences[index].CustomAttributes = new CustomAttributeData*[MemberReferences[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::MemberReference &&
                    CustomAttributes[searchIndex].Parent.MemberReference == &MemberReferences[index])
                {
                    MemberReferences[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= StandAloneSignatureCount; ++index)
    {
        if (StandAloneSignatures[index].CustomAttributeCount > 0)
        {
            StandAloneSignatures[index].CustomAttributes = new CustomAttributeData*[StandAloneSignatures[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::StandAloneSignature &&
                    CustomAttributes[searchIndex].Parent.StandAloneSignature == &StandAloneSignatures[index])
                {
                    StandAloneSignatures[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= EventCount; ++index)
    {
        if (Events[index].CustomAttributeCount > 0)
        {
            Events[index].CustomAttributes = new CustomAttributeData*[Events[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::Event &&
                    CustomAttributes[searchIndex].Parent.Event == &Events[index])
                {
                    Events[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= MethodSemanticsCount; ++index)
    {
		switch(MethodSemantics[index].TypeOfAssociation)
		{
		case HasSemanticsType::Event:
			MethodSemantics[index].Association.Event->MethodSemantics = &MethodSemantics[index];
			break;
		case HasSemanticsType::Property:
			MethodSemantics[index].Association.Property->MethodSemantics = &MethodSemantics[index];
			break;
		default:
			Panic("Unknown MethodSemantics Association Table");
			break;
		}
    }

    for (uint32_t index = 1; index <= PropertyCount; ++index)
    {
        if (Properties[index].CustomAttributeCount > 0)
        {
            Properties[index].CustomAttributes = new CustomAttributeData*[Properties[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::Property &&
                    CustomAttributes[searchIndex].Parent.Property == &Properties[index])
                {
                    Properties[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= ModuleReferenceCount; ++index)
    {
        if (ModuleReferences[index].CustomAttributeCount > 0)
        {
            ModuleReferences[index].CustomAttributes = new CustomAttributeData*[ModuleReferences[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::ModuleReference &&
                    CustomAttributes[searchIndex].Parent.ModuleReference == &ModuleReferences[index])
                {
                    ModuleReferences[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
        if (ModuleReferences[index].MemberReferenceCount > 0)
        {
            ModuleReferences[index].MemberReferences = new MemberReferenceData*[ModuleReferences[index].MemberReferenceCount];
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= MemberReferenceCount; ++searchIndex)
            {
                if (MemberReferences[searchIndex].TypeOfParent == MemberRefParentType::ModuleReference &&
                    MemberReferences[searchIndex].Parent.ModuleReference == &ModuleReferences[index])
                {
                    ModuleReferences[index].MemberReferences[memberReferenceIndex] = &MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= TypeSpecificationCount; ++index)
    {
        if (TypeSpecifications[index].CustomAttributeCount > 0)
        {
            TypeSpecifications[index].CustomAttributes = new CustomAttributeData*[TypeSpecifications[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::TypeSpecification &&
                    CustomAttributes[searchIndex].Parent.TypeSpecification == &TypeSpecifications[index])
                {
                    TypeSpecifications[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }

        if (TypeSpecifications[index].MemberReferenceCount > 0)
        {
            TypeSpecifications[index].MemberReferences = new MemberReferenceData*[TypeSpecifications[index].MemberReferenceCount];
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= MemberReferenceCount; ++searchIndex)
            {
                if (MemberReferences[searchIndex].TypeOfParent == MemberRefParentType::TypeSpecification &&
                    MemberReferences[searchIndex].Parent.TypeSpecification == &TypeSpecifications[index])
                {
                    TypeSpecifications[index].MemberReferences[memberReferenceIndex] = &MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
	}

    for (uint32_t index = 1; index <= AssemblyDefinitionCount; ++index)
    {
        if (AssemblyDefinitions[index].CustomAttributeCount > 0)
        {
            AssemblyDefinitions[index].CustomAttributes = new CustomAttributeData*[AssemblyDefinitions[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::AssemblyDefinition &&
                    CustomAttributes[searchIndex].Parent.AssemblyDefinition == &AssemblyDefinitions[index])
                {
                    AssemblyDefinitions[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= AssemblyReferenceCount; ++index)
    {
        if (AssemblyReferences[index].CustomAttributeCount > 0)
        {
            AssemblyReferences[index].CustomAttributes = new CustomAttributeData*[AssemblyReferences[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::AssemblyReference &&
                    CustomAttributes[searchIndex].Parent.AssemblyReference == &AssemblyReferences[index])
                {
                    AssemblyReferences[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= FileCount; ++index)
    {
        if (Files[index].CustomAttributeCount > 0)
        {
            Files[index].CustomAttributes = new CustomAttributeData*[Files[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::File &&
                    CustomAttributes[searchIndex].Parent.File == &Files[index])
                {
                    Files[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= ExportedTypeCount; ++index)
    {
        if (ExportedTypes[index].CustomAttributeCount > 0)
        {
            ExportedTypes[index].CustomAttributes = new CustomAttributeData*[ExportedTypes[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::ExportedType &&
                    CustomAttributes[searchIndex].Parent.ExportedType == &ExportedTypes[index])
                {
                    ExportedTypes[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= ManifestResourceCount; ++index)
    {
        if (ManifestResources[index].CustomAttributeCount > 0)
        {
            ManifestResources[index].CustomAttributes = new CustomAttributeData*[ManifestResources[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::ManifestResource &&
                    CustomAttributes[searchIndex].Parent.ManifestResource == &ManifestResources[index])
                {
                    ManifestResources[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= GenericParameterCount; ++index)
    {
        if (GenericParameters[index].CustomAttributeCount > 0)
        {
            GenericParameters[index].CustomAttributes = new CustomAttributeData*[GenericParameters[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::GenericParameter &&
                    CustomAttributes[searchIndex].Parent.GenericParameter == &GenericParameters[index])
                {
                    GenericParameters[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
        if (GenericParameters[index].GenericParameterConstraintCount > 0)
        {
            GenericParameters[index].GenericParameterConstraints = new GenericParameterConstraintData*[GenericParameters[index].GenericParameterConstraintCount];
            for (uint32_t searchIndex = 1, genericParameterConstraintIndex = 0; searchIndex <= GenericParameterConstraintCount; ++searchIndex)
            {
                if (GenericParameterConstraints[searchIndex].Owner == &GenericParameters[index])
                {
                    GenericParameters[index].GenericParameterConstraints[genericParameterConstraintIndex] = &GenericParameterConstraints[searchIndex];
                    ++genericParameterConstraintIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= MethodSpecificationCount; ++index)
    {
        if (MethodSpecifications[index].CustomAttributeCount > 0)
        {
            MethodSpecifications[index].CustomAttributes = new CustomAttributeData*[MethodSpecifications[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::MethodSpecification &&
                    CustomAttributes[searchIndex].Parent.MethodSpecification == &MethodSpecifications[index])
                {
                    MethodSpecifications[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }

    for (uint32_t index = 1; index <= GenericParameterConstraintCount; ++index)
    {
        if (GenericParameterConstraints[index].CustomAttributeCount > 0)
        {
            GenericParameterConstraints[index].CustomAttributes = new CustomAttributeData*[GenericParameterConstraints[index].CustomAttributeCount];
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= CustomAttributeCount; ++searchIndex)
            {
                if (CustomAttributes[searchIndex].TypeOfParent == HasCustomAttributeType::GenericParameterConstraint &&
                    CustomAttributes[searchIndex].Parent.GenericParameterConstraint == &GenericParameterConstraints[index])
                {
                    GenericParameterConstraints[index].CustomAttributes[customAttributeIndex] = &CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
}
