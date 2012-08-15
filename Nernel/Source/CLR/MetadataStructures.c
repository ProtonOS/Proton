#include <CLR/MetadataStructures.h>

// AssemblyDefinition

uint8_t* AssemblyDefinition_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_AssemblyDefinition)) != 0)
    {
        pFile->AssemblyDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->AssemblyDefinitions = (AssemblyDefinition*)calloc(1, (pFile->AssemblyDefinitionCount + 1) * sizeof(AssemblyDefinition));
    }
    return pTableData;
}

void AssemblyDefinition_Cleanup(CLIFile* pFile)
{
    if (pFile->AssemblyDefinitions)
    {
        for (uint32_t index = 1; index <= pFile->AssemblyDefinitionCount; ++index)
        {
            if (pFile->AssemblyDefinitions[index].CustomAttributes) free(pFile->AssemblyDefinitions[index].CustomAttributes);
        }
        free(pFile->AssemblyDefinitions);
        pFile->AssemblyDefinitions = NULL;
    }
}

uint8_t* AssemblyDefinition_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->AssemblyDefinitionCount; ++index)
    {
        pFile->AssemblyDefinitions[index].TableIndex = index;
        pFile->AssemblyDefinitions[index].HashAlgorithmID = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyDefinitions[index].MajorVersion = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].MinorVersion = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].Build = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].Revision = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyDefinitions[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyDefinitions[index].PublicKey = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->AssemblyDefinitions[index].PublicKeyLength);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyDefinitions[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyDefinitions[index].Culture = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}


// AssemblyOperatingSystem

uint8_t* AssemblyOperatingSystem_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_AssemblyOperatingSystem)) != 0)
    {
        pFile->AssemblyOperatingSystemCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->AssemblyOperatingSystems = (AssemblyOperatingSystem*)calloc(pFile->AssemblyOperatingSystemCount + 1, sizeof(AssemblyOperatingSystem));
    }
    return pTableData;
}

void AssemblyOperatingSystem_Cleanup(CLIFile* pFile)
{
    if (pFile->AssemblyOperatingSystems)
    {
        for (uint32_t index = 1; index <= pFile->AssemblyOperatingSystemCount; ++index)
        {
        }
        free(pFile->AssemblyOperatingSystems);
        pFile->AssemblyOperatingSystems = NULL;
    }
}

uint8_t* AssemblyOperatingSystem_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1; index <= pFile->AssemblyOperatingSystemCount; ++index)
    {
        pFile->AssemblyOperatingSystems[index].TableIndex = index;
        pFile->AssemblyOperatingSystems[index].PlatformID = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyOperatingSystems[index].MajorVersion = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyOperatingSystems[index].MinorVersion = *(uint32_t* )pTableData; pTableData += 4;
    }
    return pTableData;
}


// AssemblyProcessor

uint8_t* AssemblyProcessor_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_AssemblyProcessor)) != 0)
    {
        pFile->AssemblyProcessorCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->AssemblyProcessors = (AssemblyProcessor*)calloc(pFile->AssemblyProcessorCount + 1, sizeof(AssemblyProcessor));
    }
    return pTableData;
}

void AssemblyProcessor_Cleanup(CLIFile* pFile)
{
    if (pFile->AssemblyProcessors)
    {
        for (uint32_t index = 1; index <= pFile->AssemblyProcessorCount; ++index)
        {
        }
        free(pFile->AssemblyProcessors);
        pFile->AssemblyProcessors = NULL;
    }
}

uint8_t* AssemblyProcessor_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1; index <= pFile->AssemblyProcessorCount; ++index)
    {
        pFile->AssemblyProcessors[index].TableIndex = index;
        pFile->AssemblyProcessors[index].Processor = *(uint32_t* )pTableData; pTableData += 4;
    }
    return pTableData;
}


// AssemblyReference

uint8_t* AssemblyReference_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if (((unsigned long long)pFile->TablesHeader->PresentTables & (1ull << MetadataTable_AssemblyReference)) != 0)
    {
        pFile->AssemblyReferenceCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->AssemblyReferences = (AssemblyReference*)calloc(pFile->AssemblyReferenceCount + 1, sizeof(AssemblyReference));
    }
    return pTableData;
}

void AssemblyReference_Cleanup(CLIFile* pFile)
{
    if (pFile->AssemblyReferences)
    {
        for (uint32_t index = 1; index <= pFile->AssemblyReferenceCount; ++index)
        {
            if (pFile->AssemblyReferences[index].CustomAttributes) free(pFile->AssemblyReferences[index].CustomAttributes);
        }
        free(pFile->AssemblyReferences);
        pFile->AssemblyReferences = NULL;
    }
}

uint8_t* AssemblyReference_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->AssemblyReferenceCount; ++index)
    {
        pFile->AssemblyReferences[index].TableIndex = index;
        pFile->AssemblyReferences[index].MajorVersion = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyReferences[index].MinorVersion = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyReferences[index].Build = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyReferences[index].Revision = *(uint16_t* )pTableData; pTableData += 2;
        pFile->AssemblyReferences[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyReferences[index].PublicKeyOrToken = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->AssemblyReferences[index].PublicKeyOrTokenLength);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyReferences[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyReferences[index].Culture = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->AssemblyReferences[index].HashValue = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->AssemblyReferences[index].HashValueLength);
    }
    return pTableData;
}


// AssemblyReferenceOperatingSystem

uint8_t* AssemblyReferenceOperatingSystem_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_AssemblyReferenceOperatingSystem)) != 0)
    {
        pFile->AssemblyReferenceOperatingSystemCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->AssemblyReferenceOperatingSystems = (AssemblyReferenceOperatingSystem*)calloc(pFile->AssemblyReferenceOperatingSystemCount + 1, sizeof(AssemblyReferenceOperatingSystem));
    }
    return pTableData;
}

void AssemblyReferenceOperatingSystem_Cleanup(CLIFile* pFile)
{
    if (pFile->AssemblyReferenceOperatingSystems)
    {
        for (uint32_t index = 1; index <= pFile->AssemblyReferenceOperatingSystemCount; ++index)
        {
        }
        free(pFile->AssemblyReferenceOperatingSystems);
        pFile->AssemblyReferenceOperatingSystems = NULL;
    }
}

uint8_t* AssemblyReferenceOperatingSystem_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t assemblyReferenceIndex = 0;
    for (uint32_t index = 1; index <= pFile->AssemblyReferenceOperatingSystemCount; ++index)
    {
        pFile->AssemblyReferenceOperatingSystems[index].TableIndex = index;
        pFile->AssemblyReferenceOperatingSystems[index].PlatformID = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyReferenceOperatingSystems[index].MajorVersion = *(uint32_t* )pTableData; pTableData += 4;
        pFile->AssemblyReferenceOperatingSystems[index].MinorVersion = *(uint32_t* )pTableData; pTableData += 4;
        if (pFile->AssemblyReferenceCount > 0xFFFF) { assemblyReferenceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { assemblyReferenceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (assemblyReferenceIndex == 0 || assemblyReferenceIndex > pFile->AssemblyReferenceCount) Panic("AssemblyReferenceOperatingSystem_Load");
        pFile->AssemblyReferenceOperatingSystems[index].AssemblyReference = &pFile->AssemblyReferences[assemblyReferenceIndex];
    }
    return pTableData;
}


// AssemblyReferenceProcessor

uint8_t* AssemblyReferenceProcessor_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_AssemblyReferenceProcessor)) != 0)
    {
        pFile->AssemblyReferenceProcessorCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->AssemblyReferenceProcessors = (AssemblyReferenceProcessor*)calloc(pFile->AssemblyReferenceProcessorCount + 1, sizeof(AssemblyReferenceProcessor));
    }
    return pTableData;
}

void AssemblyReferenceProcessor_Cleanup(CLIFile* pFile)
{
    if (pFile->AssemblyReferenceProcessors)
    {
        for (uint32_t index = 1; index <= pFile->AssemblyReferenceProcessorCount; ++index)
        {
        }
        free(pFile->AssemblyReferenceProcessors);
        pFile->AssemblyReferenceProcessors = NULL;
    }
}

uint8_t* AssemblyReferenceProcessor_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t assemblyReferenceIndex = 0;
    for (uint32_t index = 1; index <= pFile->AssemblyReferenceProcessorCount; ++index)
    {
        pFile->AssemblyReferenceProcessors[index].TableIndex = index;
        pFile->AssemblyReferenceProcessors[index].Processor = *(uint32_t* )pTableData; pTableData += 4;
        if (pFile->AssemblyReferenceCount > 0xFFFF) { assemblyReferenceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { assemblyReferenceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (assemblyReferenceIndex == 0 || assemblyReferenceIndex > pFile->AssemblyReferenceCount) Panic("AssemblyReferenceProcessor_Load");
        pFile->AssemblyReferenceProcessors[index].AssemblyReference = &pFile->AssemblyReferences[assemblyReferenceIndex];
    }
    return pTableData;
}


// ClassLayout

uint8_t* ClassLayout_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_ClassLayout)) != 0)
    {
        pFile->ClassLayoutCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ClassLayouts = (ClassLayout*)calloc(pFile->ClassLayoutCount + 1, sizeof(ClassLayout));
    }
    return pTableData;
}

void ClassLayout_Cleanup(CLIFile* pFile)
{
    if (pFile->ClassLayouts)
    {
        for (uint32_t index = 1; index <= pFile->ClassLayoutCount; ++index)
        {
        }
        free(pFile->ClassLayouts);
        pFile->ClassLayouts = NULL;
    }
}

uint8_t* ClassLayout_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    for (uint32_t index = 1; index <= pFile->ClassLayoutCount; ++index)
    {
        pFile->ClassLayouts[index].TableIndex = index;
        pFile->ClassLayouts[index].PackingSize = *(uint16_t*)pTableData; pTableData += 2;
        pFile->ClassLayouts[index].ClassSize = *(uint32_t*)pTableData; pTableData += 4;
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (parentIndex == 0 || parentIndex > pFile->TypeDefinitionCount) Panic("ClassLayout_Load");
        pFile->ClassLayouts[index].Parent = &pFile->TypeDefinitions[parentIndex];
    }
    return pTableData;
}


// Constant

uint8_t* Constant_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_Constant)) != 0)
    {
        pFile->ConstantCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Constants = (Constant*)calloc(pFile->ConstantCount + 1, sizeof(Constant));
    }
    return pTableData;
}

void Constant_Cleanup(CLIFile* pFile)
{
    if (pFile->Constants)
    {
        for (uint32_t index = 1; index <= pFile->ConstantCount; ++index)
        {
        }
        free(pFile->Constants);
        pFile->Constants = NULL;
    }
}

uint8_t* Constant_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ConstantCount; ++index)
    {
        pFile->Constants[index].TableIndex = index;
        pFile->Constants[index].Type = *pTableData; pTableData += 2; // 1 unused padding byte
        if (pFile->FieldCount > HasConstantType_MaxRows16Bit ||
            pFile->ParameterCount > HasConstantType_MaxRows16Bit ||
            pFile->PropertyCount > HasConstantType_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Constants[index].TypeOfParent = parentIndex & HasConstantType_Mask;
        parentRow = parentIndex >> HasConstantType_Bits;
        switch (pFile->Constants[index].TypeOfParent)
        {
        case HasConstantType_Field:
            if (parentRow == 0 || parentRow > pFile->FieldCount) Panic("Constant_Load Field");
            pFile->Constants[index].Parent.Field = &pFile->Fields[parentRow];
            break;
        case HasConstantType_Parameter:
            if (parentRow == 0 || parentRow > pFile->ParameterCount) Panic("Constant_Load Parameter");
            pFile->Constants[index].Parent.Parameter = &pFile->Parameters[parentRow];
            break;
        case HasConstantType_Property:
            if (parentRow == 0 || parentRow > pFile->PropertyCount) Panic("Constant_Load Property");
            pFile->Constants[index].Parent.Property = &pFile->Properties[parentRow];
            break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Constants[index].Value = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->Constants[index].ValueLength);
    }
    return pTableData;
}


// CustomAttribute

uint8_t* CustomAttribute_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_CustomAttribute)) != 0)
    {
        pFile->CustomAttributeCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->CustomAttributes = (CustomAttribute*)calloc(pFile->CustomAttributeCount + 1, sizeof(CustomAttribute));
    }
    return pTableData;
}

void CustomAttribute_Cleanup(CLIFile* pFile)
{
    if (pFile->CustomAttributes)
    {
        for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
        {
        }
        free(pFile->CustomAttributes);
        pFile->CustomAttributes = NULL;
    }
}

