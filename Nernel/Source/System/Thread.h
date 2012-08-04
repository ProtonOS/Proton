#pragma once

typedef struct _Thread Thread;

#include "APIC.h"
#include "IDT.h"
#include "Process.h"
#include <CLR/AppDomain.h>

#include <reent.h>

struct _Thread
{
	Process* Process;
	AppDomain* Domain;
	bool_t Suspended;
	bool_t InCriticalSection;
	Thread* Next;
	Thread* Prev;
	uint8_t* StackStream;
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
void Thread_EnterCriticalRegion();
void Thread_LeaveCriticalRegion();
