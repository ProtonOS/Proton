#pragma once

#include <array>

using namespace std;

namespace Core
{
    class GlobalDescriptorTable
    {
    public:
        static const uint8_t MaxDescriptors = 5;

        struct Register
        {
            uint16_t Limit;
            uint32_t Address;
        };

        struct Descriptor
        {
            uint16_t LimitLow;
            uint16_t AddressLow;
            uint8_t AddressMid;
            uint8_t Access;
            uint8_t LimitHighAndFlags;
            uint8_t AddressHigh;
        };

        enum Access
        {
            AccessedAccess = 0x01,
            ReadWriteAccess = 0x02,
            DirectionConformingAccess = 0x04,
            ExecutableAccess = 0x08,
            OneAccess = 0x10,

            Ring0Access = 0x00,
            Ring1Access = 0x20,
            Ring2Access = 0x40,
            Ring3Access = 0x60,
            PresentAccess = 0x80,

            ReadWriteOnePresentAccess = ReadWriteAccess | OneAccess | PresentAccess
        };

        enum Flags
        {
            Selector32BitFlag = 0x40,
            Granularity4KBFlag = 0x80,

            Selector32BitGranularity4KBFlags = Selector32BitFlag | Granularity4KBFlag
        };

        static bool Startup();
        static void Shutdown();

    private:
        typedef array<Descriptor, MaxDescriptors> DescriptorsArray;

        static Register sRegister;
        static DescriptorsArray sDescriptors;

        static void SetSegment(uint8_t pIndex,
                               uint32_t pAddress,
                               uint32_t pLimit,
                               uint8_t pAccess,
                               uint8_t pFlags);
    };
}
