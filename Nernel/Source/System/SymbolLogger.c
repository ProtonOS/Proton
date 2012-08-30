#include <System/SymbolLogger.h>
#include <System/x86/PortIO.h>

#define SYMBOLLOGGER__BaseIOPort			0x03F8
#define SYMBOLLOGGER__IOPort__Data			SYMBOLLOGGER__BaseIOPort + 0x00
#define SYMBOLLOGGER__IOPort__Interrupt		SYMBOLLOGGER__BaseIOPort + 0x01
#define SYMBOLLOGGER__IOPort__FIFO			SYMBOLLOGGER__BaseIOPort + 0x02
#define SYMBOLLOGGER__IOPort__LineData		SYMBOLLOGGER__BaseIOPort + 0x03
#define SYMBOLLOGGER__IOPort__ModemData		SYMBOLLOGGER__BaseIOPort + 0x04
#define SYMBOLLOGGER__IOPort__LineStatus	SYMBOLLOGGER__BaseIOPort + 0x05
#define SYMBOLLOGGER__IOPort__ModemStatus	SYMBOLLOGGER__BaseIOPort + 0x06
#define SYMBOLLOGGER__IOPort__Scratch		SYMBOLLOGGER__BaseIOPort + 0x07

#define SYMBOLLOGGER__TransmitWait			1000

#include <CLR/GC.h>

void SymbolLogger_Startup()
{
	char symbolBuffer[512];
	snprintf(symbolBuffer, 512, "4:Panic %u", (unsigned int)Panic);
	SymbolLogger_WriteLine(symbolBuffer);
	memset(&symbolBuffer, 0, 512);
	snprintf(symbolBuffer, 512, "4:GC_AllocateInternedStringFromUnicode %u", (unsigned int)GC_AllocateInternedStringFromUnicode);
	SymbolLogger_WriteLine(symbolBuffer);
	memset(&symbolBuffer, 0, 512);
	snprintf(symbolBuffer, 512, "4:GC_AllocateArray %u", (unsigned int)GC_AllocateArray);
	SymbolLogger_WriteLine(symbolBuffer);
	memset(&symbolBuffer, 0, 512);
	snprintf(symbolBuffer, 512, "4:GC_AllocateObject %u", (unsigned int)GC_AllocateObject);
	SymbolLogger_WriteLine(symbolBuffer);
	memset(&symbolBuffer, 0, 512);
}

void SymbolLogger_Shutdown()
{
}

bool_t SymbolLogger_IsTransmitEmpty()
{
	return (inb(SYMBOLLOGGER__IOPort__LineStatus) & 0x20) != 0;
}

void SymbolLogger_WriteByte(uint8_t pByte)
{
	uint32_t waits = SYMBOLLOGGER__TransmitWait;
	while (waits && !SymbolLogger_IsTransmitEmpty()) --waits;
	if (SymbolLogger_IsTransmitEmpty()) outb(SYMBOLLOGGER__IOPort__Data, pByte);
}

void SymbolLogger_WriteLine(const char* pLine)
{
	for (const char* iterator = pLine; *iterator; ++iterator)
	{
		SymbolLogger_WriteByte(*iterator);
	}
	SymbolLogger_WriteByte('\r');
	SymbolLogger_WriteByte('\n');
}
