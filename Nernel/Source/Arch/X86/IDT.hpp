#pragma once

#include "../../Core/Types.hpp"

struct IDTRegister;
extern "C" void IDTUpdate(IDTRegister*);

struct InterruptRegisters
{
public:
    UInt32 ds;
    UInt32 edi;
    UInt32 esi;
    UInt32 ebp;
    UInt32 esp;
    UInt32 ebx;
    UInt32 edx;
    UInt32 ecx;
    UInt32 eax;

    UInt32 int_no;
    UInt32 err_code;

    UInt32 eip;
    UInt32 cs;
    UInt32 eflags;
    UInt32 useresp;
    UInt32 ss;
};

namespace IDT
{
    constexpr UInt32 IDTDescriptorMax = 256;

    constexpr UInt16 SelectorPriviledgeLevelRing0 = 0x0000;
    constexpr UInt16 SelectorPriviledgeLevelRing1 = 0x0001;
    constexpr UInt16 SelectorPriviledgeLevelRing2 = 0x0002;
    constexpr UInt16 SelectorPriviledgeLevelRing3 = 0x0003;
    constexpr UInt16 SelectorTableIndex = 0x0004;
    constexpr UInt16 SelectorDescriptorIndex = 0x0008;

    constexpr UInt8 TypeTask386Gate32Bit = 0x05;
    constexpr UInt8 TypeInterrupt286Gate16Bit = 0x06;
    constexpr UInt8 TypeTrap286Gate16Bit = 0x07;
    constexpr UInt8 TypeInterrupt386Gate32Bit = 0x0E;
    constexpr UInt8 TypeTrap386Gate32Bit = 0x0F;
    constexpr UInt8 TypePresent = 0x80;

    typedef void (*InterruptHandler)(InterruptRegisters pRegisters);

    void Load();

    void RegisterHandler(UInt8 pInterrupt, InterruptHandler pHandler);
    InterruptHandler GetHandler(UInt8 pInterrupt);
};
