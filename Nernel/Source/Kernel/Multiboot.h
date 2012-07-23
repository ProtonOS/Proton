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
    public:
		size_t Address;
		size_t Size;
		const char* Identifier;
		size_t Reserved;

		LoadedModule() : Address(0), Size(0), Identifier(0), Reserved(0) {}
	};

	void Startup(uint32_t pMultibootMagic, Header* pMultibootHeader);
	void Shutdown();
	LoadedModule* GetLoadedModule(const char* pFilename);
	void* AdjustMemory(ptrdiff_t pAdjustment);
};