uint8_t* CustomAttribute_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    uint32_t typeIndex = 0;
    uint32_t typeRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->CustomAttributeCount; ++index)
    {
        pFile->CustomAttributes[index].TableIndex = index;
        if (pFile->MethodDefinitionCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->FieldCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->TypeReferenceCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->TypeDefinitionCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->ParameterCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->InterfaceImplementationCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->MemberReferenceCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->ModuleDefinitionCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->DeclSecurityCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->PropertyCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->EventCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->StandAloneSignatureCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->ModuleReferenceCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->TypeSpecificationCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->AssemblyDefinitionCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->AssemblyReferenceCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->FileCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->ExportedTypeCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->ManifestResourceCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->GenericParameterCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->GenericParameterConstraintCount > HasCustomAttributeType_MaxRows16Bit ||
            pFile->MethodSpecificationCount > HasCustomAttributeType_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->CustomAttributes[index].TypeOfParent = parentIndex & HasCustomAttributeType_Mask;
        parentRow = parentIndex >> HasCustomAttributeType_Bits;
        switch (pFile->CustomAttributes[index].TypeOfParent)
        {
        case HasCustomAttributeType_MethodDefinition:
            if (parentRow == 0 || parentRow > pFile->MethodDefinitionCount) Panic("CustomAttribute_Load MethodDefinition");
            pFile->CustomAttributes[index].Parent.MethodDefinition = &pFile->MethodDefinitions[parentRow];
            break;
        case HasCustomAttributeType_Field:
            if (parentRow == 0 || parentRow > pFile->FieldCount) Panic("CustomAttribute_Load Field");
            pFile->CustomAttributes[index].Parent.Field = &pFile->Fields[parentRow];
            break;
        case HasCustomAttributeType_TypeReference:
            if (parentRow == 0 || parentRow > pFile->TypeReferenceCount) Panic("CustomAttribute_Load TypeReference");
            pFile->CustomAttributes[index].Parent.TypeReference = &pFile->TypeReferences[parentRow];
            break;
        case HasCustomAttributeType_TypeDefinition:
            if (parentRow == 0 || parentRow > pFile->TypeDefinitionCount) Panic("CustomAttribute_Load TypeDefinition");
            pFile->CustomAttributes[index].Parent.TypeDefinition = &pFile->TypeDefinitions[parentRow];
            break;
        case HasCustomAttributeType_Parameter:
            if (parentRow == 0 || parentRow > pFile->ParameterCount) Panic("CustomAttribute_Load Parameter");
            pFile->CustomAttributes[index].Parent.Parameter = &pFile->Parameters[parentRow];
            break;
        case HasCustomAttributeType_InterfaceImplementation:
            if (parentRow == 0 || parentRow > pFile->InterfaceImplementationCount) Panic("CustomAttribute_Load InterfaceImplementation");
            pFile->CustomAttributes[index].Parent.InterfaceImplementation = &pFile->InterfaceImplementations[parentRow];
            break;
        case HasCustomAttributeType_MemberReference:
            if (parentRow == 0 || parentRow > pFile->MemberReferenceCount) Panic("CustomAttribute_Load MemberReference");
            pFile->CustomAttributes[index].Parent.MemberReference = &pFile->MemberReferences[parentRow];
            break;
        case HasCustomAttributeType_ModuleDefinition:
            if (parentRow == 0 || parentRow > pFile->ModuleDefinitionCount) Panic("CustomAttribute_Load ModuleDefinition");
            pFile->CustomAttributes[index].Parent.ModuleDefinition = &pFile->ModuleDefinitions[parentRow];
            break;
        case HasCustomAttributeType_DeclSecurity:
            if (parentRow == 0 || parentRow > pFile->DeclSecurityCount) Panic("CustomAttribute_Load DeclSecurity");
            pFile->CustomAttributes[index].Parent.DeclSecurity = &pFile->DeclSecurities[parentRow];
            break;
        case HasCustomAttributeType_Property:
            if (parentRow == 0 || parentRow > pFile->PropertyCount) Panic("CustomAttribute_Load Property");
            pFile->CustomAttributes[index].Parent.Property = &pFile->Properties[parentRow];
            break;
        case HasCustomAttributeType_Event:
            if (parentRow == 0 || parentRow > pFile->EventCount) Panic("CustomAttribute_Load Event");
            pFile->CustomAttributes[index].Parent.Event = &pFile->Events[parentRow];
            break;
        case HasCustomAttributeType_StandAloneSignature:
            if (parentRow == 0 || parentRow > pFile->StandAloneSignatureCount) Panic("CustomAttribute_Load StandAloneSignature");
            pFile->CustomAttributes[index].Parent.StandAloneSignature = &pFile->StandAloneSignatures[parentRow];
            break;
        case HasCustomAttributeType_ModuleReference:
            if (parentRow == 0 || parentRow > pFile->ModuleReferenceCount) Panic("CustomAttribute_Load ModuleReference");
            pFile->CustomAttributes[index].Parent.ModuleReference = &pFile->ModuleReferences[parentRow];
            break;
        case HasCustomAttributeType_TypeSpecification:
            if (parentRow == 0 || parentRow > pFile->TypeSpecificationCount) Panic("CustomAttribute_Load TypeSpecification");
            pFile->CustomAttributes[index].Parent.TypeSpecification = &pFile->TypeSpecifications[parentRow];
            break;
        case HasCustomAttributeType_AssemblyDefinition:
            if (parentRow == 0 || parentRow > pFile->AssemblyDefinitionCount) Panic("CustomAttribute_Load AssemblyDefinition");
            pFile->CustomAttributes[index].Parent.AssemblyDefinition = &pFile->AssemblyDefinitions[parentRow];
            break;
        case HasCustomAttributeType_AssemblyReference:
            if (parentRow == 0 || parentRow > pFile->AssemblyReferenceCount) Panic("CustomAttribute_Load AssemblyReference");
            pFile->CustomAttributes[index].Parent.AssemblyReference = &pFile->AssemblyReferences[parentRow];
            break;
        case HasCustomAttributeType_File:
            if (parentRow == 0 || parentRow > pFile->FileCount) Panic("CustomAttribute_Load File");
            pFile->CustomAttributes[index].Parent.File = &pFile->Files[parentRow];
            break;
        case HasCustomAttributeType_ExportedType:
            if (parentRow == 0 || parentRow > pFile->ExportedTypeCount) Panic("CustomAttribute_Load ExportedType");
            pFile->CustomAttributes[index].Parent.ExportedType = &pFile->ExportedTypes[parentRow];
            break;
        case HasCustomAttributeType_ManifestResource:
            if (parentRow == 0 || parentRow > pFile->ManifestResourceCount) Panic("CustomAttribute_Load ManifestResource");
            pFile->CustomAttributes[index].Parent.ManifestResource = &pFile->ManifestResources[parentRow];
            break;
        case HasCustomAttributeType_GenericParameter:
            if (parentRow == 0 || parentRow > pFile->GenericParameterCount) Panic("CustomAttribute_Load GenericParameter");
            pFile->CustomAttributes[index].Parent.GenericParameter = &pFile->GenericParameters[parentRow];
            break;
        case HasCustomAttributeType_GenericParameterConstraint:
            if (parentRow == 0 || parentRow > pFile->GenericParameterConstraintCount) Panic("CustomAttribute_Load GenericParameterConstraint");
            pFile->CustomAttributes[index].Parent.GenericParameterConstraint = &pFile->GenericParameterConstraints[parentRow];
            break;
        case HasCustomAttributeType_MethodSpecification:
            if (parentRow == 0 || parentRow > pFile->MethodSpecificationCount) Panic("CustomAttribute_Load MethodSpecification");
            pFile->CustomAttributes[index].Parent.MethodSpecification = &pFile->MethodSpecifications[parentRow];
            break;
        default: break;
        }
        if (pFile->MethodDefinitionCount > CustomAttributeType_MaxRows16Bit ||
            pFile->MemberReferenceCount > CustomAttributeType_MaxRows16Bit) { typeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { typeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->CustomAttributes[index].TypeOfType = typeIndex & CustomAttributeType_Mask;
        typeRow = typeIndex >> CustomAttributeType_Bits;
        switch (pFile->CustomAttributes[index].TypeOfType)
        {
        case CustomAttributeType_MethodDefinition:
            if (typeRow == 0 || typeRow > pFile->MethodDefinitionCount) Panic("CustomAttribute_Load Type MethodDefinition");
            pFile->CustomAttributes[index].Type.MethodDefinition = &pFile->MethodDefinitions[typeRow];
            break;
        case CustomAttributeType_MemberReference:
            if (typeRow == 0 || typeRow > pFile->MemberReferenceCount) Panic("CustomAttribute_Load Type MemberReference");
            pFile->CustomAttributes[index].Type.MemberReference = &pFile->MemberReferences[typeRow];
            break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->CustomAttributes[index].Value = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->CustomAttributes[index].ValueLength);
    }
    return pTableData;
}


// DeclSecurity

uint8_t* DeclSecurity_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_DeclSecurity)) != 0)
    {
        pFile->DeclSecurityCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->DeclSecurities = (DeclSecurity*)calloc(pFile->DeclSecurityCount + 1, sizeof(DeclSecurity));
    }
    return pTableData;
}

void DeclSecurity_Cleanup(CLIFile* pFile)
{
    if (pFile->DeclSecurities)
    {
        for (uint32_t index = 1; index <= pFile->DeclSecurityCount; ++index)
        {
            if (pFile->DeclSecurities[index].CustomAttributes) free(pFile->DeclSecurities[index].CustomAttributes);
        }
        free(pFile->DeclSecurities);
        pFile->DeclSecurities = NULL;
    }
}

uint8_t* DeclSecurity_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->DeclSecurityCount; ++index)
    {
        pFile->DeclSecurities[index].TableIndex = index;
        pFile->DeclSecurities[index].Action = *(uint16_t*)pTableData; pTableData += 2;

        if (pFile->TypeDefinitionCount > HasDeclSecurityType_MaxRows16Bit ||
            pFile->MethodDefinitionCount > HasDeclSecurityType_MaxRows16Bit ||
            pFile->AssemblyDefinitionCount > HasDeclSecurityType_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->DeclSecurities[index].TypeOfParent = parentIndex & HasDeclSecurityType_Mask;
        parentRow = parentIndex >> HasDeclSecurityType_Bits;
        switch (pFile->DeclSecurities[index].TypeOfParent)
        {
        case HasDeclSecurityType_TypeDefinition:
            if (parentRow == 0 || parentRow > pFile->TypeDefinitionCount) Panic("DeclSecurity_Load TypeDefinition");
            pFile->DeclSecurities[index].Parent.TypeDefinition = &pFile->TypeDefinitions[parentRow];
            break;
        case HasDeclSecurityType_MethodDefinition:
            if (parentRow == 0 || parentRow > pFile->MethodDefinitionCount) Panic("DeclSecurity_Load MethodDefinition");
            pFile->DeclSecurities[index].Parent.MethodDefinition = &pFile->MethodDefinitions[parentRow];
            break;
        case HasDeclSecurityType_AssemblyDefinition:
            if (parentRow == 0 || parentRow > pFile->AssemblyDefinitionCount) Panic("DeclSecurity_Load AssemblyDefinition");
            pFile->DeclSecurities[index].Parent.AssemblyDefinition = &pFile->AssemblyDefinitions[parentRow];
            break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->DeclSecurities[index].PermissionSet = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->DeclSecurities[index].PermissionSetLength);
    }
    return pTableData;
}


// Event

uint8_t* Event_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_Event)) != 0)
    {
        pFile->EventCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Events = (Event*)calloc(pFile->EventCount + 1, sizeof(Event));
    }
    return pTableData;
}

void Event_Cleanup(CLIFile* pFile)
{
    if (pFile->Events)
    {
        for (uint32_t index = 1; index <= pFile->EventCount; ++index)
        {
            if (pFile->Events[index].CustomAttributes) free(pFile->Events[index].CustomAttributes);
        }
        free(pFile->Events);
        pFile->Events = NULL;
    }
}

uint8_t* Event_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t eventTypeIndex = 0;
    uint32_t eventTypeRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->EventCount; ++index)
    {
        pFile->Events[index].TableIndex = index;
        pFile->Events[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Events[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if (pFile->TypeDefinitionCount > TypeDefRefOrSpecType_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefRefOrSpecType_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefRefOrSpecType_MaxRows16Bit) { eventTypeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { eventTypeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Events[index].TypeOfEventType = eventTypeIndex & TypeDefRefOrSpecType_Mask;
        eventTypeRow = eventTypeIndex >> TypeDefRefOrSpecType_Bits;
        switch (pFile->Events[index].TypeOfEventType)
        {
        case TypeDefRefOrSpecType_TypeDefinition:
            if (eventTypeRow == 0 || eventTypeRow > pFile->TypeDefinitionCount) Panic("Event_Load TypeDefinition");
            pFile->Events[index].EventType.TypeDefinition = &pFile->TypeDefinitions[eventTypeRow];
            break;
        case TypeDefRefOrSpecType_TypeReference:
            if (eventTypeRow == 0 || eventTypeRow > pFile->TypeReferenceCount) Panic("Event_Load TypeReference");
            pFile->Events[index].EventType.TypeReference = &pFile->TypeReferences[eventTypeRow];
            break;
        case TypeDefRefOrSpecType_TypeSpecification:
            if (eventTypeRow == 0 || eventTypeRow > pFile->TypeSpecificationCount) Panic("Event_Load TypeSpecification");
            pFile->Events[index].EventType.TypeSpecification = &pFile->TypeSpecifications[eventTypeRow];
            break;
        default: break;
        }
    }
    return pTableData;
}


// EventMap

uint8_t* EventMap_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_EventMap)) != 0)
    {
        pFile->EventMapCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->EventMaps = (EventMap*)calloc(pFile->EventMapCount + 1, sizeof(EventMap));
    }
    return pTableData;
}

void EventMap_Cleanup(CLIFile* pFile)
{
    if (pFile->EventMaps)
    {
        for (uint32_t index = 1; index <= pFile->EventMapCount; ++index)
        {
        }
        free(pFile->EventMaps);
        pFile->EventMaps = NULL;
    }
}

uint8_t* EventMap_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t eventListIndex = 0;
    uint32_t* eventListIndexes = (uint32_t*)calloc(pFile->EventMapCount + 1, sizeof(uint32_t));
    for (uint32_t index = 1; index <= pFile->EventMapCount; ++index)
    {
        pFile->EventMaps[index].TableIndex = index;
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (parentIndex == 0 || parentIndex > pFile->TypeDefinitionCount) Panic("EventMap_Load TypeDefinition");
        pFile->EventMaps[index].Parent = &pFile->TypeDefinitions[parentIndex];
        if (pFile->EventCount > 0xFFFF) { eventListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { eventListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (eventListIndex == 0 || eventListIndex > pFile->EventCount + 1) Panic("EventMap_Load Event");
        if (eventListIndex <= pFile->EventCount)
        {
            pFile->EventMaps[index].EventList = &pFile->Events[eventListIndex];
            eventListIndexes[index] = eventListIndex;
        }
    }
    uint32_t eventListCount = 0;
    for (uint32_t index = 1, used = 0; index <= pFile->EventMapCount; ++index, used += eventListCount)
    {
        if (index == pFile->EventMapCount || eventListIndexes[index + 1] == 0) eventListCount = pFile->EventCount - used;
        else eventListCount = eventListIndexes[index + 1] - eventListIndexes[index];
        pFile->EventMaps[index].EventListCount = eventListCount;
    }
    free(eventListIndexes);
    return pTableData;
}


// ExportedType

uint8_t* ExportedType_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_ExportedType)) != 0)
    {
        pFile->ExportedTypeCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ExportedTypes = (ExportedType*)calloc(pFile->ExportedTypeCount + 1, sizeof(ExportedType));
    }
    return pTableData;
}

void ExportedType_Cleanup(CLIFile* pFile)
{
    if (pFile->ExportedTypes)
    {
        for (uint32_t index = 1; index <= pFile->ExportedTypeCount; ++index)
        {
            if (pFile->ExportedTypes[index].CustomAttributes) free(pFile->ExportedTypes[index].CustomAttributes);
        }
        free(pFile->ExportedTypes);
        pFile->ExportedTypes = NULL;
    }
}

uint8_t* ExportedType_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t typeDefinitionIndex = 0;
    uint32_t implementationIndex = 0;
    uint32_t implementationRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ExportedTypeCount; ++index)
    {
        pFile->ExportedTypes[index].TableIndex = index;
        pFile->ExportedTypes[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if (pFile->TypeDefinitionCount > 0xFFFF) { typeDefinitionIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { typeDefinitionIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (typeDefinitionIndex == 0 || typeDefinitionIndex > pFile->TypeDefinitionCount) Panic("ExportedType_Load TypeDefinition");
        pFile->ExportedTypes[index].TypeDefinitionID = &pFile->TypeDefinitions[typeDefinitionIndex];
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ExportedTypes[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ExportedTypes[index].Namespace = (const char*)(pFile->StringsHeap + heapIndex);

        if (pFile->FileCount > ImplementationType_MaxRows16Bit ||
            pFile->AssemblyReferenceCount > ImplementationType_MaxRows16Bit ||
            pFile->ExportedTypeCount > ImplementationType_MaxRows16Bit) { implementationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ExportedTypes[index].TypeOfImplementation = implementationIndex & ImplementationType_Mask;
        implementationRow = implementationIndex >> ImplementationType_Bits;
        switch (pFile->ExportedTypes[index].TypeOfImplementation)
        {
        case ImplementationType_File:
            if (implementationRow == 0 || implementationRow > pFile->FileCount) Panic("ExportedType_Load File");
            pFile->ExportedTypes[index].Implementation.File = &pFile->Files[implementationRow];
            break;
        case ImplementationType_AssemblyReference:
            if (implementationRow == 0 || implementationRow > pFile->AssemblyReferenceCount) Panic("ExportedType_Load AssemblyReference");
            pFile->ExportedTypes[index].Implementation.AssemblyReference = &pFile->AssemblyReferences[implementationRow];
            break;
        case ImplementationType_ExportedType:
            if (implementationRow == 0 || implementationRow > pFile->ExportedTypeCount) Panic("ExportedType_Load ExportedType");
            pFile->ExportedTypes[index].Implementation.ExportedType = &pFile->ExportedTypes[implementationRow];
            break;
        default: break;
        }
    }
    return pTableData;
}


// Field

uint8_t* Field_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_Field)) != 0)
    {
        pFile->FieldCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Fields = (Field*)calloc(pFile->FieldCount + 1, sizeof(Field));
    }
    return pTableData;
}

void Field_Cleanup(CLIFile* pFile)
{
    if (pFile->Fields)
    {
        for (uint32_t index = 1; index <= pFile->FieldCount; ++index)
        {
            if (pFile->Fields[index].CustomAttributes) free(pFile->Fields[index].CustomAttributes);
        }
        free(pFile->Fields);
        pFile->Fields = NULL;
    }
}

