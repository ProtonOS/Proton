#include <Core/SerialWriter.h>

#include <PortIO.h>

void SerialWriter_Startup() { }

void SerialWriter_Shutdown() { }

bool_t SerialWriter_IsTransmitEmpty() { return (inb(SerialWriter_LineStatusIOPort) & 0x20) != 0; }

void SerialWriter_WriteByte(uint8_t pByte)
{
	uint32_t waits = SerialWriter_TransmitWait;
	while (waits && !SerialWriter_IsTransmitEmpty()) --waits;
	if (SerialWriter_IsTransmitEmpty()) outb(SerialWriter_DataIOPort, pByte);
}
