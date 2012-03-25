#include "Common.h"
#include "Multiboot.h"

void Main(uint32_t pMultibootMagic, MultibootHeader* pMultibootHeader)
{
	Multiboot_Startup(pMultibootMagic, pMultibootHeader);
}

void Panic(const char* pMessage)
{
	//Console_Clear(Console_CreateAttributes(Console_DarkBlack, Console_LightRed));
	//Console_WriteLine(pMessage);
	Halt();
}
