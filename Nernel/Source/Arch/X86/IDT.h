#pragma once

#include "../../Core/Types.hpp"

struct IDTRegister;
extern "C" void IDTUpdate(IDTRegister*);

struct InterruptRegisters
{
public:
    uint32 ds;
    uint32 edi;
    uint32 esi;
    uint32 ebp;
    uint32 esp;
    uint32 ebx;
    uint32 edx;
    uint32 ecx;
    uint32 eax;

    uint32 int_no;
    uint32 err_code;

    uint32 eip;
    uint32 cs;
    uint32 eflags;
    uint32 useresp;
    uint32 ss;
};

class IDT
{
private:
	static void SetInterrupt(uint8 pIndex, uint32 pAddress, uint16 pSelector, uint8 pTypeAndFlags);

public:
	static const uint16 SelectorPriviledgeLevelRing0 = 0x0000;
	static const uint16 SelectorPriviledgeLevelRing1 = 0x0001;
	static const uint16 SelectorPriviledgeLevelRing2 = 0x0002;
	static const uint16 SelectorPriviledgeLevelRing3 = 0x0003;
	static const uint16 SelectorTableIndex = 0x0004;
	static const uint16 SelectorDescriptorIndex = 0x0008;

	static const uint8 TypeTask386Gate32Bit = 0x05;
	static const uint8 TypeInterrupt286Gate16Bit = 0x06;
	static const uint8 TypeTrap286Gate16Bit = 0x07;
	static const uint8 TypeInterrupt386Gate32Bit = 0x0E;
	static const uint8 TypeTrap386Gate32Bit = 0x0F;
	static const uint8 TypePresent = 0x80;

	static void Load();
};