uint8_t* Field_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->FieldCount; ++index)
    {
		pFile->Fields[index].File = pFile;
        pFile->Fields[index].TableIndex = index;
        pFile->Fields[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) 
		{
			heapIndex = *(uint32_t*)pTableData; pTableData += 4;
		}
        else
		{
			heapIndex = *(uint16_t*)pTableData; pTableData += 2;
		}
        pFile->Fields[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) 
		{
			heapIndex = *(uint32_t*)pTableData; pTableData += 4; 
		}
        else
		{
			heapIndex = *(uint16_t*)pTableData; pTableData += 2; 
		}
        pFile->Fields[index].Signature = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->Fields[index].SignatureLength);
    }

    return pTableData;
}


// FieldLayout

uint8_t* FieldLayout_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_FieldLayout)) != 0)
    {
        pFile->FieldLayoutCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->FieldLayouts = (FieldLayout*)calloc(pFile->FieldLayoutCount + 1, sizeof(FieldLayout));
    }
    return pTableData;
}

void FieldLayout_Cleanup(CLIFile* pFile)
{
    if (pFile->FieldLayouts)
    {
        for (uint32_t index = 1; index <= pFile->FieldLayoutCount; ++index)
        {
        }
        free(pFile->FieldLayouts);
        pFile->FieldLayouts = NULL;
    }
}

uint8_t* FieldLayout_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t fieldIndex = 0;
    for (uint32_t index = 1; index <= pFile->FieldLayoutCount; ++index)
    {
        pFile->FieldLayouts[index].TableIndex = index;
        pFile->FieldLayouts[index].Offset = *(uint32_t*)pTableData; pTableData += 4;
        if (pFile->FieldCount > 0xFFFF) { fieldIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (fieldIndex == 0 || fieldIndex > pFile->FieldCount) Panic("FieldLayout_Load Field");
        pFile->FieldLayouts[index].Field = &pFile->Fields[fieldIndex];
    }
    return pTableData;

}


// FieldMarshal

uint8_t* FieldMarshal_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_FieldMarshal)) != 0)
    {
        pFile->FieldMarshalCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->FieldMarshals = (FieldMarshal*)calloc(pFile->FieldMarshalCount + 1, sizeof(FieldMarshal));
    }
    return pTableData;
}

void FieldMarshal_Cleanup(CLIFile* pFile)
{
    if (pFile->FieldMarshals)
    {
        for (uint32_t index = 1; index <= pFile->FieldMarshalCount; ++index)
        {
        }
        free(pFile->FieldMarshals);
        pFile->FieldMarshals = NULL;
    }
}

uint8_t* FieldMarshal_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->FieldMarshalCount; ++index)
    {
        pFile->FieldMarshals[index].TableIndex = index;
        if (pFile->FieldCount > HasFieldMarshalType_MaxRows16Bit ||
            pFile->ParameterCount > HasFieldMarshalType_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->FieldMarshals[index].TypeOfParent = parentIndex & HasFieldMarshalType_Mask;
        parentRow = parentIndex >> HasFieldMarshalType_Bits;
        switch (pFile->FieldMarshals[index].TypeOfParent)
        {
        case HasFieldMarshalType_Field:
            if (parentRow == 0 || parentRow > pFile->FieldCount) Panic("FieldMarshal_Load Field");
            pFile->FieldMarshals[index].Parent.Field = &pFile->Fields[parentRow];
            break;
        case HasFieldMarshalType_Parameter:
            if (parentRow == 0 || parentRow > pFile->ParameterCount) Panic("FieldMarshal_Load Parameter");
            pFile->FieldMarshals[index].Parent.Parameter = &pFile->Parameters[parentRow];
            break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->FieldMarshals[index].NativeType = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->FieldMarshals[index].NativeTypeLength);
    }
    return pTableData;
}


// FieldRVA

uint8_t* FieldRVA_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_FieldRVA)) != 0)
    {
        pFile->FieldRVACount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->FieldRVAs = (FieldRVA*)calloc(pFile->FieldRVACount + 1, sizeof(FieldRVA));
    }
    return pTableData;
}

void FieldRVA_Cleanup(CLIFile* pFile)
{
    if (pFile->FieldRVAs)
    {
        for (uint32_t index = 1; index <= pFile->FieldRVACount; ++index)
        {
        }
        free(pFile->FieldRVAs);
        pFile->FieldRVAs = NULL;
    }
}

uint8_t* FieldRVA_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t fieldInitialValueVirtualAddress = 0;
    PESectionHeader* fieldInitialValueSectionHeader = NULL;
    const uint8_t* fieldInitialValue = NULL;
    uint32_t fieldIndex = 0;
    for (uint32_t index = 1; index <= pFile->FieldRVACount; ++index)
    {
        pFile->FieldRVAs[index].TableIndex = index;
        fieldInitialValueVirtualAddress = *(uint32_t*)pTableData; pTableData += 4;
        fieldInitialValueSectionHeader = CLIFile_GetSection(pFile->SectionHeaders, pFile->NTHeader->NumberOfSections, fieldInitialValueVirtualAddress);
        fieldInitialValue = pFile->Data + fieldInitialValueSectionHeader->PointerToRawData + (fieldInitialValueVirtualAddress - fieldInitialValueSectionHeader->VirtualAddress);
        pFile->FieldRVAs[index].InitialValue = fieldInitialValue;

        if (pFile->FieldCount > 0xFFFF) { fieldIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (fieldIndex == 0 || fieldIndex > pFile->FieldCount) Panic("FieldRVA_Load Field");
        pFile->FieldRVAs[index].Field = &pFile->Fields[fieldIndex];
    }
    return pTableData;
}


// File

uint8_t* File_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_File)) != 0)
    {
        pFile->FileCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Files = (File*)calloc(pFile->FileCount + 1, sizeof(File));
    }
    return pTableData;
}

void File_Cleanup(CLIFile* pFile)
{
    if (pFile->Files)
    {
        for (uint32_t index = 1; index <= pFile->FileCount; ++index)
        {
            if (pFile->Files[index].CustomAttributes) free(pFile->Files[index].CustomAttributes);
        }
        free(pFile->Files);
        pFile->Files = NULL;
    }
}

uint8_t* File_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->FileCount; ++index)
    {
        pFile->Files[index].TableIndex = index;
        pFile->Files[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Files[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Files[index].HashValue = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->Files[index].HashValueLength);
    }
    return pTableData;
}


// GenericParameter

uint8_t* GenericParameter_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_GenericParameter)) != 0)
    {
        pFile->GenericParameterCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->GenericParameters = (GenericParameter*)calloc(pFile->GenericParameterCount + 1, sizeof(GenericParameter));
    }
    return pTableData;
}

void GenericParameter_Cleanup(CLIFile* pFile)
{
    if (pFile->GenericParameters)
    {
        for (uint32_t index = 1; index <= pFile->GenericParameterCount; ++index)
        {
            if (pFile->GenericParameters[index].CustomAttributes) free(pFile->GenericParameters[index].CustomAttributes);
            if (pFile->GenericParameters[index].GenericParameterConstraints) free(pFile->GenericParameters[index].GenericParameterConstraints);
        }
        free(pFile->GenericParameters);
        pFile->GenericParameters = NULL;
    }
}

uint8_t* GenericParameter_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t ownerIndex = 0;
    uint32_t ownerRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->GenericParameterCount; ++index)
    {
        pFile->GenericParameters[index].TableIndex = index;
        pFile->GenericParameters[index].Index = *(uint16_t* )pTableData; pTableData += 2;
        pFile->GenericParameters[index].Flags = *(uint16_t* )pTableData; pTableData += 2;
        if (pFile->TypeDefinitionCount > TypeOrMethodDefType_MaxRows16Bit ||
            pFile->MethodDefinitionCount > TypeOrMethodDefType_MaxRows16Bit) { ownerIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { ownerIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->GenericParameters[index].TypeOfOwner = ownerIndex & TypeOrMethodDefType_Mask;
        ownerRow = ownerIndex >> TypeOrMethodDefType_Bits;
        switch (pFile->GenericParameters[index].TypeOfOwner)
        {
        case TypeOrMethodDefType_TypeDefinition:
            if (ownerRow == 0 || ownerRow > pFile->TypeDefinitionCount) Panic("GenericParameter_Load TypeDefinition");
            pFile->GenericParameters[index].Owner.TypeDefinition = &pFile->TypeDefinitions[ownerRow];
            break;
        case TypeOrMethodDefType_MethodDefinition:
            if (ownerRow == 0 || ownerRow > pFile->MethodDefinitionCount) Panic("GenericParameter_Load MethodDefinition");
            pFile->GenericParameters[index].Owner.MethodDefinition = &pFile->MethodDefinitions[ownerRow];
            break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->GenericParameters[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}


// GenericParameterConstraint

uint8_t* GenericParameterConstraint_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_GenericParameterConstraint)) != 0)
    {
        pFile->GenericParameterConstraintCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->GenericParameterConstraints = (GenericParameterConstraint*)calloc(pFile->GenericParameterConstraintCount + 1, sizeof(GenericParameterConstraint));
    }
    return pTableData;
}

void GenericParameterConstraint_Cleanup(CLIFile* pFile)
{
    if (pFile->GenericParameterConstraints)
    {
        for (uint32_t index = 1; index <= pFile->GenericParameterConstraintCount; ++index)
        {
            if (pFile->GenericParameterConstraints[index].CustomAttributes) free(pFile->GenericParameterConstraints[index].CustomAttributes);
        }
        free(pFile->GenericParameterConstraints);
        pFile->GenericParameterConstraints = NULL;
    }
}

uint8_t* GenericParameterConstraint_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t ownerIndex = 0;
    uint32_t constraintIndex = 0;
    uint32_t constraintRow = 0;
    for (uint32_t index = 1; index <= pFile->GenericParameterConstraintCount; ++index)
    {
        pFile->GenericParameterConstraints[index].TableIndex = index;
        if (pFile->GenericParameterCount > 0xFFFF) { ownerIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { ownerIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (ownerIndex == 0 || ownerIndex > pFile->GenericParameterCount) Panic("GenericParameterConstraint_Load GenericParameter");
        pFile->GenericParameterConstraints[index].Owner = &pFile->GenericParameters[ownerIndex];
        if (pFile->TypeDefinitionCount > TypeDefRefOrSpecType_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefRefOrSpecType_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefRefOrSpecType_MaxRows16Bit) { constraintIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { constraintIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->GenericParameterConstraints[index].TypeOfConstraint = constraintIndex & TypeDefRefOrSpecType_Mask;
        constraintRow = constraintIndex >> TypeDefRefOrSpecType_Bits;
        switch (pFile->GenericParameterConstraints[index].TypeOfConstraint)
        {
        case TypeDefRefOrSpecType_TypeDefinition:
            if (constraintRow == 0 || constraintRow > pFile->TypeDefinitionCount) Panic("GenericParameterConstraint_Load TypeDefinition");
            pFile->GenericParameterConstraints[index].Constraint.TypeDefinition = &pFile->TypeDefinitions[constraintRow];
            break;
        case TypeDefRefOrSpecType_TypeReference:
            if (constraintRow == 0 || constraintRow > pFile->TypeReferenceCount) Panic("GenericParameterConstraint_Load TypeReference");
            pFile->GenericParameterConstraints[index].Constraint.TypeReference = &pFile->TypeReferences[constraintRow];
            break;
        case TypeDefRefOrSpecType_TypeSpecification:
            if (constraintRow == 0 || constraintRow > pFile->TypeSpecificationCount) Panic("GenericParameterConstraint_Load TypeSpecification");
            pFile->GenericParameterConstraints[index].Constraint.TypeSpecification = &pFile->TypeSpecifications[constraintRow];
            break;
        default: break;
        }
    }
    return pTableData;
}


// ImplementationMap

uint8_t* ImplementationMap_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_ImplementationMap)) != 0)
    {
        pFile->ImplementationMapCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ImplementationMaps = (ImplementationMap*)calloc(pFile->ImplementationMapCount + 1, sizeof(ImplementationMap));
    }
    return pTableData;
}

void ImplementationMap_Cleanup(CLIFile* pFile)
{
    if (pFile->ImplementationMaps)
    {
        for (uint32_t index = 1; index <= pFile->ImplementationMapCount; ++index)
        {
        }
        free(pFile->ImplementationMaps);
        pFile->ImplementationMaps = NULL;
    }
}

uint8_t* ImplementationMap_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t memberForwardedIndex = 0;
    uint32_t memberForwardedRow = 0;
    uint32_t importScopeIndex = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ImplementationMapCount; ++index)
    {
        pFile->ImplementationMaps[index].TableIndex = index;
        pFile->ImplementationMaps[index].MappingFlags = *(uint16_t*)pTableData; pTableData += 2;

        if (pFile->FieldCount > MemberForwardedType_MaxRows16Bit ||
            pFile->MethodDefinitionCount > MemberForwardedType_MaxRows16Bit) { memberForwardedIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { memberForwardedIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ImplementationMaps[index].TypeOfMemberForwarded = memberForwardedIndex & MemberForwardedType_Mask;
        memberForwardedRow = memberForwardedIndex >> MemberForwardedType_Bits;
        switch (pFile->ImplementationMaps[index].TypeOfMemberForwarded)
        {
        case MemberForwardedType_Field:
            if (memberForwardedRow == 0 || memberForwardedRow > pFile->FieldCount) Panic("ImplementationMap_Load Field");
            pFile->ImplementationMaps[index].MemberForwarded.Field = &pFile->Fields[memberForwardedRow];
            break;
        case MemberForwardedType_MethodDefinition:
            if (memberForwardedRow == 0 || memberForwardedRow > pFile->MethodDefinitionCount) Panic("ImplementationMap_Load MethodDefinition");
            pFile->ImplementationMaps[index].MemberForwarded.MethodDefinition = &pFile->MethodDefinitions[memberForwardedRow];
            break;
        default: break;
        }

        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ImplementationMaps[index].ImportName = (const char*)(pFile->StringsHeap + heapIndex);

        if (pFile->ModuleReferenceCount > 0xFFFF) { importScopeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { importScopeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (importScopeIndex == 0 || importScopeIndex > pFile->ModuleReferenceCount) Panic("ImplementationMap_Load ModuleReference");
        pFile->ImplementationMaps[index].ImportScope = &pFile->ModuleReferences[importScopeIndex];
    }
    return pTableData;
}


// InterfaceImplementation

uint8_t* InterfaceImplementation_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_InterfaceImplementation)) != 0)
    {
        pFile->InterfaceImplementationCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->InterfaceImplementations = (InterfaceImplementation*)calloc(pFile->InterfaceImplementationCount + 1, sizeof(InterfaceImplementation));
    }
    return pTableData;
}

void InterfaceImplementation_Cleanup(CLIFile* pFile)
{
    if (pFile->InterfaceImplementations)
    {
        for (uint32_t index = 1; index <= pFile->InterfaceImplementationCount; ++index)
        {
            if (pFile->InterfaceImplementations[index].CustomAttributes) free(pFile->InterfaceImplementations[index].CustomAttributes);
        }
        free(pFile->InterfaceImplementations);
        pFile->InterfaceImplementations = NULL;
    }
}

