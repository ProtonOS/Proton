#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* FieldLayout_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_FieldLayout)) != 0)
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

const uint8_t* FieldLayout_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t fieldIndex = 0;
    for (uint32_t index = 1; index <= pFile->FieldLayoutCount; ++index)
    {
        pFile->FieldLayouts[index].Offset = *(uint32_t*)pTableData; pTableData += 4;
        if (pFile->FieldCount > 0xFFFF) { fieldIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldIndex = *(uint16_t*)pTableData; pTableData += 2; }
        if (fieldIndex == 0 || fieldIndex > pFile->FieldCount) Panic("FieldLayout_Load Field");
        pFile->FieldLayouts[index].Field = &pFile->Fields[fieldIndex];
    }
    return pTableData;

}

void FieldLayout_Link(CLIFile* pFile)
{
}
