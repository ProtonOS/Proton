#include <Core/DeviceManager.h>

#include <PortIO.h>

using namespace Core;

COMPortLogger::COMPortLogger()
{
}

bool COMPortLogger::Initialize()
{
    if (!IsIOPortAvailable(DataIOPort) ||
        !IsIOPortAvailable(InterruptIOPort) ||
        !IsIOPortAvailable(FIFOIOPort) ||
        !IsIOPortAvailable(LineDataIOPort) ||
        !IsIOPortAvailable(ModemDataIOPort) ||
        !IsIOPortAvailable(LineStatusIOPort) ||
        !IsIOPortAvailable(ModemStatusIOPort) ||
        !IsIOPortAvailable(ScratchIOPort)) return false;

    ClaimIOPort(DataIOPort);
    ClaimIOPort(InterruptIOPort);
    ClaimIOPort(FIFOIOPort);
    ClaimIOPort(LineDataIOPort);
    ClaimIOPort(ModemDataIOPort);
    ClaimIOPort(LineStatusIOPort);
    ClaimIOPort(ModemStatusIOPort);
    ClaimIOPort(ScratchIOPort);

	outb(InterruptIOPort, 0x00);
	outb(LineDataIOPort, 0x80);
	outb(DataIOPort, 0x03);
	outb(InterruptIOPort, 0x00);
	outb(LineDataIOPort, 0x03);
	outb(FIFOIOPort, 0xC7);
	outb(ModemDataIOPort, 0x0B);

    return true;
}

void COMPortLogger::Cleanup()
{
    ReleaseIOPort(DataIOPort);
    ReleaseIOPort(InterruptIOPort);
    ReleaseIOPort(FIFOIOPort);
    ReleaseIOPort(LineDataIOPort);
    ReleaseIOPort(ModemDataIOPort);
    ReleaseIOPort(LineStatusIOPort);
    ReleaseIOPort(ModemStatusIOPort);
    ReleaseIOPort(ScratchIOPort);
}

void COMPortLogger::WriteByte(uint8_t pByte)
{
	uint32_t attempts = WriteAttempts;
	while (attempts && !IsTransmitEmpty()) --attempts;
	outb(DataIOPort, pByte);
}

void COMPortLogger::WriteString(const char* pString)
{
	const char* iterator = pString;
	while (*iterator)
	{
		WriteByte(*iterator);
		++iterator;
	}
}

void COMPortLogger::WriteLine(const char * pLine)
{
	WriteString(pLine);
	WriteByte(0x0D);
	WriteByte(0x0A);
}

bool COMPortLogger::IsTransmitEmpty()
{
	return (inb(LineStatusIOPort) & 0x20) != 0;
}
