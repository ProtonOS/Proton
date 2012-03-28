#pragma once

typedef struct _Thread Thread;

#include "IDT.h"
#include "Process.h"

struct _Thread
{
	Process* Process;
	Thread* Next;
	Thread* Prev;
	uint8_t* Stack;
	uint32_t StackSize;
	size_t EntryPoint;
	uint8_t Priority;
	uint8_t TimeConsumed;
	uint8_t Busy;
	InterruptRegisters SavedRegisterState;
};

Thread* Thread_Create(Process* pProcess, size_t pEntryPoint, size_t pStackSize);
void Thread_Destroy(Thread* pThread);
