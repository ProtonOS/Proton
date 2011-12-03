#include <Core/DeviceManager.h>
#include <Core/SystemClock.h>

uint32_t Core::SystemClock::sStartupTime = 0;

bool Core::SystemClock::Startup()
{
    sStartupTime = Core::DeviceManager::GetRTC().GetSecondsSinceEpoch();
    return true;
}

void Core::SystemClock::Shutdown()
{
    sStartupTime = 0;
}

bool Core::SystemClock::IsReady() { return sStartupTime != 0; }

uint32_t Core::SystemClock::GetSecondsSinceEpoch() { return sStartupTime + Core::DeviceManager::GetPIT().GetSecondsElapsed(); }

uint16_t Core::SystemClock::GetMilliseconds() { return Core::DeviceManager::GetPIT().GetMillisecondsElapsed(); }
