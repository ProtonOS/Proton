#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* FieldRVA_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_FieldRVA)) != 0)
    {
        pFile->FieldRVACount = *(uint32_t*)pTableData; pTableData += 4;
        pFile->FieldRVAs = (FieldRVA*)calloc(pFile->FieldRVACount, sizeof(FieldRVA));
    }
    return pTableData;
}

void FieldRVA_Cleanup(CLIFile* pFile)
{
    if (pFile->FieldRVAs)
    {
        for (uint32_t index = 0; index < pFile->FieldRVACount; ++index)
        {
        }
        free(pFile->FieldRVAs);
        pFile->FieldRVAs = NULL;
    }
}

const uint8_t* FieldRVA_Load(CLIFile* pFile, const uint8_t* pTableData)
{
    uint32_t fieldInitialValueVirtualAddress = 0;
    PESectionHeader* fieldInitialValueSectionHeader = NULL;
    const uint8_t* fieldInitialValue = NULL;
    uint32_t fieldIndex = 0;
    for (uint32_t index = 0; index < pFile->FieldRVACount; ++index)
    {
        fieldInitialValueVirtualAddress = *(uint32_t*)pTableData; pTableData += 4;
        fieldInitialValueSectionHeader = PEFile_GetSection(pFile->PEFile->SectionHeaders, pFile->PEFile->PEHeader->NumberOfSections, fieldInitialValueVirtualAddress);
        fieldInitialValue = pFile->PEFile->Data + fieldInitialValueSectionHeader->PointerToRawData + (fieldInitialValueVirtualAddress - fieldInitialValueSectionHeader->VirtualAddress);
        pFile->FieldRVAs[index].InitialValue = fieldInitialValue;

        if (pFile->FieldCount > 0xFFFF) { fieldIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { fieldIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->FieldRVAs[index].Field = &pFile->Fields[fieldIndex];
    }
    return pTableData;
}