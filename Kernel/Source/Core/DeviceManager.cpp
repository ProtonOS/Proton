#include <Core/DeviceManager.h>

#include <algorithm>

using namespace Core;

DeviceManager::DeviceList DeviceManager::sDevices;
DeviceManager::COMPortLoggerList DeviceManager::sCOMPortLoggers;
Console* DeviceManager::sConsole = nullptr;

void DeviceManager::Initialize()
{
    new(&sDevices) DeviceList;
    new(&sCOMPortLoggers) COMPortLoggerList;
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
    if (!pDevice->Initialize()) return false;
    sDevices.push_back(pDevice);
    return true;
}

bool DeviceManager::RegisterCOMPortLogger(COMPortLogger* pCOMPortLogger)
{
    if (!Register(pCOMPortLogger)) return false;
    sCOMPortLoggers.push_back(pCOMPortLogger);
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
    pDevice->Cleanup();
    sDevices.remove(pDevice);
}

void DeviceManager::UnregisterCOMPortLogger(COMPortLogger* pCOMPortLogger)
{
    Unregister(pCOMPortLogger);
    sCOMPortLoggers.remove(pCOMPortLogger);
}

void DeviceManager::UnregisterConsole(Console* pConsole)
{
    Unregister(pConsole);
    sConsole = nullptr;
}

void DeviceManager::COMPortLoggersWriteByte(uint8_t pByte)
{
    std::for_each(sCOMPortLoggers.begin(),
                  sCOMPortLoggers.end(),
                  [&](COMPortLogger* pLogger){ pLogger->WriteByte(pByte); });
}

void DeviceManager::COMPortLoggersWriteString(const char* pString)
{
    std::for_each(sCOMPortLoggers.begin(),
                  sCOMPortLoggers.end(),
                  [&](COMPortLogger* pLogger){ pLogger->WriteString(pString); });
}

void DeviceManager::COMPortLoggersWriteLine(const char* pLine)
{
    std::for_each(sCOMPortLoggers.begin(),
                  sCOMPortLoggers.end(),
                  [&](COMPortLogger* pLogger){ pLogger->WriteLine(pLine); });
}

const DeviceManager::DeviceList& DeviceManager::GetDevices() { return sDevices; }

Console& DeviceManager::GetConsole() { return *sConsole; }
