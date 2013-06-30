#pragma once

#include "../../Core/Types.hpp"

extern "C" void GDTUpdate();

class GDT
{
private:
	static void SetSegment(UInt32 pIndex, UInt32 pAddress, UInt32 pLimit, UInt8 pAccess, UInt8 pFlags);

public:
	static const UInt8 AccessAccessed = 0x01;
	static const UInt8 AccessReadWrite = 0x02;
	static const UInt8 AccessDirectionConforming = 0x04;
	static const UInt8 AccessExecutable = 0x08;
	static const UInt8 AccessOne = 0x10;
	static const UInt8 AccessRing0 = 0x00;
	static const UInt8 AccessRing1 = 0x20;
	static const UInt8 AccessRing2 = 0x40;
	static const UInt8 AccessRing3 = 0x60;
	static const UInt8 AccessPresent = 0x80;
	static const UInt8 AccessReadWriteOnePresent = AccessReadWrite | AccessOne | AccessPresent;

	static const UInt8 FlagsSelector32Bit = 0x40;
	static const UInt8 FlagsGranularity4KB = 0x80;
	static const UInt8 FlagsSelector32BitGranularity4KB = FlagsSelector32Bit | FlagsGranularity4KB;

	static void Load();

	static void SetTSSStack(UInt32 pIndex, UInt pStackAddress);
};
