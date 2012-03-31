#pragma once

typedef struct _Process Process;

#include "Thread.h"

struct _Process
{
	Thread** Threads;
	uint32_t ThreadCount;
	uint16_t Identifier;
};

Process* Process_Create(size_t pEntryPoint, size_t pThreadStackSize);
Process* Process_PriorityCreate(size_t pEntryPoint, size_t pThreadStackSize, uint8_t pPriority);
void Process_Destroy(Process* pProcess);
