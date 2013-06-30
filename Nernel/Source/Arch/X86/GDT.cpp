#include "../../Core/Types.hpp"
#include "GDT.h"

struct GDTRegister
{
public:
    UInt16 Limit;
    UInt32 Address;
};

struct GDTDescriptor
{
public:
    UInt16 LimitLow;
    UInt16 AddressLow;
    UInt8 AddressMid;
    UInt8 Access;
    UInt8 LimitHighAndFlags;
    UInt8 AddressHigh;
};

struct TSSDescriptor
{
public:
   UInt32 prevss;   // The previous TSS - if we used hardware task switching this would form a linked list.
   UInt32 esp0;     // The stack pointer to load when we change to kernel mode.
   UInt32 ss0;      // The stack segment to load when we change to kernel mode.
   UInt32 esp1;     // everything below here is unusued now.. 
   UInt32 ss1;
   UInt32 esp2;
   UInt32 ss2;
   UInt32 cr3;
   UInt32 eip;
   UInt32 eflags;
   UInt32 eax;
   UInt32 ecx;
   UInt32 edx;
   UInt32 ebx;
   UInt32 esp;
   UInt32 ebp;
   UInt32 esi;
   UInt32 edi;
   UInt32 es;         
   UInt32 cs;        
   UInt32 ss;        
   UInt32 ds;        
   UInt32 fs;       
   UInt32 gs;         
   UInt32 ldt;      
   UInt16 trap;
   UInt16 iomap_base;
};

static const UInt32 GDTDescriptorCount = 5;

// These will be assigned by Boot.asm
GDTRegister* gGDTRegister = nullptr;
GDTDescriptor* gGDTDescriptors = nullptr;
UInt32 gGDTDescriptorMax = 0;
UInt32 gGDTDescriptorCount = 0;

// 512 comes from the number of GDTDescriptors that can be stored within 4096 bytes at 0xA000
// would be nice to get this out of Boot.asm, but it would not be constant, which is required
// for the gTSSDescriptors static allocation
static const UInt32 TSSDescriptorMax = 512 - GDTDescriptorCount;

TSSDescriptor gTSSDescriptors[TSSDescriptorMax];


void GDT::SetSegment(UInt32 pIndex, UInt32 pAddress, UInt32 pLimit, UInt8 pAccess, UInt8 pFlags)
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
    gGDTRegister->Address = (UInt)&gGDTDescriptors[0];

	for (UInt32 gdtDescriptorIndex = 0; gdtDescriptorIndex < gGDTDescriptorMax; ++gdtDescriptorIndex) SetSegment(0, 0x00000000, 0x00000000, 0x00, 0x00);

    SetSegment(1, 0x00000000, 0xFFFFFFFF, AccessReadWriteOnePresent | AccessExecutable, FlagsSelector32BitGranularity4KB);
    SetSegment(2, 0x00000000, 0xFFFFFFFF, AccessReadWriteOnePresent, FlagsSelector32BitGranularity4KB);
    SetSegment(3, 0x00000000, 0xFFFFFFFF, AccessReadWriteOnePresent | AccessExecutable | AccessRing3, FlagsSelector32BitGranularity4KB);
    SetSegment(4, 0x00000000, 0xFFFFFFFF, AccessReadWriteOnePresent | AccessRing3, FlagsSelector32BitGranularity4KB);

	for (UInt32 gdtDescriptorIndex = 5, tssDescriptorIndex = 0; gdtDescriptorIndex < gGDTDescriptorMax; ++gdtDescriptorIndex, ++tssDescriptorIndex)
	{
		UInt baseAddress = (UInt)&gTSSDescriptors[tssDescriptorIndex];
	    SetSegment(gdtDescriptorIndex, baseAddress, baseAddress + sizeof(TSSDescriptor), 0xE9, 0x00);
		gTSSDescriptors[tssDescriptorIndex].ss0 = 0x10;
	}

	GDTUpdate();
}

void GDT::SetTSSStack(UInt32 pIndex, UInt pStackAddress)
{
	gTSSDescriptors[pIndex].esp0 = pStackAddress;
}
