#include "InterruptManager.h"

IDT::InterruptHandler InterruptManager::sHandlers[IDT::IDTDescriptorMax];


void InterruptManager::RegisterHandler(UInt8 pInterrupt, IDT::InterruptHandler pHandler)
{
	sHandlers[pInterrupt] = pHandler;
}

IDT::InterruptHandler InterruptManager::GetHandler(UInt8 pInterrupt)
{
	return sHandlers[pInterrupt];
}
