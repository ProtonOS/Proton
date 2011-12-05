#include <PortIO.h>
#include <Core/IDT.h>
#include <Core/PIC.h>

extern void IDT_Update(IDT_Register* pRegister);
#include <Core/IDTExternalStubs.h> // Must remain here
void IDT_ISRHandler(InterruptRegisters pRegisters);
void IDT_IRQHandler(InterruptRegisters pRegisters);

IDT_Register gIDT_Register;
IDT_Descriptor gIDT_Descriptors[IDT_MaxDescriptors];
bool_t gIDT_Scheduled[IDT_MaxDescriptors];
IDT_Handler gIDT_Handlers[IDT_MaxDescriptors];

void IDT_Startup()
{
	gIDT_Register.Limit = (sizeof(IDT_Descriptor) * IDT_MaxDescriptors) - 1;
	gIDT_Register.Address = (uint32_t)&gIDT_Descriptors[0];

#include <Core/IDTInitializeStubs.h> // Must remain here

	IDT_Update(&gIDT_Register);
}

void IDT_Shutdown()
{
}

void IDT_Schedule(uint8_t pInterrupt) { gIDT_Scheduled[pInterrupt] = TRUE; }

void IDT_Unschedule(uint8_t pInterrupt) { gIDT_Scheduled[pInterrupt] = FALSE; }

void IDT_WaitFor(uint8_t pInterrupt) { while (gIDT_Scheduled[pInterrupt]) IOWAIT(); }

void IDT_RegisterHandler(uint8_t pInterrupt, IDT_Handler pHandler) { gIDT_Handlers[pInterrupt] = pHandler; }

IDT_Handler IDT_GetHandler(uint8_t pInterrupt) { return gIDT_Handlers[pInterrupt]; }

void IDT_SetInterrupt(uint8_t pIndex, uint32_t pAddress, uint16_t pSelector, uint8_t pTypeAndFlags)
{
    gIDT_Descriptors[pIndex].AddressLow = pAddress & 0xFFFF;
    gIDT_Descriptors[pIndex].AddressHigh = (pAddress >> 16) & 0xFFFF;
    gIDT_Descriptors[pIndex].Selector = pSelector;
    gIDT_Descriptors[pIndex].Zero = 0;
    gIDT_Descriptors[pIndex].TypeAndFlags = pTypeAndFlags;
}

void IDT_ISRHandler(InterruptRegisters pRegisters)
{
    IDT_Unschedule(pRegisters.int_no);
    IDT_Handler handler = IDT_GetHandler(pRegisters.int_no);
    if (handler) handler(pRegisters);
}

void IDT_IRQHandler(InterruptRegisters pRegisters)
{
    IDT_Unschedule(IDT_RemappedIRQBase + pRegisters.int_no);
    IDT_Handler handler = IDT_GetHandler(IDT_RemappedIRQBase + pRegisters.int_no);
    if (handler) handler(pRegisters);
    PIC_ResetInterrupts(pRegisters.int_no >= 9);
}
