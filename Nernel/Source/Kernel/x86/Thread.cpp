#include "Process.h"
#include "Thread.h"
#include "ThreadScheduler.h"

// Make intellisense not complain
#ifdef _WIN32
#undef _REENT_INIT_PTR
#define _REENT_INIT_PTR(reentrancyStructurePtr)
#endif

Thread::Thread(Process* pOwner, size_t pEntryPoint, size_t pStackSize, uint8_t pPriority)
{
	Owner = pOwner;
	Next = nullptr;
	Prev = nullptr;
	SleepingNext = nullptr;
	SleepingPrev = nullptr;
	Stack = new uint8_t[pStackSize];
	StackSize = pStackSize;
	EntryPoint = pEntryPoint;
	CurrentProcessor = nullptr;
	Priority = pPriority;
	SliceRemaining = 0;
	SleepRemaining = 0;
	Busy = 0;
	SavedRegisterState.useresp = (uint32_t)(Stack + StackSize);
	SavedRegisterState.eip = pEntryPoint;
	SavedRegisterState.cs = 0x1B;
	SavedRegisterState.ds = 0x23;
	SavedRegisterState.ss = 0x23;
	SavedRegisterState.eflags = 0x200; // Interrupts enabled, maybe need 0x202
	MallocLockDepth = 0;
	_REENT_INIT_PTR(&Reentrant);
	Reentrant._stdin = stdin;
	Reentrant._stdout = stdout;
	Reentrant._stderr = stderr;
	Reentrant.__sdidinit = 1;
	Reentrant._new._reent._unused_rand = (uint32_t)this;
//	Log_WriteLine(LOGLEVEL__Memory, "Memory: Thread_Create @ 0x%x, Stack @ 0x%x", (unsigned int)thread, (unsigned int)thread->Stack);
	ThreadScheduler::Add(this);
}

Thread::~Thread()
{
	ThreadScheduler::Remove(this);
	//Log_WriteLine(LOGLEVEL__Memory, "Memory: Thread_Destroy @ 0x%x", (unsigned int)pThread);
	delete [] Stack;
}
