#pragma once

#define GLOBALDESCRIPTORTABLE_DESCRIPTORS_MAX	5

#define GLOBALDESCRIPTORTABLE_ACCESSEDACCESS                      0x01
#define GLOBALDESCRIPTORTABLE_READWRITEACCESS                     0x02
#define GLOBALDESCRIPTORTABLE_DIRECTIONCONFORMINGACCESS           0x04
#define GLOBALDESCRIPTORTABLE_EXECUTABLEACCESS                    0x08
#define GLOBALDESCRIPTORTABLE_ONEACCESS                           0x10
#define GLOBALDESCRIPTORTABLE_RING0ACCESS                         0x00
#define GLOBALDESCRIPTORTABLE_RING1ACCESS                         0x20
#define GLOBALDESCRIPTORTABLE_RING2ACCESS                         0x40
#define GLOBALDESCRIPTORTABLE_RING3ACCESS                         0x60
#define GLOBALDESCRIPTORTABLE_PRESENTACCESS                       0x80
#define GLOBALDESCRIPTORTABLE_READWRITEONEPRESENTACCESS           GLOBALDESCRIPTORTABLE_READWRITEACCESS | GLOBALDESCRIPTORTABLE_ONEACCESS | GLOBALDESCRIPTORTABLE_PRESENTACCESS

#define GLOBALDESCRIPTORTABLE_SELECTOR32BITFLAG                   0x40
#define GLOBALDESCRIPTORTABLE_GRANULARITY4KBFLAG                  0x80
#define GLOBALDESCRIPTORTABLE_SELECTOR32BITGRANULARITY4KBFLAGS    GLOBALDESCRIPTORTABLE_SELECTOR32BITFLAG | GLOBALDESCRIPTORTABLE_GRANULARITY4KBFLAG

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