uint8_t* InterfaceImplementation_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t implementorIndex = 0;
    uint32_t interfaceIndex = 0;
    uint32_t interfaceRow = 0;
    for (uint32_t index = 1; index <= pFile->InterfaceImplementationCount; ++index)
    {
        pFile->InterfaceImplementations[index].TableIndex = index;
        if (pFile->TypeDefinitionCount > 0xFFFF) { implementorIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementorIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (implementorIndex == 0 || implementorIndex > pFile->TypeDefinitionCount) Panic("InterfaceImplementation_Load TypeDefinition");
        pFile->InterfaceImplementations[index].Implementor = &pFile->TypeDefinitions[implementorIndex];

        if (pFile->TypeDefinitionCount > TypeDefRefOrSpecType_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefRefOrSpecType_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefRefOrSpecType_MaxRows16Bit) { interfaceIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { interfaceIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->InterfaceImplementations[index].TypeOfInterface = interfaceIndex & TypeDefRefOrSpecType_Mask;
        interfaceRow = interfaceIndex >> TypeDefRefOrSpecType_Bits;
        switch (pFile->InterfaceImplementations[index].TypeOfInterface)
        {
        case TypeDefRefOrSpecType_TypeDefinition:
            if (interfaceRow == 0 || interfaceRow > pFile->TypeDefinitionCount) Panic("InterfaceImplementation_Load TypeDefinition");
            pFile->InterfaceImplementations[index].Interface.TypeDefinition = &pFile->TypeDefinitions[interfaceRow];
            break;
        case TypeDefRefOrSpecType_TypeReference:
            if (interfaceRow == 0 || interfaceRow > pFile->TypeReferenceCount) Panic("InterfaceImplementation_Load TypeReference");
            pFile->InterfaceImplementations[index].Interface.TypeReference = &pFile->TypeReferences[interfaceRow];
            break;
        case TypeDefRefOrSpecType_TypeSpecification:
            if (interfaceRow == 0 || interfaceRow > pFile->TypeSpecificationCount) Panic("InterfaceImplementation_Load TypeSpecification");
            pFile->InterfaceImplementations[index].Interface.TypeSpecification = &pFile->TypeSpecifications[interfaceRow];
            break;
        default: break;
        }
    }
    return pTableData;
}


// ManifestResource

uint8_t* ManifestResource_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_ManifestResource)) != 0)
    {
        pFile->ManifestResourceCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ManifestResources = (ManifestResource*)calloc(pFile->ManifestResourceCount + 1, sizeof(ManifestResource));
    }
    return pTableData;
}

void ManifestResource_Cleanup(CLIFile* pFile)
{
    if (pFile->ManifestResources)
    {
        for (uint32_t index = 1; index <= pFile->ManifestResourceCount; ++index)
        {
            if (pFile->ManifestResources[index].CustomAttributes) free(pFile->ManifestResources[index].CustomAttributes);
        }
        free(pFile->ManifestResources);
        pFile->ManifestResources = NULL;
    }
}

uint8_t* ManifestResource_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t implementationIndex = 0;
    uint32_t implementationRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ManifestResourceCount; ++index)
    {
        pFile->ManifestResources[index].TableIndex = index;
        pFile->ManifestResources[index].Offset = *(uint32_t* )pTableData; pTableData += 4;
        pFile->ManifestResources[index].Flags = *(uint32_t* )pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ManifestResources[index].Name = (const char*)(pFile->StringsHeap + heapIndex);

        if (pFile->FileCount > ImplementationType_MaxRows16Bit ||
            pFile->AssemblyReferenceCount > ImplementationType_MaxRows16Bit ||
            pFile->ExportedTypeCount > ImplementationType_MaxRows16Bit) { implementationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { implementationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ManifestResources[index].TypeOfImplementation = implementationIndex & ImplementationType_Mask;
        implementationRow = implementationIndex >> ImplementationType_Bits;
        switch (pFile->ManifestResources[index].TypeOfImplementation)
        {
        case ImplementationType_File:
            if (implementationRow > pFile->FileCount) Panic("ManifestResource_Load File");
            pFile->ManifestResources[index].Implementation.File = &pFile->Files[implementationRow];
            break;
        case ImplementationType_AssemblyReference:
            if (implementationRow == 0 || implementationRow > pFile->AssemblyReferenceCount) Panic("ManifestResource_Load AssemblyReference");
            pFile->ManifestResources[index].Implementation.AssemblyReference = &pFile->AssemblyReferences[implementationRow];
            break;
        case ImplementationType_ExportedType:
            if (implementationRow == 0 || implementationRow > pFile->ExportedTypeCount) Panic("ManifestResource_Load ExportedType");
            pFile->ManifestResources[index].Implementation.ExportedType = &pFile->ExportedTypes[implementationRow];
            break;
        default: break;
        }
    }
    return pTableData;
}


// MemberReference

uint8_t* MemberReference_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_MemberReference)) != 0)
    {
        pFile->MemberReferenceCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->MemberReferences = (MemberReference*)calloc(pFile->MemberReferenceCount + 1, sizeof(MemberReference));
    }
    return pTableData;
}

void MemberReference_Cleanup(CLIFile* pFile)
{
    if (pFile->MemberReferences)
    {
        for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
        {
            if (pFile->MemberReferences[index].CustomAttributes) free(pFile->MemberReferences[index].CustomAttributes);
        }
        free(pFile->MemberReferences);
        pFile->MemberReferences = NULL;
    }
}

uint8_t* MemberReference_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t parentRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->MemberReferenceCount; ++index)
    {
        pFile->MemberReferences[index].TableIndex = index;
        if (pFile->TypeDefinitionCount > MemberRefParentType_MaxRows16Bit ||
            pFile->TypeReferenceCount > MemberRefParentType_MaxRows16Bit ||
            pFile->ModuleReferenceCount > MemberRefParentType_MaxRows16Bit ||
            pFile->MethodDefinitionCount > MemberRefParentType_MaxRows16Bit ||
            pFile->TypeSpecificationCount > MemberRefParentType_MaxRows16Bit) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MemberReferences[index].TypeOfParent = parentIndex & MemberRefParentType_Mask;
        parentRow = parentIndex >> MemberRefParentType_Bits;
        switch (pFile->MemberReferences[index].TypeOfParent)
        {
        case MemberRefParentType_TypeDefinition:
            if (parentRow == 0 || parentRow > pFile->TypeDefinitionCount) Panic("MemberReference_Load TypeDefinition");
            pFile->MemberReferences[index].Parent.TypeDefinition = &pFile->TypeDefinitions[parentRow];
            break;
        case MemberRefParentType_TypeReference:
            if (parentRow == 0 || parentRow > pFile->TypeReferenceCount) Panic("MemberReference_Load TypeReference");
            pFile->MemberReferences[index].Parent.TypeReference = &pFile->TypeReferences[parentRow];
            break;
        case MemberRefParentType_ModuleReference:
            if (parentRow == 0 || parentRow > pFile->ModuleReferenceCount) Panic("MemberReference_Load ModuleReference");
            pFile->MemberReferences[index].Parent.ModuleReference = &pFile->ModuleReferences[parentRow];
            break;
        case MemberRefParentType_MethodDefinition:
            if (parentRow == 0 || parentRow > pFile->MethodDefinitionCount) Panic("MemberReference_Load MethodDefinition");
            pFile->MemberReferences[index].Parent.MethodDefinition = &pFile->MethodDefinitions[parentRow];
            break;
        case MemberRefParentType_TypeSpecification:
            if (parentRow == 0 || parentRow > pFile->TypeSpecificationCount) Panic("MemberReference_Load TypeSpecification");
            pFile->MemberReferences[index].Parent.TypeSpecification = &pFile->TypeSpecifications[parentRow];
            break;
        default: break;
        }

        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MemberReferences[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MemberReferences[index].Signature = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->MemberReferences[index].SignatureLength);
    }
    return pTableData;
}


// MethodDefinition

uint8_t* MethodDefinition_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_MethodDefinition)) != 0)
    {
        pFile->MethodDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->MethodDefinitions = (MethodDefinition*)calloc(pFile->MethodDefinitionCount + 1, sizeof(MethodDefinition));
    }
    return pTableData;
}

void MethodDefinition_Cleanup(CLIFile* pFile)
{
    if (pFile->MethodDefinitions)
    {
        for (uint32_t index = 1; index <= pFile->MethodDefinitionCount; ++index)
        {
            if (pFile->MethodDefinitions[index].Exceptions) free(pFile->MethodDefinitions[index].Exceptions);
            if (pFile->MethodDefinitions[index].CustomAttributes) free(pFile->MethodDefinitions[index].CustomAttributes);
            if (pFile->MethodDefinitions[index].GenericParameters) free(pFile->MethodDefinitions[index].GenericParameters);
            if (pFile->MethodDefinitions[index].MemberReferences) free(pFile->MethodDefinitions[index].MemberReferences);
        }
        free(pFile->MethodDefinitions);
        pFile->MethodDefinitions = NULL;
    }
}

uint8_t* MethodDefinition_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t methodBodyVirtualAddress = 0;
    PESectionHeader* methodBodySectionHeader = NULL;
    uint8_t* methodBody = NULL;
    uint8_t methodBodySize = 0;
    uint32_t parameterListIndex = 0;
    uint32_t* parameterListIndexes = (uint32_t*)calloc(pFile->MethodDefinitionCount + 1, sizeof(uint32_t));
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->MethodDefinitionCount; ++index)
    {
		pFile->MethodDefinitions[index].File = pFile;
        pFile->MethodDefinitions[index].TableIndex = index;
        methodBodyVirtualAddress = *(uint32_t*)pTableData; pTableData += 4;
        pFile->MethodDefinitions[index].Body.Flags = 0;
        pFile->MethodDefinitions[index].Body.MaxStack = 0;
        pFile->MethodDefinitions[index].Body.CodeSize = 0;
        pFile->MethodDefinitions[index].Body.LocalVariableSignatureToken = 0;
        pFile->MethodDefinitions[index].Body.Code = NULL;
        pFile->MethodDefinitions[index].ExceptionCount = 0;
        pFile->MethodDefinitions[index].Exceptions = NULL;

        pFile->MethodDefinitions[index].ImplFlags = *(uint16_t*)pTableData; pTableData += 2;
        pFile->MethodDefinitions[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodDefinitions[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodDefinitions[index].Signature = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->MethodDefinitions[index].SignatureLength);
        if (pFile->ParameterCount > 0xFFFF) { parameterListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parameterListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (parameterListIndex == 0 || parameterListIndex > pFile->ParameterCount + 1) Panic("MethodDefinition_Load Parameter");
        if (parameterListIndex <= pFile->ParameterCount)
        {
            pFile->MethodDefinitions[index].ParameterList = &pFile->Parameters[parameterListIndex];
            parameterListIndexes[index] = parameterListIndex;
        }

        if (methodBodyVirtualAddress != 0)
        {
            methodBodySectionHeader = CLIFile_GetSection(pFile->SectionHeaders, pFile->NTHeader->NumberOfSections, methodBodyVirtualAddress);
            methodBody = pFile->Data + methodBodySectionHeader->PointerToRawData + (methodBodyVirtualAddress - methodBodySectionHeader->VirtualAddress);
            methodBodySize = 1;
            pFile->MethodDefinitions[index].Body.Flags = *(methodBody + MethodDefinitionBody__Offset_Flags) & MethodDefinitionBodyFlags_HeaderType_Mask;
            pFile->MethodDefinitions[index].Body.IsFat = FALSE;
            pFile->MethodDefinitions[index].Body.MaxStack = MethodDefinitionBody_Tiny_MaxStack;
            pFile->MethodDefinitions[index].Body.CodeSize = *(methodBody + MethodDefinitionBody__Offset_Flags) >> MethodDefinitionBodyFlags_HeaderType_Bits;
            pFile->MethodDefinitions[index].Body.LocalVariableSignatureToken = 0;
            if (pFile->MethodDefinitions[index].Body.Flags == MethodDefinitionBodyFlags_HeaderType_Fat)
            {
                pFile->MethodDefinitions[index].Body.Flags = (*(uint16_t*)(methodBody + MethodDefinitionBody__Offset_Fat_Flags)) & MethodDefinitionBody_Fat_Flags_Mask;
                methodBodySize = ((*(uint16_t*)(methodBody + MethodDefinitionBody__Offset_Fat_Flags)) >> MethodDefinitionBody_Fat_Flags_Bits) * MethodDefinitionBody_Fat_BodySize_Multiplier;
                pFile->MethodDefinitions[index].Body.IsFat = TRUE;
                pFile->MethodDefinitions[index].Body.MaxStack = *(uint16_t*)(methodBody + MethodDefinitionBody__Offset_Fat_MaxStack);
                pFile->MethodDefinitions[index].Body.CodeSize = *(uint32_t*)(methodBody + MethodDefinitionBody__Offset_Fat_CodeSize);
                pFile->MethodDefinitions[index].Body.LocalVariableSignatureToken = *(uint32_t*)(methodBody + MethodDefinitionBody__Offset_Fat_LocalVariableSignatureToken);
            }
            pFile->MethodDefinitions[index].Body.Code = methodBody + methodBodySize;
            //printf("Method: %s, RVA: %u, HeaderSize: %u, CodeSize: %u\n", pFile->MethodDefinitions[index].Name, (unsigned int)methodBodyVirtualAddress, (unsigned int)methodBodySize, (unsigned int)pFile->MethodDefinitions[index].Body.CodeSize);

            if (pFile->MethodDefinitions[index].Body.IsFat && (pFile->MethodDefinitions[index].Body.Flags & MethodDefinitionBody_Fat_Flags_HasDataSection) != 0)
            {
                uint32_t methodDataVirtualAddress = methodBodyVirtualAddress + methodBodySize + pFile->MethodDefinitions[index].Body.CodeSize;
                while ((methodDataVirtualAddress % MethodDefinitionData__ByteAlignment) != 0) ++methodDataVirtualAddress;
                const uint8_t* methodDataStart = pFile->Data + methodBodySectionHeader->PointerToRawData + (methodDataVirtualAddress - methodBodySectionHeader->VirtualAddress);
                const uint8_t* methodData = methodDataStart;
                bool_t hasData = TRUE;
                while (hasData)
                {
                    uint8_t methodDataFlags = *(methodData + MethodDefinitionData__Offset_Flags);
                    uint32_t methodDataSize = *(methodData + MethodDefinitionData__Offset_Size);
                    bool_t methodDataFlagsExceptions = (methodDataFlags & MethodDefinitionDataFlags_ExceptionTable) != 0;
                    bool_t methodDataFlagsFat = (methodDataFlags & MethodDefinitionDataFlags_Fat) != 0;
                    bool_t methodDataFlagsHasData = (methodDataFlags & MethodDefinitionDataFlags_HasAnotherDataSection) != 0;
                    if (methodDataFlagsFat) methodDataSize = (*(uint32_t*)(methodData + MethodDefinitionData__Offset_Size)) & MethodDefinitionData_Fat_Size_Mask;

                    if (methodDataFlagsExceptions)
                    {
                        uint32_t methodDataExceptionCount = (methodDataSize - MethodDefinitionData__HeaderLength) / MethodDefinitionException__HeaderLength;
                        if (methodDataFlagsFat) methodDataExceptionCount = (methodDataSize - MethodDefinitionData__HeaderLength) / MethodDefinitionException__Fat_HeaderLength;
                        pFile->MethodDefinitions[index].ExceptionCount += methodDataExceptionCount;

                        //printf("  HasExceptions, RVA: %u, Flags: 0x%x, Size: %u, Count: %u\n", (unsigned int)methodDataVirtualAddress, (unsigned int)methodDataFlags, (unsigned int)methodDataSize, (unsigned int)pFile->MethodDefinitions[index].ExceptionCount);
                    }

                    hasData = methodDataFlagsHasData;
                    if (hasData) methodData += methodDataSize;
                }
                if (pFile->MethodDefinitions[index].ExceptionCount > 0)
                {
                    uint32_t methodExceptionIndex = 0;
                    pFile->MethodDefinitions[index].Exceptions = calloc(pFile->MethodDefinitions[index].ExceptionCount, sizeof(MethodDefinitionException));

                    methodData = methodDataStart;
                    hasData = TRUE;
                    while (hasData)
                    {
                        uint8_t methodDataFlags = *(methodData + MethodDefinitionData__Offset_Flags);
                        uint32_t methodDataSize = *(methodData + MethodDefinitionData__Offset_Size);
                        bool_t methodDataFlagsExceptions = (methodDataFlags & MethodDefinitionDataFlags_ExceptionTable) != 0;
                        bool_t methodDataFlagsFat = (methodDataFlags & MethodDefinitionDataFlags_Fat) != 0;
                        bool_t methodDataFlagsHasData = (methodDataFlags & MethodDefinitionDataFlags_HasAnotherDataSection) != 0;
                        if (methodDataFlagsFat) methodDataSize = (*(uint32_t*)(methodData + MethodDefinitionData__Offset_Size)) & MethodDefinitionData_Fat_Size_Mask;

                        if (methodDataFlagsExceptions)
                        {
                            uint32_t methodDataExceptionCount = (methodDataSize - MethodDefinitionData__HeaderLength) / MethodDefinitionException__HeaderLength;
                            if (methodDataFlagsFat) methodDataExceptionCount = (methodDataSize - MethodDefinitionData__HeaderLength) / MethodDefinitionException__Fat_HeaderLength;
                            const uint8_t* methodDataException = methodData + MethodDefinitionData__HeaderLength;
                            for (uint32_t methodDataExceptionIndex = 0; methodDataExceptionIndex < methodDataExceptionCount; ++methodDataExceptionIndex)
                            {
                                uint32_t methodDataExceptionOffset = methodDataExceptionIndex * MethodDefinitionException__HeaderLength;
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].Flags = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_Flags);
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].TryOffset = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_TryOffset);
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].TryLength = *(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_TryLength);
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerOffset = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_HandlerOffset);
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerLength = *(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_HandlerLength);
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].ClassTokenOrFilterOffset = *(uint32_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_ClassTokenOrFilterOffset);
                                if (methodDataFlagsFat)
                                {
                                    methodDataExceptionOffset = methodDataExceptionIndex * MethodDefinitionException__Fat_HeaderLength;
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].Flags = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_Fat_Flags);
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].TryOffset = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_Fat_TryOffset);
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].TryLength = *(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_Fat_TryLength);
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerOffset = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_Fat_HandlerOffset);
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerLength = *(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_Fat_HandlerLength);
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].ClassTokenOrFilterOffset = *(uint32_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException__Offset_Fat_ClassTokenOrFilterOffset);
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
    for (uint32_t index = 1, used = 0; index <= pFile->MethodDefinitionCount; ++index, used += parameterListCount)
    {
        if (index == pFile->MethodDefinitionCount || parameterListIndexes[index + 1] == 0) parameterListCount = pFile->ParameterCount - used;
        else parameterListCount = parameterListIndexes[index + 1] - parameterListIndexes[index];
        pFile->MethodDefinitions[index].ParameterListCount = parameterListCount;
    }
    free(parameterListIndexes);
    return pTableData;
}


