#include <System/GDT.h>

#define GDT__Descriptors_Max					5

#define GDT__Access__Accessed						0x01
#define GDT__Access__ReadWrite						0x02
#define GDT__Access__DirectionConforming			0x04
#define GDT__Access__Executable						0x08
#define GDT__Access__One							0x10
#define GDT__Access__Ring_0							0x00
#define GDT__Access__Ring_1							0x20
#define GDT__Access__Ring_2							0x40
#define GDT__Access__Ring_3							0x60
#define GDT__Access__Present						0x80
#define GDT__Access__ReadWriteOnePresent			GDT__Access__ReadWrite | GDT__Access__One | GDT__Access__Present

#define GDT__Flags__Selector32Bit					0x40
#define GDT__Flags__Granularity4KB					0x80
#define GDT__Flags__Selector32BitGranularity4KB		GDT__Flags__Selector32Bit | GDT__Flags__Granularity4KB


GDTRegister gGDT_Register;
GDTDescriptor gGDT_Descriptors[GDT__Descriptors_Max];

void GDT_SetSegment(uint8_t pIndex, uint32_t pAddress, uint32_t pLimit, uint8_t pAccess, uint8_t pFlags)
{
    gGDT_Descriptors[pIndex].AddressLow = pAddress & 0xFFFF;
    gGDT_Descriptors[pIndex].AddressMid = (pAddress >> 16) & 0xFF;
    gGDT_Descriptors[pIndex].AddressHigh = (pAddress >> 24) & 0xFF;
    gGDT_Descriptors[pIndex].LimitLow = pLimit & 0xFFFF;
    gGDT_Descriptors[pIndex].LimitHighAndFlags = ((pLimit >> 16) & 0x0F) | (pFlags & 0xF0);
    gGDT_Descriptors[pIndex].Access = pAccess;
}

void GDT_Startup()
{
    gGDT_Register.Limit = (sizeof(GDTDescriptor) * GDT__Descriptors_Max) - 1;
    gGDT_Register.Address = (size_t)&gGDT_Descriptors[0];

    GDT_SetSegment(0, 0x00000000, 0x00000000, 0x00, 0x00);
    GDT_SetSegment(1, 0x00000000, 0xFFFFFFFF, GDT__Access__ReadWriteOnePresent | GDT__Access__Executable, GDT__Flags__Selector32BitGranularity4KB);
    GDT_SetSegment(2, 0x00000000, 0xFFFFFFFF, GDT__Access__ReadWriteOnePresent, GDT__Flags__Selector32BitGranularity4KB);
    GDT_SetSegment(3, 0x00000000, 0xFFFFFFFF, GDT__Access__ReadWriteOnePresent | GDT__Access__Executable | GDT__Access__Ring_3, GDT__Flags__Selector32BitGranularity4KB);
    GDT_SetSegment(4, 0x00000000, 0xFFFFFFFF, GDT__Access__ReadWriteOnePresent | GDT__Access__Ring_3, GDT__Flags__Selector32BitGranularity4KB);

    GDT_Update(&gGDT_Register);
}

void GDT_Shutdown()
{
}
