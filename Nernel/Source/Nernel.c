#include <Common.h>
#include <CLR/AppDomain.h>
#include <CLR/ILDecomposition.h>
#include <CLR/JIT.h>
#include <System/APIC.h>
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
#include <System/SMP.h>
#include <System/SystemClock.h>
#include <System/ThreadScheduler.h>
#include <System/x86/Registers.h>

AppDomain* gMernelDomain = NULL;

void CPUInterruptHandler(InterruptRegisters pRegisters)
{
	char buf[128];
	snprintf(buf, 128, "CPU Exception: %d", (int)pRegisters.int_no);
	Panic(buf);
}

void Mernel_EntrypointCompiled()
{

}

void Startup();
#define LocalDef__TickCountMod 50000000
void Startup()
{
	time_t startupTime = time(NULL);
	printf("Nernel Started @ %24.24s\n", ctime(&startupTime));
	uint32_t tickCount = 0;
	uint32_t trueTickCount = 0;
	uint64_t startedTicks = SystemClock_GetTicks();
    gMernelDomain = AppDomain_Create();
	uint64_t stoppedTicks = SystemClock_GetTicks();
	printf("AppDomain_Create took %uMS\n", (unsigned int)(stoppedTicks - startedTicks));
	LoadedModule* mernelModule = Multiboot_GetLoadedModule("/boot/mernel.exe");
	AppDomain_AddAssembly(gMernelDomain, ILDecomposition_CreateAssembly(gMernelDomain, CLIFile_Create((uint8_t*)mernelModule->Address, mernelModule->Size, "mernel.exe")));
	JIT_CompileMethod(gMernelDomain->IRAssemblies[1]->EntryPoint);

	Mernel_EntrypointCompiled();

	while(TRUE)
	{
		++tickCount;
		if ((tickCount % LocalDef__TickCountMod) == 0)
		{
			tickCount = 0;
			trueTickCount++;
			printf("Startup Tick %i!\n", (int)trueTickCount);
		}
	}
}


extern uint32_t gStack;

void EnteredUserMode()
{
	Log_WriteLine(LOGLEVEL__Processor, "Processor: Started");
	++gSMP_StartedProcessors;
	if (gSMP_StartingBootstrapProcessor)
	{
		gThreadScheduler_Running = TRUE;
	}
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
	PIC_StartInterrupts();
	APIC* bootstrapAPIC = APIC_Create();
	ThreadScheduler_Startup((size_t)&Startup, 0x100000);
	SMP_Startup(bootstrapAPIC);
	//PIC_StartInterrupts();

	GDT_SwitchToUserMode();
	while(TRUE);
}

void Panic(const char* pMessage)
{
	//Console_Clear(CONSOLE_CREATEATTRIBUTES(CONSOLE_DARKBLACK, CONSOLE_LIGHTRED));
	Console_WriteLine(pMessage);
	while (TRUE) ;
}
