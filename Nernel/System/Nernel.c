#include "Common.h"
#include "Console.h"
#include "GlobalDescriptorTable.h"
#include "InterruptDescriptorTable.h"
#include "Log.h"
#include "Multiboot.h"

void Main(uint32_t pMultibootMagic, MultibootHeader* pMultibootHeader)
{
	Multiboot_Startup(pMultibootMagic, pMultibootHeader);
	Console_Startup();
	Log_Startup();
	GlobalDescriptorTable_Startup();
	InterruptDescriptorTable_Startup();

	Log_WriteLine(LOGLEVEL_INFORMATION, "Nernel Started");
}

void Panic(const char* pMessage)
{
	Console_Clear(CONSOLE_CREATEATTRIBUTES(CONSOLE_DARKBLACK, CONSOLE_LIGHTRED));
	Console_WriteLine(pMessage);
	Halt();
}
