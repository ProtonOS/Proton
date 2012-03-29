#include <Common.h>
#include <System/Thread.h>
#include <System/ThreadScheduler.h>
#include <System/x86/Registers.h>


Thread* Thread_Create(Process* pProcess, size_t pEntryPoint, size_t pStackSize)
{
	Thread* thread = (Thread*)calloc(1, sizeof(Thread));
	Log_WriteLine(LOGLEVEL__Memory, "Memory: Thread_Create @ 0x%x", (unsigned int)thread);
	thread->Process = pProcess;
	thread->EntryPoint = pEntryPoint;
	thread->Stack = (uint8_t*)calloc(1, pStackSize);
	printf("Stack memory: 0x%x\n", (unsigned int)thread->Stack);
	thread->StackSize = pStackSize;
	thread->Priority = 2;
	thread->SavedRegisterState.esp = (uint32_t)(thread->Stack + thread->StackSize);
	thread->SavedRegisterState.ebp = thread->SavedRegisterState.esp;
	thread->SavedRegisterState.eip = pEntryPoint;
	thread->SavedRegisterState.cs = Register_GetCodeSegment();
	thread->SavedRegisterState.ds = Register_GetDataSegment();
	thread->SavedRegisterState.ss = 0x10;
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
