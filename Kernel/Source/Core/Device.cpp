#include <Core/DeviceManager.h>

bool Core::Device::HasMemoryConflict(uint32_t pAddress, uint32_t pLength)
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

bool Core::Device::HasIOPortConflict(uint16_t pIOPort) { return mClaimedIOPorts.find(pIOPort) != mClaimedIOPorts.end(); }

bool Core::Device::HasInterruptConflict(uint8_t pInterrupt) { return mClaimedInterrupts.find(pInterrupt) != mClaimedInterrupts.end(); }

bool Core::Device::IsMemoryAvailable(uint32_t pAddress, uint32_t pLength) { return DeviceManager::IsMemoryAvailable(pAddress, pLength); }

bool Core::Device::IsIOPortAvailable(uint16_t pIOPort) { return DeviceManager::IsIOPortAvailable(pIOPort); }

bool Core::Device::IsInterruptAvailable(uint8_t pInterrupt) { return DeviceManager::IsInterruptAvailable(pInterrupt); }

void Core::Device::ClaimMemory(uint32_t pAddress, uint32_t pLength) { mClaimedMemory[pAddress] = pLength; }

void Core::Device::ClaimIOPort(uint16_t pIOPort) { mClaimedIOPorts.insert(pIOPort); }

void Core::Device::ClaimInterrupt(uint8_t pInterrupt) { mClaimedInterrupts.insert(pInterrupt); }

void Core::Device::ReleaseMemory(uint32_t pAddress) { mClaimedMemory.erase(pAddress); }

void Core::Device::ReleaseIOPort(uint16_t pIOPort) { mClaimedIOPorts.erase(pIOPort); }

void Core::Device::ReleaseInterrupt(uint8_t pInterrupt) { mClaimedInterrupts.erase(pInterrupt); }
