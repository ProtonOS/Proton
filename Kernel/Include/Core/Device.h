#pragma once

#include <map>
#include <set>

namespace Core
{
    class Device
    {
    public:
        virtual bool OnRegister() = 0;
        virtual void OnUnregister() = 0;

        bool HasMemoryConflict(uint32_t pAddress, uint32_t pLength);
        bool HasIOPortConflict(uint16_t pIOPort);
        bool HasInterruptConflict(uint8_t pInterrupt);

    protected:
        static bool IsMemoryAvailable(uint32_t pAddress, uint32_t pLength);
        static bool IsIOPortAvailable(uint16_t pIOPort);
        static bool IsInterruptAvailable(uint8_t pInterrupt);

        void ClaimMemory(uint32_t pAddress, uint32_t pLength);
        void ClaimIOPort(uint16_t pIOPort);
        void ClaimInterrupt(uint8_t pInterrupt);
        void ReleaseMemory(uint32_t pAddress);
        void ReleaseIOPort(uint16_t pIOPort);
        void ReleaseInterrupt(uint8_t pInterrupt);

    private:
        typedef std::map<uint32_t, uint32_t> ClaimedMemoryMap;
        typedef std::set<uint16_t> ClaimedIOPortsSet;
        typedef std::set<uint8_t> ClaimedInterruptsSet;

        ClaimedMemoryMap mClaimedMemory;
        ClaimedIOPortsSet mClaimedIOPorts;
        ClaimedInterruptsSet mClaimedInterrupts;
    };
}
