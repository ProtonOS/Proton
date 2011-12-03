#pragma once

#include <list>

#include <Core/Device.h>
#include <Core/COMPortLogger.h>
#include <Core/SystemClock.h>
#include <Core/Driver/Console.h>
#include <Core/Driver/PIC.h>
#include <Core/Driver/PIT.h>
#include <Core/Driver/RTC.h>

namespace Core
{
    class DeviceManager
    {
    public:
        typedef std::list<Core::Device*> DeviceList;

        static bool Startup();
        static void Shutdown();

        static bool IsMemoryAvailable(uint32_t pAddress, uint32_t pLength);
        static bool IsIOPortAvailable(uint16_t pIOPort);
        static bool IsInterruptAvailable(uint8_t pInterrupt);

        static bool Register(Core::Device* pDevice);
        static bool RegisterCOMPortLogger(Core::COMPortLogger* pCOMPortLogger);
        static bool RegisterConsole(Core::Driver::Console* pConsole);
        static bool RegisterRTC(Core::Driver::RTC* pRTC);
        static bool RegisterPIC(Core::Driver::PIC* pPIC);
        static bool RegisterPIT(Core::Driver::PIT* pPIT);

        static void Unregister(Core::Device* pDevice);
        static void UnregisterCOMPortLogger(Core::COMPortLogger* pCOMPortLogger);
        static void UnregisterConsole(Core::Driver::Console* pConsole);
        static void UnregisterRTC(Core::Driver::RTC* pRTC);
        static void UnregisterPIC(Core::Driver::PIC* pPIC);
        static void UnregisterPIT(Core::Driver::PIT* pPIT);

        static const DeviceList& GetDevices();
        static Core::COMPortLogger& GetCOMPortLogger();
        static Core::Driver::Console& GetConsole();
        static Core::Driver::RTC& GetRTC();
        static Core::Driver::PIC& GetPIC();
        static Core::Driver::PIT& GetPIT();

    private:
        static DeviceList sDevices;
        static Core::COMPortLogger* sCOMPortLogger;
        static Core::Driver::Console* sConsole;
        static Core::Driver::RTC* sRTC;
        static Core::Driver::PIC* sPIC;
        static Core::Driver::PIT* sPIT;

        DeviceManager();
    };
}
