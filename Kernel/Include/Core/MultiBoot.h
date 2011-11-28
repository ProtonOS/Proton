#pragma once

#include <array>

namespace Core
{
    class MultiBoot
    {
    private:
        static const uint32_t Magic = 0x2BADB002;

        static struct Header
        {
	        uint32_t flags;
	        uint32_t mem_lower;
	        uint32_t mem_upper;
	        uint32_t boot_device;
	        uint32_t cmdline;
	        uint32_t mods_count;
	        uint32_t mods_addr;
	        uint32_t sym_0;
	        uint32_t sym_1;
	        uint32_t sym_2;
	        uint32_t sym_3;
	        uint32_t mmap_length;
	        uint32_t mmap_addr;
	        uint32_t drives_length;
	        uint32_t drives_addr;
	        uint32_t config_table;
	        uint32_t boot_loader_name;
	        uint32_t apm_table;
	        uint32_t vbe_control_info;
	        uint32_t vbe_mode_info;
	        uint16_t vbe_mode;
	        uint16_t vbe_interface_seg;
	        uint16_t vbe_interface_off;
	        uint16_t vbe_interface_len;
        } * sHeader;

        static const uint32_t MemoryMapAvailable = 1;
        static const uint32_t MemoryMapConventional = (1024 * 640);

        struct MemoryMap
        {
	        uint32_t size;
	        uint32_t addr0;
	        uint32_t addr1;
	        uint32_t length0;
	        uint32_t length1;
	        uint32_t type;
        };

    public:
        static const uint8_t MaxMemoryBlocks = 64;

        struct MemoryBlock
        {
        	uint32_t Address;
        	uint32_t Length;
        	uint32_t Used;
            uint32_t Padding;
        };

    private:
        typedef std::array<MemoryBlock, MaxMemoryBlocks> MemoryBlockArray;

        static MemoryBlockArray sMemoryBlocks;
        static uint8_t sMemoryBlockCount;

    public:
        static bool Initialize(uint32_t pMultiBootMagic, void * pMultiBootData);
        static MemoryBlock * GetMemoryBlock(uint8_t pIndex) { return &sMemoryBlocks[pIndex]; }
        static uint8_t GetMemoryBlockCount() { return sMemoryBlockCount; }
    };
}
