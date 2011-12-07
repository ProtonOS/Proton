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

            int index = mClaimedMemory.FindIndex(m => m.Address > pAddress);
            if (index >= 0 && pAddress + pLength >= mClaimedMemory[index].Address) return true;
            if (index > 0)
            {
                --index;
                if (mClaimedMemory[index].Address + mClaimedMemory[index].Length >= pAddress) return true;
            }
            return false;
        }

        internal bool HasPortConflict(ushort pPort) { return mClaimedPorts.Contains(pPort); }

        protected void ClaimMemory(uint pAddress, uint pLength)
        {
            int index = mClaimedMemory.FindIndex(m => m.Address > pAddress);
            if (index < 0)
            {
                if (mClaimedMemory.Count > 0) index = mClaimedMemory.Count - 1;
                else index = 0;
            }
            mClaimedMemory.Insert(index, new ClaimedMemory(pAddress, pLength));
        }

        protected Proton.IO.Port ClaimPort(ushort pPort)
        {
            mClaimedPorts.Add(pPort);
            return new Proton.IO.Port(pPort);
        }

        protected void ReleaseAllMemory() { mClaimedMemory.Clear(); }

        protected void ReleaseAllPorts() { mClaimedPorts.Clear(); }

        protected internal abstract bool OnRegister();
        protected internal abstract void OnUnregister();


    }
}
