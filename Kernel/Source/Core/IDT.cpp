#include <Core/DeviceManager.h>
#include <Core/IDT.h>
#include <PortIO.h>

using namespace Core;

extern "C" {
extern void IDTUpdate(IDT::Register* pRegister);
#include <Core/IDTExternalStubs.h> // Must remain here
void IDTISRHandler(IDT::Registers pRegisters);
void IDTIRQHandler(IDT::Registers pRegisters);
}

IDT::Register IDT::sRegister;
IDT::DescriptorsArray IDT::sDescriptors;
IDT::ScheduledArray IDT::sScheduled;
IDT::HandlersArray IDT::sHandlers;

bool IDT::Startup()
{
    sDescriptors.fill(Descriptor());
    sScheduled.fill(false);
    sHandlers.fill(nullptr);

	sRegister.Limit = (sizeof(Descriptor) * MaxDescriptors) - 1;
	sRegister.Address = (uint32_t)sDescriptors.data();

#include <Core/IDTInitializeStubs.h> // Must remain here

	IDTUpdate(&sRegister);

    return true;
}

void IDT::Shutdown()
{
}

void IDT::Schedule(uint8_t pInterrupt) { sScheduled[pInterrupt] = true; }

void IDT::Unschedule(uint8_t pInterrupt) { sScheduled[pInterrupt] = false; }

void IDT::WaitFor(uint8_t pInterrupt) { while (sScheduled[pInterrupt]) IOWAIT(); }

void IDT::RegisterHandler(uint8_t pInterrupt, IDTHandler pHandler) { sHandlers[pInterrupt] = pHandler; }

IDT::IDTHandler IDT::GetHandler(uint8_t pInterrupt) { return sHandlers[pInterrupt]; }

void IDT::SetInterrupt(uint8_t pIndex, uint32_t pAddress, uint16_t pSelector, uint8_t pTypeAndFlags)
{
    sDescriptors[pIndex].AddressLow = pAddress & 0xFFFF;
    sDescriptors[pIndex].AddressHigh = (pAddress >> 16) & 0xFFFF;
    sDescriptors[pIndex].Selector = pSelector;
    sDescriptors[pIndex].Zero = 0;
    sDescriptors[pIndex].TypeAndFlags = pTypeAndFlags;
}

void IDT::SetInterrupt(uint8_t pIndex, uint32_t pAddress) { SetInterrupt(pIndex, pAddress, Selector::DescriptorIndexSelector, Type::Interrupt386Gate32BitType | Type::PresentType); }

void IDTISRHandler(IDT::Registers pRegisters)
{
    IDT::Unschedule(pRegisters.int_no);
    IDT::IDTHandler handler = IDT::GetHandler(pRegisters.int_no);
    if (handler) handler(pRegisters);
}

void IDTIRQHandler(IDT::Registers pRegisters)
{
    IDT::Unschedule(IDT::RemappedIRQBase + pRegisters.int_no);
    IDT::IDTHandler handler = IDT::GetHandler(IDT::RemappedIRQBase + pRegisters.int_no);
    if (handler) handler(pRegisters);
    DeviceManager::GetPIC().ResetInterrupts(pRegisters.int_no >= 9);
}
