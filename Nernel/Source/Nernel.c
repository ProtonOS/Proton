#include <System/APIC.h>
#include <Common.h>
#include <System/Console.h>
#include <System/CPUID.h>
#include <System/GDT.h>
#include <System/IDT.h>
#include <System/Log.h>
#include <System/Multiboot.h>
#include <System/PIC.h>
#include <System/PIT.h>
#include <System/RTC.h>
#include <System/SerialLogger.h>
#include <System/SystemClock.h>
#include <System/ThreadScheduler.h>
#include <System/x86/Registers.h>

void CPUInterruptHandler(InterruptRegisters pRegisters)
{
	char buf[128];
	snprintf(buf, 128, "CPU Exception: %d", (int)pRegisters.int_no);
	Panic(buf);
}

void Startup();
void Startup2();
void Startup3();
void Startup4();
void Startup5();
#define LocalDef__TickCountMod 50000000
void Startup()
{
	time_t startupTime = time(NULL);
	Log_WriteLine(LOGLEVEL__Information, "Nernel Started @ %24.24s", ctime(&startupTime));
	usleep(999999);
	startupTime = time(NULL);
	Log_WriteLine(LOGLEVEL__Information, "Nernel Started @ %24.24s", ctime(&startupTime));
	Process_Create((size_t)&Startup2, 0x100000);
	uint32_t tickCount = 0;
	uint32_t trueTickCount = 0;
	while(TRUE)
	{
		//printf("ESP: 0x%x\n", (unsigned int)Register_GetESP());
		//printf("SS: 0x%x\n", (unsigned int)Register_GetStackSegment());
		++tickCount;
		if ((tickCount % LocalDef__TickCountMod) == 0)
		{
			tickCount = 0;
			trueTickCount++;
			printf("Startup Tick %i!\n", (int)trueTickCount);
		}
	}
}

void Startup2()
{
	Process_Create((size_t)&Startup3, 0x100000);
	uint32_t trueTickCount = 0;
	uint32_t tickCount = 0;
	while(TRUE)
	{
		//printf("ESP2: 0x%x\n", (unsigned int)Register_GetESP());
		//printf("SS2: 0x%x\n", (unsigned int)Register_GetStackSegment());
		++tickCount;
		if ((tickCount % LocalDef__TickCountMod) == 0)
		{
			tickCount = 0;
			trueTickCount++;
			printf("Startup2 Tick %i!\n", (int)trueTickCount);
		}
	}
}

void Startup3()
{
	Process_Create((size_t)&Startup4, 0x100000);
	uint32_t trueTickCount = 0;
	uint32_t tickCount = 0;
	while(TRUE)
	{
		//printf("ESP3: 0x%x\n", (unsigned int)Register_GetESP());
		//printf("SS3: 0x%x\n", (unsigned int)Register_GetStackSegment());
		++tickCount;
		if ((tickCount % LocalDef__TickCountMod) == 0)
		{
			tickCount = 0;
			trueTickCount++;
			printf("Startup3 Tick %i!\n", (int)trueTickCount);
		}
	}
}

void Startup4()
{
	Process_Create((size_t)&Startup5, 0x100000);
	uint32_t trueTickCount = 0;
	uint32_t tickCount = 0;
	while(TRUE)
	{
		//printf("ESP4: 0x%x\n", (unsigned int)Register_GetESP());
		//printf("SS4: 0x%x\n", (unsigned int)Register_GetStackSegment());
		++tickCount;
		if ((tickCount % LocalDef__TickCountMod) == 0)
		{
			tickCount = 0;
			trueTickCount++;
			printf("Startup4 Tick %i!\n", (int)trueTickCount);
		}
	}
}

void Startup5()
{
	uint32_t trueTickCount = 0;
	uint32_t tickCount = 0;
	while(TRUE)
	{
		//printf("ESP5: 0x%x\n", (unsigned int)Register_GetESP());
		//printf("SS5: 0x%x\n", (unsigned int)Register_GetStackSegment());
		++tickCount;
		if ((tickCount % LocalDef__TickCountMod) == 0)
		{
			tickCount = 0;
			trueTickCount++;
			printf("Startup5 Tick %i!\n", (int)trueTickCount);
		}
	}
}
extern uint32_t gStack;

void EnteredUserMode()
{
	printf("Entered User Mode: cs = 0x%x, esp = 0x%x, ss = 0x%x\n", (unsigned int)Register_GetCodeSegment(), (unsigned int)Register_GetESP(), (unsigned int)Register_GetStackSegment());
	while(TRUE) ;
}

uint32_t gEnteredUserModeAddress = (uint32_t)&EnteredUserMode;

void Main(uint32_t pMultibootMagic, MultibootHeader* pMultibootHeader)
{
	Multiboot_Startup(pMultibootMagic, pMultibootHeader);
	SerialLogger_Startup();
	Console_Startup();
	GDT_Startup();
	IDT_Startup();
	for (uint8_t interrupt = 0; interrupt < IDT__IRQ__RemappedBase; ++interrupt) IDT_RegisterHandler(interrupt, &CPUInterruptHandler);
	PIC_Startup();
	PIT_Startup();
	RTC_Startup();
	SystemClock_Startup();
	//CPUID_Startup();
	APIC_Create(APIC__LocalMSR);
	ThreadScheduler_Startup((size_t)&Startup, 0x100000);
	//PIC_StartInterrupts();
	printf("Entering User Mode: cs = 0x%x, esp = 0x%x, ss = 0x%x\n", (unsigned int)Register_GetCodeSegment(), (unsigned int)Register_GetESP(), (unsigned int)Register_GetStackSegment());
	GDT_SwitchToUserMode();
	while(TRUE);
}

void Panic(const char* pMessage)
{
	//Console_Clear(CONSOLE_CREATEATTRIBUTES(CONSOLE_DARKBLACK, CONSOLE_LIGHTRED));
	Console_WriteLine(pMessage);
	Halt();
}
