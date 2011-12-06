using System;

namespace Proton.Hardware
{
    public abstract class SerialPort : Device
    {
        private readonly ushort mBasePort;
        private readonly byte mIRQ;

        protected SerialPort(ushort pBasePort, byte pIRQ)
        {
            mBasePort = pBasePort;
            mIRQ = pIRQ;
        }

        public ushort BasePort { get { return mBasePort; } }
        public byte IRQ { get { return mIRQ; } }

        public abstract Proton.IO.Port DataPort { get; }
        public abstract Proton.IO.Port InterruptEnablePort { get; }
        public abstract Proton.IO.Port FIFOControlPort { get; }
        public abstract Proton.IO.Port LineControlPort { get; }
        public abstract Proton.IO.Port ModemControlPort { get; }
        public abstract Proton.IO.Port LineStatusPort { get; }
        public abstract Proton.IO.Port ModemStatusPort { get; }
        public abstract Proton.IO.Port ScratchPort { get; }

        public abstract void WriteByte(byte pByte);

//        public void WriteString(string pString) { for (int index = 0; index < pString.Length; ++index) WriteByte((byte)pString[index]); }

//        public void WriteLine(string pLine) { WriteString(pLine + "\r\n"); }
    }
}
