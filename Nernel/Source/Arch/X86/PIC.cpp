#include "PIC.h"
#include "PortIO.h"

void PIC::Load()
{
	outb(IOPortMasterCommand, CommandConfigMode | CommandInitialize); IOWAIT();
	outb(IOPortSlaveCommand, CommandConfigMode | CommandInitialize); IOWAIT();

	outb(IOPortMasterData, IRQBaseISR); IOWAIT();
	outb(IOPortSlaveData, IRQBaseISR + 0x08); IOWAIT();

	outb(IOPortMasterData, LineMasterToSlave); IOWAIT();
	outb(IOPortSlaveData, LineSlaveToMaster); IOWAIT();

	outb(IOPortMasterData, ModeMicroprocessor8086); IOWAIT();
	outb(IOPortSlaveData, ModeMicroprocessor8086); IOWAIT();

	outb(IOPortMasterData, 0x00); IOWAIT();
	outb(IOPortSlaveData, 0x00); IOWAIT();
}

void PIC::ResetInterrupts(bool pSlave)
{
    if (pSlave) outb(IOPortSlaveCommand, CommandReset);
    outb(IOPortMasterCommand, CommandReset);
}