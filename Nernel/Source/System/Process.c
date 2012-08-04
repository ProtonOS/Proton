#include <System/Atomics.h>
#include <Common.h>
#include <System/Process.h>
#include <System/x86/PortIO.h>

#define PROCESS__Max						2048

uint8_t gProcess_Busy = 0;
Process* gProcess_Array[PROCESS__Max];
uint32_t gProcess_NextIndex = 0;

Process* Process_Create(size_t pEntryPoint, size_t pThreadStackSize, uint8_t pPriority)
{
	Process* process = (Process*)calloc(1, sizeof(Process));
	Atomic_AquireLock(&gProcess_Busy);
	bool_t foundUnused = FALSE;
	uint32_t identifier = 0;
	for (uint32_t index; index < gProcess_NextIndex; ++index)
	{
		if (!gProcess_Array[index])
		{
			foundUnused = TRUE;
			identifier = index;
			break;
		}
	}
	if (foundUnused) process->Identifier = identifier;
	else if (gProcess_NextIndex >= PROCESS__Max) Panic("Reached maximum process count");
	else process->Identifier = gProcess_NextIndex++;
	gProcess_Array[process->Identifier] = process;
	Log_WriteLine(LOGLEVEL__Memory, "Memory: Process_Create @ 0x%x", (unsigned int)process);
	process->ThreadCount = 8;
	process->Threads = (Thread**)calloc(1, (sizeof(Thread*) * process->ThreadCount));
	process->Threads[0] = Thread_Create(process, pEntryPoint, pThreadStackSize, pPriority);
	Atomic_ReleaseLock(&gProcess_Busy);
	return process;
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
	for (uint32_t index = 0; index < pProcess->DomainCount; ++index)
	{
		if (pProcess->Domains[index])
		{
			AppDomain_Destroy(pProcess->Domains[index]);
		}
	}
	if (pProcess->Domains) free(pProcess->Domains);
	free(pProcess->Threads);
	free(pProcess);

	Atomic_ReleaseLock(&gProcess_Busy);
}

Thread* Process_CreateThread(Process* pProcess, size_t pEntryPoint, size_t pThreadStackSize, uint8_t pPriority)
{
	bool_t foundUnused = FALSE;
	uint32_t unusedIndex = 0;
	for (uint32_t index = 0; index < pProcess->ThreadCount; ++index)
	{
		if (!pProcess->Threads[index])
		{
			unusedIndex = index;
			foundUnused = TRUE;
			break;
		}
	}
	if (!foundUnused)
	{
		unusedIndex = pProcess->ThreadCount;
		pProcess->ThreadCount <<= 1;
		pProcess->Threads = (Thread**)realloc(pProcess->Threads, pProcess->ThreadCount * sizeof(Thread*));
	}
	pProcess->Threads[unusedIndex] = Thread_Create(pProcess, pEntryPoint, pThreadStackSize, pPriority);
	return pProcess->Threads[unusedIndex];
}

void Process_RemoveThread(Process* pProcess, Thread* pThread)
{
	for (uint32_t index = 0; index < pProcess->ThreadCount; ++index)
	{
		if (pProcess->Threads[index] == pThread)
		{
			pProcess->Threads[index] = NULL;
			break;
		}
	}
}

void Process_AddDomain(Process* pProcess, AppDomain* pDomain)
{
	bool_t foundUnused = FALSE;
	uint32_t unusedIndex = 0;
	for (uint32_t index = 0; index < pProcess->DomainCount; ++index)
	{
		if (!pProcess->Domains[index])
		{
			unusedIndex = index;
			foundUnused = TRUE;
			break;
		}
	}
	if (!foundUnused)
	{
		if (!pProcess->Domains)
		{
			unusedIndex = 0;
			pProcess->DomainCount = 1;
			pProcess->Domains = (AppDomain**)calloc(1, sizeof(AppDomain*));
		}
		else
		{
			unusedIndex = pProcess->DomainCount;
			pProcess->DomainCount <<= 1;
			pProcess->Domains = (AppDomain**)realloc(pProcess->Domains, pProcess->DomainCount * sizeof(AppDomain*));
		}
	}
	pProcess->Domains[unusedIndex] = pDomain;
}

void Process_RemoveDomain(Process* pProcess, AppDomain* pDomain)
{
	for (uint32_t index = 0; index < pProcess->DomainCount; ++index)
	{
		if (pProcess->Domains[index] == pDomain)
		{
			pProcess->Domains[index] = NULL;
			break;
		}
	}
}
