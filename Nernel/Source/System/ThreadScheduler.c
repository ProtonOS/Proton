#include <System/APIC.h>
#include <System/Atomics.h>
#include <Common.h>
#include <System/Process.h>
#include <System/x86/Registers.h>
#include <System/SystemClock.h>
#include <System/ThreadScheduler.h>

extern bool_t gMultitasking;

volatile bool_t gThreadScheduler_Running = FALSE;
volatile uint8_t gThreadScheduler_Busy = 0;
Thread* gThreadScheduler_Window = NULL;
//Thread* gThreadScheduler_SleepingWindow = NULL;
Process* gThreadScheduler_IdleProcess = NULL;
Process* gThreadScheduler_KernelProcess = NULL;

void ThreadScheduler_Idle()
{
	while (TRUE) ;
}

void ThreadScheduler_Timer(InterruptRegisters pRegisters)
{
	APIC* apic = gAPIC_Array[(pRegisters.int_no - 128) >> 1];
	if (gThreadScheduler_Running)
	{
		if (gThreadScheduler_Window)
		{
			if (!apic->CurrentThread || (apic->CurrentThread && !apic->CurrentThread->InCriticalSection))
			{
				ThreadScheduler_Schedule(&pRegisters, apic);
			}
			//else if (apic->CurrentThread && apic->CurrentThread->InCriticalSection)
			//{
			//	printf("We've gone critical!\n");
			//}
			gMultitasking = TRUE;
		}
	}
	*(size_t*)(apic->BaseAddress + APIC__Register__EndOfInterrupt) = 0;
}

void ThreadScheduler_Startup(size_t pEntryPoint, size_t pThreadStackSize)
{
	gThreadScheduler_IdleProcess = Process_Create((size_t)&ThreadScheduler_Idle, 8192, 0);
	gThreadScheduler_KernelProcess = Process_Create(pEntryPoint, pThreadStackSize, 200);
}

void ThreadScheduler_Shutdown()
{
}

void ThreadScheduler_Add(Thread* pThread)
{
	Thread_EnterCriticalRegion();
	Atomic_AquireLock(&gThreadScheduler_Busy);
	if (!gThreadScheduler_Window)
	{
		gThreadScheduler_Window = pThread;
		pThread->Next = pThread;
		pThread->Prev = pThread;
	}
	else
	{
		pThread->Prev = gThreadScheduler_Window;
		pThread->Next = gThreadScheduler_Window->Next;
		gThreadScheduler_Window->Next->Prev = pThread;
		gThreadScheduler_Window->Next = pThread;
	}
	Atomic_ReleaseLock(&gThreadScheduler_Busy);
	Thread_LeaveCriticalRegion();
}

void ThreadScheduler_Remove(Thread* pThread)
{
	Thread_EnterCriticalRegion();
	Atomic_AquireLock(&gThreadScheduler_Busy);
	pThread->Prev->Next = pThread->Next;
	pThread->Next->Prev = pThread->Prev;
	if (pThread == gThreadScheduler_Window)
	{
		gThreadScheduler_Window = gThreadScheduler_Window->Next;
		if (gThreadScheduler_Window == pThread) gThreadScheduler_Window = NULL;
	}
	Atomic_ReleaseLock(&gThreadScheduler_Busy);
	Thread_LeaveCriticalRegion();
}

void ThreadScheduler_Suspend(Thread* pThread)
{
	pThread->Suspended = TRUE;
}

void ThreadScheduler_Resume(Thread* pThread)
{
	pThread->Suspended = FALSE;
}

Thread* ThreadScheduler_FindThread(APIC* pAPIC)
{
	Thread* found = NULL;
	Thread* started = gThreadScheduler_Window;
	bool_t hasExecutableThreads = FALSE;
	while (TRUE)
	{
		if (gThreadScheduler_Window->Priority > 0 &&
			!gThreadScheduler_Window->Busy &&
			!gThreadScheduler_Window->Suspended &&
			gThreadScheduler_Window->SleepRemaining == 0)
		{
			hasExecutableThreads = TRUE;
			if (gThreadScheduler_Window->SliceRemaining)
			{
				found = gThreadScheduler_Window;
				break;
			}
		}
		gThreadScheduler_Window = gThreadScheduler_Window->Next;
		if (gThreadScheduler_Window == started) break;
	}
	if (!found && hasExecutableThreads)
	{
		while (TRUE)
		{
			if (gThreadScheduler_Window->Priority > 0 &&
				!gThreadScheduler_Window->Busy &&
				!gThreadScheduler_Window->Suspended &&
				gThreadScheduler_Window->SleepRemaining == 0)
			{
				// TODO: Implement better priority support, for now it gets (Priority * Initial Count) / 16 cycles
				gThreadScheduler_Window->SliceRemaining = (gThreadScheduler_Window->Priority * (*(size_t*)(pAPIC->BaseAddress + APIC__Register__Timer__InitialCount))) * 10;
				if (!found) found = gThreadScheduler_Window;
			}
			gThreadScheduler_Window = gThreadScheduler_Window->Next;
			if (gThreadScheduler_Window == started) break;
		}
	}
	if (found) gThreadScheduler_Window = found->Next;
	return found;
}

