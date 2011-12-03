#pragma once

namespace Core
{
    class SystemClock
    {
    public:
        static bool Startup();
        static void Shutdown();

        static bool IsReady();
        static uint32_t GetSecondsSinceEpoch();
        static uint16_t GetMilliseconds();

    private:
        static uint32_t sStartupTime;
    };
}
