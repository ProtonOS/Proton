#include "IDT.h"
#include "PortIO.h"
#include "PIC.h"

extern "C" {
void PICStartInterrupts(void);
void PICStopInterrupts(void);
}

namespace PIC
{
	const uint16_t IOPORT_MASTER = 0x0020;
	const uint16_t IOPORT_MASTER_COMMAND = IOPORT_MASTER + 0x00;
	const uint16_t IOPORT_MASTER_DATA = IOPORT_MASTER + 0x01;

	const uint16_t IOPORT_SLAVE = 0x00A0;
	const uint16_t IOPORT_SLAVE_COMMAND = IOPORT_SLAVE + 0x00;
	const uint16_t IOPORT_SLAVE_DATA = IOPORT_SLAVE + 0x01;

	const uint8_t COMMAND_CONFIGMODE = 0x01;
	const uint8_t COMMAND_SINGLEPIC = 0x02;
	const uint8_t COMMAND_INTERVAL4 = 0x04;
	const uint8_t COMMAND_LEVELMODE = 0x08;
	const uint8_t COMMAND_INITIALIZE = 0x10;
	const uint8_t COMMAND_RESET = 0x20;

	const uint8_t LINE_MASTERTOSLAVE = 0x04;
	const uint8_t LINE_SLAVETOMASTER = 0x02;

	const uint8_t MODE_MICROPROCESSOR8086 = 0x01;
	const uint8_t MODE_AUTOENDOFINTERRUPT = 0x02;
	const uint8_t MODE_MASTER = 0x04;
	const uint8_t MODE_BUFFERED = 0x08;
	const uint8_t MODE_SPECIALFULLYNESTED = 0x10;

	void Startup()
	{
		outb(IOPORT_MASTER_COMMAND, COMMAND_CONFIGMODE | COMMAND_INITIALIZE); IOWAIT();
		outb(IOPORT_SLAVE_COMMAND, COMMAND_CONFIGMODE | COMMAND_INITIALIZE); IOWAIT();

		outb(IOPORT_MASTER_DATA, IDT::IRQ_BASE); IOWAIT();
		outb(IOPORT_SLAVE_DATA, IDT::IRQ_BASE + 0x08); IOWAIT();

		outb(IOPORT_MASTER_DATA, LINE_MASTERTOSLAVE); IOWAIT();
		outb(IOPORT_SLAVE_DATA, LINE_SLAVETOMASTER); IOWAIT();

		outb(IOPORT_MASTER_DATA, MODE_MICROPROCESSOR8086); IOWAIT();
		outb(IOPORT_SLAVE_DATA, MODE_MICROPROCESSOR8086); IOWAIT();

		outb(IOPORT_MASTER_DATA, 0x00); IOWAIT();
		outb(IOPORT_SLAVE_DATA, 0x00); IOWAIT();
	}

	void ResetInterrupts(bool pSlave)
	{
		if (pSlave) outb(IOPORT_SLAVE_COMMAND, COMMAND_RESET);
		outb(IOPORT_MASTER_COMMAND, COMMAND_RESET);
	}

	void StartInterrupts()
	{
		PICStartInterrupts();
	}

	void StopInterrupts()
	{
		PICStopInterrupts();
	}
}
