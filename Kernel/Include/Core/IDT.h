#pragma once

#include <array>

namespace Core
{
    class IDT
    {
    public:
        static const uint16_t MaxDescriptors = 256;
        static const uint8_t RemappedIRQBase = 0x20;

        struct Register
        {
            uint16_t Limit;
            uint32_t Address;
        };

        struct Descriptor
        {
            uint16_t AddressLow;
            uint16_t Selector;
            uint8_t Zero;
            uint8_t TypeAndFlags;
            uint16_t AddressHigh;
        };

        struct Registers
        {
            uint32_t ds;
            uint32_t edi;
            uint32_t esi;
            uint32_t ebp;
            uint32_t esp;
            uint32_t ebx;
            uint32_t edx;
            uint32_t ecx;
            uint32_t eax;

            uint32_t int_no;
            uint32_t err_code;

            uint32_t eip;
            uint32_t cs;
            uint32_t eflags;
            uint32_t useresp;
            uint32_t ss;
        };

        typedef void (*IDTHandler)(Registers pRegisters);

        static bool Startup();
        static void Shutdown();
        static void Schedule(uint8_t pInterrupt);
        static void Unschedule(uint8_t pInterrupt);
        static void WaitFor(uint8_t pInterrupt);
        static void RegisterHandler(uint8_t pInterrupt, IDTHandler pHandler);
        static IDTHandler GetHandler(uint8_t pInterrupt);

    private:
        enum Selector
        {
            PriviledgeLevelRing0Selector = 0x0000,
            PriviledgeLevelRing1Selector = 0x0001,
            PriviledgeLevelRing2Selector = 0x0002,
            PriviledgeLevelRing3Selector = 0x0003,
            TableIndexSelector = 0x0004,
            DescriptorIndexSelector = 0x0008
        };

        enum Type
        {
            Task386Gate32BitType = 0x05,
            Interrupt286Gate16BitType = 0x06,
            Trap286Gate16BitType = 0x07,
            Interrupt386Gate32BitType = 0x0E,
            Trap386Gate32BitType = 0x0F,
            PresentType = 0x80
        };

        typedef std::array<Descriptor, MaxDescriptors> DescriptorsArray;
        typedef std::array<bool, MaxDescriptors> ScheduledArray;
        typedef std::array<IDTHandler, MaxDescriptors> HandlersArray;

        static Register sRegister;
        static DescriptorsArray sDescriptors;
        static ScheduledArray sScheduled;
        static HandlersArray sHandlers;

        IDT();
        IDT(const IDT&);
        static void SetInterrupt(uint8_t pIndex,
                                    uint32_t pAddress,
                                    uint16_t pSelector,
                                    uint8_t pTypeAndFlags);
        static void SetInterrupt(uint8_t pIndex,
                                    uint32_t pAddress);
    };
}
