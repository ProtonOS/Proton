#pragma once

#include "../../Core/Types.hpp"

class PIC
{
public:
	static const UInt8 IRQBaseISR = 0x20;

	static const UInt16 IOPortMaster = 0x0020;
	static const UInt16 IOPortMasterCommand = IOPortMaster + 0x00;
	static const UInt16 IOPortMasterData = IOPortMaster + 0x01;

	static const UInt16 IOPortSlave = 0x00A0;
	static const UInt16 IOPortSlaveCommand = IOPortSlave + 0x00;
	static const UInt16 IOPortSlaveData = IOPortSlave + 0x01;

	static const UInt8 CommandConfigMode = 0x01;
	static const UInt8 CommandSinglePIC = 0x02;
	static const UInt8 CommandInterval4 = 0x04;
	static const UInt8 CommandLevelMode = 0x08;
	static const UInt8 CommandInitialize = 0x10;
	static const UInt8 CommandReset = 0x20;

	static const UInt8 LineMasterToSlave = 0x04;
	static const UInt8 LineSlaveToMaster = 0x02;

	static const UInt8 ModeMicroprocessor8086 = 0x01;
	static const UInt8 ModeAutoEndOfInterrupt = 0x02;
	static const UInt8 ModeMaster = 0x04;
	static const UInt8 ModeBuffered = 0x08;
	static const UInt8 ModeSpecialFullyNested = 0x10;

	static void Load();

	static void ResetInterrupts(bool pSlave);
};