// MethodImplementation

uint8_t* MethodImplementation_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_MethodImplementation)) != 0)
    {
        pFile->MethodImplementationCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->MethodImplementations = (MethodImplementation*)calloc(pFile->MethodImplementationCount + 1, sizeof(MethodImplementation));
    }
    return pTableData;
}

void MethodImplementation_Cleanup(CLIFile* pFile)
{
    if (pFile->MethodImplementations)
    {
        for (uint32_t index = 1; index <= pFile->MethodImplementationCount; ++index)
        {
        }
        free(pFile->MethodImplementations);
        pFile->MethodImplementations = NULL;
    }
}

uint8_t* MethodImplementation_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t methodIndex = 0;
    uint32_t methodRow = 0;
    for (uint32_t index = 1; index <= pFile->MethodImplementationCount; ++index)
    {
        pFile->MethodImplementations[index].TableIndex = index;
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (parentIndex == 0 || parentIndex > pFile->TypeDefinitionCount) Panic("MethodImplementation_Load TypeDefinition");
        pFile->MethodImplementations[index].Parent = &pFile->TypeDefinitions[parentIndex];

        if (pFile->MethodDefinitionCount > MethodDefOrRefType_MaxRows16Bit ||
            pFile->MemberReferenceCount > MethodDefOrRefType_MaxRows16Bit) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodImplementations[index].TypeOfMethodBody = methodIndex & MethodDefOrRefType_Mask;
        methodRow = methodIndex >> MethodDefOrRefType_Bits;
        switch (pFile->MethodImplementations[index].TypeOfMethodBody)
        {
        case MethodDefOrRefType_MethodDefinition:
            if (methodRow == 0 || methodRow > pFile->MethodDefinitionCount) Panic("MethodImplementation_Load MethodDefinition");
            pFile->MethodImplementations[index].MethodBody.MethodDefinition = &pFile->MethodDefinitions[methodRow];
            break;
        case MethodDefOrRefType_MemberReference:
            if (methodRow == 0 || methodRow > pFile->MemberReferenceCount) Panic("MethodImplementation_Load MemberReference");
            pFile->MethodImplementations[index].MethodBody.MemberReference = &pFile->MemberReferences[methodRow];
            break;
        default: break;
        }

        if (pFile->MethodDefinitionCount > MethodDefOrRefType_MaxRows16Bit ||
            pFile->MemberReferenceCount > MethodDefOrRefType_MaxRows16Bit) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodImplementations[index].TypeOfMethodDeclaration = methodIndex & MethodDefOrRefType_Mask;
        methodRow = methodIndex >> MethodDefOrRefType_Bits;
        switch (pFile->MethodImplementations[index].TypeOfMethodDeclaration)
        {
        case MethodDefOrRefType_MethodDefinition:
            if (methodRow == 0 || methodRow > pFile->MethodDefinitionCount) Panic("MethodImplementation_Load MethodDefinition");
            pFile->MethodImplementations[index].MethodDeclaration.MethodDefinition = &pFile->MethodDefinitions[methodRow];
            break;
        case MethodDefOrRefType_MemberReference:
            if (methodRow == 0 || methodRow > pFile->MemberReferenceCount) Panic("MethodImplementation_Load MemberReference");
            pFile->MethodImplementations[index].MethodDeclaration.MemberReference = &pFile->MemberReferences[methodRow];
            break;
        default: break;
        }
    }
    return pTableData;
}


// MethodSemantics

uint8_t* MethodSemantics_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_MethodSemantics)) != 0)
    {
        pFile->MethodSemanticsCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->MethodSemantics = (MethodSemantics*)calloc(pFile->MethodSemanticsCount + 1, sizeof(MethodSemantics));
    }
    return pTableData;
}

void MethodSemantics_Cleanup(CLIFile* pFile)
{
    if (pFile->MethodSemantics)
    {
        for (uint32_t index = 1; index <= pFile->MethodSemanticsCount; ++index)
        {
        }
        free(pFile->MethodSemantics);
        pFile->MethodSemantics = NULL;
    }
}

uint8_t* MethodSemantics_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t methodIndex = 0;
    uint32_t associationIndex = 0;
    uint32_t associationRow = 0;
    for (uint32_t index = 1; index <= pFile->MethodSemanticsCount; ++index)
    {
        pFile->MethodSemantics[index].TableIndex = index;
        pFile->MethodSemantics[index].Semantics = *(uint16_t*)pTableData; pTableData += 2;
        if (pFile->MethodDefinitionCount > 0xFFFF) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (methodIndex == 0 || methodIndex > pFile->MethodDefinitionCount) Panic("MethodSemantics_Load MethodDefinition");
        pFile->MethodSemantics[index].Method = &pFile->MethodDefinitions[methodIndex];

        if (pFile->EventCount > HasSemanticsType_MaxRows16Bit ||
            pFile->PropertyCount > HasSemanticsType_MaxRows16Bit) { associationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { associationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodSemantics[index].TypeOfAssociation = associationIndex & HasSemanticsType_Mask;
        associationRow = associationIndex >> HasSemanticsType_Bits;
        switch (pFile->MethodSemantics[index].TypeOfAssociation)
        {
        case HasSemanticsType_Event:
            if (associationRow == 0 || associationRow > pFile->EventCount) Panic("MethodSemantics_Load Event");
            pFile->MethodSemantics[index].Association.Event = &pFile->Events[associationRow];
            break;
        case HasSemanticsType_Property:
            if (associationRow == 0 || associationRow > pFile->PropertyCount) Panic("MethodSemantics_Load Property");
            pFile->MethodSemantics[index].Association.Property = &pFile->Properties[associationRow];
            break;
        default: break;
        }
    }
    return pTableData;
}


// MethodSpecification

uint8_t* MethodSpecification_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_MethodSpecification)) != 0)
    {
        pFile->MethodSpecificationCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->MethodSpecifications = (MethodSpecification*)calloc(pFile->MethodSpecificationCount + 1, sizeof(MethodSpecification));
    }
    return pTableData;
}

void MethodSpecification_Cleanup(CLIFile* pFile)
{
    if (pFile->MethodSpecifications)
    {
        for (uint32_t index = 1; index <= pFile->MethodSpecificationCount; ++index)
        {
            if (pFile->MethodSpecifications[index].CustomAttributes) free(pFile->MethodSpecifications[index].CustomAttributes);
        }
        free(pFile->MethodSpecifications);
        pFile->MethodSpecifications = NULL;
    }
}

uint8_t* MethodSpecification_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t methodIndex = 0;
    uint32_t methodRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->MethodSpecificationCount; ++index)
    {
        pFile->MethodSpecifications[index].TableIndex = index;
        if (pFile->MethodDefinitionCount > MethodDefOrRefType_MaxRows16Bit ||
            pFile->MemberReferenceCount > MethodDefOrRefType_MaxRows16Bit) { methodIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodSpecifications[index].TypeOfMethod = methodIndex & MethodDefOrRefType_Mask;
        methodRow = methodIndex >> MethodDefOrRefType_Bits;
        switch (pFile->MethodSpecifications[index].TypeOfMethod)
        {
        case MethodDefOrRefType_MethodDefinition:
            if (methodRow == 0 || methodRow > pFile->MethodDefinitionCount) Panic("MethodSpecification_Load MethodDefinition");
            pFile->MethodSpecifications[index].Method.MethodDefinition = &pFile->MethodDefinitions[methodRow];
            break;
        case MethodDefOrRefType_MemberReference:
            if (methodRow == 0 || methodRow > pFile->MemberReferenceCount) Panic("MethodSpecification_Load MemberReference");
            pFile->MethodSpecifications[index].Method.MemberReference = &pFile->MemberReferences[methodRow];
            break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodSpecifications[index].Instantiation = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->MethodSpecifications[index].InstantiationLength);
    }
    return pTableData;
}


// ModuleDefinition

uint8_t* ModuleDefinition_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_ModuleDefinition)) != 0)
    {
        pFile->ModuleDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ModuleDefinitions = (ModuleDefinition*)calloc(pFile->ModuleDefinitionCount + 1, sizeof(ModuleDefinition));
    }
    return pTableData;
}

void ModuleDefinition_Cleanup(CLIFile* pFile)
{
    if (pFile->ModuleDefinitions)
    {
        for (uint32_t index = 1; index <= pFile->ModuleDefinitionCount; ++index)
        {
            if (pFile->ModuleDefinitions[index].CustomAttributes) free(pFile->ModuleDefinitions[index].CustomAttributes);
        }
        free(pFile->ModuleDefinitions);
        pFile->ModuleDefinitions = NULL;
    }
}

uint8_t* ModuleDefinition_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ModuleDefinitionCount; ++index)
    {
        pFile->ModuleDefinitions[index].TableIndex = index;
        pFile->ModuleDefinitions[index].Generation = *(uint16_t*)pTableData; 
		pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0)  { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else  { heapIndex = *((uint16_t*)pTableData); pTableData += 2; }
        pFile->ModuleDefinitions[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2;  }
        pFile->ModuleDefinitions[index].ModuleVersionID = pFile->GUIDsHeap + heapIndex;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2;  }
        pFile->ModuleDefinitions[index].EncID = pFile->GUIDsHeap + heapIndex;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_GUIDs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ModuleDefinitions[index].EncBaseID = pFile->GUIDsHeap + heapIndex;
    }
    return pTableData;
}


// ModuleReference

uint8_t* ModuleReference_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_ModuleReference)) != 0)
    {
        pFile->ModuleReferenceCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->ModuleReferences = (ModuleReference*)calloc(pFile->ModuleReferenceCount + 1, sizeof(ModuleReference));
    }
    return pTableData;
}

void ModuleReference_Cleanup(CLIFile* pFile)
{
    if (pFile->ModuleReferences)
    {
        for (uint32_t index = 1; index <= pFile->ModuleReferenceCount; ++index)
        {
            if (pFile->ModuleReferences[index].CustomAttributes) free(pFile->ModuleReferences[index].CustomAttributes);
            if (pFile->ModuleReferences[index].MemberReferences) free(pFile->ModuleReferences[index].MemberReferences);
        }
        free(pFile->ModuleReferences);
        pFile->ModuleReferences = NULL;
    }
}

uint8_t* ModuleReference_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ModuleReferenceCount; ++index)
    {
        pFile->ModuleReferences[index].TableIndex = index;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->ModuleReferences[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}


// NestedClass

uint8_t* NestedClass_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_NestedClass)) != 0)
    {
        pFile->NestedClassCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->NestedClasses = (NestedClass*)calloc(pFile->NestedClassCount + 1, sizeof(NestedClass));
    }
    return pTableData;
}

void NestedClass_Cleanup(CLIFile* pFile)
{
    if (pFile->NestedClasses)
    {
        for (uint32_t index = 1; index <= pFile->NestedClassCount; ++index)
        {
        }
        free(pFile->NestedClasses);
        pFile->NestedClasses = NULL;
    }
}

uint8_t* NestedClass_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t nestedIndex = 0;
    uint32_t enclosingIndex = 0;
    for (uint32_t index = 1; index <= pFile->NestedClassCount; ++index)
    {
        pFile->NestedClasses[index].TableIndex = index;
        if (pFile->TypeDefinitionCount > 0xFFFF) { nestedIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { nestedIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (nestedIndex == 0 || nestedIndex > pFile->TypeDefinitionCount) Panic("NestedClass_Load TypeDefinition");
        pFile->NestedClasses[index].Nested = &pFile->TypeDefinitions[nestedIndex];
        if (pFile->TypeDefinitionCount > 0xFFFF) { enclosingIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { enclosingIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (enclosingIndex == 0 || enclosingIndex > pFile->TypeDefinitionCount) Panic("NestedClass_Load TypeDefinition");
        pFile->NestedClasses[index].Enclosing = &pFile->TypeDefinitions[enclosingIndex];
    }
    return pTableData;
}


// Parameter

uint8_t* Parameter_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_Parameter)) != 0)
    {
        pFile->ParameterCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Parameters = (Parameter*)calloc(pFile->ParameterCount + 1, sizeof(Parameter));
    }
    return pTableData;
}

void Parameter_Cleanup(CLIFile* pFile)
{
    if (pFile->Parameters)
    {
        for (uint32_t index = 1; index <= pFile->ParameterCount; ++index)
        {
            if (pFile->Parameters[index].CustomAttributes) free(pFile->Parameters[index].CustomAttributes);
        }
        free(pFile->Parameters);
        pFile->Parameters = NULL;
    }
}

uint8_t* Parameter_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ParameterCount; ++index)
    {
        pFile->Parameters[index].TableIndex = index;
        pFile->Parameters[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        pFile->Parameters[index].Sequence = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Parameters[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}


// Property

uint8_t* Property_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_Property)) != 0)
    {
        pFile->PropertyCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->Properties = (Property*)calloc(pFile->PropertyCount + 1, sizeof(Property));
    }
    return pTableData;
}

void Property_Cleanup(CLIFile* pFile)
{
    if (pFile->Properties)
    {
        for (uint32_t index = 1; index <= pFile->PropertyCount; ++index)
        {
            if (pFile->Properties[index].CustomAttributes) free(pFile->Properties[index].CustomAttributes);
        }
        free(pFile->Properties);
        pFile->Properties = NULL;
    }
}

