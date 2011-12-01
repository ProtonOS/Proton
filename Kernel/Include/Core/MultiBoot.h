#pragma once

#include <array>

namespace Core
{
    class MultiBoot
    {
    public:
        static const uint8_t MaxMemoryBlocks = 64;

        struct MemoryBlock
        {
        	uint32_t Address;
        	uint32_t Length;
        	uint32_t Used;
            uint32_t Padding;
        };

        static bool Startup(uint32_t pMultiBootMagic, void* pMultiBootData);
        static void Shutdown();
        static const char* GetCommandLine();
        static MemoryBlock& GetMemoryBlock(uint8_t pIndex);
        static uint8_t GetMemoryBlockCount();

    private:
        static const uint32_t Magic = 0x2BADB002;
        static const uint32_t MemoryMapAvailable = 1;
        static const uint32_t MemoryMapConventional = (1024 * 640);

        typedef std::array<MemoryBlock, MaxMemoryBlocks> MemoryBlockArray;

        struct Header
        {
	        uint32_t Flags;
	        uint32_t LowerMemory;
	        uint32_t UpperMemory;
	        uint32_t BootDevice;
	        uint32_t CommandLine;
	        uint32_t ModulesCount;
	        uint32_t ModulesAddress;
	        uint32_t Symbolic0;
	        uint32_t Symbolic1;
	        uint32_t Symbolic2;
	        uint32_t Symbolic3;
	        uint32_t MemoryMapsLength;
	        uint32_t MemoryMapsAddress;
	        uint32_t DrivesLength;
	        uint32_t DrivesAddress;
	        uint32_t ConfigTable;
	        uint32_t BootLoader;
	        uint32_t APMTable;
	        uint32_t VBEControlInfo;
	        uint32_t VBEModeInfo;
	        uint16_t VBEMode;
	        uint16_t VBEInterfaceSegment;
	        uint16_t VBEInterfaceOffset;
	        uint16_t VBEInterfaceLength;
        };

        struct MemoryMap
        {
	        uint32_t Size;
	        uint32_t AddressLower;
	        uint32_t AddressUpper;
	        uint32_t LengthLower;
	        uint32_t LengthUpper;
	        uint32_t Type;
        };

        static const char* sCommandLine;
        static MemoryBlockArray sMemoryBlocks;
        static uint8_t sMemoryBlockCount;

        MultiBoot();
        MultiBoot(const MultiBoot&);
    };
}
