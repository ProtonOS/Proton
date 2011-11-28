#include <Core/Console.h>
#include <Core/Device.h>
#include <Core/DeviceManager.h>

using namespace Core;

bool Device::HasMemoryConflict(uint32_t pAddress, uint32_t pLength)
{
    ClaimedMemoryMap::iterator it = mClaimedMemory.lower_bound(pAddress);
    if (it != mClaimedMemory.end() && pAddress + pLength >= it->first) return true;
    if (it != mClaimedMemory.begin())
    {
        --it;
        if (it->first + it->second >= pAddress) return true;
    }
    return false;
}

bool Device::HasIOPortConflict(uint16_t pIOPort) { return mClaimedIOPorts.find(pIOPort) != mClaimedIOPorts.end(); }

bool Device::IsMemoryAvailable(uint32_t pAddress, uint32_t pLength) { return DeviceManager::IsMemoryAvailable(pAddress, pLength); }

bool Device::IsIOPortAvailable(uint16_t pIOPort) { return DeviceManager::IsIOPortAvailable(pIOPort); }

void Device::ClaimMemory(uint32_t pAddress, uint32_t pLength) { mClaimedMemory[pAddress] = pLength; }

void Device::ClaimIOPort(uint16_t pIOPort) { mClaimedIOPorts.insert(pIOPort); }

void Device::ReleaseMemory(uint32_t pAddress) { mClaimedMemory.erase(pAddress); }

void Device::ReleaseIOPort(uint16_t pIOPort) { mClaimedIOPorts.erase(pIOPort); }

