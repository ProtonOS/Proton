#pragma once

class Processor;

namespace PIT
{
	void Startup();

	uint32_t GetSecondsElapsed();
	uint32_t GetMillisecondsElapsed();

	uint32_t CalculateProcessorBus(Processor* pProcessor);
};
