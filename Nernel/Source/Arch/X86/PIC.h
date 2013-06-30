#pragma once

#include "../../Core/Types.hpp"

class PIC
{
public:
	static const uint8 IRQBaseISR = 0x20;

	static const uint16 IOPortMaster = 0x0020;
	static const uint16 IOPortMasterCommand = IOPortMaster + 0x00;
	static const uint16 IOPortMasterData = IOPortMaster + 0x01;

	static const uint16 IOPortSlave = 0x00A0;
	static const uint16 IOPortSlaveCommand = IOPortSlave + 0x00;
	static const uint16 IOPortSlaveData = IOPortSlave + 0x01;

	static const uint8 CommandConfigMode = 0x01;
	static const uint8 CommandSinglePIC = 0x02;
	static const uint8 CommandInterval4 = 0x04;
	static const uint8 CommandLevelMode = 0x08;
	static const uint8 CommandInitialize = 0x10;
	static const uint8 CommandReset = 0x20;

	static const uint8 LineMasterToSlave = 0x04;
	static const uint8 LineSlaveToMaster = 0x02;

	static const uint8 ModeMicroprocessor8086 = 0x01;
	static const uint8 ModeAutoEndOfInterrupt = 0x02;
	static const uint8 ModeMaster = 0x04;
	static const uint8 ModeBuffered = 0x08;
	static const uint8 ModeSpecialFullyNested = 0x10;

	static void Load();

	static void ResetInterrupts(bool pSlave);
};
