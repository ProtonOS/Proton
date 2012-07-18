#include "../Atomics.h"
#include "Process.h"
#include "Processor.h"
#include "Thread.h"
#include "ThreadScheduler.h"

namespace ThreadScheduler
{
	bool sRunning = false;
	Process* sKernelProcess = nullptr;
	uint8_t sBusy = 0;
	Thread* sWindow = nullptr;
	Thread* sSleepingWindow = nullptr;

	void Startup(uint32_t pKernelThreadStartAddress, size_t pKernelThreadStackSize)
	{
		sKernelProcess = new Process(pKernelThreadStartAddress, pKernelThreadStackSize);
		sRunning = true;
	}

	void Add(Thread* pThread)
	{
		AtomicAquireLock(&sBusy);
		if (!sWindow)
		{
			sWindow = pThread;
			pThread->Next = pThread;
			pThread->Prev = pThread;
		}
		else
		{
			pThread->Prev = sWindow;
			pThread->Next = sWindow->Next;
			sWindow->Next->Prev = pThread;
			sWindow->Next = pThread;
		}
		AtomicReleaseLock(&sBusy);
	}

	void Remove(Thread* pThread)
	{
		AtomicAquireLock(&sBusy);
		pThread->Prev->Next = pThread->Next;
		pThread->Next->Prev = pThread->Prev;
		if (pThread == sWindow)
		{
			sWindow = sWindow->Next;
			if (sWindow == pThread) sWindow = NULL;
		}
		AtomicReleaseLock(&sBusy);
	}

