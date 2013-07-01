#pragma once

namespace PIT
{
    constexpr UInt16 IOPortChannel0 = 0x0040;
    constexpr UInt16 IOPortChannel0Data = IOPortChannel0 + 0x00;

	constexpr UInt16 IOPortMode = 0x0043;

	constexpr UInt8 ModeBinary = 0x00;
	constexpr UInt8 ModeBCD = 0x01;

	constexpr UInt8 ModeTerminalCount = 0x00;
	constexpr UInt8 ModeHardwareOneShot = 0x08;
	constexpr UInt8 ModeRateGenerator = 0x04;
	constexpr UInt8 ModeSquareWaveGenerator = 0x0C;
	constexpr UInt8 ModeSoftwareStrobe = 0x02;
	constexpr UInt8 ModeHardwareStrobe = 0x0A;

	constexpr UInt8 ModeLatchCount = 0x00;
	constexpr UInt8 ModeLowByteOnly = 0x20;
	constexpr UInt8 ModeHighByteOnly = 0x10;
	constexpr UInt8 ModeLowByteThenHighByte = 0x30;

	constexpr UInt8 ModeChannel0 = 0x00;
	constexpr UInt8 ModeChannel1 = 0x80;
	constexpr UInt8 ModeChannel2 = 0x40;
	constexpr UInt8 ModeReadBack = 0xC0;

	constexpr UInt32 MaxHertz = 1193182;
	constexpr UInt16 CycleHertz = 100;
	constexpr UInt16 MillisecondsPerCycle = 1000 / CycleHertz;

    void Load();

	extern UInt16 MillisecondsSinceSecond;
	extern UInt64 SecondsSinceStartup;
};
