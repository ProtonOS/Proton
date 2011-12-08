using Proton.IO;
using System;
using System.Collections.Generic;

namespace Proton.Devices
{
    public abstract class Device
    {
        private class ClaimedMemory
        {
            public readonly uint Address;
            public readonly uint Length;

            public ClaimedMemory(uint pAddress, uint pLength)
            {
                Address = pAddress;
                Length = pLength;
            }
        }

        protected static bool IsMemoryAvailable(uint pAddress, uint pLength) { return DeviceManager.IsMemoryAvailable(pAddress, pLength); }

        protected static bool IsPortAvailable(ushort pPort) { return DeviceManager.IsPortAvailable(pPort); }

        private List<ClaimedMemory> mClaimedMemory = new List<ClaimedMemory>();
        private List<ushort> mClaimedPorts = new List<ushort>();

        internal bool HasMemoryConflict(uint pAddress, uint pLength)
        {
            if (mClaimedMemory.Count == 0) return false;

            int conflictIndex = -1;
            for (int index = 0; index < mClaimedMemory.Count; ++index)
            {
                if (mClaimedMemory[index].Address > pAddress)
                {
                    conflictIndex = index;
                    break;
                }
            }
            if (conflictIndex >= 0 && pAddress + pLength >= mClaimedMemory[conflictIndex].Address) return true;
            if (conflictIndex > 0)
            {
                --conflictIndex;
                if (mClaimedMemory[conflictIndex].Address + mClaimedMemory[conflictIndex].Length >= pAddress) return true;
            }
            return false;
        }

        internal bool HasPortConflict(ushort pPort) { return mClaimedPorts.Contains(pPort); }

        protected void ClaimMemory(uint pAddress, uint pLength)
        {
            int insertIndex = -1;
            for (int index = 0; index < mClaimedMemory.Count; ++index)
            {
                if (mClaimedMemory[index].Address > pAddress)
                {
                    insertIndex = index;
                    break;
                }
            }
            if (insertIndex < 0)
            {
                if (mClaimedMemory.Count > 0) insertIndex = mClaimedMemory.Count - 1;
                else insertIndex = 0;
            }
            mClaimedMemory.Insert(insertIndex, new ClaimedMemory(pAddress, pLength));
        }

        protected Port ClaimPort(ushort pPort)
        {
            mClaimedPorts.Add(pPort);
            return new Port(pPort);
        }

        protected void ReleaseAllMemory() { mClaimedMemory.Clear(); }

        protected void ReleaseAllPorts() { mClaimedPorts.Clear(); }

        protected internal abstract bool OnRegister();
        protected internal abstract void OnUnregister();


    }
}
