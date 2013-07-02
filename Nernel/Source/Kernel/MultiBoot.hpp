#pragma once

namespace MultiBoot
{
    constexpr UInt32 LoadedModulesMax = 64;
    constexpr UInt32 LoadedModuleIdentifierMax = 128;
    constexpr UInt32 AvailableMemoryBlocksMax = 64;

    struct LoadedModule
    {
    public:
        UInt Address;
        UInt Size;
        char Identifier[LoadedModuleIdentifierMax];
    };

    struct AvailableMemoryBlock
    {
    public:
        UInt Address;
        UInt Size;
        UInt Used;
    };

	extern UInt TotalPhysicalMemory;

    extern LoadedModule LoadedModules[];
    extern UInt32 LoadedModulesCount;

    extern AvailableMemoryBlock AvailableMemoryBlocks[];
    extern UInt32 AvailableMemoryBlocksCount;

	struct MultiBootHeader;
    void Load(UInt32 pMultiBootMagic, Core::Ptr<MultiBootHeader> pMultiBootHeader);
};
