#include <System/SerialLogger.h>
#include <System/x86/PortIO.h>

#define SERIALLOGGER__BaseIOPort			0x02F8
#define SERIALLOGGER__IOPort__Data			SERIALLOGGER__BaseIOPort + 0x00
#define SERIALLOGGER__IOPort__Interrupt		SERIALLOGGER__BaseIOPort + 0x01
#define SERIALLOGGER__IOPort__FIFO			SERIALLOGGER__BaseIOPort + 0x02
#define SERIALLOGGER__IOPort__LineData		SERIALLOGGER__BaseIOPort + 0x03
#define SERIALLOGGER__IOPort__ModemData		SERIALLOGGER__BaseIOPort + 0x04
#define SERIALLOGGER__IOPort__LineStatus	SERIALLOGGER__BaseIOPort + 0x05
#define SERIALLOGGER__IOPort__ModemStatus	SERIALLOGGER__BaseIOPort + 0x06
#define SERIALLOGGER__IOPort__Scratch		SERIALLOGGER__BaseIOPort + 0x07

#define SERIALLOGGER__TransmitWait			1000

void SerialLogger_Startup()
{
}

void SerialLogger_Shutdown()
{
}

bool_t SerialLogger_IsTransmitEmpty()
{
	return (inb(SERIALLOGGER__IOPort__LineStatus) & 0x20) != 0;
}

void SerialLogger_WriteByte(uint8_t pByte)
{
	uint32_t waits = SERIALLOGGER__TransmitWait;
	while (waits && !SerialLogger_IsTransmitEmpty()) --waits;
	if (SerialLogger_IsTransmitEmpty()) outb(SERIALLOGGER__IOPort__Data, pByte);
}