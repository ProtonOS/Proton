#pragma once

#include <list>

#include <Core/Device.h>
#include <Core/COMPortLogger.h>
#include <Core/Console.h>

namespace Core
{
    class DeviceManager
    {
    public:
        typedef std::list<Device*> DeviceList;

        static void Initialize();

        static bool IsMemoryAvailable(uint32_t pAddress, uint32_t pLength);
        static bool IsIOPortAvailable(uint16_t pIOPort);
        static bool IsInterruptAvailable(uint8_t pInterrupt);

        static bool Register(Device* pDevice);
        static bool RegisterCOMPortLogger(COMPortLogger* pCOMPortLogger);
        static bool RegisterConsole(Console* pConsole);

        static void Unregister(Device* pDevice);
        static void UnregisterCOMPortLogger(COMPortLogger* pCOMPortLogger);
        static void UnregisterConsole(Console* pConsole);

        static const DeviceList& GetDevices();
        static COMPortLogger& GetCOMPortLogger();
        static Console& GetConsole();

    private:
        static DeviceList sDevices;
        static COMPortLogger* sCOMPortLogger;
        static Console* sConsole;

        DeviceManager();
    };
}
