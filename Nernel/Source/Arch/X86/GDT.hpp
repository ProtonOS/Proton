#pragma once

extern "C" void GDTUpdate();

namespace GDT
{
    constexpr UInt8 AccessAccessed = 0x01;
    constexpr UInt8 AccessReadWrite = 0x02;
    constexpr UInt8 AccessDirectionConforming = 0x04;
    constexpr UInt8 AccessExecutable = 0x08;
    constexpr UInt8 AccessOne = 0x10;
    constexpr UInt8 AccessRing0 = 0x00;
    constexpr UInt8 AccessRing1 = 0x20;
    constexpr UInt8 AccessRing2 = 0x40;
    constexpr UInt8 AccessRing3 = 0x60;
    constexpr UInt8 AccessPresent = 0x80;
    constexpr UInt8 AccessReadWriteOnePresent = AccessReadWrite | AccessOne | AccessPresent;

    constexpr UInt8 FlagsSelector32Bit = 0x40;
    constexpr UInt8 FlagsGranularity4KB = 0x80;
    constexpr UInt8 FlagsSelector32BitGranularity4KB = FlagsSelector32Bit | FlagsGranularity4KB;

    void Load();

    void SetTSSStack(UInt32 pIndex, UInt pStackAddress);
};
