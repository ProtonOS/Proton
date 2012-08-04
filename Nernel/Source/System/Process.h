#pragma once

typedef struct _Process Process;

#include "Thread.h"
#include <CLR/AppDomain.h>

struct _Process
{
	uint16_t Identifier;
	Thread** Threads;
	uint32_t ThreadCount;
	AppDomain** Domains;
	uint32_t DomainCount;
};

Process* Process_Create(size_t pEntryPoint, size_t pThreadStackSize, uint8_t pPriority);
Thread* Process_CreateThread(Process* pProcess, size_t pEntryPoint, size_t pThreadStackSize, uint8_t pPriority);
void Process_RemoveThread(Process* pProcess, Thread* pThread);
void Process_AddDomain(Process* pProcess, AppDomain* pDomain);
void Process_RemoveDomain(Process* pProcess, AppDomain* pDomain);
void Process_Destroy(Process* pProcess);
