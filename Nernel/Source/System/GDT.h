#pragma once

typedef struct _GDTRegister GDTRegister;
typedef struct _GDTDescriptor GDTDescriptor;
typedef struct _TSSDescriptor TSSDescriptor;

#define GDT__Descriptors_Max						(5 + APIC__Max)

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

struct _TSSDescriptor
{
   uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
   uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
   uint32_t ss0;        // The stack segment to load when we change to kernel mode.
   uint32_t esp1;       // everything below here is unusued now.. 
   uint32_t ss1;
   uint32_t esp2;
   uint32_t ss2;
   uint32_t cr3;
   uint32_t eip;
   uint32_t eflags;
   uint32_t eax;
   uint32_t ecx;
   uint32_t edx;
   uint32_t ebx;
   uint32_t esp;
   uint32_t ebp;
   uint32_t esi;
   uint32_t edi;
   uint32_t es;         
   uint32_t cs;        
   uint32_t ss;        
   uint32_t ds;        
   uint32_t fs;       
   uint32_t gs;         
   uint32_t ldt;      
   uint16_t trap;
   uint16_t iomap_base;
};

void GDT_Startup();
void GDT_Shutdown();

void GDT_AssignTSS(uint8_t pIndex, uint32_t pStackAddress);

extern void GDT_Update(GDTRegister* pRegister);
extern void TSS_Update(uint32_t pSegmentSelector);
extern uint32_t TSS_GetTaskRegister();
extern void GDT_SwitchToUserMode();
extern void GDT_LogicalProcessorInit();
extern void GDT_LogicalProcessorInitEnd();
extern GDTRegister gGDT_Register;
extern GDTDescriptor gGDT_Descriptors[];
