#pragma once

typedef struct _GDTRegister GDTRegister;
typedef struct _GDTDescriptor GDTDescriptor;

struct _GDTRegister
{
    uint16_t Limit;
    uint32_t Address;
};

struct _GDTDescriptor
{
    uint16_t LimitLow;
    uint16_t AddressLow;
    uint8_t AddressMid;
    uint8_t Access;
    uint8_t LimitHighAndFlags;
    uint8_t AddressHigh;
};

void GDT_Startup();
void GDT_Shutdown();

extern void GDT_Update(GDTRegister* pRegister);
