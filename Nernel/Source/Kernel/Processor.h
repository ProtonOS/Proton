#pragma once

class Thread;

class Processor
{
private:
	uint8_t mIndex;
	Thread* mCurrentThread;

	static Processor* sProcessors[];
	static uint8_t sProcessorCount;

public:
	static Processor* GetProcessor(uint8_t pProcessor);

	Processor();
	~Processor();

	uint8_t GetIndex() const { return mIndex; }
	Thread* GetCurrentThread() const { return mCurrentThread; }
};
