#include "GDT.h"
#include "IDT.h"
#include "MSR.h"
#include "../PIT.h"
#include "Processor.h"
#include "TSS.h"

extern "C" {
uint64_t MSRRead(uint32_t pRegister);
void MSRWrite(uint32_t pRegister, uint32_t pValue);

void ProcessorBusFrequencyTimer(IDT::InterruptRegisters pRegisters)
{
	uint32_t taskRegister = TSSGetRegister();
	uint16_t processorIndex = (taskRegister - 0x28) >> 3;
	Processor* processor = Processor::GetProcessor(processorIndex);
	*reinterpret_cast<uint32_t*>(processor->GetBaseAddress() + Processor::LAPIC_REGISTER_ENDOFINTERRUPT) = 0;
}

void ProcessorThreadSchedulerTimer(IDT::InterruptRegisters pRegisters)
{
	uint32_t taskRegister = TSSGetRegister();
	uint16_t processorIndex = (taskRegister - 0x28) >> 3;
	Processor* processor = Processor::GetProcessor(processorIndex);
	*reinterpret_cast<uint32_t*>(processor->GetBaseAddress() + Processor::LAPIC_REGISTER_ENDOFINTERRUPT) = 0;
}
}


#define PROCESSOR__Max						250

Processor* Processor::sProcessors[PROCESSOR__Max];
uint16_t Processor::sProcessorCount = 0;

Processor* Processor::GetProcessor(uint16_t pProcessor)
{
	return sProcessors[pProcessor];
}

Processor::Processor()
{
	mIndex = sProcessorCount;
	mCurrentThread = nullptr;
	sProcessors[sProcessorCount++] = this;
	
	mBaseAddress = MSRRead(MSR::REGISTER_LAPIC) & MSR::LAPIC_ADDRESSMASK;
	MSRWrite(MSR::REGISTER_LAPIC, mBaseAddress | MSR::LAPIC_ENABLE);

	if (sProcessorCount == 1)
	{
		IDT::RegisterCallback(0xFD, ProcessorBusFrequencyTimer);
		IDT::SetEmptyInterrupt(0xFE);
		IDT::RegisterCallback(0xFF, ProcessorThreadSchedulerTimer);
	}

	GDT::SetTSSStackAndRegister(mIndex, reinterpret_cast<uint32_t>(&mStack[0] + STACK_SIZE));

	*reinterpret_cast<uint32_t*>(mBaseAddress + LAPIC_REGISTER_SPURIOUSINTERRUPTVECTOR) = 0xFE | LAPIC_FLAGS_SOFTWAREENABLE;
	*reinterpret_cast<uint32_t*>(mBaseAddress + LAPIC_REGISTER_LVT_TIMER) = 0xFD;
	*reinterpret_cast<uint32_t*>(mBaseAddress + LAPIC_REGISTER_TIMER_DIVISOR) = 0x03;

	mBusFrequency = PIT::CalculateProcessorBus(this);

	uint32_t divisor = mBusFrequency / 16 / LAPIC_TIMER_CYCLEHERTZ;

	*reinterpret_cast<uint32_t*>(mBaseAddress + LAPIC_REGISTER_TIMER_INITIALCOUNT) = divisor < 16 ? 16 : divisor;
	*reinterpret_cast<uint32_t*>(mBaseAddress + LAPIC_REGISTER_LVT_TIMER) = 0xFF | LAPIC_TIMER_PERIODIC;
	*reinterpret_cast<uint32_t*>(mBaseAddress + LAPIC_REGISTER_TIMER_DIVISOR) = 0x03;
}

Processor::~Processor()
{
	sProcessors[mIndex] = nullptr;
}

void Processor::SendInterruptCommand(uint32_t pLowRegister, uint32_t pHighRegister)
{
	*reinterpret_cast<uint32_t*>(mBaseAddress + LAPIC_REGISTER_INTERRUPTCOMMANDHIGH) = pHighRegister;
	*reinterpret_cast<uint32_t*>(mBaseAddress + LAPIC_REGISTER_INTERRUPTCOMMANDLOW) = pLowRegister;
	while ((*reinterpret_cast<uint32_t*>(mBaseAddress + LAPIC_REGISTER_INTERRUPTCOMMANDLOW) & (1 << 12))) ;
}

