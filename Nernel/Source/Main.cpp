#include "Kernel/Multiboot.h"

#include "Kernel/x86/Drivers/SerialPortLogger.h"
#include "Kernel/x86/Drivers/VGAConsole.h"

extern "C" {
void Main(uint32_t pMultibootMagic, void* pMultibootHeader);
}

void Main(uint32_t pMultibootMagic, void* pMultibootHeader)
{
	Multiboot::Startup(pMultibootMagic, pMultibootHeader);

	// x86
	new SerialPortLogger();
	new VGAConsole();
}

void Panic(const char* pMessage)
{
	Console::Instance->Clear(Console::Color::DarkBlack, Console::Color::LightRed);
	Console::Instance->WriteLine(pMessage);
	while (true) ;
}
