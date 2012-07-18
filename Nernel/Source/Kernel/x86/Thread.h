#pragma once

#include "IDT.h"
#include <reent.h>

class Process;
class Processor;

class Thread
{
public:
	Process* Owner;
	Thread* Next;
	Thread* Prev;
	Thread* SleepingNext;
	Thread* SleepingPrev;
	uint8_t* Stack;
	uint32_t StackSize;
	size_t EntryPoint;
	Processor* CurrentProcessor;
	uint8_t Priority;
	uint32_t SliceRemaining;
	uint64_t SleepRemaining;
	uint8_t Busy;
	IDT::InterruptRegisters SavedRegisterState;
	uint32_t MallocLockDepth;
	struct _reent Reentrant;

	Thread(Process* pOwner, size_t pEntryPoint, size_t pStackSize, uint8_t pPriority);
	~Thread();
};
