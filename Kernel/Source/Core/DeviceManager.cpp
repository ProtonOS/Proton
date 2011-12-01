#include <Core/DeviceManager.h>

using namespace Core;

DeviceManager::DeviceList DeviceManager::sDevices;
COMPortLogger* DeviceManager::sCOMPortLogger = nullptr;
Console* DeviceManager::sConsole = nullptr;

bool DeviceManager::Startup()
{
    new(&sDevices) DeviceList;
    return true;
}

void DeviceManager::Shutdown()
{
}

bool DeviceManager::IsMemoryAvailable(uint32_t pAddress, uint32_t pLength)
{
    Device* device = nullptr;
    for (DeviceList::iterator it = sDevices.begin(); it != sDevices.end(); ++it)
    {
        device = *it;
        if (device->HasMemoryConflict(pAddress, pLength)) return false;
    }
    return true;
}

bool DeviceManager::IsIOPortAvailable(uint16_t pIOPort)
{
    Device* device = nullptr;
    for (DeviceList::iterator it = sDevices.begin(); it != sDevices.end(); ++it)
    {
        device = *it;
        if (device->HasIOPortConflict(pIOPort)) return false;
    }
    return true;
}

bool DeviceManager::IsInterruptAvailable(uint8_t pInterrupt)
{
    Device* device = nullptr;
    for (DeviceList::iterator it = sDevices.begin(); it != sDevices.end(); ++it)
    {
        device = *it;
        if (device->HasInterruptConflict(pInterrupt)) return false;
    }
    return true;
}

bool DeviceManager::Register(Device* pDevice)
{
    if (!pDevice->OnRegister()) return false;
    sDevices.push_back(pDevice);
    return true;
}

bool DeviceManager::RegisterCOMPortLogger(COMPortLogger* pCOMPortLogger)
{
    if (!Register(pCOMPortLogger)) return false;
    sCOMPortLogger = pCOMPortLogger;
    return true;
}

bool DeviceManager::RegisterConsole(Console* pConsole)
{
    if (!Register(pConsole)) return false;
    sConsole = pConsole;
    return true;
}

void DeviceManager::Unregister(Device* pDevice)
{
    pDevice->OnUnregister();
    sDevices.remove(pDevice);
}

void DeviceManager::UnregisterCOMPortLogger(COMPortLogger* pCOMPortLogger)
{
    Unregister(pCOMPortLogger);
    sCOMPortLogger = nullptr;
}

void DeviceManager::UnregisterConsole(Console* pConsole)
{
    Unregister(pConsole);
    sConsole = nullptr;
}

const DeviceManager::DeviceList& DeviceManager::GetDevices() { return sDevices; }

COMPortLogger& DeviceManager::GetCOMPortLogger() { return *sCOMPortLogger; }

Console& DeviceManager::GetConsole() { return *sConsole; }
