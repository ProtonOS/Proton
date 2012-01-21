#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* MethodDefinition_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_MethodDefinition)) != 0)
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

const uint8_t* MethodDefinition_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t methodBodyVirtualAddress = 0;
    PESectionHeader* methodBodySectionHeader = NULL;
    const uint8_t* methodBody = NULL;
    uint8_t methodBodySize = 0;
    uint32_t parameterListIndex = 0;
    uint32_t* parameterListIndexes = (uint32_t*)calloc(pFile->MethodDefinitionCount + 1, sizeof(uint32_t));
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->MethodDefinitionCount; ++index)
    {
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
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodDefinitions[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Blobs32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodDefinitions[index].Signature = MetaData_GetCompressedUnsigned(pFile->BlobsHeap + heapIndex, &pFile->MethodDefinitions[index].SignatureLength);
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
            methodBodySectionHeader = PEFile_GetSection(pFile->PEFile->SectionHeaders, pFile->PEFile->PEHeader->NumberOfSections, methodBodyVirtualAddress);
            methodBody = pFile->PEFile->Data + methodBodySectionHeader->PointerToRawData + (methodBodyVirtualAddress - methodBodySectionHeader->VirtualAddress);
            methodBodySize = 1;
            pFile->MethodDefinitions[index].Body.Flags = *(methodBody + MethodDefinitionBody_Offset_Flags) & MethodDefinitionBody_Flags_HeaderType_Mask;
            pFile->MethodDefinitions[index].Body.IsFat = FALSE;
            pFile->MethodDefinitions[index].Body.MaxStack = MethodDefinitionBody_Tiny_MaxStack;
            pFile->MethodDefinitions[index].Body.CodeSize = *(methodBody + MethodDefinitionBody_Offset_Flags) >> MethodDefinitionBody_Flags_HeaderType_Bits;
            pFile->MethodDefinitions[index].Body.LocalVariableSignatureToken = 0;
            if (pFile->MethodDefinitions[index].Body.Flags == MethodDefinitionBody_Flags_HeaderType_Fat)
            {
                pFile->MethodDefinitions[index].Body.Flags = (*(uint16_t*)(methodBody + MethodDefinitionBody_Offset_Fat_Flags)) & MethodDefinitionBody_Fat_Flags_Mask;
                methodBodySize = ((*(uint16_t*)(methodBody + MethodDefinitionBody_Offset_Fat_Flags)) >> MethodDefinitionBody_Fat_Flags_Bits) * MethodDefinitionBody_Fat_BodySize_Multiplier;
                pFile->MethodDefinitions[index].Body.IsFat = TRUE;
                pFile->MethodDefinitions[index].Body.MaxStack = *(uint16_t*)(methodBody + MethodDefinitionBody_Offset_Fat_MaxStack);
                pFile->MethodDefinitions[index].Body.CodeSize = *(uint32_t*)(methodBody + MethodDefinitionBody_Offset_Fat_CodeSize);
                pFile->MethodDefinitions[index].Body.LocalVariableSignatureToken = *(uint32_t*)(methodBody + MethodDefinitionBody_Offset_Fat_LocalVariableSignatureToken);
            }
            pFile->MethodDefinitions[index].Body.Code = methodBody + methodBodySize;
            //printf("Method: %s, RVA: %u, HeaderSize: %u, CodeSize: %u\n", pFile->MethodDefinitions[index].Name, (unsigned int)methodBodyVirtualAddress, (unsigned int)methodBodySize, (unsigned int)pFile->MethodDefinitions[index].Body.CodeSize);

            if (pFile->MethodDefinitions[index].Body.IsFat && (pFile->MethodDefinitions[index].Body.Flags & MethodDefinitionBody_Fat_Flags_HasDataSection) != 0)
            {
                uint32_t methodDataVirtualAddress = methodBodyVirtualAddress + methodBodySize + pFile->MethodDefinitions[index].Body.CodeSize;
                while ((methodDataVirtualAddress % MethodDefinitionData_ByteAlignment) != 0) ++methodDataVirtualAddress;
                const uint8_t* methodDataStart = pFile->PEFile->Data + methodBodySectionHeader->PointerToRawData + (methodDataVirtualAddress - methodBodySectionHeader->VirtualAddress);
                const uint8_t* methodData = methodDataStart;
                bool_t hasData = TRUE;
                while (hasData)
                {
                    uint8_t methodDataFlags = *(methodData + MethodDefinitionData_Offset_Flags);
                    uint32_t methodDataSize = *(methodData + MethodDefinitionData_Offset_Size);
                    bool_t methodDataFlagsExceptions = (methodDataFlags & MethodDefinitionData_Flags_ExceptionTable) != 0;
                    bool_t methodDataFlagsFat = (methodDataFlags & MethodDefinitionData_Flags_Fat) != 0;
                    bool_t methodDataFlagsHasData = (methodDataFlags & MethodDefinitionData_Flags_HasAnotherDataSection) != 0;
                    if (methodDataFlagsFat) methodDataSize = (*(uint32_t*)(methodData + MethodDefinitionData_Offset_Size)) & MethodDefinitionData_Fat_Size_Mask;

                    if (methodDataFlagsExceptions)
                    {
                        uint32_t methodDataExceptionCount = (methodDataSize - MethodDefinitionData_HeaderLength) / MethodDefinitionException_HeaderLength;
                        if (methodDataFlagsFat) methodDataExceptionCount = (methodDataSize - MethodDefinitionData_HeaderLength) / MethodDefinitionException_Fat_HeaderLength;
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
                        uint8_t methodDataFlags = *(methodData + MethodDefinitionData_Offset_Flags);
                        uint32_t methodDataSize = *(methodData + MethodDefinitionData_Offset_Size);
                        bool_t methodDataFlagsExceptions = (methodDataFlags & MethodDefinitionData_Flags_ExceptionTable) != 0;
                        bool_t methodDataFlagsFat = (methodDataFlags & MethodDefinitionData_Flags_Fat) != 0;
                        bool_t methodDataFlagsHasData = (methodDataFlags & MethodDefinitionData_Flags_HasAnotherDataSection) != 0;
                        if (methodDataFlagsFat) methodDataSize = (*(uint32_t*)(methodData + MethodDefinitionData_Offset_Size)) & MethodDefinitionData_Fat_Size_Mask;

                        if (methodDataFlagsExceptions)
                        {
                            uint32_t methodDataExceptionCount = (methodDataSize - MethodDefinitionData_HeaderLength) / MethodDefinitionException_HeaderLength;
                            if (methodDataFlagsFat) methodDataExceptionCount = (methodDataSize - MethodDefinitionData_HeaderLength) / MethodDefinitionException_Fat_HeaderLength;
                            const uint8_t* methodDataException = methodData + MethodDefinitionData_HeaderLength;
                            for (uint32_t methodDataExceptionIndex = 0; methodDataExceptionIndex < methodDataExceptionCount; ++methodDataExceptionIndex)
                            {
                                uint32_t methodDataExceptionOffset = methodDataExceptionIndex * MethodDefinitionException_HeaderLength;
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].Flags = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_Flags);
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].TryOffset = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_TryOffset);
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].TryLength = *(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_TryLength);
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerOffset = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_HandlerOffset);
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerLength = *(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_HandlerLength);
                                pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].ClassTokenOrFilterOffset = *(uint32_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_ClassTokenOrFilterOffset);
                                if (methodDataFlagsFat)
                                {
                                    methodDataExceptionOffset = methodDataExceptionIndex * MethodDefinitionException_Fat_HeaderLength;
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].Flags = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_Fat_Flags);
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].TryOffset = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_Fat_TryOffset);
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].TryLength = *(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_Fat_TryLength);
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerOffset = *(uint16_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_Fat_HandlerOffset);
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].HandlerLength = *(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_Fat_HandlerLength);
                                    pFile->MethodDefinitions[index].Exceptions[methodExceptionIndex].ClassTokenOrFilterOffset = *(uint32_t*)(methodDataException + methodDataExceptionOffset + MethodDefinitionException_Offset_Fat_ClassTokenOrFilterOffset);
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

