#include <stdio.h>
#include <stdlib.h>

#include <CLR/CLIFile.h>

const uint8_t* MethodSemantics_Initialize(CLIFile* pFile, const uint8_t* pTableData)
{
    if ((pFile->TablesHeader->PresentTables & (1ull << MetaData_Table_MethodSemantics)) != 0)
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

const uint8_t* MethodSemantics_Load(CLIFile* pFile, const uint8_t* pTableData)
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

        if (pFile->EventCount > HasSemantics_Type_MaxRows16Bit ||
            pFile->PropertyCount > HasSemantics_Type_MaxRows16Bit) { associationIndex = *(uint32_t*)pTableData; pTableData += 4; }
        else { associationIndex = *(uint16_t*)pTableData; pTableData += 2; }
        pFile->MethodSemantics[index].TypeOfAssociation = associationIndex & HasSemantics_Type_Mask;
        associationRow = associationIndex >> HasSemantics_Type_Bits;
        switch (pFile->MethodSemantics[index].TypeOfAssociation)
        {
        case HasSemantics_Type_Event:
            if (associationRow == 0 || associationRow > pFile->EventCount) Panic("MethodSemantics_Load Event");
            pFile->MethodSemantics[index].Association.Event = &pFile->Events[associationRow];
            break;
        case HasSemantics_Type_Property:
            if (associationRow == 0 || associationRow > pFile->PropertyCount) Panic("MethodSemantics_Load Property");
            pFile->MethodSemantics[index].Association.Property = &pFile->Properties[associationRow];
            break;
        default: break;
        }
    }
    return pTableData;
}

void MethodSemantics_Link(CLIFile* pFile)
{
}
