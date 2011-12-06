using System;
using System.Collections.Generic;

namespace Proton.Hardware
{
    public static class DeviceManager
    {
        private static List<Device> sDevices = new List<Device>();

        public static bool IsMemoryAvailable(uint pAddress, uint pLength)
        {
            foreach (Device device in sDevices) if (device.HasMemoryConflict(pAddress, pLength)) return false;
            return true;
        }

        public static bool IsPortAvailable(ushort pPort)
        {
            foreach (Device device in sDevices) if (device.HasPortConflict(pPort)) return false;
            return true;
        }

        public static bool Register(Device pDevice)
        {
            if (!pDevice.OnRegister()) return false;
            sDevices.Add(pDevice);
            return true;
        }

        public static void Unregister(Device pDevice) { if (sDevices.Remove(pDevice)) pDevice.OnUnregister(); }
    }
}
