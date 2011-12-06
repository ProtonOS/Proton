using System;

namespace Proton.IO
{
    public sealed class Port
    {
        private readonly ushort mPort;

        public Port(ushort pPort) { mPort = pPort; }

        public byte Byte { get { return PortIO.InByte(mPort); } set { PortIO.OutByte(mPort, value); } }
        public ushort UShort { get { return PortIO.InUShort(mPort); } set { PortIO.OutUShort(mPort, value); } }
        public uint UInt { get { return PortIO.InUInt(mPort); } set { PortIO.OutUInt(mPort, value); } }
    }
}
