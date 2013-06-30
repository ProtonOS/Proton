#pragma once

#include "../../Core/Types.hpp"

extern "C" void GDTUpdate();

class GDT
{
private:
	static void SetSegment(uint32 pIndex, uint32 pAddress, uint32 pLimit, uint8 pAccess, uint8 pFlags);

public:
	static const uint8 AccessAccessed = 0x01;
	static const uint8 AccessReadWrite = 0x02;
	static const uint8 AccessDirectionConforming = 0x04;
	static const uint8 AccessExecutable = 0x08;
	static const uint8 AccessOne = 0x10;
	static const uint8 AccessRing0 = 0x00;
	static const uint8 AccessRing1 = 0x20;
	static const uint8 AccessRing2 = 0x40;
	static const uint8 AccessRing3 = 0x60;
	static const uint8 AccessPresent = 0x80;
	static const uint8 AccessReadWriteOnePresent = AccessReadWrite | AccessOne | AccessPresent;

	static const uint8 FlagsSelector32Bit = 0x40;
	static const uint8 FlagsGranularity4KB = 0x80;
	static const uint8 FlagsSelector32BitGranularity4KB = FlagsSelector32Bit | FlagsGranularity4KB;

	static void Load();

	static void SetTSSStack(uint32 pIndex, uintptr pStackAddress);
};