uint8_t* Property_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->PropertyCount; ++index)
    {
        pFile->Properties[index].TableIndex = index;
        pFile->Properties[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Properties[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Properties[index].Signature = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->Properties[index].SignatureLength);
    }
    return pTableData;
}


// PropertyMap

uint8_t* PropertyMap_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_PropertyMap)) != 0)
    {
        pFile->PropertyMapCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->PropertyMaps = (PropertyMap*)calloc(pFile->PropertyMapCount + 1, sizeof(PropertyMap));
    }
    return pTableData;
}

void PropertyMap_Cleanup(CLIFile* pFile)
{
    if (pFile->PropertyMaps)
    {
        for (uint32_t index = 1; index <= pFile->PropertyMapCount; ++index)
        {
        }
        free(pFile->PropertyMaps);
        pFile->PropertyMaps = NULL;
    }
}

uint8_t* PropertyMap_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t parentIndex = 0;
    uint32_t propertyListIndex = 0;
    uint32_t* propertyListIndexes = (uint32_t*)calloc(pFile->PropertyMapCount + 1, sizeof(uint32_t));
    for (uint32_t index = 1; index <= pFile->PropertyMapCount; ++index)
    {
        pFile->PropertyMaps[index].TableIndex = index;
        if (pFile->TypeDefinitionCount > 0xFFFF) { parentIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { parentIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (parentIndex == 0 || parentIndex > pFile->TypeDefinitionCount) Panic("PropertyMap_Load TypeDefinition");
        pFile->PropertyMaps[index].Parent = &pFile->TypeDefinitions[parentIndex];
        if (pFile->PropertyCount > 0xFFFF) { propertyListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { propertyListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (propertyListIndex == 0 || propertyListIndex > pFile->PropertyCount + 1) Panic("PropertyMap_Load Property");
        if (propertyListIndex <= pFile->PropertyCount)
        {
            pFile->PropertyMaps[index].PropertyList = &pFile->Properties[propertyListIndex];
            propertyListIndexes[index] = propertyListIndex;
        }
    }
    uint32_t propertyListCount = 0;
    for (uint32_t index = 1, used = 0; index <= pFile->PropertyMapCount; ++index, used += propertyListCount)
    {
        if (index == pFile->PropertyMapCount || propertyListIndexes[index + 1] == 0) propertyListCount = pFile->PropertyCount - used;
        else propertyListCount = propertyListIndexes[index + 1] - propertyListIndexes[index];
        pFile->PropertyMaps[index].PropertyListCount = propertyListCount;
    }
    free(propertyListIndexes);
    return pTableData;
}


// StandAloneSignature

uint8_t* StandAloneSignature_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_StandAloneSignature)) != 0)
    {
        pFile->StandAloneSignatureCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->StandAloneSignatures = (StandAloneSignature*)calloc(pFile->StandAloneSignatureCount + 1, sizeof(StandAloneSignature));
    }
    return pTableData;
}

void StandAloneSignature_Cleanup(CLIFile* pFile)
{
    if (pFile->StandAloneSignatures)
    {
        for (uint32_t index = 1; index <= pFile->StandAloneSignatureCount; ++index)
        {
            if (pFile->StandAloneSignatures[index].CustomAttributes) free(pFile->StandAloneSignatures[index].CustomAttributes);
        }
        free(pFile->StandAloneSignatures);
        pFile->StandAloneSignatures = NULL;
    }
}

uint8_t* StandAloneSignature_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->StandAloneSignatureCount; ++index)
    {
        pFile->StandAloneSignatures[index].TableIndex = index;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->StandAloneSignatures[index].Signature = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->StandAloneSignatures[index].SignatureLength);
    }
    return pTableData;
}


// TypeDefinition

uint8_t* TypeDefinition_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_TypeDefinition)) != 0)
    {
        pFile->TypeDefinitionCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->TypeDefinitions = (TypeDefinition*)calloc(pFile->TypeDefinitionCount + 1, sizeof(TypeDefinition));
    }
    return pTableData;
}

void TypeDefinition_Cleanup(CLIFile* pFile)
{
    if (pFile->TypeDefinitions)
    {
        for (uint32_t index = 1; index <= pFile->TypeDefinitionCount; ++index)
        {
            if (pFile->TypeDefinitions[index].CustomAttributes) free(pFile->TypeDefinitions[index].CustomAttributes);
            if (pFile->TypeDefinitions[index].GenericParameters) free(pFile->TypeDefinitions[index].GenericParameters);
            if (pFile->TypeDefinitions[index].InterfaceImplementations) free(pFile->TypeDefinitions[index].InterfaceImplementations);
            if (pFile->TypeDefinitions[index].MemberReferences) free(pFile->TypeDefinitions[index].MemberReferences);
            if (pFile->TypeDefinitions[index].MethodImplementations) free(pFile->TypeDefinitions[index].MethodImplementations);
            if (pFile->TypeDefinitions[index].NestedClasses) free(pFile->TypeDefinitions[index].NestedClasses);
        }
        free(pFile->TypeDefinitions);
        pFile->TypeDefinitions = NULL;
    }
}

uint8_t* TypeDefinition_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t extendsIndex = 0;
    uint32_t extendsRow = 0;
    uint32_t fieldListIndex = 0;
    uint32_t methodDefinitionListIndex = 0;
    uint32_t* fieldListIndexes = (uint32_t*)calloc(pFile->TypeDefinitionCount + 1, sizeof(uint32_t));
    uint32_t* methodDefinitionListIndexes = (uint32_t*)calloc(pFile->TypeDefinitionCount + 1, sizeof(uint32_t));
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->TypeDefinitionCount; ++index)
    {
		pFile->TypeDefinitions[index].File = pFile;
        pFile->TypeDefinitions[index].TableIndex = index;
        pFile->TypeDefinitions[index].Flags = *(uint32_t*)pTableData; pTableData += 4;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].Namespace = (const char*)(pFile->StringsHeap + heapIndex);
        if (pFile->TypeDefinitionCount > TypeDefRefOrSpecType_MaxRows16Bit ||
            pFile->TypeReferenceCount > TypeDefRefOrSpecType_MaxRows16Bit ||
            pFile->TypeSpecificationCount > TypeDefRefOrSpecType_MaxRows16Bit) { extendsIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { extendsIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeDefinitions[index].TypeOfExtends = extendsIndex & TypeDefRefOrSpecType_Mask;
        extendsRow = extendsIndex >> TypeDefRefOrSpecType_Bits;
        switch (pFile->TypeDefinitions[index].TypeOfExtends)
        {
        case TypeDefRefOrSpecType_TypeDefinition:
            if (extendsRow > pFile->TypeDefinitionCount) Panic("TypeDefinition_Load TypeDefinition");
            if (extendsRow > 0) pFile->TypeDefinitions[index].Extends.TypeDefinition = &pFile->TypeDefinitions[extendsRow];
            break;
        case TypeDefRefOrSpecType_TypeReference:
            if (extendsRow > pFile->TypeReferenceCount) Panic("TypeDefinition_Load TypeReference");
            if (extendsRow > 0) pFile->TypeDefinitions[index].Extends.TypeReference = &pFile->TypeReferences[extendsRow];
            break;
        case TypeDefRefOrSpecType_TypeSpecification:
            if (extendsRow > pFile->TypeSpecificationCount) Panic("TypeDefinition_Load TypeSpecification");
            if (extendsRow > 0) pFile->TypeDefinitions[index].Extends.TypeSpecification = &pFile->TypeSpecifications[extendsRow];
            break;
        default: break;
        }
        if (pFile->FieldCount > 0xFFFF) { fieldListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (fieldListIndex == 0 || fieldListIndex > pFile->FieldCount + 1) Panic("TypeDefinition_Load Field");
        if (fieldListIndex <= pFile->FieldCount)
        {
			//printf("TypeDef Field Link: %s.%s, start @ %u\n", pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, fieldListIndex);
			//printf("Field List: 0x%x\n", (unsigned int)&pFile->Fields[fieldListIndex]);
            pFile->TypeDefinitions[index].FieldList = &pFile->Fields[fieldListIndex];
            fieldListIndexes[index] = fieldListIndex;
        }
        if (pFile->MethodDefinitionCount > 0xFFFF) { methodDefinitionListIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { methodDefinitionListIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (methodDefinitionListIndex == 0 || methodDefinitionListIndex > pFile->MethodDefinitionCount + 1) Panic("TypeDefinition_Load MethodDefinition");
        if (methodDefinitionListIndex <= pFile->MethodDefinitionCount)
        {
            pFile->TypeDefinitions[index].MethodDefinitionList = &pFile->MethodDefinitions[methodDefinitionListIndex];
            methodDefinitionListIndexes[index] = methodDefinitionListIndex;
        }
    }
    uint32_t fieldListCount = 0;
    for (uint32_t index = 1, used = 0; index <= pFile->TypeDefinitionCount; ++index, used += fieldListCount)
    {
        if (index == pFile->TypeDefinitionCount || fieldListIndexes[index + 1] == 0) fieldListCount = pFile->FieldCount - used;
        else fieldListCount = fieldListIndexes[index + 1] - fieldListIndexes[index];
        pFile->TypeDefinitions[index].FieldListCount = fieldListCount;
		//printf("TypeDef: %s.%s, Owns %u Fields starting @ %u\n", pFile->TypeDefinitions[index].Namespace, pFile->TypeDefinitions[index].Name, fieldListCount, fieldListIndexes[index]);
    }
    free(fieldListIndexes);
    uint32_t methodDefinitionListCount = 0;
    for (uint32_t index = 1, used = 0; index <= pFile->TypeDefinitionCount; ++index, used += methodDefinitionListCount)
    {
        if (index == pFile->TypeDefinitionCount || methodDefinitionListIndexes[index + 1] == 0) methodDefinitionListCount = pFile->MethodDefinitionCount - used;
        else methodDefinitionListCount = methodDefinitionListIndexes[index + 1] - methodDefinitionListIndexes[index];
        pFile->TypeDefinitions[index].MethodDefinitionListCount = methodDefinitionListCount;
    }
    free(methodDefinitionListIndexes);
    return pTableData;
}


// TypeReference

uint8_t* TypeReference_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_TypeReference)) != 0)
    {
        pFile->TypeReferenceCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->TypeReferences = (TypeReference*)calloc(pFile->TypeReferenceCount + 1, sizeof(TypeReference));
    }
    return pTableData;
}

void TypeReference_Cleanup(CLIFile* pFile)
{
    if (pFile->TypeReferences)
    {
        for (uint32_t index = 1; index <= pFile->TypeReferenceCount; ++index)
        {
            if (pFile->TypeReferences[index].CustomAttributes) free(pFile->TypeReferences[index].CustomAttributes);
            if (pFile->TypeReferences[index].MemberReferences) free(pFile->TypeReferences[index].MemberReferences);
        }
        free(pFile->TypeReferences);
        pFile->TypeReferences = NULL;
    }
}

uint8_t* TypeReference_Load(CLIFile* pFile, uint8_t* pTableData)
{
    uint32_t resolutionScopeIndex = 0;
    uint32_t resolutionScopeRow = 0;
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->TypeReferenceCount; ++index)
    {
        pFile->TypeReferences[index].TableIndex = index;
        if (pFile->ModuleDefinitionCount > ResolutionScopeType_MaxRows16Bit ||
            pFile->ModuleReferenceCount > ResolutionScopeType_MaxRows16Bit ||
            pFile->AssemblyReferenceCount > ResolutionScopeType_MaxRows16Bit ||
            pFile->TypeReferenceCount > ResolutionScopeType_MaxRows16Bit) { resolutionScopeIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { resolutionScopeIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeReferences[index].TypeOfResolutionScope = resolutionScopeIndex & ResolutionScopeType_Mask;
        resolutionScopeRow = resolutionScopeIndex >> ResolutionScopeType_Bits;
        switch (pFile->TypeReferences[index].TypeOfResolutionScope)
        {
        case ResolutionScopeType_ModuleDefinition:
            if (resolutionScopeRow == 0 || resolutionScopeRow > pFile->ModuleDefinitionCount) Panic("TypeReference_Load ModuleDefinition");
            pFile->TypeReferences[index].ResolutionScope.ModuleDefinition = &pFile->ModuleDefinitions[resolutionScopeRow];
            break;
        case ResolutionScopeType_ModuleReference:
            if (resolutionScopeRow == 0 || resolutionScopeRow > pFile->ModuleReferenceCount) Panic("TypeReference_Load ModuleReference");
            pFile->TypeReferences[index].ResolutionScope.ModuleReference = &pFile->ModuleReferences[resolutionScopeRow];
            break;
        case ResolutionScopeType_AssemblyReference:
            if (resolutionScopeRow == 0 || resolutionScopeRow > pFile->AssemblyReferenceCount) Panic("TypeReference_Load AssemblyReference");
            pFile->TypeReferences[index].ResolutionScope.AssemblyReference = &pFile->AssemblyReferences[resolutionScopeRow];
            break;
        case ResolutionScopeType_TypeReference:
            if (resolutionScopeRow == 0 || resolutionScopeRow > pFile->TypeReferenceCount) Panic("TypeReference_Load TypeReference");
            pFile->TypeReferences[index].ResolutionScope.TypeReference = &pFile->TypeReferences[resolutionScopeRow];
            break;
        default: break;
        }
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeReferences[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeReferences[index].Namespace = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}


// TypeSpecification

uint8_t* TypeSpecification_Initialize(CLIFile* pFile, uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetadataTable_TypeSpecification)) != 0)
    {
        pFile->TypeSpecificationCount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->TypeSpecifications = (TypeSpecification*)calloc(pFile->TypeSpecificationCount + 1, sizeof(TypeSpecification));
    }
    return pTableData;
}

void TypeSpecification_Cleanup(CLIFile* pFile)
{
    if (pFile->TypeSpecifications)
    {
        for (uint32_t index = 1; index <= pFile->TypeSpecificationCount; ++index)
        {
            if (pFile->TypeSpecifications[index].CustomAttributes) free(pFile->TypeSpecifications[index].CustomAttributes);
            if (pFile->TypeSpecifications[index].MemberReferences) free(pFile->TypeSpecifications[index].MemberReferences);
        }
        free(pFile->TypeSpecifications);
        pFile->TypeSpecifications = NULL;
    }
}

uint8_t* TypeSpecification_Load(CLIFile* pFile, uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->TypeSpecificationCount; ++index)
    {
		pFile->TypeSpecifications[index].File = pFile;
        pFile->TypeSpecifications[index].TableIndex = index;
        if ((pFile->TablesHeader->HeapOffsetSizes & METADATAHEAP__OffsetSize_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->TypeSpecifications[index].Signature = CLIFile_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->TypeSpecifications[index].SignatureLength);
    }
    return pTableData;
}


// Signatures

bool_t Signature_Equals(uint8_t* pSignature1, uint32_t pSignature1Length, uint8_t* pSignature2, uint32_t pSignature2Length)
{
	if (pSignature1Length != pSignature2Length) return FALSE;
	return memcmp(pSignature1, pSignature2, pSignature1Length) == 0;
}

// MethodSignature

MethodSignature* MethodSignature_Create()
{
    return (MethodSignature*)calloc(1, sizeof(MethodSignature));
}

void MethodSignature_Destroy(MethodSignature* pMethodSignature)
{
    if (pMethodSignature->Parameters)
    {
        for (uint32_t index = 0; index < pMethodSignature->ParameterCount; ++index) SignatureParameter_Destroy(pMethodSignature->Parameters[index]);
        free(pMethodSignature->Parameters);
    }
    if (pMethodSignature->ReturnType) SignatureReturnType_Destroy(pMethodSignature->ReturnType);
    free(pMethodSignature);
}

