#pragma once

#include <types.h>

#define GDT_MaxDescriptors                      5

#define GDT_AccessedAccess                      0x01
#define GDT_ReadWriteAccess                     0x02
#define GDT_DirectionConformingAccess           0x04
#define GDT_ExecutableAccess                    0x08
#define GDT_OneAccess                           0x10
#define GDT_Ring0Access                         0x00
#define GDT_Ring1Access                         0x20
#define GDT_Ring2Access                         0x40
#define GDT_Ring3Access                         0x60
#define GDT_PresentAccess                       0x80
#define GDT_ReadWriteOnePresentAccess           GDT_ReadWriteAccess | GDT_OneAccess | GDT_PresentAccess

#define GDT_Selector32BitFlag                   0x40
#define GDT_Granularity4KBFlag                  0x80
#define GDT_Selector32BitGranularity4KBFlags    GDT_Selector32BitFlag | GDT_Granularity4KBFlag

typedef struct 
{
    uint16_t Limit;
    uint32_t Address;
} GDT_Register;

typedef struct
{
    uint16_t LimitLow;
    uint16_t AddressLow;
    uint8_t AddressMid;
    uint8_t Access;
    uint8_t LimitHighAndFlags;
    uint8_t AddressHigh;
} GDT_Descriptor;

void GDT_Startup();
void GDT_Shutdown();

void GDT_SetSegment(uint8_t pIndex, uint32_t pAddress, uint32_t pLimit, uint8_t pAccess, uint8_t pFlags);

extern GDT_Register gGDT_Register;
extern GDT_Descriptor gGDT_Descriptors[];
