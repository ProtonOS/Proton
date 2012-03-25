#include "Common.h"
#include "Console.h"
#include "GlobalDescriptorTable.h"
#include "InterruptDescriptorTable.h"
#include "Log.h"
#include "Multiboot.h"
#include "RealTimeClock.h"
#include "SystemClock.h"

#include <time.h>

void CPUInterruptHandler(InterruptRegisters pRegisters)
{
	char buf[128];
	snprintf(buf, 128, "CPU Exception: %d", (int)pRegisters.int_no);
	Panic(buf);
}

void Main(uint32_t pMultibootMagic, MultibootHeader* pMultibootHeader)
{
	Multiboot_Startup(pMultibootMagic, pMultibootHeader);
	Console_Startup();
	Log_Startup();
	GlobalDescriptorTable_Startup();
	InterruptDescriptorTable_Startup();
	for (uint8_t interrupt = 0; interrupt < INTERRUPTDESCRIPTORTABLE_REMAPPEDIRQBASE; ++interrupt) InterruptDescriptorTable_RegisterHandler(interrupt, &CPUInterruptHandler);
	RealTimeClock_Startup();
	SystemClock_Startup();

	time_t startupTime = time(NULL);
	Log_WriteLine(LOGLEVEL_INFORMATION, "Nernel Started @ %24.24s", ctime(&startupTime));
}

void Panic(const char* pMessage)
{
	Console_Clear(CONSOLE_CREATEATTRIBUTES(CONSOLE_DARKBLACK, CONSOLE_LIGHTRED));
	Console_WriteLine(pMessage);
	Halt();
}
