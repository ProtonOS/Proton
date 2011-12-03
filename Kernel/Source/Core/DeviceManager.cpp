#include <Core/DeviceManager.h>

Core::DeviceManager::DeviceList Core::DeviceManager::sDevices;
Core::COMPortLogger* Core::DeviceManager::sCOMPortLogger = nullptr;
Core::Driver::Console* Core::DeviceManager::sConsole = nullptr;
Core::Driver::RTC* Core::DeviceManager::sRTC = nullptr;
Core::Driver::PIC* Core::DeviceManager::sPIC = nullptr;
Core::Driver::PIT* Core::DeviceManager::sPIT = nullptr;

bool Core::DeviceManager::Startup()
{
    new(&sDevices) DeviceList;
    return true;
}

void Core::DeviceManager::Shutdown()
{
}

bool Core::DeviceManager::IsMemoryAvailable(uint32_t pAddress, uint32_t pLength)
{
    Core::Device* device = nullptr;
    for (DeviceList::iterator it = sDevices.begin(); it != sDevices.end(); ++it)
    {
        device = *it;
        if (device->HasMemoryConflict(pAddress, pLength)) return false;
    }
    return true;
}

bool Core::DeviceManager::IsIOPortAvailable(uint16_t pIOPort)
{
    Core::Device* device = nullptr;
    for (DeviceList::iterator it = sDevices.begin(); it != sDevices.end(); ++it)
    {
        device = *it;
        if (device->HasIOPortConflict(pIOPort)) return false;
    }
    return true;
}

bool Core::DeviceManager::IsInterruptAvailable(uint8_t pInterrupt)
{
    Core::Device* device = nullptr;
    for (DeviceList::iterator it = sDevices.begin(); it != sDevices.end(); ++it)
    {
        device = *it;
        if (device->HasInterruptConflict(pInterrupt)) return false;
    }
    return true;
}

bool Core::DeviceManager::Register(Core::Device* pDevice)
{
    if (!pDevice->OnRegister()) return false;
    sDevices.push_back(pDevice);
    return true;
}

bool Core::DeviceManager::RegisterCOMPortLogger(Core::COMPortLogger* pCOMPortLogger)
{
    if (!Register(pCOMPortLogger)) return false;
    sCOMPortLogger = pCOMPortLogger;
    return true;
}

bool Core::DeviceManager::RegisterConsole(Core::Driver::Console* pConsole)
{
    if (!Register(pConsole)) return false;
    sConsole = pConsole;
    return true;
}

bool Core::DeviceManager::RegisterRTC(Core::Driver::RTC* pRTC)
{
    if (!Register(pRTC)) return false;
    sRTC = pRTC;
    return true;
}

bool Core::DeviceManager::RegisterPIC(Core::Driver::PIC* pPIC)
{
    if (!Register(pPIC)) return false;
    sPIC = pPIC;
    return true;
}

bool Core::DeviceManager::RegisterPIT(Core::Driver::PIT* pPIT)
{
    if (!Register(pPIT)) return false;
    sPIT = pPIT;
    return true;
}

void Core::DeviceManager::Unregister(Core::Device* pDevice)
{
    pDevice->OnUnregister();
    sDevices.remove(pDevice);
}

void Core::DeviceManager::UnregisterCOMPortLogger(Core::COMPortLogger* pCOMPortLogger)
{
    Unregister(pCOMPortLogger);
    sCOMPortLogger = nullptr;
}

void Core::DeviceManager::UnregisterConsole(Core::Driver::Console* pConsole)
{
    Unregister(pConsole);
    sConsole = nullptr;
}

void Core::DeviceManager::UnregisterRTC(Core::Driver::RTC* pRTC)
{
    Unregister(pRTC);
    sRTC = nullptr;
}

void Core::DeviceManager::UnregisterPIC(Core::Driver::PIC* pPIC)
{
    Unregister(pPIC);
    sPIC = nullptr;
}

void Core::DeviceManager::UnregisterPIT(Core::Driver::PIT* pPIT)
{
    Unregister(pPIT);
    sPIT = nullptr;
}

const Core::DeviceManager::DeviceList& Core::DeviceManager::GetDevices() { return sDevices; }

Core::COMPortLogger& Core::DeviceManager::GetCOMPortLogger() { return *sCOMPortLogger; }

Core::Driver::Console& Core::DeviceManager::GetConsole() { return *sConsole; }

Core::Driver::RTC& Core::DeviceManager::GetRTC() { return *sRTC; }

Core::Driver::PIC& Core::DeviceManager::GetPIC() { return *sPIC; }

Core::Driver::PIT& Core::DeviceManager::GetPIT() { return *sPIT; }
