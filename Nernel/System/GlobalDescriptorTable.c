#include "GlobalDescriptorTable.h"

extern void GlobalDescriptorTable_Update(GlobalDescriptorTableRegister* pRegister);

GlobalDescriptorTableRegister gGlobalDescriptorTableRegister;
GlobalDescriptorTableDescriptor gGlobalDescriptorTable_Descriptors[GLOBALDESCRIPTORTABLE_DESCRIPTORS_MAX];

void GlobalDescriptorTable_Startup()
{
    gGlobalDescriptorTableRegister.Limit = (sizeof(GlobalDescriptorTableDescriptor) * GLOBALDESCRIPTORTABLE_DESCRIPTORS_MAX) - 1;
    gGlobalDescriptorTableRegister.Address = (size_t)&gGlobalDescriptorTable_Descriptors[0];

    GlobalDescriptorTable_SetSegment(0, 0x00000000, 0x00000000, 0x00, 0x00);
    GlobalDescriptorTable_SetSegment(1, 0x00000000, 0xFFFFFFFF, GLOBALDESCRIPTORTABLE_READWRITEONEPRESENTACCESS | GLOBALDESCRIPTORTABLE_EXECUTABLEACCESS, GLOBALDESCRIPTORTABLE_SELECTOR32BITGRANULARITY4KBFLAGS);
    GlobalDescriptorTable_SetSegment(2, 0x00000000, 0xFFFFFFFF, GLOBALDESCRIPTORTABLE_READWRITEONEPRESENTACCESS, GLOBALDESCRIPTORTABLE_SELECTOR32BITGRANULARITY4KBFLAGS);
    GlobalDescriptorTable_SetSegment(3, 0x00000000, 0xFFFFFFFF, GLOBALDESCRIPTORTABLE_READWRITEONEPRESENTACCESS | GLOBALDESCRIPTORTABLE_EXECUTABLEACCESS | GLOBALDESCRIPTORTABLE_RING3ACCESS, GLOBALDESCRIPTORTABLE_SELECTOR32BITGRANULARITY4KBFLAGS);
    GlobalDescriptorTable_SetSegment(4, 0x00000000, 0xFFFFFFFF, GLOBALDESCRIPTORTABLE_READWRITEONEPRESENTACCESS | GLOBALDESCRIPTORTABLE_RING3ACCESS, GLOBALDESCRIPTORTABLE_SELECTOR32BITGRANULARITY4KBFLAGS);

    GlobalDescriptorTable_Update(&gGlobalDescriptorTableRegister);
}

void GlobalDescriptorTable_Shutdown()
{
}

void GlobalDescriptorTable_SetSegment(uint8_t pIndex, uint32_t pAddress, uint32_t pLimit, uint8_t pAccess, uint8_t pFlags)
{
    gGlobalDescriptorTable_Descriptors[pIndex].AddressLow = pAddress & 0xFFFF;
    gGlobalDescriptorTable_Descriptors[pIndex].AddressMid = (pAddress >> 16) & 0xFF;
    gGlobalDescriptorTable_Descriptors[pIndex].AddressHigh = (pAddress >> 24) & 0xFF;
    gGlobalDescriptorTable_Descriptors[pIndex].LimitLow = pLimit & 0xFFFF;
    gGlobalDescriptorTable_Descriptors[pIndex].LimitHighAndFlags = ((pLimit >> 16) & 0x0F) | (pFlags & 0xF0);
    gGlobalDescriptorTable_Descriptors[pIndex].Access = pAccess;
}
