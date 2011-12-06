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
            //Proton.Debugging.ComPortLogger.WriteLine("Hey Look! Managed code can do PortIO!");
        }
    }
}

namespace Proton
{
    namespace Debugging
    {
        public static class ComPortLogger
        {
            private const ushort PortBase = 0x02F8;
            private static Proton.IO.Port sDataPort = new Proton.IO.Port(PortBase + 0);
            private static Proton.IO.Port sInterruptEnablePort = new Proton.IO.Port(PortBase + 1);
            private static Proton.IO.Port sFIFOControlPort = new Proton.IO.Port(PortBase + 2);
            private static Proton.IO.Port sLineControlPort = new Proton.IO.Port(PortBase + 3);
            private static Proton.IO.Port sModemControlPort = new Proton.IO.Port(PortBase + 4);
            private static Proton.IO.Port sLineStatusPort = new Proton.IO.Port(PortBase + 5);
            private static Proton.IO.Port sModemStatusPort = new Proton.IO.Port(PortBase + 6);
            private static Proton.IO.Port sScratchPort = new Proton.IO.Port(PortBase + 7);

            public static void Initialize()
            {
                /*
                sInterruptEnablePort.Byte = 0x00;
                sLineControlPort.Byte = 0x80;
                sDataPort.Byte = 0x03;
                sInterruptEnablePort.Byte = 0x00;
                sLineControlPort.Byte = 0x03;
                sFIFOControlPort.Byte = 0xC7;
                sModemControlPort.Byte = 0x0B;
                */
            }

            private static bool IsTransmitEmpty { get { return (sLineStatusPort.Byte & 0x20) != 0; } }

            /// <summary>
            /// Writes the specified byte to the Log.
            /// </summary>
            /// <param name="b">The byte to write.</param>
            public static void WriteData(byte b)
            {
                // Empty loop, will allow us to timeout
                for (uint i = 0; i < 1000 && !IsTransmitEmpty; i++) ;
                sDataPort.Byte = b;
            }

            /// <summary>
            /// Writes the specified string to the Log, followed by EOL.
            /// </summary>
            /// <param name="s">The string to write.</param>
            public static void WriteLine(string s)
            {
                WriteString(s + "\r\n");
            }

            /// <summary>
            /// Writes the specified string to the Log.
            /// </summary>
            /// <param name="s">The string to write.</param>
            public static void WriteString(string s)
            {
                for (int i = 0; i < s.Length; i++)
                {
                    WriteData((byte)s[i]);
                }
            }
        }
    }
}