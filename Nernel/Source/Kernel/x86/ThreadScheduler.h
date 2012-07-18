#pragma once

#include "IDT.h"

class Processor;

namespace ThreadScheduler
{
	void Startup(uint32_t pKernelThreadStartAddress, size_t pKernelThreadStackSize);
	void Add(Thread* pThread);
	void Remove(Thread* pThread);
	void Schedule(Processor* pProcessor, IDT::InterruptRegisters* pRegisters);
};