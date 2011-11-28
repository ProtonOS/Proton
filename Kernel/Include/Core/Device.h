#pragma once

#include <map>
#include <set>

namespace Core
{
    class Device
    {
    public:
        virtual bool Initialize() = 0;
        virtual void Cleanup() = 0;

        bool HasMemoryConflict(uint32_t pAddress, uint32_t pLength);
        bool HasIOPortConflict(uint16_t pIOPort);

    protected:
        static bool IsMemoryAvailable(uint32_t pAddress, uint32_t pLength);
        static bool IsIOPortAvailable(uint16_t pIOPort);

        void ClaimMemory(uint32_t pAddress, uint32_t pLength);
        void ClaimIOPort(uint16_t pIOPort);
        void ReleaseMemory(uint32_t pAddress);
        void ReleaseIOPort(uint16_t pIOPort);

    private:

        typedef std::map<uint32_t, uint32_t> ClaimedMemoryMap;
        typedef std::set<uint16_t> ClaimedIOPortsSet;

        ClaimedMemoryMap mClaimedMemory;
        ClaimedIOPortsSet mClaimedIOPorts;
    };
}
