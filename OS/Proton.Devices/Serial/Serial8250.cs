using Proton.IO;
using System;

namespace Proton.Devices
{
    public sealed class Serial8250 : Serial
    {
        public Serial8250(ushort pBasePort, byte pIRQ) : base(pBasePort, pIRQ) { }

        public bool IsTransmitEmpty { get { return (LineStatusPort.Byte & 0x20) != 0; } }

        public override void WriteByte(byte pByte)
        {
            for (int retries = 1000; retries > 0 && !IsTransmitEmpty; --retries) ;
            DataPort.Byte = pByte;
        }
    }
}
