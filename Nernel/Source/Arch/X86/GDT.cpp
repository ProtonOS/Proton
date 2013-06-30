#include "GDT.h"

struct GDTRegister
{
public:
    uint16 Limit;
    uint32 Address;
};

struct GDTDescriptor
{
public:
    uint16 LimitLow;
    uint16 AddressLow;
    uint8 AddressMid;
    uint8 Access;
    uint8 LimitHighAndFlags;
    uint8 AddressHigh;
};

struct TSSDescriptor
{
public:
   uint32 prevss;   // The previous TSS - if we used hardware task switching this would form a linked list.
   uint32 esp0;     // The stack pointer to load when we change to kernel mode.
   uint32 ss0;      // The stack segment to load when we change to kernel mode.
   uint32 esp1;     // everything below here is unusued now.. 
   uint32 ss1;
   uint32 esp2;
   uint32 ss2;
   uint32 cr3;
   uint32 eip;
   uint32 eflags;
   uint32 eax;
   uint32 ecx;
   uint32 edx;
   uint32 ebx;
   uint32 esp;
   uint32 ebp;
   uint32 esi;
   uint32 edi;
   uint32 es;         
   uint32 cs;        
   uint32 ss;        
   uint32 ds;        
   uint32 fs;       
   uint32 gs;         
   uint32 ldt;      
   uint16 trap;
   uint16 iomap_base;
};

static const uint32 GDTDescriptorCount = 5;

// These will be assigned by Boot.asm
GDTRegister* gGDTRegister = nullptr;
GDTDescriptor* gGDTDescriptors = nullptr;
uint32 gGDTDescriptorMax = 0;

// 512 comes from the number of GDTDescriptors that can be stored within 4096 bytes at 0xA000
// would be nice to get this out of Boot.asm, but it would not be constant, which is required
// for the gTSSDescriptors static allocation
static const uint32 TSSDescriptorMax = 512 - GDTDescriptorCount;

TSSDescriptor gTSSDescriptors[TSSDescriptorMax];


void GDT::SetSegment(uint32 pIndex, uint32 pAddress, uint32 pLimit, uint8 pAccess, uint8 pFlags)
{
    gGDTDescriptors[pIndex].AddressLow = pAddress & 0xFFFF;
    gGDTDescriptors[pIndex].AddressMid = (pAddress >> 16) & 0xFF;
    gGDTDescriptors[pIndex].AddressHigh = (pAddress >> 24) & 0xFF;
    gGDTDescriptors[pIndex].LimitLow = pLimit & 0xFFFF;
    gGDTDescriptors[pIndex].LimitHighAndFlags = ((pLimit >> 16) & 0x0F) | (pFlags & 0xF0);
    gGDTDescriptors[pIndex].Access = pAccess;
}

void GDT::Load()
{
    gGDTRegister->Limit = (sizeof(GDTDescriptor) * gGDTDescriptorMax) - 1;
    gGDTRegister->Address = (uintptr)&gGDTDescriptors[0];

	for (uint32 gdtDescriptorIndex = 0; gdtDescriptorIndex < gGDTDescriptorMax; ++gdtDescriptorIndex) SetSegment(gdtDescriptorIndex, 0x00000000, 0x00000000, 0x00, 0x00);

    SetSegment(1, 0x00000000, 0xFFFFFFFF, AccessReadWriteOnePresent | AccessExecutable, FlagsSelector32BitGranularity4KB);
    SetSegment(2, 0x00000000, 0xFFFFFFFF, AccessReadWriteOnePresent, FlagsSelector32BitGranularity4KB);
    SetSegment(3, 0x00000000, 0xFFFFFFFF, AccessReadWriteOnePresent | AccessExecutable | AccessRing3, FlagsSelector32BitGranularity4KB);
    SetSegment(4, 0x00000000, 0xFFFFFFFF, AccessReadWriteOnePresent | AccessRing3, FlagsSelector32BitGranularity4KB);

	for (uint32 gdtDescriptorIndex = GDTDescriptorCount, tssDescriptorIndex = 0; gdtDescriptorIndex < gGDTDescriptorMax; ++gdtDescriptorIndex, ++tssDescriptorIndex)
	{
		uintptr baseAddress = (uintptr)&gTSSDescriptors[tssDescriptorIndex];
	    SetSegment(gdtDescriptorIndex, baseAddress, baseAddress + sizeof(TSSDescriptor), 0xE9, 0x00);
		gTSSDescriptors[tssDescriptorIndex].ss0 = 0x10;
	}

	GDTUpdate();
}

void GDT::SetTSSStack(uint32 pIndex, uintptr pStackAddress)
{
	gTSSDescriptors[pIndex].esp0 = pStackAddress;
}
