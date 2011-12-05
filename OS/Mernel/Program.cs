using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Mernel: Hello World!");
            Proton.Debugging.ComPortLogger.Initialize();
            Proton.Debugging.ComPortLogger.WriteLine("Hey Look! Managed code can do PortIO!");
        }
    }
}



namespace Proton
{
    namespace Debugging
    {
        public static class ComPortLogger
        {
            private const ushort PortBase = 0x2F8;
            private const ushort DataPort = PortBase + 0;
            private const ushort InterruptEnablePort = PortBase + 1;
            private const ushort FIFOControlPort = PortBase + 2;
            private const ushort LineControlPort = PortBase + 3;
            private const ushort ModemControlPort = PortBase + 4;
            private const ushort LineStatusPort = PortBase + 5;
            private const ushort ModemStatusPort = PortBase + 6;
            private const ushort ScratchPort = PortBase + 7;

            public static void Initialize()
            {
                /*
                Proton.IO.PortIO.OutByte(InterruptEnablePort, 0x00);
                Proton.IO.PortIO.OutByte(LineControlPort, 0x80);
                Proton.IO.PortIO.OutByte(DataPort, 0x03);
                Proton.IO.PortIO.OutByte(InterruptEnablePort, 0x00);
                Proton.IO.PortIO.OutByte(LineControlPort, 0x03);
                Proton.IO.PortIO.OutByte(FIFOControlPort, 0xC7);
                Proton.IO.PortIO.OutByte(ModemControlPort, 0x0B);
                */
            }

            private static int IsTransmitEmpty()
            {
                return Proton.IO.PortIO.InByte(LineStatusPort) & 0x20;
            }

            /// <summary>
            /// Writes the specified byte to the Log.
            /// </summary>
            /// <param name="b">The byte to write.</param>
            public static void WriteData(byte b)
            {
                // Empty loop, will allow us to timeout
                for (uint i = 0; i < 1000 && (IsTransmitEmpty() == 0); i++) ;
                Proton.IO.PortIO.OutByte(DataPort, b);
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

    namespace IO
    {
        internal static class PortIO
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern byte InByte(ushort port);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern ushort InUShort(ushort port);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern uint InUInt(ushort port);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void OutByte(ushort port, byte value);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void OutUShort(ushort port, ushort value);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void OutUInt(ushort port, uint value);
        }
    }
}