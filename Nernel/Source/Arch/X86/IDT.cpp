#include "../../Kernel/KernelConsole.hpp"
#include "../../Kernel/InterruptManager.hpp"

#include "IDT.hpp"
#include "PIC.hpp"

struct IDTRegister
{
public:
    UInt16 Limit;
    UInt32 Address;
};

struct IDTDescriptor
{
public:
    UInt16 AddressLow;
    UInt16 Selector;
    UInt8 Zero;
    UInt8 TypeAndFlags;
    UInt16 AddressHigh;
};

#include "IDTExternal.hpp"

typedef void (* IDTStub)(void);

IDTStub gIDTStubs[IDT::IDTDescriptorMax] = {
    IDTISR00, IDTISR01, IDTISR02, IDTISR03, IDTISR04, IDTISR05, IDTISR06, IDTISR07, IDTISR08, IDTISR09, IDTISR0A, IDTISR0B, IDTISR0C, IDTISR0D, IDTISR0E, IDTISR0F,
    IDTISR10, IDTISR11, IDTISR12, IDTISR13, IDTISR14, IDTISR15, IDTISR16, IDTISR17, IDTISR18, IDTISR19, IDTISR1A, IDTISR1B, IDTISR1C, IDTISR1D, IDTISR1E, IDTISR1F,
    IDTIRQ00, IDTIRQ01, IDTIRQ02, IDTIRQ03, IDTIRQ04, IDTIRQ05, IDTIRQ06, IDTIRQ07, IDTIRQ08, IDTIRQ09, IDTIRQ0A, IDTIRQ0B, IDTIRQ0C, IDTIRQ0D, IDTIRQ0E, IDTIRQ0F,
    IDTISR30, IDTISR31, IDTISR32, IDTISR33, IDTISR34, IDTISR35, IDTISR36, IDTISR37, IDTISR38, IDTISR39, IDTISR3A, IDTISR3B, IDTISR3C, IDTISR3D, IDTISR3E, IDTISR3F,
    IDTISR40, IDTISR41, IDTISR42, IDTISR43, IDTISR44, IDTISR45, IDTISR46, IDTISR47, IDTISR48, IDTISR49, IDTISR4A, IDTISR4B, IDTISR4C, IDTISR4D, IDTISR4E, IDTISR4F,
    IDTISR50, IDTISR51, IDTISR52, IDTISR53, IDTISR54, IDTISR55, IDTISR56, IDTISR57, IDTISR58, IDTISR59, IDTISR5A, IDTISR5B, IDTISR5C, IDTISR5D, IDTISR5E, IDTISR5F,
    IDTISR60, IDTISR61, IDTISR62, IDTISR63, IDTISR64, IDTISR65, IDTISR66, IDTISR67, IDTISR68, IDTISR69, IDTISR6A, IDTISR6B, IDTISR6C, IDTISR6D, IDTISR6E, IDTISR6F,
    IDTISR70, IDTISR71, IDTISR72, IDTISR73, IDTISR74, IDTISR75, IDTISR76, IDTISR77, IDTISR78, IDTISR79, IDTISR7A, IDTISR7B, IDTISR7C, IDTISR7D, IDTISR7E, IDTISR7F,
    IDTISR80, IDTISR81, IDTISR82, IDTISR83, IDTISR84, IDTISR85, IDTISR86, IDTISR87, IDTISR88, IDTISR89, IDTISR8A, IDTISR8B, IDTISR8C, IDTISR8D, IDTISR8E, IDTISR8F,
    IDTISR90, IDTISR91, IDTISR92, IDTISR93, IDTISR94, IDTISR95, IDTISR96, IDTISR97, IDTISR98, IDTISR99, IDTISR9A, IDTISR9B, IDTISR9C, IDTISR9D, IDTISR9E, IDTISR9F,
    IDTISRA0, IDTISRA1, IDTISRA2, IDTISRA3, IDTISRA4, IDTISRA5, IDTISRA6, IDTISRA7, IDTISRA8, IDTISRA9, IDTISRAA, IDTISRAB, IDTISRAC, IDTISRAD, IDTISRAE, IDTISRAF,
    IDTISRB0, IDTISRB1, IDTISRB2, IDTISRB3, IDTISRB4, IDTISRB5, IDTISRB6, IDTISRB7, IDTISRB8, IDTISRB9, IDTISRBA, IDTISRBB, IDTISRBC, IDTISRBD, IDTISRBE, IDTISRBF,
    IDTISRC0, IDTISRC1, IDTISRC2, IDTISRC3, IDTISRC4, IDTISRC5, IDTISRC6, IDTISRC7, IDTISRC8, IDTISRC9, IDTISRCA, IDTISRCB, IDTISRCC, IDTISRCD, IDTISRCE, IDTISRCF,
    IDTISRD0, IDTISRD1, IDTISRD2, IDTISRD3, IDTISRD4, IDTISRD5, IDTISRD6, IDTISRD7, IDTISRD8, IDTISRD9, IDTISRDA, IDTISRDB, IDTISRDC, IDTISRDD, IDTISRDE, IDTISRDF,
    IDTISRE0, IDTISRE1, IDTISRE2, IDTISRE3, IDTISRE4, IDTISRE5, IDTISRE6, IDTISRE7, IDTISRE8, IDTISRE9, IDTISREA, IDTISREB, IDTISREC, IDTISRED, IDTISREE, IDTISREF,
    IDTISRF0, IDTISRF1, IDTISRF2, IDTISRF3, IDTISRF4, IDTISRF5, IDTISRF6, IDTISRF7, IDTISRF8, IDTISRF9, IDTISRFA, IDTISRFB, IDTISRFC, IDTISRFD, IDTISRFE, IDTISRFF,
};

