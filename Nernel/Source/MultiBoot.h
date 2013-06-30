#pragma once

#include "PrimitiveTypes.h"

class MultiBoot
{
private:
	static const uint32 MagicNumber = 0x2BADB002;
	static const uint32 MemoryMapAvailableType = 1;

	struct MultiBootModule
	{
	public:
		uint32 Start;
		uint32 End;
		char* Identifier;
		uint32 Reserved;
	};

	struct MultiBootMemoryMap
	{
	public:
		uint32 Size;
		uint32 AddressLower;
		uint32 AddressUpper;
		uint32 LengthLower;
		uint32 LengthUpper;
		uint32 Type;
	};

	static const uint32 ReservedMemoryBlocksMax = 64;

	struct ReservedMemoryBlock
	{
	public:
		uintptr Address;
		uintptr Size;
	};

public:
	struct MultiBootHeader
	{
	public:
		uint32 Flags;
		uint32 LowerMemory;
		uint32 UpperMemory;
		uint32 BootDevice;
		uint32 CommandLine;
		uint32 ModulesCount;
		MultiBootModule* Modules;
		uint32 Symbolic0;
		uint32 Symbolic1;
		uint32 Symbolic2;
		uint32 Symbolic3;
		uint32 MemoryMapsSize;
		MultiBootMemoryMap* MemoryMaps;
		uint32 DrivesLength;
		uint32 DrivesAddress;
		uint32 ConfigTable;
		uint32 BootLoader;
		uint32 APMTable;
		uint32 VBEControlInfo;
		uint32 VBEModeInfo;
		uint16 VBEMode;
		uint16 VBEInterfaceSegment;
		uint16 VBEInterfaceOffset;
		uint16 VBEInterfaceLength;
	};

public:
	static const uint32 LoadedModulesMax = 64;
	static const uint32 LoadedModuleIdentifierMax = 128;
	static const uint32 AvailableMemoryBlocksMax = 64;

	struct LoadedModule
	{
	public:
		uintptr Address;
		uintptr Size;
		char Identifier[LoadedModuleIdentifierMax];
	};

	struct AvailableMemoryBlock
	{
	public:
		uintptr Address;
		uintptr Size;
		uintptr Used;
	};

	static LoadedModule LoadedModules[];
	static uint32 LoadedModulesCount;

	static AvailableMemoryBlock AvailableMemoryBlocks[AvailableMemoryBlocksMax];
	static uint32 AvailableMemoryBlocksCount;

	static void Load(uint32 pMultiBootMagic, MultiBootHeader* pMultiBootHeader);
};
