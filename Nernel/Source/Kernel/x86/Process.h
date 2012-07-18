#pragma once

class Thread;

class Process
{
public:
	static uint8_t sBusy;
	static Process* sProcesses[];
	static uint32_t sNextIdentifier;

	Thread** Threads;
	uint32_t ThreadCount;
	uint16_t Identifier;

	Process(size_t pEntryPoint, size_t pThreadStackSize, uint8_t pPriority = 2);
	~Process();
};