IDTRegister gIDTRegister;
IDTDescriptor gIDTDescriptors[IDT::IDTDescriptorMax];

extern "C" void IDTISRHandler(InterruptRegisters pRegisters)
{
    IDT::InterruptHandler handler = InterruptManager::GetHandler(pRegisters.int_no);
    if (handler) handler(pRegisters);
}

extern "C" void IDTIRQHandler(InterruptRegisters pRegisters)
{
    IDT::InterruptHandler handler = InterruptManager::GetHandler(PIC::IRQBaseISR + pRegisters.int_no);
    if (handler) handler(pRegisters);
    PIC::ResetInterrupts(pRegisters.int_no >= 9);
}

extern "C" void CPUInterruptHandler(InterruptRegisters pRegisters)
{
    //char buf[128];
    //snprintf(buf, 128, "CPU Exception: %d", (int)pRegisters.int_no);
    //Panic(buf);
    while (true);
}

namespace IDT
{
    void SetInterrupt(UInt8 pIndex, UInt32 pAddress, UInt16 pSelector, UInt8 pTypeAndFlags)
    {
        gIDTDescriptors[pIndex].AddressLow = pAddress & 0xFFFF;
        gIDTDescriptors[pIndex].AddressHigh = (pAddress >> 16) & 0xFFFF;
        gIDTDescriptors[pIndex].Selector = pSelector;
        gIDTDescriptors[pIndex].Zero = 0;
        gIDTDescriptors[pIndex].TypeAndFlags = pTypeAndFlags;
    }

    void Load()
    {
        gIDTRegister.Limit = (sizeof(IDTDescriptor) * IDTDescriptorMax) - 1;
        gIDTRegister.Address = reinterpret_cast<UInt32>(gIDTDescriptors);

        for (UInt32 idtDescriptorIndex = 0; idtDescriptorIndex < IDTDescriptorMax; ++idtDescriptorIndex)
            SetInterrupt(static_cast<UInt8>(idtDescriptorIndex), reinterpret_cast<UInt32>(gIDTStubs[idtDescriptorIndex]), SelectorDescriptorIndex, TypeInterrupt386Gate32Bit | TypePresent);

        IDTUpdate(&gIDTRegister);

        for (UInt8 interrupt = 0; interrupt < PIC::IRQBaseISR; ++interrupt) InterruptManager::RegisterHandler(interrupt, &CPUInterruptHandler);
    }

}