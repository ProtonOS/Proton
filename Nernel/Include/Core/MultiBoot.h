#pragma once

#define MultiBoot_Magic                     0x2BADB002
#define MultiBoot_MemoryMapAvailable        1
#define MultiBoot_MemoryMapConventional     (1024 * 640)
#define MultiBoot_MaxLoadedModules          64
#define MultiBoot_MaxMemoryBlocks           64

typedef struct 
{
    uint32_t Address;
    uint32_t Length;
    const char* Identifier;
    uint32_t Reserved;
} MultiBoot_LoadedModule;

typedef struct
{
    uint32_t Address;
    uint32_t Length;
    uint32_t Used;
    uint32_t Reserved;
} MultiBoot_MemoryBlock;

typedef struct
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
} MultiBoot_Header;

typedef struct 
{
    uint32_t Start;
    uint32_t End;
    uint32_t Identifier;
    uint32_t Reserved;
} MultiBoot_Module;

typedef struct
{
	uint32_t Size;
	uint32_t AddressLower;
	uint32_t AddressUpper;
	uint32_t LengthLower;
	uint32_t LengthUpper;
	uint32_t Type;
} MultiBoot_MemoryMap;


bool_t MultiBoot_Startup(uint32_t pMultiBootMagic, void* pMultiBootData);
void MultiBoot_Shutdown();
void* MultiBoot_GetModuleByFileName(const char* pFileName);
MultiBoot_LoadedModule* MultiBoot_GetLoadedModuleByFileName(const char* pFileName);

extern const char* gMultiBoot_CommandLine;
extern MultiBoot_LoadedModule gMultiBoot_LoadedModules[];
extern uint8_t gMultiBoot_LoadedModuleCount;
extern MultiBoot_MemoryBlock gMultiBoot_MemoryBlocks[];
extern uint8_t gMultiBoot_MemoryBlockCount;
