#pragma once

class Thread;

class Processor
{
private:
	uint16_t mIndex;
	Thread* mCurrentThread;

	static Processor* sProcessors[];
	static uint16_t sProcessorCount;

public:
	static Processor* GetProcessor(uint16_t pProcessor);

	Processor();
	~Processor();

	uint8_t GetIndex() const { return mIndex; }
	Thread* GetCurrentThread() const { return mCurrentThread; }
};
