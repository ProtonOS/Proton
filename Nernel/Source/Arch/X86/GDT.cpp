#include "../../PrimitiveTypes.h"

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


// These will be assigned by Boot.asm
GDTRegister* gGDTRegister = nullptr;
GDTDescriptor* gGDTDescriptors = nullptr;

