#include <System/APIC.h>
#include <System/IDT.h>
#include <System/PIC.h>
#include <System/x86/PortIO.h>

#include <stdio.h>

#define IDT__Descriptors_Max				256


#include "IDT_ExternalStubs.h" // Must remain here
void IDT_ISRHandler(InterruptRegisters pRegisters);
void IDT_IRQHandler(InterruptRegisters pRegisters);

IDTRegister gIDT_Register;
IDTDescriptor gIDT_Descriptors[IDT__Descriptors_Max];
bool_t gIDT_Scheduled[IDT__Descriptors_Max];
IDTHandler gIDT_Handlers[IDT__Descriptors_Max];

void IDT_Startup()
{
	gIDT_Register.Limit = (sizeof(IDTDescriptor) * IDT__Descriptors_Max) - 1;
	gIDT_Register.Address = (uint32_t)&gIDT_Descriptors[0];

#include "IDT_InitializeStubs.h" // Must remain here

	IDT_Update(&gIDT_Register);
}

void IDT_Shutdown()
{
}

void IDT_Schedule(uint8_t pInterrupt) { gIDT_Scheduled[pInterrupt] = TRUE; }

void IDT_Unschedule(uint8_t pInterrupt) { gIDT_Scheduled[pInterrupt] = FALSE; }

void IDT_WaitFor(uint8_t pInterrupt) { while (gIDT_Scheduled[pInterrupt]) IOWAIT(); }

void IDT_RegisterHandler(uint8_t pInterrupt, IDTHandler pHandler) { gIDT_Handlers[pInterrupt] = pHandler; }

IDTHandler IDT_GetHandler(uint8_t pInterrupt) { return gIDT_Handlers[pInterrupt]; }

void IDT_SetInterrupt(uint8_t pIndex, uint32_t pAddress, uint16_t pSelector, uint8_t pTypeAndFlags)
{
    gIDT_Descriptors[pIndex].AddressLow = pAddress & 0xFFFF;
    gIDT_Descriptors[pIndex].AddressHigh = (pAddress >> 16) & 0xFFFF;
    gIDT_Descriptors[pIndex].Selector = pSelector;
    gIDT_Descriptors[pIndex].Zero = 0;
    gIDT_Descriptors[pIndex].TypeAndFlags = pTypeAndFlags;
}

uint32_t IDT_GetInterrupt(uint8_t pIndex)
{
	return gIDT_Descriptors[pIndex].AddressLow | (gIDT_Descriptors[pIndex].AddressHigh << 16);
}

void IDT_ISRHandler(InterruptRegisters pRegisters)
{
	//printf("ISRHandler: %u\n", (unsigned int)pRegisters.int_no);
    IDT_Unschedule(pRegisters.int_no);
    IDTHandler handler = IDT_GetHandler(pRegisters.int_no);
    if (handler) handler(pRegisters);
	//APIC_EndOfInterrupt();
}

void IDT_IRQHandler(InterruptRegisters pRegisters)
{
	//printf("IRQHandler: %u\n", (unsigned int)pRegisters.int_no);
    IDT_Unschedule(IDT__IRQ__RemappedBase + pRegisters.int_no);
    IDTHandler handler = IDT_GetHandler(IDT__IRQ__RemappedBase + pRegisters.int_no);
    if (handler) handler(pRegisters);
	//APIC_EndOfInterrupt();
    PIC_ResetInterrupts(pRegisters.int_no >= 9);
}
