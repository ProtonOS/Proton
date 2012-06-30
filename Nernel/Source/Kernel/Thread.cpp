#include "Process.h"
#include "Thread.h"
//#include <System/ThreadScheduler.h>
//#include <System/x86/Registers.h>

// Make intellisense not complain
#ifdef _WIN32
#undef _REENT_INIT_PTR
#define _REENT_INIT_PTR(reentrancyStructurePtr)
#endif

Thread::Thread(Process* pProcess, size_t pEntryPoint, size_t pStackSize, uint8_t pPriority)
{
	mProcess = pProcess;
	mEntryPoint = pEntryPoint;
	mStack = new uint8_t[pStackSize];
	_REENT_INIT_PTR(&mReentrant);
	mReentrant._stdin = stdin;
	mReentrant._stdout = stdout;
	mReentrant._stderr = stderr;
	mReentrant.__sdidinit = 1;
	mReentrant._new._reent._unused_rand = (uint32_t)this;
//	Log_WriteLine(LOGLEVEL__Memory, "Memory: Thread_Create @ 0x%x, Stack @ 0x%x", (unsigned int)thread, (unsigned int)thread->Stack);
	mStackSize = pStackSize;
	mPriority = pPriority;
//	mSavedRegisterState.useresp = (uint32_t)(thread->Stack + thread->StackSize);
//	mSavedRegisterState.eip = pEntryPoint;
//	mSavedRegisterState.cs = 0x1B;
//	mSavedRegisterState.ds = 0x23;
//	mSavedRegisterState.ss = 0x23;
//	mSavedRegisterState.eflags = 0x200; // Interrupts enabled, maybe need 0x202
//	ThreadScheduler_Add(thread);
}

Thread::~Thread()
{
	//ThreadScheduler_Remove(pThread);
	//Log_WriteLine(LOGLEVEL__Memory, "Memory: Thread_Destroy @ 0x%x", (unsigned int)pThread);
	delete [] mStack;
}
