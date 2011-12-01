#pragma once

#include <list>

#include <Core/Device.h>
#include <Core/COMPortLogger.h>
#include <Core/Driver/Console.h>
#include <Core/Driver/PIC.h>

using namespace std;
using namespace Core::Driver;

namespace Core
{
    class DeviceManager
    {
    public:
        typedef list<Device*> DeviceList;

        static bool Startup();
        static void Shutdown();

        static bool IsMemoryAvailable(uint32_t pAddress, uint32_t pLength);
        static bool IsIOPortAvailable(uint16_t pIOPort);
        static bool IsInterruptAvailable(uint8_t pInterrupt);

        static bool Register(Device* pDevice);
        static bool RegisterCOMPortLogger(COMPortLogger* pCOMPortLogger);
        static bool RegisterConsole(Console* pConsole);
        static bool RegisterPIC(PIC* pPIC);

        static void Unregister(Device* pDevice);
        static void UnregisterCOMPortLogger(COMPortLogger* pCOMPortLogger);
        static void UnregisterConsole(Console* pConsole);
        static void UnregisterPIC(PIC* pPIC);

        static const DeviceList& GetDevices();
        static COMPortLogger& GetCOMPortLogger();
        static Console& GetConsole();
        static PIC& GetPIC();

    private:
        static DeviceList sDevices;
        static COMPortLogger* sCOMPortLogger;
        static Console* sConsole;
        static PIC* sPIC;

        DeviceManager();
    };
}
