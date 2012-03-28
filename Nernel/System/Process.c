#include "Atomics.h"
#include "Common.h"
#include "Process.h"
#include "x86/PortIO.h"

#define PROCESS__DefaultThreadPool_Max		128

#define PROCESS__Max						2048

uint8_t gProcess_Busy = 0;
Process* gProcess_Array[PROCESS__Max];
uint32_t gProcess_NextIndex = 0;

Process* Process_Create(size_t pEntryPoint, size_t pThreadStackSize)
{
	Process* process = (Process*)calloc(1, sizeof(Process));
	Atomic_AquireLock(&gProcess_Busy);
	if (gProcess_NextIndex >= PROCESS__Max)
	{
		// Compact down unused process pointers
		Panic("Reached maximum process count, need to compact");
	}
	process->Identifier = gProcess_NextIndex;
	gProcess_Array[gProcess_NextIndex++] = process;
	Log_WriteLine(LOGLEVEL__Memory, "Memory: Process_Create @ 0x%x", (unsigned int)process);
	process->Threads = (Thread**)calloc(1, (sizeof(Thread*) * PROCESS__DefaultThreadPool_Max));
	process->Threads[0] = Thread_Create(process, pEntryPoint, pThreadStackSize);
	process->ThreadCount = 1;
	Atomic_ReleaseLock(&gProcess_Busy);
}

void Process_Destroy(Process* pProcess)
{
	Atomic_AquireLock(&gProcess_Busy);
	gProcess_Array[pProcess->Identifier] = NULL;
	Log_WriteLine(LOGLEVEL__Memory, "Memory: Process_Destroy @ 0x%x", (unsigned int)pProcess);
	for (uint32_t index = 0; index < pProcess->ThreadCount; ++index)
	{
		if (pProcess->Threads[index])
		{
			Thread_Destroy(pProcess->Threads[index]);
		}
	}
	free(pProcess->Threads);
	free(pProcess);

	Atomic_ReleaseLock(&gProcess_Busy);
}
