#include <PortIO.h>
#include <Core/IDT.h>
#include <Core/PIC.h>

void PIC_Startup()
{
	outb(PIC_MasterCommandIOPort, PIC_ConfigModeCommand | PIC_InitializeCommand); IOWAIT();
	outb(PIC_SlaveCommandIOPort, PIC_ConfigModeCommand | PIC_InitializeCommand); IOWAIT();

	outb(PIC_MasterDataIOPort, IDT_RemappedIRQBase); IOWAIT();
	outb(PIC_SlaveDataIOPort, IDT_RemappedIRQBase + 0x08); IOWAIT();

	outb(PIC_MasterDataIOPort, PIC_MasterToSlaveLine); IOWAIT();
	outb(PIC_SlaveDataIOPort, PIC_SlaveToMasterLine); IOWAIT();

	outb(PIC_MasterDataIOPort, PIC_Microprocessor8086Mode); IOWAIT();
	outb(PIC_SlaveDataIOPort, PIC_Microprocessor8086Mode); IOWAIT();

	outb(PIC_MasterDataIOPort, 0x00); IOWAIT();
	outb(PIC_SlaveDataIOPort, 0x00); IOWAIT();

	PIC_StartInterrupts();
}

void PIC_Shutdown()
{
}

void PIC_StartInterrupts() { __asm("sti"); }

void PIC_StopInterrupts() { __asm("cli"); }

void PIC_ResetInterrupts(bool_t pSlave)
{
    if (pSlave) outb(PIC_SlaveCommandIOPort, PIC_ResetCommand);
    outb(PIC_MasterCommandIOPort, PIC_ResetCommand);
}
