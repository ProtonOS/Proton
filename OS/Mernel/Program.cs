using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Program
    {
        private static void Main(string[] pArgs)
        {
            Console.WriteLine("Mernel: Hello World!");

            Proton.Hardware.SerialPort serialPort2 = new Proton.Drivers.Serial.SerialPort8250(0x02F8, 0x03);
            if (!Proton.Hardware.DeviceManager.Register(serialPort2)) return;
            Proton.Diagnostics.SerialPortWriter.SetSerialPort(serialPort2);

            Proton.Diagnostics.SerialPortWriter.WriteLine("SerialPortWriter Enabled");
        }
    }
}
