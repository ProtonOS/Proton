using Proton.IO;
using System;

namespace Proton.Hardware
{
    public abstract class Serial : Device
    {
        private readonly ushort mBasePort;
        private readonly byte mIRQ;

        protected Serial(ushort pBasePort, byte pIRQ)
        {
            mBasePort = pBasePort;
            mIRQ = pIRQ;
        }

        public ushort BasePort { get { return mBasePort; } }
        public byte IRQ { get { return mIRQ; } }

        public abstract Port DataPort { get; }
        public abstract Port InterruptEnablePort { get; }
        public abstract Port FIFOControlPort { get; }
        public abstract Port LineControlPort { get; }
        public abstract Port ModemControlPort { get; }
        public abstract Port LineStatusPort { get; }
        public abstract Port ModemStatusPort { get; }
        public abstract Port ScratchPort { get; }

        public abstract void WriteByte(byte pByte);
    }
}
