#include "Common.h"
#include "Console.h"
#include "Log.h"
#include "Multiboot.h"

void Main(uint32_t pMultibootMagic, MultibootHeader* pMultibootHeader)
{
	Multiboot_Startup(pMultibootMagic, pMultibootHeader);
	Console_Startup();
	Log_Startup();

	Log_WriteLine(LOGLEVEL_INFORMATION, "Nernel Startup");
}

void Panic(const char* pMessage)
{
	Console_Clear(CONSOLE_CREATEATTRIBUTES(CONSOLE_DARKBLACK, CONSOLE_LIGHTRED));
	Console_WriteLine(pMessage);
	Halt();
}