void ThreadScheduler_Schedule(InterruptRegisters* pRegisters, APIC* pAPIC)
{
	Atomic_AquireLock(&gThreadScheduler_Busy);
	uint32_t consumed = (*(size_t*)(pAPIC->BaseAddress + APIC__Register__Timer__InitialCount) - pAPIC->PreemptedTimerCount);
	pAPIC->PreemptedTimerCount = 0;
	bool_t switched = FALSE;

	if (!pAPIC->CurrentThread)
	{
		// Entering scheduling for first time with this processor, find a thread,
		// or give it the preallocated idle thread for the processor
		switched = TRUE;
		pAPIC->CurrentThread = ThreadScheduler_FindThread(pAPIC);
		if (!pAPIC->CurrentThread) pAPIC->CurrentThread = gThreadScheduler_IdleProcess->Threads[pAPIC->Index];
		Atomic_AquireLock(&pAPIC->CurrentThread->Busy);
		pAPIC->CurrentThread->CurrentAPIC = pAPIC;
	}
	else
	{
		Thread* started = gThreadScheduler_Window;
		while (TRUE)
		{
			if (gThreadScheduler_Window->SleepRemaining &&
				gThreadScheduler_Window->CurrentAPIC == pAPIC)
			{
				if (gThreadScheduler_Window->SleepRemaining < consumed)
				{
					gThreadScheduler_Window->SleepRemaining = 0;
					gThreadScheduler_Window->CurrentAPIC = NULL;
				}
				else gThreadScheduler_Window->SleepRemaining -= consumed;
			}
			gThreadScheduler_Window = gThreadScheduler_Window->Next;
			if (gThreadScheduler_Window == started) break;
		}
		if (pAPIC->CurrentThread->Priority > 0)
		{
			// Have a thread, that is executable (not an idle thread), so adjust it's slice remaining
			if (pAPIC->CurrentThread->SliceRemaining < consumed) pAPIC->CurrentThread->SliceRemaining = 0;
			else pAPIC->CurrentThread->SliceRemaining -= consumed;
		}

		Thread* newThread = ThreadScheduler_FindThread(pAPIC);
		if (!newThread) newThread = gThreadScheduler_IdleProcess->Threads[pAPIC->Index];
		if (newThread != pAPIC->CurrentThread)
		{
			switched = TRUE;
			Atomic_AquireLock(&newThread->Busy);

			if (pAPIC->CurrentThread)
			{
				//pAPIC->CurrentThread->SavedFPUState = 
				if (!pAPIC->CurrentThread->SleepRemaining) pAPIC->CurrentThread->CurrentAPIC = NULL;
				pAPIC->CurrentThread->SavedRegisterState = *pRegisters;
				Atomic_ReleaseLock(&pAPIC->CurrentThread->Busy);
			}

			newThread->CurrentAPIC = pAPIC;
			pAPIC->CurrentThread = newThread;
		}
	}

	if (switched)
	{
		*(size_t*)(pRegisters->esp - 36) = pAPIC->CurrentThread->SavedRegisterState.ds;
		*(size_t*)(pRegisters->esp - 32) = pAPIC->CurrentThread->SavedRegisterState.edi;
		*(size_t*)(pRegisters->esp - 28) = pAPIC->CurrentThread->SavedRegisterState.esi;
		*(size_t*)(pRegisters->esp - 24) = pAPIC->CurrentThread->SavedRegisterState.ebp;
		*(size_t*)(pRegisters->esp - 16) = pAPIC->CurrentThread->SavedRegisterState.ebx;
		*(size_t*)(pRegisters->esp - 12) = pAPIC->CurrentThread->SavedRegisterState.edx;
		*(size_t*)(pRegisters->esp - 8) = pAPIC->CurrentThread->SavedRegisterState.ecx;
		*(size_t*)(pRegisters->esp - 4) = pAPIC->CurrentThread->SavedRegisterState.eax;
		*(size_t*)(pRegisters->esp + 8) = pAPIC->CurrentThread->SavedRegisterState.eip;
		*(size_t*)(pRegisters->esp + 12) = pAPIC->CurrentThread->SavedRegisterState.cs;
		*(size_t*)(pRegisters->esp + 16) = pAPIC->CurrentThread->SavedRegisterState.eflags;
		*(size_t*)(pRegisters->esp + 20) = pAPIC->CurrentThread->SavedRegisterState.useresp;
		*(size_t*)(pRegisters->esp + 24) = pAPIC->CurrentThread->SavedRegisterState.ss;
	}
	Atomic_ReleaseLock(&gThreadScheduler_Busy);
}
