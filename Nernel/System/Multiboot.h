#pragma once

typedef struct _MultibootHeader MultibootHeader;
typedef struct _MultibootModule MultibootModule;
typedef struct _MultibootMemoryMap MultibootMemoryMap;
typedef struct _ReservedMemoryBlock ReservedMemoryBlock;
typedef struct _LoadedModule LoadedModule;
typedef struct _MemoryBlock MemoryBlock;

struct _MultibootHeader
{
	uint32_t Flags;
	uint32_t LowerMemory;
	uint32_t UpperMemory;
	uint32_t BootDevice;
	uint32_t CommandLine;
	uint32_t ModulesCount;
	MultibootModule* Modules;
	uint32_t Symbolic0;
	uint32_t Symbolic1;
	uint32_t Symbolic2;
	uint32_t Symbolic3;
	uint32_t MemoryMapsSize;
	MultibootMemoryMap* MemoryMaps;
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

struct _MultibootModule
{
    uint32_t Start;
    uint32_t End;
    uint32_t Identifier;
    uint32_t Reserved;
};

struct _MultibootMemoryMap
{
	uint32_t Size;
	uint32_t AddressLower;
	uint32_t AddressUpper;
	uint32_t LengthLower;
	uint32_t LengthUpper;
	uint32_t Type;
};

struct _ReservedMemoryBlock
{
	size_t Address;
	size_t Size;
};

struct _LoadedModule
{
    size_t Address;
    size_t Size;
    const char* Identifier;
    size_t Reserved;
};

struct _MemoryBlock
{
    size_t Address;
    size_t Size;
    size_t Used;
    size_t Reserved;
};

void Multiboot_Startup(uint32_t pMultibootMagic, MultibootHeader* pMultibootHeader);
void Multiboot_Shutdown();

extern const char* gMultiboot_CommandLine;
extern uint8_t gLoadedModuleCount;