	void Schedule(Processor* pProcessor, IDT::InterruptRegisters* pRegisters)
	{
		if (!sRunning) return;

		AtomicAquireLock(&sBusy);
		uint32_t consumed = (*(size_t*)(pProcessor->BaseAddress + Processor::LAPIC_REGISTER_TIMER_INITIALCOUNT) - pProcessor->PreemptedTimerCount);
		if (pProcessor->PreemptedTimerCount)
		{
			pProcessor->PreemptedTimerCount = 0;
		}
		if (pProcessor->CurrentThread && !pProcessor->Sleeping)
		{
			if (pProcessor->CurrentThread->SleepRemaining)
			{
				pProcessor->CurrentThread->SliceRemaining = 0;

				pProcessor->CurrentThread->Prev->Next = pProcessor->CurrentThread->Next;
				pProcessor->CurrentThread->Next->Prev = pProcessor->CurrentThread->Prev;
				if (pProcessor->CurrentThread == sWindow)
				{
					sWindow = sWindow->Next;
					if (sWindow == pProcessor->CurrentThread) sWindow = nullptr;
				}

				if (!sSleepingWindow)
				{
					sSleepingWindow = pProcessor->CurrentThread;
					pProcessor->CurrentThread->SleepingNext = pProcessor->CurrentThread;
					pProcessor->CurrentThread->SleepingPrev = pProcessor->CurrentThread;
				}
				else
				{
					pProcessor->CurrentThread->SleepingPrev = sSleepingWindow;
					pProcessor->CurrentThread->SleepingNext = sSleepingWindow->SleepingNext;
					sSleepingWindow->SleepingNext->SleepingPrev = pProcessor->CurrentThread;
					sSleepingWindow->SleepingNext = pProcessor->CurrentThread;
				}
			}
			else
			{
				if (pProcessor->CurrentThread->SliceRemaining < consumed)
				{
					pProcessor->CurrentThread->SliceRemaining = 0;
				}
				else
				{
					pProcessor->CurrentThread->SliceRemaining -= consumed;
				}
				if (pProcessor->CurrentThread->SliceRemaining)
				{
					AtomicReleaseLock(&sBusy);
					return;
				}
				pProcessor->CurrentThread->CurrentProcessor = nullptr;
			}
		}
		bool firstRetry = true;
		Thread* found = NULL;
		Thread* firstThread = sSleepingWindow;

		if (pProcessor->CurrentThread)
		{
			if (sSleepingWindow)
			{
				while (true)
				{
					if (sSleepingWindow->SleepRemaining && sSleepingWindow->CurrentProcessor == pProcessor)
					{
						if (sSleepingWindow->SleepRemaining < consumed)
						{
							sSleepingWindow->SleepRemaining = 0;
						}
						else
						{
							sSleepingWindow->SleepRemaining -= consumed;
						}
						if (!sSleepingWindow->SleepRemaining)
						{
							pProcessor->CurrentThread->SleepingPrev->SleepingNext = pProcessor->CurrentThread->SleepingNext;
							pProcessor->CurrentThread->SleepingNext->SleepingPrev = pProcessor->CurrentThread->SleepingPrev;
							if (pProcessor->CurrentThread == sSleepingWindow)
							{
								sSleepingWindow = sSleepingWindow->SleepingNext;
								if (sSleepingWindow == pProcessor->CurrentThread) sSleepingWindow = nullptr;
							}

							if (!sWindow)
							{
								sWindow = pProcessor->CurrentThread;
								sWindow->Next = sWindow;
								sWindow->Prev = sWindow;
							}
							else
							{
								pProcessor->CurrentThread->Prev = sWindow;
								pProcessor->CurrentThread->Next = sWindow->Next;
								sWindow->Next->Prev = pProcessor->CurrentThread;
								sWindow->Next = pProcessor->CurrentThread;
								sWindow = sWindow->Next;
							}

							sWindow->SliceRemaining = sWindow->Priority * (*(size_t*)(pProcessor->BaseAddress + Processor::LAPIC_REGISTER_TIMER_INITIALCOUNT));
							AtomicReleaseLock(&sWindow->Busy);
						}
					}
					if (sSleepingWindow)
					{
						sSleepingWindow = sSleepingWindow->SleepingNext;
					}
					if (!sSleepingWindow || sSleepingWindow == firstThread) break;
				}
			}
		}
		if (!sWindow)
		{
			pProcessor->Sleeping = true;
			AtomicReleaseLock(&sBusy);
			return;
		}
		pProcessor->Sleeping = false;
	
	Retry:
		found = nullptr;
		firstThread = sWindow;
		while (!found)
		{
			if (!sWindow->Busy && sWindow->SliceRemaining)
			{
				found = sWindow;
				break;
			}
			sWindow = sWindow->Next;
			if (sWindow == firstThread) break;
		}
		if (firstRetry && !found)
		{
			firstThread = sWindow;
			do
			{
				if (!sWindow->Busy || !sWindow->SliceRemaining)
				{
					sWindow->SliceRemaining = sWindow->Priority * (*(size_t*)(pProcessor->BaseAddress + Processor::LAPIC_REGISTER_TIMER_INITIALCOUNT));
				}
				sWindow = sWindow->Next;
			} while (firstThread != sWindow);
			firstRetry = false;
			goto Retry;
		}
		else if (!firstRetry && !found)
		{
			AtomicReleaseLock(&sBusy);
			// Release lock and return, whether we had a thread or not
			// We continue processing it if we had a thread when we return, as we had no other threads
			// to process, otherwise if we had no thread then we end up waiting until the next tick to
			// look for one again, which puts the processor back into the empty while loop it entered from
		}
		else if (found)
		{
			AtomicAquireLock(&found->Busy);
			AtomicReleaseLock(&sBusy);
			if (pProcessor->CurrentThread)
			{
				//pAPIC->CurrentThread->SavedFPUState = 
				pProcessor->CurrentThread->SavedRegisterState = *pRegisters;
				AtomicReleaseLock(&pProcessor->CurrentThread->Busy);
			}
			*(size_t*)(pRegisters->esp - 36) = found->SavedRegisterState.ds;
			*(size_t*)(pRegisters->esp - 32) = found->SavedRegisterState.edi;
			*(size_t*)(pRegisters->esp - 28) = found->SavedRegisterState.esi;
			*(size_t*)(pRegisters->esp - 24) = found->SavedRegisterState.ebp;
			*(size_t*)(pRegisters->esp - 16) = found->SavedRegisterState.ebx;
			*(size_t*)(pRegisters->esp - 12) = found->SavedRegisterState.edx;
			*(size_t*)(pRegisters->esp - 8) = found->SavedRegisterState.ecx;
			*(size_t*)(pRegisters->esp - 4) = found->SavedRegisterState.eax;
			*(size_t*)(pRegisters->esp + 8) = found->SavedRegisterState.eip;
			*(size_t*)(pRegisters->esp + 12) = found->SavedRegisterState.cs;
			*(size_t*)(pRegisters->esp + 16) = found->SavedRegisterState.eflags;
			*(size_t*)(pRegisters->esp + 20) = found->SavedRegisterState.useresp;
			*(size_t*)(pRegisters->esp + 24) = found->SavedRegisterState.ss;

			found->CurrentProcessor = pProcessor;
			pProcessor->CurrentThread = found;
		}
		else
		{
			Panic("ThreadScheduler Logic Failure!");
		}
	}
};
