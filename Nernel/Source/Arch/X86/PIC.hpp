#pragma once

namespace PIC
{
    constexpr UInt8 IRQBaseISR = 0x20;

    constexpr UInt16 IOPortMaster = 0x0020;
    constexpr UInt16 IOPortMasterCommand = IOPortMaster + 0x00;
    constexpr UInt16 IOPortMasterData = IOPortMaster + 0x01;

    constexpr UInt16 IOPortSlave = 0x00A0;
    constexpr UInt16 IOPortSlaveCommand = IOPortSlave + 0x00;
    constexpr UInt16 IOPortSlaveData = IOPortSlave + 0x01;

    constexpr UInt8 CommandConfigMode = 0x01;
    constexpr UInt8 CommandSinglePIC = 0x02;
    constexpr UInt8 CommandInterval4 = 0x04;
    constexpr UInt8 CommandLevelMode = 0x08;
    constexpr UInt8 CommandInitialize = 0x10;
    constexpr UInt8 CommandReset = 0x20;

    constexpr UInt8 LineMasterToSlave = 0x04;
    constexpr UInt8 LineSlaveToMaster = 0x02;

    constexpr UInt8 ModeMicroprocessor8086 = 0x01;
    constexpr UInt8 ModeAutoEndOfInterrupt = 0x02;
    constexpr UInt8 ModeMaster = 0x04;
    constexpr UInt8 ModeBuffered = 0x08;
    constexpr UInt8 ModeSpecialFullyNested = 0x10;

    void Load();

    void ResetInterrupts(bool pSlave);
};
