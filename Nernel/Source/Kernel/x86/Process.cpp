#include "../Atomics.h"
#include "Process.h"
#include "Thread.h"

#define PROCESS__DefaultThreadPool_Max		128

#define PROCESS__Max						2048

uint8_t Process::sBusy = 0;
Process* Process::sProcesses[PROCESS__Max];
uint32_t Process::sNextIdentifier = 0;

Process::Process(size_t pEntryPoint, size_t pThreadStackSize, uint8_t pPriority)
{
	AtomicAquireLock(&sBusy);
	if (sNextIdentifier >= PROCESS__Max)
	{
		// Compact down unused process pointers
		Panic("Reached maximum process count, need to compact");
	}
	Identifier = sNextIdentifier;
	sProcesses[sNextIdentifier++] = this;
	//Log_WriteLine(LOGLEVEL__Memory, "Memory: Process_Create @ 0x%x", (unsigned int)process);
	Threads = new Thread*[PROCESS__DefaultThreadPool_Max];
	Threads[0] = new Thread(this, pEntryPoint, pThreadStackSize, pPriority);
	ThreadCount = 1;
	AtomicReleaseLock(&sBusy);
}

Process::~Process()
{
	AtomicAquireLock(&sBusy);
	sProcesses[Identifier] = NULL;
	//Log_WriteLine(LOGLEVEL__Memory, "Memory: Process_Destroy @ 0x%x", (unsigned int)pProcess);
	for (uint32_t index = 0; index < ThreadCount; ++index)
	{
		if (Threads[index])
		{
			delete Threads[index];
		}
	}
	delete [] Threads;
	AtomicReleaseLock(&sBusy);
}
