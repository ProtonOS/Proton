#pragma once

typedef struct _Thread Thread;

#include "APIC.h"
#include "IDT.h"
#include "Process.h"

#include <reent.h>

struct _Thread
{
	Process* Process;
	Thread* Next;
	Thread* Prev;
	Thread* SleepingNext;
	Thread* SleepingPrev;
	uint8_t* Stack;
	uint32_t StackSize;
	size_t EntryPoint;
	APIC* CurrentAPIC;
	uint8_t Priority;
	uint32_t SliceRemaining;
	uint64_t SleepRemaining;
	uint8_t Busy;
	InterruptRegisters SavedRegisterState;
	uint32_t MallocLockDepth;
	struct _reent Reentrant;
};

Thread* Thread_Create(Process* pProcess, size_t pEntryPoint, size_t pStackSize, uint8_t pPriority);
void Thread_Destroy(Thread* pThread);
