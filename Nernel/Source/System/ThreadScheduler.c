#include <System/APIC.h>
#include <System/Atomics.h>
#include <Common.h>
#include <System/Process.h>
#include <System/x86/Registers.h>
#include <System/SystemClock.h>
#include <System/ThreadScheduler.h>

extern bool_t gMultitasking;

uint8_t gThreadScheduler_Busy = 0;
Thread* gThreadScheduler_Window = NULL;
Thread* gThreadScheduler_SleepingWindow = NULL;
Process* gThreadScheduler_KernelProcess = NULL;

void ThreadScheduler_Timer(InterruptRegisters pRegisters)
{
	APIC* apic = gAPIC_Array[(pRegisters.int_no - 128) >> 1];
	if (gThreadScheduler_Window || gThreadScheduler_SleepingWindow)
	{
		ThreadScheduler_Schedule(&pRegisters, apic);
		gMultitasking = TRUE;
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
	uint32_t consumed = (*(size_t*)(pAPIC->BaseAddress + APIC__Register__Timer__InitialCount) - pAPIC->PreemptedTimerCount);
	if (pAPIC->PreemptedTimerCount)
	{
		printf("Preempted Timer @ 0x%x of 0x%x, consumed %u\n", (unsigned int)pAPIC->PreemptedTimerCount, (unsigned int)*(size_t*)(pAPIC->BaseAddress + APIC__Register__Timer__InitialCount), (unsigned int)consumed);
		pAPIC->PreemptedTimerCount = 0;
	}
	if (pAPIC->CurrentThread && !pAPIC->Sleeping)
	{
		//printf("GotHere3\n");
		if (pAPIC->CurrentThread->SleepRemaining)
		{
			pAPIC->CurrentThread->SliceRemaining = 0;

			pAPIC->CurrentThread->Prev->Next = pAPIC->CurrentThread->Next;
			pAPIC->CurrentThread->Next->Prev = pAPIC->CurrentThread->Prev;
			if (pAPIC->CurrentThread == gThreadScheduler_Window)
			{
				gThreadScheduler_Window = gThreadScheduler_Window->Next;
				if (gThreadScheduler_Window == pAPIC->CurrentThread) gThreadScheduler_Window = NULL;
			}

			if (!gThreadScheduler_SleepingWindow)
			{
				gThreadScheduler_SleepingWindow = pAPIC->CurrentThread;
				pAPIC->CurrentThread->SleepingNext = pAPIC->CurrentThread;
				pAPIC->CurrentThread->SleepingPrev = pAPIC->CurrentThread;
			}
			else
			{
				pAPIC->CurrentThread->SleepingPrev = gThreadScheduler_SleepingWindow;
				pAPIC->CurrentThread->SleepingNext = gThreadScheduler_SleepingWindow->SleepingNext;
				gThreadScheduler_Window->SleepingNext->SleepingPrev = pAPIC->CurrentThread;
				gThreadScheduler_Window->SleepingNext = pAPIC->CurrentThread;
			}
		}
		else
		{
			if (pAPIC->CurrentThread->SliceRemaining < consumed)
			{
				pAPIC->CurrentThread->SliceRemaining = 0;
			}
			else
			{
				pAPIC->CurrentThread->SliceRemaining -= consumed;
			}
			if (pAPIC->CurrentThread->SliceRemaining)
			{
				Atomic_ReleaseLock(&gThreadScheduler_Busy);
				//printf("GotHere1\n");
				return;
			}
			//printf("GotHere2\n");
			pAPIC->CurrentThread->CurrentAPIC = NULL;
		}
	}
	bool_t firstRetry = TRUE;
	Thread* found = NULL;
	Thread* firstThread = gThreadScheduler_SleepingWindow;

	if (pAPIC->CurrentThread)
	{
		//printf("GotHere4\n");
		if (gThreadScheduler_SleepingWindow)
		{
			while (TRUE)
			{
				if (gThreadScheduler_SleepingWindow->SleepRemaining && gThreadScheduler_SleepingWindow->CurrentAPIC == pAPIC)
				{
					if (gThreadScheduler_SleepingWindow->SleepRemaining < consumed)
					{
						gThreadScheduler_SleepingWindow->SleepRemaining = 0;
					}
					else
					{
						gThreadScheduler_SleepingWindow->SleepRemaining -= consumed;
					}
					if (!gThreadScheduler_SleepingWindow->SleepRemaining)
					{
						pAPIC->CurrentThread->SleepingPrev->SleepingNext = pAPIC->CurrentThread->SleepingNext;
						pAPIC->CurrentThread->SleepingNext->SleepingPrev = pAPIC->CurrentThread->SleepingPrev;
						if (pAPIC->CurrentThread == gThreadScheduler_SleepingWindow)
						{
							gThreadScheduler_SleepingWindow = gThreadScheduler_SleepingWindow->SleepingNext;
							if (gThreadScheduler_SleepingWindow == pAPIC->CurrentThread) gThreadScheduler_SleepingWindow = NULL;
						}

						if (!gThreadScheduler_Window)
						{
							//printf("GotHere12\n");
							gThreadScheduler_Window = pAPIC->CurrentThread;
							gThreadScheduler_Window->Next = gThreadScheduler_Window;
							gThreadScheduler_Window->Prev = gThreadScheduler_Window;
						}
						else
						{
							pAPIC->CurrentThread->Prev = gThreadScheduler_Window;
							pAPIC->CurrentThread->Next = gThreadScheduler_Window->Next;
							gThreadScheduler_Window->Next->Prev = pAPIC->CurrentThread;
							gThreadScheduler_Window->Next = pAPIC->CurrentThread;
							gThreadScheduler_Window = gThreadScheduler_Window->Next;
						}

						gThreadScheduler_Window->SliceRemaining = gThreadScheduler_Window->Priority * (*(size_t*)(pAPIC->BaseAddress + APIC__Register__Timer__InitialCount));
						Atomic_ReleaseLock(&gThreadScheduler_Window->Busy);
						//printf("gThreadScheduler_Window->SliceRemaining: %i\n", (int)gThreadScheduler_Window->SliceRemaining);
					}
				}
				if (gThreadScheduler_SleepingWindow)
				{
					gThreadScheduler_SleepingWindow = gThreadScheduler_SleepingWindow->SleepingNext;
				}
				if (!gThreadScheduler_SleepingWindow || gThreadScheduler_SleepingWindow == firstThread) break;
			}
		}
	}
	if (!gThreadScheduler_Window)
	{
		//printf("GotHere5\n");
		pAPIC->Sleeping = TRUE;
		Atomic_ReleaseLock(&gThreadScheduler_Busy);
		return;
	}
	pAPIC->Sleeping = FALSE;
	
Retry:
	found = NULL;
	firstThread = gThreadScheduler_Window;
	while (!found)
	{
		//printf("GotHere6\n");
		if (!gThreadScheduler_Window->Busy && gThreadScheduler_Window->SliceRemaining)
		{
			//printf("GotHere9\n");
			found = gThreadScheduler_Window;
			break;
		}
		gThreadScheduler_Window = gThreadScheduler_Window->Next;
		if (gThreadScheduler_Window == firstThread) break;
	}
	if (firstRetry && !found)
	{
		firstThread = gThreadScheduler_Window;
		do
		{
			if (!gThreadScheduler_Window->Busy || !gThreadScheduler_Window->SliceRemaining)
			{
				//printf("GotHere7\n");
				gThreadScheduler_Window->SliceRemaining = gThreadScheduler_Window->Priority * (*(size_t*)(pAPIC->BaseAddress + APIC__Register__Timer__InitialCount));
			}
			gThreadScheduler_Window = gThreadScheduler_Window->Next;
		} while (firstThread != gThreadScheduler_Window);
		firstRetry = FALSE;
		goto Retry;
	}
	else if (!firstRetry && !found)
	{
		//printf("GotHere8\n");
		Atomic_ReleaseLock(&gThreadScheduler_Busy);
		Panic("Need to handle more cores available than threads to process");
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
		Atomic_ReleaseLock(&gThreadScheduler_Busy);
		if (pAPIC->CurrentThread)
		{
			pAPIC->CurrentThread->SavedRegisterState = *pRegisters;
			Atomic_ReleaseLock(&pAPIC->CurrentThread->Busy);
		}
		//printf("ThreadScheduler: eip = 0x%x, useresp = 0x%x, ss = 0x%x\n", (unsigned int)*(size_t*)(esp + 8), (unsigned int)*(size_t*)(esp + 20), (unsigned int)*(size_t*)(esp + 24));
		*(size_t*)(pRegisters->esp - 36) = found->SavedRegisterState.ds;
		*(size_t*)(pRegisters->esp - 32) = found->SavedRegisterState.edi;
		*(size_t*)(pRegisters->esp - 28) = found->SavedRegisterState.esi;
		*(size_t*)(pRegisters->esp - 24) = found->SavedRegisterState.ebp;
		//*(size_t*)(pRegisters->esp - 20) = found->SavedRegisterState.esp;
		*(size_t*)(pRegisters->esp - 16) = found->SavedRegisterState.ebx;
		*(size_t*)(pRegisters->esp - 12) = found->SavedRegisterState.edx;
		*(size_t*)(pRegisters->esp - 8) = found->SavedRegisterState.ecx;
		*(size_t*)(pRegisters->esp - 4) = found->SavedRegisterState.eax;
		*(size_t*)(pRegisters->esp + 8) = found->SavedRegisterState.eip;
		*(size_t*)(pRegisters->esp + 12) = found->SavedRegisterState.cs;
		*(size_t*)(pRegisters->esp + 16) = found->SavedRegisterState.eflags;
		*(size_t*)(pRegisters->esp + 20) = found->SavedRegisterState.useresp;
		*(size_t*)(pRegisters->esp + 24) = found->SavedRegisterState.ss;
		//*pRegisters = found->SavedRegisterState;
		//printf("ThreadScheduler: eip = 0x%x, useresp = 0x%x, ss = 0x%x\n", (unsigned int)*(size_t*)(esp + 8), (unsigned int)*(size_t*)(esp + 20), (unsigned int)*(size_t*)(esp + 24));
		pAPIC->CurrentThread = found;
		found->CurrentAPIC = pAPIC;
		//printf("GotHere10\n");
		//printf("ThreadScheduler: Found, found: 0x%x ESP: 0x%x EBP: 0x%x EAX: 0x%x EBX: 0x%x ECX: 0x%x EDX: 0x%x SS: 0x%x\n", (unsigned int)found, (unsigned int)found->SavedRegisterState.esp, (unsigned int)found->SavedRegisterState.ebp, (unsigned int)found->SavedRegisterState.eax, (unsigned int)found->SavedRegisterState.ebx, (unsigned int)found->SavedRegisterState.ecx, (unsigned int)found->SavedRegisterState.edx, (unsigned int)found->SavedRegisterState.ss);
		
	}
	else
	{
		Panic("ThreadScheduler Logic Failure!");
	}
}
