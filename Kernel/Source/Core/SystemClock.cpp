#include <Core/DeviceManager.h>
#include <Core/SystemClock.h>

uint32_t Core::SystemClock::sStartupTime = 0;

bool Core::SystemClock::Startup()
{
    uint32_t startTime = Core::DeviceManager::GetRTC().GetSecondsSinceEpoch();
    uint8_t initSecond = Core::DeviceManager::GetRTC().GetSecond();
    while (Core::DeviceManager::GetRTC().GetSecond() == initSecond);
    sStartupTime = startTime + 1;
    return true;
}

void Core::SystemClock::Shutdown()
{
    sStartupTime = 0;
}

bool Core::SystemClock::IsReady() { return sStartupTime != 0; }

uint32_t Core::SystemClock::GetSecondsSinceEpoch() { return sStartupTime + Core::DeviceManager::GetPIT().GetSecondsElapsed(); }

uint16_t Core::SystemClock::GetMilliseconds() { return Core::DeviceManager::GetPIT().GetMillisecondsElapsed(); }
