#include "Kernel/Arch.h"
#include "Kernel/Console.h"
#include "Kernel/Multiboot.h"

extern "C" {
void Main(uint32_t pMultibootMagic, void* pMultibootHeader);
void Started();
}

void Main(uint32_t pMultibootMagic, void* pMultibootHeader)
{
	Multiboot::Startup(pMultibootMagic, reinterpret_cast<Multiboot::Header*>(pMultibootHeader));
	Console::Startup();
	Arch::Startup();

	while (true);
}

void Started()
{
	time_t startupTime = time(NULL);
	printf("Nernel Started @ %24.24s\n", ctime(&startupTime));

	while (true) ;
}

void Panic(const char* pMessage)
{
	Console::Clear(Console::Color::DarkBlack, Console::Color::LightRed);
	Console::WriteLine(pMessage);
	while (true);
}
