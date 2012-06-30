#pragma once

class Multiboot
{
public:
	class LoadedModule;
private:
	class Header;
	class Module;
	class MemoryMap;
	class ReservedMemoryBlock;
	class MemoryBlock;

	class Header
	{
	public:
		uint32_t Flags;
		uint32_t LowerMemory;
		uint32_t UpperMemory;
		uint32_t BootDevice;
		const char* CommandLine;
		uint32_t ModulesCount;
		Module* Modules;
		uint32_t Symbolic0;
		uint32_t Symbolic1;
		uint32_t Symbolic2;
		uint32_t Symbolic3;
		uint32_t MemoryMapsSize;
		MemoryMap* MemoryMaps;
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

	class Module
	{
	public:
		uint32_t Start;
		uint32_t End;
		const char* Identifier;
		uint32_t Reserved;
	};

	class MemoryMap
	{
	public:
		uint32_t Size;
		uint32_t AddressLower;
		uint32_t AddressUpper;
		uint32_t LengthLower;
		uint32_t LengthUpper;
		uint32_t Type;
	};

	class ReservedMemoryBlock
	{
	public:
		size_t Address;
		size_t Size;
	};

	class MemoryBlock
	{
	public:
		size_t Address;
		size_t Size;
		size_t Used;
		size_t Reserved;
	};

	Multiboot() { }

	static const char* sCommandLine;
	static LoadedModule sLoadedModules[];
	static uint8_t sLoadedModuleCount;
	static MemoryBlock sMemoryBlocks[];
	static uint8_t sMemoryBlockCount;
	static uint8_t sCurrentMemoryBlock;

public:
	class LoadedModule
	{
	private:
		size_t mAddress;
		size_t mSize;
		const char* mIdentifier;
		size_t mReserved;

	public:
		LoadedModule();
		void Initialize(size_t pAddress, size_t pSize, const char* pIdentifier);

		size_t GetAddress() const { return mAddress; }
		size_t GetSize() const { return mSize; }
		const char* GetIdentifier() const { return mIdentifier; }
	};

	static void Startup(uint32_t pMultibootMagic, void* pMultibootHeader);
	static void Shutdown();
	static LoadedModule* GetLoadedModule(const char* pFilename);
	static void* AdjustMemory(ptrdiff_t pAdjustment);
};
