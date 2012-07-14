#include "Processor.h"

#define PROCESSOR__Max						250

Processor* Processor::sProcessors[PROCESSOR__Max];
uint16_t Processor::sProcessorCount = 0;

Processor* Processor::GetProcessor(uint16_t pProcessor)
{
	return sProcessors[pProcessor];
}

Processor::Processor()
{
	mCurrentThread = nullptr;
	mIndex = sProcessorCount;
	sProcessors[sProcessorCount++] = this;
}

Processor::~Processor()
{
	sProcessors[mIndex] = nullptr;
}
