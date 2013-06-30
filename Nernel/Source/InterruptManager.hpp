#pragma once

#ifdef X86
#include "Arch/X86/IDT.hpp"
#endif

class InterruptManager
{
public:
	static void RegisterHandler(UInt8 pInterrupt, IDT::InterruptHandler pHandler);
	static IDT::InterruptHandler GetHandler(UInt8 pInterrupt);

private:
	static IDT::InterruptHandler sHandlers[IDT::IDTDescriptorMax];

};