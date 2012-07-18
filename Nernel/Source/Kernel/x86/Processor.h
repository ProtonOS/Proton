#pragma once

class Thread;

class Processor
{
private:
	static const uint32_t STACK_SIZE = 1024 * 128;
	uint16_t mIndex;
	Thread* mCurrentThread;
	uint32_t mBaseAddress;
	uint32_t mBusFrequency;
	uint8_t mStack[STACK_SIZE];

	static Processor* sProcessors[];
	static uint16_t sProcessorCount;

public:
	static const uint16_t LAPIC_REGISTER_APICID = 0x020;
	static const uint16_t LAPIC_REGISTER_APICVERSION = 0x030;
	static const uint16_t LAPIC_REGISTER_TASKPRIORITY = 0x080;
	static const uint16_t LAPIC_REGISTER_ENDOFINTERRUPT = 0x0B0;
	static const uint16_t LAPIC_REGISTER_LOGICALDESTINATION = 0x0D0;
	static const uint16_t LAPIC_REGISTER_DESTINATIONFORMAT = 0x0E0;
	static const uint16_t LAPIC_REGISTER_SPURIOUSINTERRUPTVECTOR = 0x0F0;
	static const uint16_t LAPIC_REGISTER_ERRORSTATUS = 0x280;
	static const uint16_t LAPIC_REGISTER_INTERRUPTCOMMANDLOW = 0x300;
	static const uint16_t LAPIC_REGISTER_INTERRUPTCOMMANDHIGH = 0x310;
	// LVT = Local Vector Table
	static const uint16_t LAPIC_REGISTER_LVT_TIMER = 0x320;
	static const uint16_t LAPIC_REGISTER_LVT_PERFORMANCECOUNTER = 0x340;
	static const uint16_t LAPIC_REGISTER_LVT_LOCALINTERRUPT0 = 0x350;
	static const uint16_t LAPIC_REGISTER_LVT_LOCALINTERRUPT1 = 0x360;
	static const uint16_t LAPIC_REGISTER_LVT_ERRORVECTOR = 0x370;
	static const uint16_t LAPIC_REGISTER_TIMER_INITIALCOUNT = 0x380;
	static const uint16_t LAPIC_REGISTER_TIMER_CURRENTCOUNT = 0x390;
	static const uint16_t LAPIC_REGISTER_TIMER_DIVISOR = 0x3E0;

	static const uint32_t LAPIC_FLAGS_SOFTWAREENABLE = 0x100;
	static const uint32_t LAPIC_FLAGS_CPUFOCUS = 0x200;
	static const uint32_t LAPIC_FLAGS_NONMASKABLEINTERRUPT = 0x400;
	static const uint32_t LAPIC_FLAGS_DISABLE = 0x10000;

	static const uint32_t LAPIC_TIMER_PERIODIC = 0x20000;
	static const uint32_t LAPIC_TIMER_BASEDIVISOR = 0x100000;

	static const uint32_t LAPIC_TIMER_CYCLEHERTZ = 100;


	static Processor* GetProcessor(uint16_t pProcessor);

	Processor();
	~Processor();

	bool IsBootstrap() const { return mIndex == 0; }
	uint16_t GetIndex() const { return mIndex; }
	Thread* GetCurrentThread() const { return mCurrentThread; }
	uint32_t GetBaseAddress() const { return mBaseAddress; }

	void SendInterruptCommand(uint32_t pLowRegister, uint32_t pHighRegister);
};
