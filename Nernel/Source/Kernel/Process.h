#pragma once

class Thread;

class Process
{
private:
	static uint8_t sBusy;
	static Process* sProcesses[];
	static uint32_t sNextIdentifier;

	Thread** mThreads;
	uint32_t mThreadCount;
	uint16_t mIdentifier;

public:
	Process(size_t pEntryPoint, size_t pThreadStackSize, uint8_t pPriority = 2);
	~Process();
};
