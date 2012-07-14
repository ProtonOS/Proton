#pragma once

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

	void Startup();
};
