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

class IDT
{
public:
	static const UInt32 IDTDescriptorMax = 256;

	static const UInt16 SelectorPriviledgeLevelRing0 = 0x0000;
	static const UInt16 SelectorPriviledgeLevelRing1 = 0x0001;
	static const UInt16 SelectorPriviledgeLevelRing2 = 0x0002;
	static const UInt16 SelectorPriviledgeLevelRing3 = 0x0003;
	static const UInt16 SelectorTableIndex = 0x0004;
	static const UInt16 SelectorDescriptorIndex = 0x0008;

	static const UInt8 TypeTask386Gate32Bit = 0x05;
	static const UInt8 TypeInterrupt286Gate16Bit = 0x06;
	static const UInt8 TypeTrap286Gate16Bit = 0x07;
	static const UInt8 TypeInterrupt386Gate32Bit = 0x0E;
	static const UInt8 TypeTrap386Gate32Bit = 0x0F;
	static const UInt8 TypePresent = 0x80;

	typedef void (*InterruptHandler)(InterruptRegisters pRegisters);

	static void Load();

private:
	static void SetInterrupt(UInt8 pIndex, UInt32 pAddress, UInt16 pSelector, UInt8 pTypeAndFlags);
};