MethodSignature* MethodSignature_Expand(uint8_t* pSignature, CLIFile* pCLIFile)
{
    MethodSignature* methodSignature = NULL;
	MethodSignature_Parse(pSignature, &methodSignature, pCLIFile);
    return methodSignature;
}

uint8_t* MethodSignature_Parse(uint8_t* pCursor, MethodSignature** pMethodSignature, CLIFile* pCLIFile)
{
    *pMethodSignature = MethodSignature_Create();
    MethodSignature* methodSignature = *pMethodSignature;
    uint8_t callingConvention = *pCursor; ++pCursor;
    methodSignature->HasThis = (callingConvention & SignatureCallConvention_HasThis) != 0;
    methodSignature->ExplicitThis = (callingConvention & SignatureCallConvention_ExplicitThis) != 0;
    callingConvention &= ~(SignatureCallConvention_HasThis | SignatureCallConvention_ExplicitThis);
    methodSignature->Default = callingConvention == SignatureCallConvention_Default;
    methodSignature->CCall = callingConvention == SignatureCallConvention_C;
    methodSignature->STDCall = callingConvention == SignatureCallConvention_STDCall;
    methodSignature->ThisCall = callingConvention == SignatureCallConvention_ThisCall;
    methodSignature->FastCall = callingConvention == SignatureCallConvention_FastCall;
    methodSignature->VarArgs = callingConvention == SignatureCallConvention_VarArgs;
    methodSignature->Generic = callingConvention == SignatureCallConvention_Generic;
    if (methodSignature->Generic) pCursor = CLIFile_GetCompressedUnsigned(pCursor, &methodSignature->GenericParameterCount);
    pCursor = CLIFile_GetCompressedUnsigned(pCursor, &methodSignature->ParameterCount);
    pCursor = SignatureReturnType_Parse(pCursor, &methodSignature->ReturnType, pCLIFile);
    if (methodSignature->ParameterCount > 0)
    {
        methodSignature->Parameters = (SignatureParameter**)calloc(methodSignature->ParameterCount, sizeof(SignatureParameter*));
        for (uint32_t index = 0; index < methodSignature->ParameterCount; ++index)
        {
            if (*pCursor == SignatureElementType_Sentinel)
            {
                methodSignature->HasSentinel = TRUE;
                methodSignature->SentinelIndex = index;
                ++pCursor;
            }
            pCursor = SignatureParameter_Parse(pCursor, &methodSignature->Parameters[index], pCLIFile);
        }
    }
    return pCursor;
}

bool_t MethodSignature_Compare(AppDomain* pDomain, IRAssembly* pAssemblyA, MethodSignature* pMethodSignatureA, IRAssembly* pAssemblyB, MethodSignature* pMethodSignatureB)
{
	if (pMethodSignatureA->HasThis != pMethodSignatureB->HasThis) return FALSE;
	if (pMethodSignatureA->ExplicitThis != pMethodSignatureB->ExplicitThis) return FALSE;
	if (pMethodSignatureA->Default != pMethodSignatureB->Default) return FALSE;
	if (pMethodSignatureA->VarArgs != pMethodSignatureB->VarArgs) return FALSE;
	if (pMethodSignatureA->CCall != pMethodSignatureB->CCall) return FALSE;
	if (pMethodSignatureA->STDCall != pMethodSignatureB->STDCall) return FALSE;
	if (pMethodSignatureA->ThisCall != pMethodSignatureB->ThisCall) return FALSE;
	if (pMethodSignatureA->FastCall != pMethodSignatureB->FastCall) return FALSE;
	if (pMethodSignatureA->GenericParameterCount != pMethodSignatureB->GenericParameterCount) return FALSE;
	if (pMethodSignatureA->ParameterCount != pMethodSignatureB->ParameterCount) return FALSE;
	if (pMethodSignatureA->HasSentinel != pMethodSignatureB->HasSentinel) return FALSE;
	if (pMethodSignatureA->SentinelIndex != pMethodSignatureB->SentinelIndex) return FALSE;
	if (pMethodSignatureA->ReturnType->Void != pMethodSignatureB->ReturnType->Void) return FALSE;
	if (!pMethodSignatureA->ReturnType->Void)
	{
		if (pMethodSignatureA->ReturnType->ByReference != pMethodSignatureB->ReturnType->ByReference) return FALSE;
		if (pMethodSignatureA->ReturnType->TypedByReference != pMethodSignatureB->ReturnType->TypedByReference) return FALSE;
		if (!pMethodSignatureA->ReturnType->TypedByReference)
		{
			IRType* typeA = AppDomain_GetIRTypeFromSignatureType(pDomain, pAssemblyA, pMethodSignatureA->ReturnType->Type);
			IRType* typeB = AppDomain_GetIRTypeFromSignatureType(pDomain, pAssemblyB, pMethodSignatureB->ReturnType->Type);
			//printf("A @ 0x%x %s, B @ 0x%x %s\n", (unsigned int)typeA, typeA->TypeDefinition->Name, (unsigned int)typeB, typeB->TypeDefinition->Name);
			if (typeA->IsArrayType)
			{
				if (!typeB->IsArrayType) return FALSE;
				if (typeA->ArrayType->ElementType != typeB->ArrayType->ElementType) return FALSE;
				//printf("Problem Here? %X and %X, %X and %X, but %X and %X!\n", (unsigned int)pMethodSignatureA->Parameters[index]->Type->ElementType, (unsigned int)pMethodSignatureB->Parameters[index]->Type->ElementType, (unsigned int)pMethodSignatureA->Parameters[index]->Type->SZArrayType->ElementType, (unsigned int)pMethodSignatureB->Parameters[index]->Type->SZArrayType->ElementType, (unsigned int)paramAType, (unsigned int)paramBType);
			}
			else if (typeA->IsPointerType)
			{
				if (!typeB->IsPointerType) return FALSE;
				if (typeA->PointerType->TypePointedTo != typeB->PointerType->TypePointedTo) return FALSE;
			}
			else if (typeA != typeB) return FALSE;
		}
	}
	for (uint32_t index = 0; index < pMethodSignatureA->ParameterCount; ++index)
	{
		if (pMethodSignatureA->Parameters[index]->ByReference != pMethodSignatureB->Parameters[index]->ByReference) return FALSE;
		if (pMethodSignatureA->Parameters[index]->TypedByReference != pMethodSignatureB->Parameters[index]->TypedByReference) return FALSE;
		if (!pMethodSignatureA->Parameters[index]->TypedByReference)
		{
			IRType* paramAType = AppDomain_GetIRTypeFromSignatureType(pDomain, pAssemblyA, pMethodSignatureA->Parameters[index]->Type);
			IRType* paramBType = AppDomain_GetIRTypeFromSignatureType(pDomain, pAssemblyB, pMethodSignatureB->Parameters[index]->Type);
			//printf("Param %u A @ 0x%x (tdef: 0x%x) %s, B @ 0x%x (tdef: 0x%x) %s\n", (unsigned int)index, (unsigned int)paramAType, (unsigned int)paramAType->TypeDefinition, paramAType->TypeDefinition->Name, (unsigned int)paramBType, (unsigned int)paramBType->TypeDefinition, paramBType->TypeDefinition->Name);
			if (paramAType->IsArrayType)
			{
				if (!paramBType->IsArrayType) return FALSE;
				if (paramAType->ArrayType->ElementType != paramBType->ArrayType->ElementType) return FALSE;
				//printf("Problem Here? %X and %X, %X and %X, but %X and %X!\n", (unsigned int)pMethodSignatureA->Parameters[index]->Type->ElementType, (unsigned int)pMethodSignatureB->Parameters[index]->Type->ElementType, (unsigned int)pMethodSignatureA->Parameters[index]->Type->SZArrayType->ElementType, (unsigned int)pMethodSignatureB->Parameters[index]->Type->SZArrayType->ElementType, (unsigned int)paramAType, (unsigned int)paramBType);
			}
			else if (paramAType->IsPointerType)
			{
				if (!paramBType->IsPointerType) return FALSE;
				if (paramAType->PointerType->TypePointedTo != paramBType->PointerType->TypePointedTo) return FALSE;
			}
			else if (paramAType->IsGenericParameter || paramBType->IsGenericParameter) 
			{
				if (paramAType->IsGenericParameter)
				{
					if (!paramBType->IsGenericParameter)
					{
						free(paramAType);
					}
					else
					{
						free(paramAType);
						free(paramBType);
					}
				}
				else
				{
					free(paramBType);
					return FALSE;
				}
			}
			else
			{
				if (paramAType != paramBType) return FALSE;
			}
		}
	}

	return TRUE;
}

// FieldSignature

FieldSignature* FieldSignature_Create()
{
    return (FieldSignature*)calloc(1, sizeof(FieldSignature));
}

void FieldSignature_Destroy(FieldSignature* pFieldSignature)
{
    if (pFieldSignature->CustomModifiers)
    {
        for (uint32_t index = 0; index < pFieldSignature->CustomModifierCount; ++index) SignatureCustomModifier_Destroy(pFieldSignature->CustomModifiers[index]);
        free(pFieldSignature->CustomModifiers);
    }
    if (pFieldSignature->Type) SignatureType_Destroy(pFieldSignature->Type);
    free(pFieldSignature);
}

FieldSignature* FieldSignature_Expand(uint8_t* pSignature, CLIFile* pCLIFile)
{
    FieldSignature* fieldSignature = NULL;
	FieldSignature_Parse(pSignature, &fieldSignature, pCLIFile);
    return fieldSignature;
}

uint8_t* FieldSignature_Parse(uint8_t* pCursor, FieldSignature** pFieldSignature, CLIFile* pCLIFile)
{
    *pFieldSignature = FieldSignature_Create();
    FieldSignature* fieldSignature = *pFieldSignature;
    ++pCursor; // 0x06
    while (*pCursor == SignatureElementType_CustomModifier_Required ||
           *pCursor == SignatureElementType_CustomModifier_Optional)
    {
        ++fieldSignature->CustomModifierCount;
        fieldSignature->CustomModifiers = (SignatureCustomModifier**)realloc(fieldSignature->CustomModifiers, sizeof(SignatureCustomModifier*) * fieldSignature->CustomModifierCount);
        pCursor = SignatureCustomModifier_Parse(pCursor, &fieldSignature->CustomModifiers[fieldSignature->CustomModifierCount - 1], pCLIFile);
    }
    pCursor = SignatureType_Parse(pCursor, &fieldSignature->Type, pCLIFile);
    return pCursor;
}

// PropertySignature

PropertySignature* PropertySignature_Create()
{
    return (PropertySignature*)calloc(1, sizeof(PropertySignature));
}

void PropertySignature_Destroy(PropertySignature* pPropertySignature)
{
    if (pPropertySignature->CustomModifiers)
    {
        for (uint32_t index = 0; index < pPropertySignature->CustomModifierCount; ++index) SignatureCustomModifier_Destroy(pPropertySignature->CustomModifiers[index]);
        free(pPropertySignature->CustomModifiers);
    }
    if (pPropertySignature->Parameters)
    {
        for (uint32_t index = 0; index < pPropertySignature->ParameterCount; ++index) SignatureParameter_Destroy(pPropertySignature->Parameters[index]);
        free(pPropertySignature->Parameters);
    }
    if (pPropertySignature->Type) SignatureType_Destroy(pPropertySignature->Type);
    free(pPropertySignature);
}

PropertySignature* PropertySignature_Expand(uint8_t* pSignature, CLIFile* pCLIFile)
{
    PropertySignature* propertySignature = NULL;
	PropertySignature_Parse(pSignature, &propertySignature, pCLIFile);
    return propertySignature;
}

uint8_t* PropertySignature_Parse(uint8_t* pCursor, PropertySignature** pPropertySignature, CLIFile* pCLIFile)
{
    *pPropertySignature = PropertySignature_Create();
    PropertySignature* propertySignature = *pPropertySignature;
    propertySignature->HasThis = (*pCursor & SignatureCallConvention_HasThis) != 0;
    ++pCursor;

    pCursor = CLIFile_GetCompressedUnsigned(pCursor, &propertySignature->ParameterCount);
    while (*pCursor == SignatureElementType_CustomModifier_Required ||
           *pCursor == SignatureElementType_CustomModifier_Optional)
    {
        ++propertySignature->CustomModifierCount;
        propertySignature->CustomModifiers = (SignatureCustomModifier**)realloc(propertySignature->CustomModifiers, sizeof(SignatureCustomModifier*) * propertySignature->CustomModifierCount);
        pCursor = SignatureCustomModifier_Parse(pCursor, &propertySignature->CustomModifiers[propertySignature->CustomModifierCount - 1], pCLIFile);
    }
    pCursor = SignatureType_Parse(pCursor, &propertySignature->Type, pCLIFile);
    if (propertySignature->ParameterCount > 0)
    {
        propertySignature->Parameters = (SignatureParameter**)calloc(propertySignature->ParameterCount, sizeof(SignatureParameter*));
        for (uint32_t index = 0; index < propertySignature->ParameterCount; ++index) pCursor = SignatureParameter_Parse(pCursor, &propertySignature->Parameters[index], pCLIFile);
    }
    return pCursor;
}

// LocalsSignature

LocalsSignature* LocalsSignature_Create()
{
    return (LocalsSignature*)calloc(1, sizeof(LocalsSignature));
}

void LocalsSignature_Destroy(LocalsSignature* pLocalsSignature)
{
    if (pLocalsSignature->LocalVariables)
    {
		//printf("Destroying all locals\n");
        for (uint32_t index = 0; index < pLocalsSignature->LocalVariableCount; ++index)
		{
			//printf("Destroying local signature at index %i\n", (int)index);
			SignatureLocalVariable_Destroy(pLocalsSignature->LocalVariables[index]);
			//printf("Finished destroying local signature at index %i\n", (int)index);
		}
		//printf("Finished destroying all locals.\n");
        free(pLocalsSignature->LocalVariables);
    }
    free(pLocalsSignature);
}

LocalsSignature* LocalsSignature_Expand(uint8_t* pSignature, CLIFile* pCLIFile)
{
    LocalsSignature* localsSignature = NULL;
	LocalsSignature_Parse(pSignature, &localsSignature, pCLIFile);
    return localsSignature;
}

uint8_t* LocalsSignature_Parse(uint8_t* pCursor, LocalsSignature** pLocalsSignature, CLIFile* pCLIFile)
{
    *pLocalsSignature = LocalsSignature_Create();
    LocalsSignature* localsSignature = *pLocalsSignature;
    ++pCursor;

    pCursor = CLIFile_GetCompressedUnsigned(pCursor, &localsSignature->LocalVariableCount);
    if (localsSignature->LocalVariableCount > 0)
    {
        localsSignature->LocalVariables = (SignatureLocalVariable**)calloc(localsSignature->LocalVariableCount, sizeof(SignatureLocalVariable*));
        for (uint32_t index = 0; index < localsSignature->LocalVariableCount; ++index) pCursor = SignatureLocalVariable_Parse(pCursor, &localsSignature->LocalVariables[index], pCLIFile);
    }
    return pCursor;
}

// SignatureReturnType

SignatureReturnType* SignatureReturnType_Create()
{
    return (SignatureReturnType*)calloc(1, sizeof(SignatureReturnType));
}

void SignatureReturnType_Destroy(SignatureReturnType* pReturnType)
{
    if (pReturnType->CustomModifiers)
    {
        for (uint32_t index = 0; index < pReturnType->CustomModifierCount; ++index) SignatureCustomModifier_Destroy(pReturnType->CustomModifiers[index]);
        free(pReturnType->CustomModifiers);
    }
    if (pReturnType->Type) SignatureType_Destroy(pReturnType->Type);
    free(pReturnType);
}

