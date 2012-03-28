#include "APIC.h"
#include "Atomics.h"
#include "Common.h"
#include "Process.h"
#include "SystemClock.h"
#include "ThreadScheduler.h"

uint8_t gThreadScheduler_Busy = 0;
Thread* gThreadScheduler_Window = NULL;
Process* gThreadScheduler_KernelProcess = NULL;

void ThreadScheduler_Timer(InterruptRegisters pRegisters)
{
	APIC* apic = gAPIC_Array[(pRegisters.int_no - 128) >> 1];
	if (gThreadScheduler_Window)
	{
		ThreadScheduler_Schedule(&pRegisters, apic);
		//printf("Done Scheduling\n");
	}
	if ((++apic->TickCount % APIC__Timer__CycleHertz) == 0) printf("Tick\n");
	*(size_t*)(apic->BaseAddress + APIC__Register__EndOfInterrupt) = 0;
}

void ThreadScheduler_Startup(size_t pEntryPoint, size_t pThreadStackSize)
{
	gThreadScheduler_KernelProcess = Process_Create(pEntryPoint, pThreadStackSize);
}

void ThreadScheduler_Shutdown()
{
}

void ThreadScheduler_Add(Thread* pThread)
{
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
}

void ThreadScheduler_Remove(Thread* pThread)
{
	Atomic_AquireLock(&gThreadScheduler_Busy);
	pThread->Prev->Next = pThread->Next;
	pThread->Next->Prev = pThread->Prev;
	if (pThread == gThreadScheduler_Window)
	{
		gThreadScheduler_Window = gThreadScheduler_Window->Next;
		if (gThreadScheduler_Window == pThread) gThreadScheduler_Window = NULL;
	}
	Atomic_ReleaseLock(&gThreadScheduler_Busy);
}

void ThreadScheduler_Schedule(InterruptRegisters* pRegisters, APIC* pAPIC)
{
	Atomic_AquireLock(&gThreadScheduler_Busy);
	//printf("ThreadScheduler AquireLock\n");
	if (pAPIC->CurrentThread)
	{
		pAPIC->CurrentThread->TimeConsumed += (1000 / APIC__Timer__CycleHertz);
		if (pAPIC->CurrentThread->TimeConsumed < pAPIC->CurrentThread->Priority * (1000 / APIC__Timer__CycleHertz))
		{
			//printf("ThreadScheduler ReleaseLock: Another Slice\n");
			Atomic_ReleaseLock(&gThreadScheduler_Busy);
			return;
		}
	}
	bool_t firstRetry = TRUE;
	Thread* found = NULL;
	Thread* firstThread = gThreadScheduler_Window;
Retry:
	found = NULL;
	firstThread = gThreadScheduler_Window;
	while (!found)
	{
		if (!gThreadScheduler_Window->Busy && gThreadScheduler_Window->TimeConsumed < gThreadScheduler_Window->Priority * (1000 / APIC__Timer__CycleHertz))
		{
			found = gThreadScheduler_Window;
			break;
		}
		gThreadScheduler_Window = gThreadScheduler_Window->Next;
		if (gThreadScheduler_Window == firstThread) break;
	}
	if (firstRetry && !found)
	{
		firstThread = gThreadScheduler_Window;
		//printf("ThreadScheduler Resetting TimeConsumed\n");
		while (firstThread != gThreadScheduler_Window || gThreadScheduler_Window->TimeConsumed)
		{
			gThreadScheduler_Window->TimeConsumed = 0;
			gThreadScheduler_Window = gThreadScheduler_Window->Next;
		}
		firstRetry = FALSE;
		goto Retry;
	}
	else if (!firstRetry && !found)
	{
		//printf("ThreadScheduler ReleaseLock: All Busy, Another Slice\n");
		Atomic_ReleaseLock(&gThreadScheduler_Busy);

		/*
		Atomic_ReleaseLock(&gThreadScheduler_Busy);
		for (uint32_t count = 0; count < 100; ++count)
		{
			IOWAIT();
		}
		Atomic_AquireLock(&gThreadScheduler_Busy);
		//printf("ThreadScheduler AquireLock: Waking\n");
		firstRetry = TRUE;
		goto Retry;
		*/
	}
	else if (found)
	{
		Atomic_AquireLock(&found->Busy);
		//printf("Thread AquireLock: Found\n");
		Atomic_ReleaseLock(&gThreadScheduler_Busy);
		//printf("ThreadScheduler ReleaseLock: Found\n");
		if (pAPIC->CurrentThread)
		{
			pAPIC->CurrentThread->SavedRegisterState = *pRegisters;
			Atomic_ReleaseLock(&pAPIC->CurrentThread->Busy);
			//printf("Thread ReleaseLock: CurrentThread\n");
		}
		*(size_t*)(pRegisters->esp + 4) = found->SavedRegisterState.eip;
		*pRegisters = found->SavedRegisterState;
		pAPIC->CurrentThread = found;
	}
	else
	{
		Panic("ThreadScheduler Logic Failure!");
	}
}
