#pragma once

namespace Multiboot
{
	class LoadedModule;
	class Header;
	class Module;
	class MemoryMap;
	class ReservedMemoryBlock;
	class MemoryBlock;

	extern const char* sCommandLine;
	extern LoadedModule sLoadedModules[];
	extern uint8_t sLoadedModuleCount;
	extern MemoryBlock sMemoryBlocks[];
	extern uint8_t sMemoryBlockCount;
	extern uint8_t sCurrentMemoryBlock;

	class LoadedModule
	{
    private:
		size_t mAddress;
		size_t mSize;
		const char* mIdentifier;
		size_t mReserved;
	public:
        LoadedModule() : mAddress(0), mSize(0), mIdentifier(0), mReserved(0) {}
		void Initialize(size_t pAddress, size_t pSize, const char* pIdentifier);
		size_t GetAddress() const { return mAddress; }
		size_t GetSize() const { return mSize; }
		const char* GetIdentifier() const { return mIdentifier; }

	};

	void Startup(uint32_t pMultibootMagic, Header* pMultibootHeader);
	void Shutdown();
	LoadedModule* GetLoadedModule(const char* pFilename);
	void* AdjustMemory(ptrdiff_t pAdjustment);
};
