#include <Common.h>
#include <System/Thread.h>
#include <System/ThreadScheduler.h>
#include <System/x86/Registers.h>


Thread* Thread_Create(Process* pProcess, size_t pEntryPoint, size_t pStackSize)
{
	Thread* thread = (Thread*)calloc(1, sizeof(Thread));
	thread->Process = pProcess;
	thread->EntryPoint = pEntryPoint;
	thread->Stack = (uint8_t*)calloc(1, pStackSize);
	_REENT_INIT_PTR(&thread->Reentrant);
	thread->Reentrant._stdin = stdin;
	thread->Reentrant._stdout = stdout;
	thread->Reentrant._stderr = stderr;
	thread->Reentrant.__sdidinit = TRUE;
	Log_WriteLine(LOGLEVEL__Memory, "Memory: Thread_Create @ 0x%x, Stack @ 0x%x", (unsigned int)thread, (unsigned int)thread->Stack);
	thread->StackSize = pStackSize;
	thread->Priority = 2;
	thread->SavedRegisterState.useresp = (uint32_t)(thread->Stack + thread->StackSize);
	thread->SavedRegisterState.eip = pEntryPoint;
	thread->SavedRegisterState.cs = 0x1B;
	thread->SavedRegisterState.ds = 0x23;
	thread->SavedRegisterState.ss = 0x23;
	thread->SavedRegisterState.eflags = 0x200; // Interrupts enabled, maybe need 0x202
	ThreadScheduler_Add(thread);
	return thread;
}

void Thread_Destroy(Thread* pThread)
{
	ThreadScheduler_Remove(pThread);
	Log_WriteLine(LOGLEVEL__Memory, "Memory: Thread_Destroy @ 0x%x", (unsigned int)pThread);
	free(pThread->Stack);
	free(pThread);
}
