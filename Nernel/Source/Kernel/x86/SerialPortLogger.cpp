#include "PortIO.h"
#include "SerialPortLogger.h"

namespace SerialPortLogger
{
	const uint16_t IOPORT_BASE = 0x02F8;
	const uint16_t IOPORT_DATA = IOPORT_BASE + 0x00;
	const uint16_t IOPORT_INTERRUPT = IOPORT_BASE + 0x01;
	const uint16_t IOPORT_FIFO = IOPORT_BASE + 0x02;
	const uint16_t IOPORT_LINEDATA = IOPORT_BASE + 0x03;
	const uint16_t IOPORT_MODEMDATA = IOPORT_BASE + 0x04;
	const uint16_t IOPORT_LINESTATUS = IOPORT_BASE + 0x05;
	const uint16_t IOPORT_MODEMSTATUS = IOPORT_BASE + 0x06;
	const uint16_t IOPORT_SCRATCH = IOPORT_BASE + 0x07;

	const uint16_t TRANSMITWAIT = 1000;

	bool IsReady()
    {
	    return (inb(IOPORT_LINESTATUS) & 0x20);
    }
    void WriteByte(uint8_t pByte)
    {
	    uint32_t waits = TRANSMITWAIT;
	    while (waits && !IsReady()) --waits;
	    if (IsReady()) outb(IOPORT_DATA, pByte);
    }
}