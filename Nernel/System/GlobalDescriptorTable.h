#pragma once

typedef struct _GlobalDescriptorTableRegister GlobalDescriptorTableRegister;
typedef struct _GlobalDescriptorTableDescriptor GlobalDescriptorTableDescriptor;

struct _GlobalDescriptorTableRegister
{
    uint16_t Limit;
    uint32_t Address;
};

struct _GlobalDescriptorTableDescriptor
{
    uint16_t LimitLow;
    uint16_t AddressLow;
    uint8_t AddressMid;
    uint8_t Access;
    uint8_t LimitHighAndFlags;
    uint8_t AddressHigh;
};

void GlobalDescriptorTable_Startup();
void GlobalDescriptorTable_Shutdown();

void GlobalDescriptorTable_SetSegment(uint8_t pIndex, uint32_t pAddress, uint32_t pLimit, uint8_t pAccess, uint8_t pFlags);
