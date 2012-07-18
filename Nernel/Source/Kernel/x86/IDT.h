#pragma once

extern "C" {
void IDTUpdateRegister(void);
}

namespace IDT
{
	const uint8_t IRQ_BASE = 0x20;

	class Register
	{
	public:
		uint16_t Limit;
		uint32_t Address;

		Register()
		{
			Limit = 0;
			Address = 0;
		}
	};

	class Descriptor
	{
	public:
		uint16_t AddressLow;
		uint16_t Selector;
		uint8_t Zero;
		uint8_t TypeAndFlags;
		uint16_t AddressHigh;

		Descriptor()
		{
			AddressLow = 0;
			Selector = 0;
			Zero = 0;
			TypeAndFlags = 0;
			AddressHigh = 0;
		}

		void Set(uint32_t pAddress, uint16_t pSelector, uint8_t pTypeAndFlags)
		{
			AddressLow = pAddress & 0xFFFF;
			AddressHigh = (pAddress >> 16) & 0xFFFF;
			Selector = pSelector;
			Zero = 0;
			TypeAndFlags = pTypeAndFlags;
		}
	};

	class InterruptRegisters
	{
	public:
		uint32_t ds;
		uint32_t edi;
		uint32_t esi;
		uint32_t ebp;
		uint32_t esp;
		uint32_t ebx;
		uint32_t edx;
		uint32_t ecx;
		uint32_t eax;

		uint32_t int_no;
		uint32_t err_code;

		uint32_t eip;
		uint32_t cs;
		uint32_t eflags;
		uint32_t useresp;
		uint32_t ss;
	};

	typedef std::function<void(InterruptRegisters)> Callback;


	void Startup();
	void RegisterCallback(uint8_t pInterrupt, Callback pCallback);
	void Schedule(uint8_t pInterrupt);
	void WaitFor(uint8_t pInterrupt);
	void SetEmptyInterrupt(uint8_t pInterrupt);
};