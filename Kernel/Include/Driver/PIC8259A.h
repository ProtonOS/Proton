#pragma once

#include <Core/Driver/PIC.h>

namespace Driver
{
    class PIC8259A : public Core::Driver::PIC
    {
    public:
        PIC8259A();
        bool OnRegister();
        void OnUnregister();

        void StartInterrupts();
        void StopInterrupts();
        void ResetInterrupts(bool pSlave);

    private:
        static const uint16_t MasterIOPort        = 0x0020;
        static const uint16_t MasterCommandIOPort = MasterIOPort + 0x00;
        static const uint16_t MasterDataIOPort    = MasterIOPort + 0x01;

        static const uint16_t SlaveIOPort         = 0x00A0;
        static const uint16_t SlaveCommandIOPort  = SlaveIOPort + 0x00;
        static const uint16_t SlaveDataIOPort     = SlaveIOPort + 0x01;

        enum Command
        {
            ConfigModeCommand = 0x01,
            SinglePICCommand = 0x02,
            Interval4Command = 0x04,
            LevelModeCommand = 0x08,
            InitializeCommand = 0x10,
            ResetCommand = 0x20
        };

        enum Line
        {
            MasterToSlaveLine = 0x04,
            SlaveToMasterLine = 0x02
        };

        enum Mode
        {
            Microprocessor8086Mode = 0x01,
            AutoEndOfInterruptMode = 0x02,
            MasterMode = 0x04,
            BufferedMode = 0x08,
            SpecialFullyNestedMode = 0x10
        };

        PIC8259A(const PIC8259A&);
    };
}
