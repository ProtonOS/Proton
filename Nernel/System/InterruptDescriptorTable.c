#include "InterruptDescriptorTable.h"
#include "x86/PortIO.h"

#define INTERRUPTDESCRIPTORTABLE_DESCRIPTORS_MAX                 256

#define INTERRUPTDESCRIPTORTABLE_PRIVILEDGELEVELRING0SELECTOR    0x0000
#define INTERRUPTDESCRIPTORTABLE_PRIVILEDGELEVELRING1SELECTOR    0x0001
#define INTERRUPTDESCRIPTORTABLE_PRIVILEDGELEVELRING2SELECTOR    0x0002
#define INTERRUPTDESCRIPTORTABLE_PRIVILEDGELEVELRING3SELECTOR    0x0003
#define INTERRUPTDESCRIPTORTABLE_TABLEINDEXSELECTOR              0x0004
#define INTERRUPTDESCRIPTORTABLE_DESCRIPTORINDEXSELECTOR         0x0008

#define INTERRUPTDESCRIPTORTABLE_TASK386GATE32BITTYPE            0x05
#define INTERRUPTDESCRIPTORTABLE_INTERRUPT286GATE16BITTYPE       0x06
#define INTERRUPTDESCRIPTORTABLE_TRAP286GATE16BITTYPE            0x07
#define INTERRUPTDESCRIPTORTABLE_INTERRUPT386GATE32BITTYPE       0x0E
#define INTERRUPTDESCRIPTORTABLE_TRAP386GATE32BITTYPE            0x0F
#define INTERRUPTDESCRIPTORTABLE_PRESENTTYPE                     0x80

extern void InterruptDescriptorTable_Update(InterruptDescriptorTableRegister* pRegister);
#include "InterruptDescriptorTable_ExternalStubs.h" // Must remain here
void InterruptDescriptorTable_ISRHandler(InterruptRegisters pRegisters);
void InterruptDescriptorTable_IRQHandler(InterruptRegisters pRegisters);

InterruptDescriptorTableRegister gInterruptDescriptorTableRegister;
InterruptDescriptorTableDescriptor gInterruptDescriptorTable_Descriptors[INTERRUPTDESCRIPTORTABLE_DESCRIPTORS_MAX];
bool_t gInterruptDescriptorTable_Scheduled[INTERRUPTDESCRIPTORTABLE_DESCRIPTORS_MAX];
InterruptDescriptorTableHandler gInterruptDescriptorTable_Handlers[INTERRUPTDESCRIPTORTABLE_DESCRIPTORS_MAX];

void InterruptDescriptorTable_Startup()
{
	gInterruptDescriptorTableRegister.Limit = (sizeof(InterruptDescriptorTableDescriptor) * INTERRUPTDESCRIPTORTABLE_DESCRIPTORS_MAX) - 1;
	gInterruptDescriptorTableRegister.Address = (uint32_t)&gInterruptDescriptorTable_Descriptors[0];

#include "InterruptDescriptorTable_InitializeStubs.h" // Must remain here

	InterruptDescriptorTable_Update(&gInterruptDescriptorTableRegister);
}

void InterruptDescriptorTable_Shutdown()
{
}

void InterruptDescriptorTable_Schedule(uint8_t pInterrupt) { gInterruptDescriptorTable_Scheduled[pInterrupt] = TRUE; }

void InterruptDescriptorTable_Unschedule(uint8_t pInterrupt) { gInterruptDescriptorTable_Scheduled[pInterrupt] = FALSE; }

void InterruptDescriptorTable_WaitFor(uint8_t pInterrupt) { while (gInterruptDescriptorTable_Scheduled[pInterrupt]) IOWAIT(); }

void InterruptDescriptorTable_RegisterHandler(uint8_t pInterrupt, InterruptDescriptorTableHandler pHandler) { gInterruptDescriptorTable_Handlers[pInterrupt] = pHandler; }

InterruptDescriptorTableHandler InterruptDescriptorTable_GetHandler(uint8_t pInterrupt) { return gInterruptDescriptorTable_Handlers[pInterrupt]; }

void InterruptDescriptorTable_SetInterrupt(uint8_t pIndex, uint32_t pAddress, uint16_t pSelector, uint8_t pTypeAndFlags)
{
    gInterruptDescriptorTable_Descriptors[pIndex].AddressLow = pAddress & 0xFFFF;
    gInterruptDescriptorTable_Descriptors[pIndex].AddressHigh = (pAddress >> 16) & 0xFFFF;
    gInterruptDescriptorTable_Descriptors[pIndex].Selector = pSelector;
    gInterruptDescriptorTable_Descriptors[pIndex].Zero = 0;
    gInterruptDescriptorTable_Descriptors[pIndex].TypeAndFlags = pTypeAndFlags;
}

void InterruptDescriptorTable_ISRHandler(InterruptRegisters pRegisters)
{
    InterruptDescriptorTable_Unschedule(pRegisters.int_no);
    InterruptDescriptorTableHandler handler = InterruptDescriptorTable_GetHandler(pRegisters.int_no);
    if (handler) handler(pRegisters);
}

void InterruptDescriptorTable_IRQHandler(InterruptRegisters pRegisters)
{
    InterruptDescriptorTable_Unschedule(INTERRUPTDESCRIPTORTABLE_REMAPPEDIRQBASE + pRegisters.int_no);
    InterruptDescriptorTableHandler handler = InterruptDescriptorTable_GetHandler(INTERRUPTDESCRIPTORTABLE_REMAPPEDIRQBASE + pRegisters.int_no);
    if (handler) handler(pRegisters);
    //PIC_ResetInterrupts(pRegisters.int_no >= 9);
}
