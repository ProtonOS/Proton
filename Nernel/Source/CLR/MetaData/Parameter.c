#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* Parameter_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_Parameter)) != 0)
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

const uint8_t* Parameter_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    for (uint32_t index = 1, heapIndex = 0; index <= pFile->ParameterCount; ++index)
    {
        pFile->Parameters[index].Flags = *(uint16_t*)pTableData; pTableData += 2;
        pFile->Parameters[index].Sequence = *(uint16_t*)pTableData; pTableData += 2;
        if ((pFile->TablesHeader->HeapOffsetSizes & MetaDataTablesHeader_HeapOffsetSizes_Strings32Bit) != 0) { heapIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { heapIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->Parameters[index].Name = (const char*)(pFile->StringsHeap + heapIndex);
    }
    return pTableData;
}

void Parameter_Link(CLIFile* pFile)
{
    for (uint32_t index = 1; index <= pFile->ConstantCount; ++index)
    {
        if (pFile->Constants[index].TypeOfParent == HasConstant_Type_Parameter) pFile->Constants[index].Parent.Parameter->Constant = &pFile->Constants[index];
    }
    for (uint32_t index = 1; index <= pFile->ParameterCount; ++index)
    {
        if (pFile->CustomAttributes[index].TypeOfParent == HasCustomAttribute_Type_Parameter) ++pFile->CustomAttributes[index].Parent.Parameter->CustomAttributeCount;
    }
    for (uint32_t index = 1; index <= pFile->ParameterCount; ++index)
    {
        if (pFile->Parameters[index].CustomAttributeCount > 0)
        {
            pFile->Parameters[index].CustomAttributes = (CustomAttribute**)calloc(pFile->Parameters[index].CustomAttributeCount, sizeof(CustomAttribute*));
            for (uint32_t searchIndex = 1, customAttributeIndex = 0; searchIndex <= pFile->CustomAttributeCount; ++searchIndex)
            {
                if (pFile->CustomAttributes[searchIndex].TypeOfParent == HasCustomAttribute_Type_Parameter &&
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
        if (pFile->FieldMarshals[index].TypeOfParent == HasConstant_Type_Parameter) pFile->FieldMarshals[index].Parent.Parameter->FieldMarshal = &pFile->FieldMarshals[index];
    }
}
