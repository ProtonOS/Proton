using System;
using System.Runtime.CompilerServices;

namespace Mernel
{
    internal static class Program
    {
        private static void Main(string[] pArgs)
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

        internal static class MemoryIO
        {
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern byte InByte(uint address);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern ushort InUShort(uint address);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern uint InUInt(uint address);

            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void OutByte(uint address, byte value);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void OutUShort(uint address, ushort value);
            [MethodImpl(MethodImplOptions.InternalCall)]
            public static extern void OutUInt(uint address, uint value);
        }

        public sealed class Port
        {
            private readonly ushort mPort;

            public Port(ushort pPort) { mPort = pPort; }

            public byte Byte { get { return PortIO.InByte(mPort); } set { PortIO.OutByte(mPort, value); } }
            public ushort UShort { get { return PortIO.InUShort(mPort); } set { PortIO.OutUShort(mPort, value); } }
            public uint UInt { get { return PortIO.InUInt(mPort); } set { PortIO.OutUInt(mPort, value); } }
        }

        public sealed class Memory
        {
            private readonly uint mAddress;
            private readonly uint mLength;

            public Memory(uint pAddress, uint pLength) { mAddress = pAddress; mLength = pLength; }

            public byte GetByte(uint pOffset)
            {
                if (pOffset >= mLength) return 0x00;
                return MemoryIO.InByte(mAddress + pOffset);
            }
            public void SetByte(uint pOffset, byte pValue)
            {
                if (pOffset >= mLength) return;
                MemoryIO.OutByte(mAddress + pOffset, pValue);
            }
            public ushort GetUShort(uint pOffset)
            {
                if (pOffset >= mLength - 1) return 0x00;
                return MemoryIO.InUShort(mAddress + pOffset);
            }
            public void SetUShort(uint pOffset, ushort pValue)
            {
                if (pOffset >= mLength - 1) return;
                MemoryIO.OutUShort(mAddress + pOffset, pValue);
            }
            public uint GetUInt(uint pOffset)
            {
                if (pOffset >= mLength - 3) return 0x00;
                return MemoryIO.InUInt(mAddress + pOffset);
            }
            public void SetUInt(uint pOffset, uint pValue)
            {
                if (pOffset >= mLength - 3) return;
                MemoryIO.OutUInt(mAddress + pOffset, pValue);
            }
        }
    }

    namespace Hardware
    {
        public static class DeviceManager
        {
        }

        public abstract class Device
        {
        }
    }

    namespace Drivers
    {
    }
}