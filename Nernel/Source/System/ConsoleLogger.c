#include <System/ConsoleLogger.h>
#include <System/x86/PortIO.h>

#define CONSOLELOGGER__BaseIOPort			0x02F8
#define CONSOLELOGGER__IOPort__Data			CONSOLELOGGER__BaseIOPort + 0x00
#define CONSOLELOGGER__IOPort__Interrupt	CONSOLELOGGER__BaseIOPort + 0x01
#define CONSOLELOGGER__IOPort__FIFO			CONSOLELOGGER__BaseIOPort + 0x02
#define CONSOLELOGGER__IOPort__LineData		CONSOLELOGGER__BaseIOPort + 0x03
#define CONSOLELOGGER__IOPort__ModemData	CONSOLELOGGER__BaseIOPort + 0x04
#define CONSOLELOGGER__IOPort__LineStatus	CONSOLELOGGER__BaseIOPort + 0x05
#define CONSOLELOGGER__IOPort__ModemStatus	CONSOLELOGGER__BaseIOPort + 0x06
#define CONSOLELOGGER__IOPort__Scratch		CONSOLELOGGER__BaseIOPort + 0x07

#define CONSOLELOGGER__TransmitWait			1000

void ConsoleLogger_Startup()
{
}

void ConsoleLogger_Shutdown()
{
}

bool_t ConsoleLogger_IsTransmitEmpty()
{
	return (inb(CONSOLELOGGER__IOPort__LineStatus) & 0x20) != 0;
}

void ConsoleLogger_WriteByte(uint8_t pByte)
{
	uint32_t waits = CONSOLELOGGER__TransmitWait;
	while (waits && !ConsoleLogger_IsTransmitEmpty()) --waits;
	if (ConsoleLogger_IsTransmitEmpty()) outb(CONSOLELOGGER__IOPort__Data, pByte);
}
