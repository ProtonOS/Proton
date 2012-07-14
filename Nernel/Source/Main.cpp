#include "Kernel/Arch.h"
#include "Kernel/Console.h"
#include "Kernel/Multiboot.h"

extern "C" {
void Main(uint32_t pMultibootMagic, void* pMultibootHeader);
}

void Main(uint32_t pMultibootMagic, void* pMultibootHeader)
{
	Multiboot::Startup(pMultibootMagic, reinterpret_cast<Multiboot::Header*>(pMultibootHeader));
	Console::Startup();
	Arch::Startup();
}

void Panic(const char* pMessage)
{
	Console::Clear(Console::Color::DarkBlack, Console::Color::LightRed);
	Console::WriteLine(pMessage);
	while (true);
}
