#pragma once

#include "Process.h"

#include <reent.h>

class Process;

class Thread
{
private:
	Process* mProcess;
	Thread* mNext;
	Thread* mPrev;
	Thread* mSleepingNext;
	Thread* mSleepingPrev;
	uint8_t* mStack;
	uint32_t mStackSize;
	size_t mEntryPoint;
//	APIC* mCurrentAPIC;
	uint8_t mPriority;
	uint32_t mSliceRemaining;
	uint64_t mSleepRemaining;
	uint8_t mBusy;
//	InterruptRegisters mSavedRegisterState;
	uint32_t mMallocLockDepth;
	struct _reent mReentrant;

public:
	Thread(Process* pProcess, size_t pEntryPoint, size_t pStackSize, uint8_t pPriority);
	~Thread();

	void IncrementMallocLockDepth() { ++mMallocLockDepth; }
	bool DecrementMallocLockDepth() { if (mMallocLockDepth == 0) return true; return (--mMallocLockDepth) == 0; }
	struct _reent* GetReentrant() { return &mReentrant; }
};
