using Proton.Diagnostics;
using Proton.Drivers;
using Proton.Hardware;
using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Program
    {
        private static void Main(string[] pArgs)
        {
            Console.WriteLine("Mernel: Hello World!");

            Serial serial2 = new Serial8250(0x02F8, 0x03);
            if (!DeviceManager.Register(serial2)) return;
            SerialWriter.SetSerial(serial2);

            SerialWriter.WriteLine("SerialWriter Enabled");
        }
    }
}
