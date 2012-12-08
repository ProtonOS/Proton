#pragma warning disable 0649
namespace System.Kernel
{
    public unsafe struct MultibootHeader
    {
        internal uint Flags;
        internal uint LowerMemory;
        internal uint UpperMemory;
        internal uint BootDevice;
        internal uint CommandLine;
        internal uint ModulesCount;
        internal MultibootModule* Modules;
        internal uint Symbolic0;
        internal uint Symbolic1;
        internal uint Symbolic2;
        internal uint Symbolic3;
        internal uint MemoryMapsSize;
        internal MultibootMemoryMap* MemoryMaps;
        internal uint DrivesLength;
        internal uint DrivesAddress;
        internal uint ConfigTable;
        internal uint BootLoader;
        internal uint APMTable;
        internal uint VBEControlInfo;
        internal uint VBEModeInfo;
        internal uint VBEMode;
        internal uint VBEInterfaceSegment;
        internal uint VBEInterfaceOffset;
        internal uint VBEInterfaceLength;
    }

    public struct MultibootModule
    {
        internal uint Start;
        internal uint End;
        internal uint Identifier;
        internal uint Reserved;
    }

    public struct MultibootMemoryMap
    {
        internal uint Size;
        internal uint AddressLower;
        internal uint AddressUpper;
        internal uint LengthLower;
        internal uint LengthUpper;
        internal uint Type;
    }
}
