#include "IDT.h"
#include "PIC.h"
#include "x86/PortIO.h"

void PIC_Startup()
{
	outb(PIC__IOPort__Master__Command, PIC__Command__ConfigMode | PIC__Command__Initialize); IOWAIT();
	outb(PIC__IOPort__Slave__Command, PIC__Command__ConfigMode | PIC__Command__Initialize); IOWAIT();

	outb(PIC__IOPort__Master__Data, IDT__IRQ__RemappedBase); IOWAIT();
	outb(PIC__IOPort__Slave__Data, IDT__IRQ__RemappedBase + 0x08); IOWAIT();

	outb(PIC__IOPort__Master__Data, PIC__Line__MasterToSlave); IOWAIT();
	outb(PIC__IOPort__Slave__Data, PIC__Line__SlaveToMaster); IOWAIT();

	outb(PIC__IOPort__Master__Data, PIC__Mode__Microprocessor8086); IOWAIT();
	outb(PIC__IOPort__Slave__Data, PIC__Mode__Microprocessor8086); IOWAIT();

	outb(PIC__IOPort__Master__Data, 0x00); IOWAIT();
	outb(PIC__IOPort__Slave__Data, 0x00); IOWAIT();
}

void PIC_Shutdown()
{
}

void PIC_ResetInterrupts(bool_t pSlave)
{
    if (pSlave) outb(PIC__IOPort__Slave__Command, PIC__Command__Reset);
    outb(PIC__IOPort__Master__Command, PIC__Command__Reset);
}