void MethodDefinition_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->CustomAttributeCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_MethodDefinition) ++pFile->CustomAttributes[index].Parent.MethodDefinition->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->MethodDefinitionCount; ++index)
    {
        if (pFile->MethodDefinitions[index].CustomAttributeCount > 0)
        {
            pFile->MethodDefinitions[index].CustomAttributes = (CustomAttribute**)calloc(pFile->MethodDefinitions[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_MethodDefinition &&
                    pFile->CustomAttributes[searchIndex].Parent.MethodDefinition == &pFile->MethodDefinitions[index])
                {
                    pFile->MethodDefinitions[index].CustomAttributes[customAttributeIndex] = &pFile->CustomAttributes[searchIndex];
                    ++customAttributeIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->DeclSecurityCount; ++index)
    {
        if (pFile->DeclSecurities[index].TypeOfParent == HasDeclSecurity_Type_MethodDefinition) pFile->DeclSecurities[index].Parent.MethodDefinition->DeclSecurity = &pFile->DeclSecurities[index];
    }
    for (uint32_t index = 1; index <= pFile->GenericParameterCount; ++index)
    {
        if (pFile->GenericParameters[index].TypeOfOwner == TypeOrMethodDef_Type_MethodDefinition) ++pFile->GenericParameters[index].Owner.MethodDefinition->GenericParameterCount;
    }
    for (uint32_t index = 1; index <= pFile->MethodDefinitionCount; ++index)
    {
        if (pFile->MethodDefinitions[index].GenericParameterCount > 0)
        {
            pFile->MethodDefinitions[index].GenericParameters = (GenericParameter**)calloc(pFile->MethodDefinitions[index].GenericParameterCount, sizeof(GenericParameter*));
            for (uint32_t searchIndex = 1, genericParameterIndex = 0; searchIndex <= pFile->GenericParameterCount; ++searchIndex)
            {
                if (pFile->GenericParameters[searchIndex].TypeOfOwner == TypeOrMethodDef_Type_MethodDefinition &&
                    pFile->GenericParameters[searchIndex].Owner.MethodDefinition == &pFile->MethodDefinitions[index])
                {
                    pFile->MethodDefinitions[index].GenericParameters[genericParameterIndex] = &pFile->GenericParameters[searchIndex];
                    ++genericParameterIndex;
                }
            }
        }
    }
    for (uint32_t index = 1; index <= pFile->ImplementationMapCount; ++index)
    {
        if (pFile->ImplementationMaps[index].TypeOfMemberForwarded == MemberForwarded_Type_MethodDefinition) pFile->ImplementationMaps[index].MemberForwarded.MethodDefinition->ImplementationMap = &pFile->ImplementationMaps[index];
    }
    for (uint32_t index = 1; index <= pFile->MemberReferenceCount; ++index)
    {
        if (pFile->MemberReferences[index].TypeOfParent == MemberRefParent_Type_MethodDefinition) ++pFile->MemberReferences[index].Parent.MethodDefinition->MemberReferenceCount;
    }
    for (uint32_t index = 1; index <= pFile->MethodDefinitionCount; ++index)
    {
        if (pFile->MethodDefinitions[index].MemberReferenceCount > 0)
        {
            pFile->MethodDefinitions[index].MemberReferences = (MemberReference**)calloc(pFile->MethodDefinitions[index].MemberReferenceCount, sizeof(MemberReference*));
            for (uint32_t searchIndex = 1, memberReferenceIndex = 0; searchIndex <= pFile->MemberReferenceCount; ++searchIndex)
            {
                if (pFile->MemberReferences[searchIndex].TypeOfParent == MemberRefParent_Type_MethodDefinition &&
                    pFile->MemberReferences[searchIndex].Parent.MethodDefinition == &pFile->MethodDefinitions[index])
                {
                    pFile->MethodDefinitions[index].MemberReferences[memberReferenceIndex] = &pFile->MemberReferences[searchIndex];
                    ++memberReferenceIndex;
                }
            }
        }
    }
}
