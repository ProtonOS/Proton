#pragma once

#include <Core/Driver/PIT.h>

namespace Driver
{
    class PIT8254 : public Core::Driver::PIT
    {
    public:
        PIT8254(uint16_t pHertz);
        bool OnRegister();
        void OnUnregister();

        void OnInterrupt(Core::InterruptRegisters pRegisters);

        uint16_t GetMillisecondsElapsed();
        uint32_t GetSecondsElapsed();

    private:
        static const uint64_t MaxHertz = 1193182;
        static const uint16_t BaseIOPort = 0x0040;
        static const uint16_t Counter0IOPort = BaseIOPort + 0x00;
        static const uint16_t Counter1IOPort = BaseIOPort + 0x01;
        static const uint16_t Counter2IOPort = BaseIOPort + 0x02;
        static const uint16_t CommandIOPort  = BaseIOPort + 0x03;
        static const uint16_t BaseIRQ = 0x00;

        enum Command
        {
            BinaryCodedDecimalCommand = 0x01,

            Mode0Command = 0x00,
            Mode1Command = 0x02,
            Mode2Command = 0x04,
            Mode3Command = 0x06,
            Mode4Command = 0x08,
            Mode5Command = 0x0A,

            ReadWriteLatchCommand = 0x00,
            ReadWriteLSBOnlyCommand = 0x10,
            ReadWriteMSBOnlyCommand = 0x20,
            ReadWriteLSBThenMSBCommand = 0x30,

            SelectCounter0Command = 0x00,
            SelectCounter1Command = 0x40,
            SelectCounter2Command = 0x80,
            SelectReadBackCommand = 0xC0,

            Counter0InitializeCommand = Mode3Command | ReadWriteLSBThenMSBCommand | SelectCounter0Command
        };

        uint16_t mHertz;
        uint16_t mCycle;
        uint32_t mMillisecondsPerCycle;
        uint32_t mMillisecondsElapsed;
        uint32_t mSecondsElapsed;

        PIT8254(const PIT8254&);
    };
}
