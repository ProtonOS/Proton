using System;

namespace Proton.IO
{
    public sealed class Port
    {
        private readonly ushort mAddress;

        public Port(ushort pAddress) { mAddress = pAddress; }

        public ushort Address { get { return mAddress; } }

        public byte Byte { get { return PortIO.InByte(mAddress); } set { PortIO.OutByte(mAddress, value); } }
        public ushort UShort { get { return PortIO.InUShort(mAddress); } set { PortIO.OutUShort(mAddress, value); } }
        public uint UInt { get { return PortIO.InUInt(mAddress); } set { PortIO.OutUInt(mAddress, value); } }
    }
}
