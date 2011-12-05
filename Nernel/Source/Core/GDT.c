#include <Core/GDT.h>

extern void GDT_Update(GDT_Register* pRegister);

GDT_Register gGDT_Register;
GDT_Descriptor gGDT_Descriptors[GDT_MaxDescriptors];

void GDT_Startup()
{
    gGDT_Register.Limit = (sizeof(GDT_Descriptor) * GDT_MaxDescriptors) - 1;
    gGDT_Register.Address = (uint32_t)&gGDT_Descriptors[0];

    GDT_SetSegment(0, 0x00000000, 0x00000000, 0x00, 0x00);
    GDT_SetSegment(1, 0x00000000, 0xFFFFFFFF, GDT_ReadWriteOnePresentAccess | GDT_ExecutableAccess, GDT_Selector32BitGranularity4KBFlags);
    GDT_SetSegment(2, 0x00000000, 0xFFFFFFFF, GDT_ReadWriteOnePresentAccess, GDT_Selector32BitGranularity4KBFlags);
    GDT_SetSegment(3, 0x00000000, 0xFFFFFFFF, GDT_ReadWriteOnePresentAccess | GDT_ExecutableAccess | GDT_Ring3Access, GDT_Selector32BitGranularity4KBFlags);
    GDT_SetSegment(4, 0x00000000, 0xFFFFFFFF, GDT_ReadWriteOnePresentAccess | GDT_Ring3Access, GDT_Selector32BitGranularity4KBFlags);

    GDT_Update(&gGDT_Register);
}

void GDT_Shutdown()
{
}

void GDT_SetSegment(uint8_t pIndex, uint32_t pAddress, uint32_t pLimit, uint8_t pAccess, uint8_t pFlags)
{
    gGDT_Descriptors[pIndex].AddressLow = pAddress & 0xFFFF;
    gGDT_Descriptors[pIndex].AddressMid = (pAddress >> 16) & 0xFF;
    gGDT_Descriptors[pIndex].AddressHigh = (pAddress >> 24) & 0xFF;
    gGDT_Descriptors[pIndex].LimitLow = pLimit & 0xFFFF;
    gGDT_Descriptors[pIndex].LimitHighAndFlags = ((pLimit >> 16) & 0x0F) | (pFlags & 0xF0);
    gGDT_Descriptors[pIndex].Access = pAccess;
}
