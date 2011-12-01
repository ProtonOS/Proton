#include <PortIO.h>
#include <Core/DeviceManager.h>
#include <Core/IDT.h>

extern "C" {
extern void IDTUpdate(Core::IDT::Register* pRegister);
#include <Core/IDTExternalStubs.h> // Must remain here
void IDTISRHandler(Core::IDT::Registers pRegisters);
void IDTIRQHandler(Core::IDT::Registers pRegisters);
}

Core::IDT::Register Core::IDT::sRegister;
Core::IDT::DescriptorsArray Core::IDT::sDescriptors;
Core::IDT::ScheduledArray Core::IDT::sScheduled;
Core::IDT::HandlersArray Core::IDT::sHandlers;

bool Core::IDT::Startup()
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

void Core::IDT::Shutdown()
{
}

void Core::IDT::Schedule(uint8_t pInterrupt) { sScheduled[pInterrupt] = true; }

void Core::IDT::Unschedule(uint8_t pInterrupt) { sScheduled[pInterrupt] = false; }

void Core::IDT::WaitFor(uint8_t pInterrupt) { while (sScheduled[pInterrupt]) IOWAIT(); }

void Core::IDT::RegisterHandler(uint8_t pInterrupt, IDTHandler pHandler) { sHandlers[pInterrupt] = pHandler; }

Core::IDT::IDTHandler Core::IDT::GetHandler(uint8_t pInterrupt) { return sHandlers[pInterrupt]; }

void Core::IDT::SetInterrupt(uint8_t pIndex, uint32_t pAddress, uint16_t pSelector, uint8_t pTypeAndFlags)
{
    sDescriptors[pIndex].AddressLow = pAddress & 0xFFFF;
    sDescriptors[pIndex].AddressHigh = (pAddress >> 16) & 0xFFFF;
    sDescriptors[pIndex].Selector = pSelector;
    sDescriptors[pIndex].Zero = 0;
    sDescriptors[pIndex].TypeAndFlags = pTypeAndFlags;
}

void Core::IDT::SetInterrupt(uint8_t pIndex, uint32_t pAddress) { SetInterrupt(pIndex, pAddress, Selector::DescriptorIndexSelector, Type::Interrupt386Gate32BitType | Type::PresentType); }

void IDTISRHandler(Core::IDT::Registers pRegisters)
{
    Core::IDT::Unschedule(pRegisters.int_no);
    Core::IDT::IDTHandler handler = Core::IDT::GetHandler(pRegisters.int_no);
    if (handler) handler(pRegisters);
}

void IDTIRQHandler(Core::IDT::Registers pRegisters)
{
    Core::IDT::Unschedule(Core::IDT::RemappedIRQBase + pRegisters.int_no);
    Core::IDT::IDTHandler handler = Core::IDT::GetHandler(Core::IDT::RemappedIRQBase + pRegisters.int_no);
    if (handler) handler(pRegisters);
    Core::DeviceManager::GetPIC().ResetInterrupts(pRegisters.int_no >= 9);
}
