#include "Common.h"
#include "Console.h"
#include "CPUID.h"
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

	CPUID_Startup();

	time_t startupTime = time(NULL);
	Log_WriteLine(LOGLEVEL_INFORMATION, "Nernel Started @ %24.24s", ctime(&startupTime));
	Log_WriteLine(LOGLEVEL_INFORMATION, "CPU Supports: Largest Standard Function = %u", (unsigned int)gCPUID_LargestStandardFunction);
	Log_WriteLine(LOGLEVEL_INFORMATION, "Processor Vendor: %s", gCPUID_ProcessorVendor);
	Log_WriteLine(LOGLEVEL_INFORMATION, "Processor Family: %u, Model: %u, Stepping: %u", (unsigned int)gCPUID_ProcessorFamily, (unsigned int)gCPUID_ProcessorModel, (unsigned int)gCPUID_ProcessorStepping);
	Log_WriteLine(LOGLEVEL_INFORMATION, "Local APIC ID: %u, Logical Processor Count: %u, Cache Flush Size: %u", (unsigned int)gCPUID_LocalAPICID, (unsigned int)gCPUID_LogicalProcessorCount, (unsigned int)gCPUID_CacheLineFlushSize);
	Log_WriteLine(LOGLEVEL_INFORMATION, "Brand ID: %u, Features1: 0x%x, Features2: 0x%x", (unsigned int)gCPUID_BrandID, (unsigned int)gCPUID_Features1, (unsigned int)gCPUID_Features2);
}

void Panic(const char* pMessage)
{
	Console_Clear(CONSOLE_CREATEATTRIBUTES(CONSOLE_DARKBLACK, CONSOLE_LIGHTRED));
	Console_WriteLine(pMessage);
	Halt();
}