uint8_t* SignatureReturnType_Parse(uint8_t* pCursor, SignatureReturnType** pReturnType, CLIFile* pCLIFile)
{
    *pReturnType = SignatureReturnType_Create();
    SignatureReturnType* returnType = *pReturnType;
    while (*pCursor == SignatureElementType_CustomModifier_Required ||
           *pCursor == SignatureElementType_CustomModifier_Optional)
    {
        ++returnType->CustomModifierCount;
        returnType->CustomModifiers = (SignatureCustomModifier**)realloc(returnType->CustomModifiers, sizeof(SignatureCustomModifier*) * returnType->CustomModifierCount);
        pCursor = SignatureCustomModifier_Parse(pCursor, &returnType->CustomModifiers[returnType->CustomModifierCount - 1], pCLIFile);
    }
    if (*pCursor == SignatureElementType_TypedByReference)
    {
        returnType->TypedByReference = TRUE;
        return pCursor + 1;
    }
    if (*pCursor == SignatureElementType_Void)
    {
        returnType->Void = TRUE;
        return pCursor + 1;
    }
    if (*pCursor == SignatureElementType_ByReference)
    {
        returnType->ByReference = TRUE;
        ++pCursor;
    }
    pCursor = SignatureType_Parse(pCursor, &returnType->Type, pCLIFile);
    return pCursor;
}

// SignatureParameter

SignatureParameter* SignatureParameter_Create()
{
    return (SignatureParameter*)calloc(1, sizeof(SignatureParameter));
}

void SignatureParameter_Destroy(SignatureParameter* pParameter)
{
    if (pParameter->CustomModifiers)
    {
        for (uint32_t index = 0; index < pParameter->CustomModifierCount; ++index) SignatureCustomModifier_Destroy(pParameter->CustomModifiers[index]);
        free(pParameter->CustomModifiers);
    }
    if (pParameter->Type) SignatureType_Destroy(pParameter->Type);
    free(pParameter);
}

uint8_t* SignatureParameter_Parse(uint8_t* pCursor, SignatureParameter** pParameter, CLIFile* pCLIFile)
{
    *pParameter = SignatureParameter_Create();
    SignatureParameter* parameter = *pParameter;
    while (*pCursor == SignatureElementType_CustomModifier_Required ||
           *pCursor == SignatureElementType_CustomModifier_Optional)
    {
        ++parameter->CustomModifierCount;
        parameter->CustomModifiers = (SignatureCustomModifier**)realloc(parameter->CustomModifiers, sizeof(SignatureCustomModifier*) * parameter->CustomModifierCount);
        pCursor = SignatureCustomModifier_Parse(pCursor, &parameter->CustomModifiers[parameter->CustomModifierCount - 1], pCLIFile);
    }
    if (*pCursor == SignatureElementType_TypedByReference) // Managed pointer
    {
        parameter->TypedByReference = TRUE;
        return pCursor + 1;
    }
    if (*pCursor == SignatureElementType_ByReference)
    {
        parameter->ByReference = TRUE;
        ++pCursor;
    }
    pCursor = SignatureType_Parse(pCursor, &parameter->Type, pCLIFile);
    return pCursor;
}

// SignatureCustomModifier

SignatureCustomModifier* SignatureCustomModifier_Create()
{
    return (SignatureCustomModifier*)calloc(1, sizeof(SignatureCustomModifier));
}

void SignatureCustomModifier_Destroy(SignatureCustomModifier* pCustomModifier)
{
    free(pCustomModifier);
}

uint8_t* SignatureCustomModifier_Parse(uint8_t* pCursor, SignatureCustomModifier** pCustomModifier, CLIFile* pCLIFile)
{
    *pCustomModifier = SignatureCustomModifier_Create();
    SignatureCustomModifier* customModifier = *pCustomModifier;
    
    customModifier->Optional = *pCursor == SignatureElementType_CustomModifier_Optional; ++pCursor;
    pCursor = CLIFile_GetCompressedUnsigned(pCursor, &customModifier->TypeDefOrRefOrSpecToken);
    return pCursor;
}

// SignatureType

SignatureType* SignatureType_Create()
{
    return (SignatureType*)calloc(1, sizeof(SignatureType));
}

void SignatureType_Destroy(SignatureType* pType)
{
	switch(pType->ElementType)
	{
		case SignatureElementType_Array:
			SignatureType_Destroy(pType->ArrayType);
			SignatureArrayShape_Destroy(pType->ArrayShape);
			break;
		case SignatureElementType_FunctionPointer:
			MethodSignature_Destroy(pType->FnPtrMethodSignature);
			break;
		case SignatureElementType_GenericInstantiation:
		{
			for (uint32_t index = 0; index < pType->GenericInstGenericArgumentCount; ++index) SignatureType_Destroy(pType->GenericInstGenericArguments[index]);
			if (pType->GenericInstGenericArguments) free(pType->GenericInstGenericArguments);
			break;
		}
		case SignatureElementType_Pointer:
		{
			for (uint32_t index = 0; index < pType->PtrCustomModifierCount; ++index) SignatureCustomModifier_Destroy(pType->PtrCustomModifiers[index]);
			if (pType->PtrCustomModifiers) free(pType->PtrCustomModifiers);
			SignatureType_Destroy(pType->PtrType);
			break;
		}
		case SignatureElementType_SingleDimensionArray:
		{
			for (uint32_t index = 0; index < pType->SZArrayCustomModifierCount; ++index) SignatureCustomModifier_Destroy(pType->SZArrayCustomModifiers[index]);
			if (pType->SZArrayCustomModifiers) free(pType->SZArrayCustomModifiers);
			SignatureType_Destroy(pType->SZArrayType);
			break;
		}

	}
    free(pType);
}

uint8_t* SignatureType_Parse(uint8_t* pCursor, SignatureType** pType, CLIFile* pCLIFile)
{
    *pType = SignatureType_Create();
    SignatureType* type = *pType;
    type->ElementType = *pCursor; ++pCursor;
    switch (type->ElementType)
    {
    case SignatureElementType_Array:
        pCursor = SignatureType_Parse(pCursor, &type->ArrayType, pCLIFile);
        pCursor = SignatureArrayShape_Parse(pCursor, &type->ArrayShape, pCLIFile);
        break;
    case SignatureElementType_Class:
        pCursor = CLIFile_GetCompressedUnsigned(pCursor, &type->ClassTypeDefOrRefOrSpecToken);
        break;
    case SignatureElementType_FunctionPointer:
        pCursor = MethodSignature_Parse(pCursor, &type->FnPtrMethodSignature, pCLIFile);
        break;
    case SignatureElementType_GenericInstantiation:
        type->GenericInstClass = *pCursor == SignatureElementType_Class;
        type->GenericInstValue = *pCursor == SignatureElementType_ValueType;
        ++pCursor;
        pCursor = CLIFile_GetCompressedUnsigned(pCursor, &type->GenericInstTypeDefOrRefOrSpecToken);
        pCursor = CLIFile_GetCompressedUnsigned(pCursor, &type->GenericInstGenericArgumentCount);
        if (type->GenericInstGenericArgumentCount > 0)
        {
            type->GenericInstGenericArguments = (SignatureType**)calloc(type->GenericInstGenericArgumentCount, sizeof(SignatureType*) * type->GenericInstGenericArgumentCount);
            for (uint32_t index = 0; index < type->GenericInstGenericArgumentCount; ++index) pCursor = SignatureType_Parse(pCursor, &type->GenericInstGenericArguments[index], pCLIFile);
        }
        break;
    case SignatureElementType_MethodVar:
        pCursor = CLIFile_GetCompressedUnsigned(pCursor, &type->MVarNumber);
        break;
    case SignatureElementType_Pointer:
        while (*pCursor == SignatureElementType_CustomModifier_Required ||
               *pCursor == SignatureElementType_CustomModifier_Optional)
        {
            ++type->PtrCustomModifierCount;
            type->PtrCustomModifiers = (SignatureCustomModifier**)realloc(type->PtrCustomModifiers, sizeof(SignatureCustomModifier*) * type->PtrCustomModifierCount);
            pCursor = SignatureCustomModifier_Parse(pCursor, &type->PtrCustomModifiers[type->PtrCustomModifierCount - 1], pCLIFile);
        }
        if (*pCursor == SignatureElementType_Void)
        {
            type->PtrVoid = TRUE;
            ++pCursor;
        }
        else pCursor = SignatureType_Parse(pCursor, &type->PtrType, pCLIFile);
        break;
    case SignatureElementType_SingleDimensionArray:
        while (*pCursor == SignatureElementType_CustomModifier_Required ||
               *pCursor == SignatureElementType_CustomModifier_Optional)
        {
            ++type->SZArrayCustomModifierCount;
            type->SZArrayCustomModifiers = (SignatureCustomModifier**)realloc(type->SZArrayCustomModifiers, sizeof(SignatureCustomModifier*) * type->SZArrayCustomModifierCount);
            pCursor = SignatureCustomModifier_Parse(pCursor, &type->SZArrayCustomModifiers[type->SZArrayCustomModifierCount - 1], pCLIFile);
        }
        pCursor = SignatureType_Parse(pCursor, &type->SZArrayType, pCLIFile);
        break;
    case SignatureElementType_ValueType:
        pCursor = CLIFile_GetCompressedUnsigned(pCursor, &type->ValueTypeDefOrRefOrSpecToken);
        break;
    case SignatureElementType_Var:
        pCursor = CLIFile_GetCompressedUnsigned(pCursor, &type->VarNumber);
        break;
    default: break;
    }
    return pCursor;
}

SignatureType* SignatureType_Expand(uint8_t* pSignature, CLIFile* pCLIFile)
{
    SignatureType* signatureType = NULL;
	SignatureType_Parse(pSignature, &signatureType, pCLIFile);
    return signatureType;
}

// SignatureMethodSpecification

SignatureMethodSpecification* SignatureMethodSpecification_Create()
{
    return (SignatureMethodSpecification*)calloc(1, sizeof(SignatureMethodSpecification));
}

void SignatureMethodSpecification_Destroy(SignatureMethodSpecification* pMethodSpecification)
{
    if (pMethodSpecification->GenericInstGenericArguments)
    {
        for (uint32_t index = 0; index < pMethodSpecification->GenericInstGenericArgumentCount; ++index) SignatureType_Destroy(pMethodSpecification->GenericInstGenericArguments[index]);
        free(pMethodSpecification->GenericInstGenericArguments);
    }
    free(pMethodSpecification);
}

uint8_t* SignatureMethodSpecification_Parse(uint8_t* pCursor, SignatureMethodSpecification** pMethodSpecification, CLIFile* pCLIFile)
{
    *pMethodSpecification = SignatureMethodSpecification_Create();
    SignatureMethodSpecification* methodSpecification = *pMethodSpecification;
    ++pCursor; // 0x0A
    pCursor = CLIFile_GetCompressedUnsigned(pCursor, &methodSpecification->GenericInstGenericArgumentCount);
    if (methodSpecification->GenericInstGenericArgumentCount > 0)
    {
        methodSpecification->GenericInstGenericArguments = (SignatureType**)calloc(methodSpecification->GenericInstGenericArgumentCount, sizeof(SignatureType*) * methodSpecification->GenericInstGenericArgumentCount);
        for (uint32_t index = 0; index < methodSpecification->GenericInstGenericArgumentCount; ++index) pCursor = SignatureType_Parse(pCursor, &methodSpecification->GenericInstGenericArguments[index], pCLIFile);
    }
    return pCursor;
}

SignatureMethodSpecification* SignatureMethodSpecification_Expand(uint8_t* pSignature, CLIFile* pCLIFile)
{
    SignatureMethodSpecification* signatureMethodSpecification = NULL;
	SignatureMethodSpecification_Parse(pSignature, &signatureMethodSpecification, pCLIFile);
    return signatureMethodSpecification;
}

// SignatureArrayShape

SignatureArrayShape* SignatureArrayShape_Create()
{
    return (SignatureArrayShape*)calloc(1, sizeof(SignatureArrayShape));
}

void SignatureArrayShape_Destroy(SignatureArrayShape* pArrayShape)
{
    if (pArrayShape->Sizes) free(pArrayShape->Sizes);
    if (pArrayShape->LowerBounds) free(pArrayShape->LowerBounds);
    free(pArrayShape);
}

uint8_t* SignatureArrayShape_Parse(uint8_t* pCursor, SignatureArrayShape** pArrayShape, CLIFile* pCLIFile)
{
    *pArrayShape = SignatureArrayShape_Create();
    SignatureArrayShape* arrayShape = *pArrayShape;
    pCursor = CLIFile_GetCompressedUnsigned(pCursor, &arrayShape->Rank);
    pCursor = CLIFile_GetCompressedUnsigned(pCursor, &arrayShape->SizeCount);
    if (arrayShape->SizeCount > 0)
    {
        arrayShape->Sizes = (uint32_t*)calloc(arrayShape->SizeCount, sizeof(uint32_t));
        for (uint32_t index = 0; index < arrayShape->SizeCount; ++index) pCursor = CLIFile_GetCompressedUnsigned(pCursor, &arrayShape->Sizes[index]);
    }
    pCursor = CLIFile_GetCompressedUnsigned(pCursor, &arrayShape->LowerBoundCount);
    if (arrayShape->LowerBoundCount > 0)
    {
        arrayShape->LowerBounds = (int32_t*)calloc(arrayShape->LowerBoundCount, sizeof(int32_t));
        for (uint32_t index = 0; index < arrayShape->LowerBoundCount; ++index) pCursor = CLIFile_GetCompressedSigned(pCursor, &arrayShape->LowerBounds[index]);
    }
    return pCursor;
}

// SignatureLocalVariable

SignatureLocalVariable* SignatureLocalVariable_Create()
{
    return (SignatureLocalVariable*)calloc(1, sizeof(SignatureLocalVariable));
}

void SignatureLocalVariable_Destroy(SignatureLocalVariable* pLocalVariable)
{
    if (pLocalVariable->CustomModifiers)
    {
        for (uint32_t index = 0; index < pLocalVariable->CustomModifierCount; ++index) SignatureCustomModifier_Destroy(pLocalVariable->CustomModifiers[index]);
        free(pLocalVariable->CustomModifiers);
    }
	//printf("Finished destroying custom modifiers\n");
    if (pLocalVariable->Type) SignatureType_Destroy(pLocalVariable->Type);
	//printf("Finished destroying the signature type\n");
    free(pLocalVariable);
}

uint8_t* SignatureLocalVariable_Parse(uint8_t* pCursor, SignatureLocalVariable** pLocalVariable, CLIFile* pCLIFile)
{
    *pLocalVariable = SignatureLocalVariable_Create();
    SignatureLocalVariable* localVariable = *pLocalVariable;
    if (*pCursor == SignatureElementType_TypedByReference)
    {
        localVariable->TypedByReference = TRUE;
        ++pCursor;
    }
    else
    {
        while (*pCursor == SignatureElementType_CustomModifier_Required ||
               *pCursor == SignatureElementType_CustomModifier_Optional)
        {
            ++localVariable->CustomModifierCount;
            localVariable->CustomModifiers = (SignatureCustomModifier**)realloc(localVariable->CustomModifiers, sizeof(SignatureCustomModifier*) * localVariable->CustomModifierCount);
            pCursor = SignatureCustomModifier_Parse(pCursor, &localVariable->CustomModifiers[localVariable->CustomModifierCount - 1], pCLIFile);
        }
        if (*pCursor == SignatureElementType_Pinned)
        {
            localVariable->IsPinned = TRUE;
            ++pCursor;
        }
        if (*pCursor == SignatureElementType_ByReference)
        {
            localVariable->ByReference = TRUE;
            ++pCursor;
        }
        pCursor = SignatureType_Parse(pCursor, &localVariable->Type, pCLIFile);
    }
    return pCursor;
}
