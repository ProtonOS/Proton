#pragma once

extern "C" {
void GDTUpdateRegister(void);
}

namespace GDT
{
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
		uint16_t LimitLow;
		uint16_t AddressLow;
		uint8_t AddressMid;
		uint8_t Access;
		uint8_t LimitHighAndFlags;
		uint8_t AddressHigh;

		Descriptor()
		{
			LimitLow = 0;
			AddressLow = 0;
			AddressMid = 0;
			Access = 0;
			LimitHighAndFlags = 0;
			AddressHigh = 0;
		}

		void Set(uint32_t pAddress, uint32_t pLimit, uint8_t pAccess, uint8_t pFlags)
		{
			AddressLow = pAddress & 0xFFFF;
			AddressMid = (pAddress >> 16) & 0xFF;
			AddressHigh = (pAddress >> 24) & 0xFF;
			LimitLow = pLimit & 0xFFFF;
			LimitHighAndFlags = ((pLimit >> 16) & 0x0F) | (pFlags & 0xF0);
			Access = pAccess;
		}
	};

	class TSSDescriptor
	{
	public:
	   uint32_t prev_tss;   // The previous TSS - if we used hardware task switching this would form a linked list.
	   uint32_t esp0;       // The stack pointer to load when we change to kernel mode.
	   uint32_t ss0;        // The stack segment to load when we change to kernel mode.
	   uint32_t esp1;       // everything below here is unusued now.. 
	   uint32_t ss1;
	   uint32_t esp2;
	   uint32_t ss2;
	   uint32_t cr3;
	   uint32_t eip;
	   uint32_t eflags;
	   uint32_t eax;
	   uint32_t ecx;
	   uint32_t edx;
	   uint32_t ebx;
	   uint32_t esp;
	   uint32_t ebp;
	   uint32_t esi;
	   uint32_t edi;
	   uint32_t es;         
	   uint32_t cs;        
	   uint32_t ss;        
	   uint32_t ds;        
	   uint32_t fs;       
	   uint32_t gs;         
	   uint32_t ldt;      
	   uint16_t trap;
	   uint16_t iomap_base;
	};


	void Startup();
	void AllocateTSS(uint16_t pProcessorCount);
	void SetTSSStackAndRegister(uint16_t pProcessorIndex, uint32_t pStackAddress);
	void CopyForRealMode(uint32_t pGDTBaseAddress, uint32_t pGDTRegisterAddress);
};
