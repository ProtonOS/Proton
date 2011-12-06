using System;

namespace Proton.Drivers.Serial
{
    public sealed class SerialPort8250 : Proton.Hardware.SerialPort
    {
        private Proton.IO.Port mDataPort = null;
        private Proton.IO.Port mInterruptEnablePort = null;
        private Proton.IO.Port mFIFOControlPort = null;
        private Proton.IO.Port mLineControlPort = null;
        private Proton.IO.Port mModemControlPort = null;
        private Proton.IO.Port mLineStatusPort = null;
        private Proton.IO.Port mModemStatusPort = null;
        private Proton.IO.Port mScratchPort = null;

        public SerialPort8250(ushort pBasePort, byte pIRQ) : base(pBasePort, pIRQ) { }

        public bool IsTransmitEmpty { get { return (mLineStatusPort.Byte & 0x20) != 0; } }

        public override Proton.IO.Port DataPort { get { return mDataPort; } }
        public override Proton.IO.Port InterruptEnablePort { get { return mInterruptEnablePort; } }
        public override Proton.IO.Port FIFOControlPort { get { return mFIFOControlPort; } }
        public override Proton.IO.Port LineControlPort { get { return mLineControlPort; } }
        public override Proton.IO.Port ModemControlPort { get { return mModemControlPort; } }
        public override Proton.IO.Port LineStatusPort { get { return mLineStatusPort; } }
        public override Proton.IO.Port ModemStatusPort { get { return mModemStatusPort; } }
        public override Proton.IO.Port ScratchPort { get { return mScratchPort; } }

        public override void WriteByte(byte pByte)
        {
            for (int retries = 1000; retries > 0 && !IsTransmitEmpty; --retries) ;
            mDataPort.Byte = pByte;
        }

        protected override bool OnRegister()
        {
            if (!IsPortAvailable((ushort)(BasePort + 0)) ||
                !IsPortAvailable((ushort)(BasePort + 1)) ||
                !IsPortAvailable((ushort)(BasePort + 2)) ||
                !IsPortAvailable((ushort)(BasePort + 3)) ||
                !IsPortAvailable((ushort)(BasePort + 4)) ||
                !IsPortAvailable((ushort)(BasePort + 5)) ||
                !IsPortAvailable((ushort)(BasePort + 6)) ||
                !IsPortAvailable((ushort)(BasePort + 7))) return false;

            mDataPort = ClaimPort((ushort)(BasePort + 0));
            mInterruptEnablePort = ClaimPort((ushort)(BasePort + 1));
            mFIFOControlPort = ClaimPort((ushort)(BasePort + 2));
            mLineControlPort = ClaimPort((ushort)(BasePort + 3));
            mModemControlPort = ClaimPort((ushort)(BasePort + 4));
            mLineStatusPort = ClaimPort((ushort)(BasePort + 5));
            mModemStatusPort = ClaimPort((ushort)(BasePort + 6));
            mScratchPort = ClaimPort((ushort)(BasePort + 7));

            return true;
        }

        protected override void OnUnregister()
        {
            mDataPort = null;
            mInterruptEnablePort = null;
            mFIFOControlPort = null;
            mLineControlPort = null;
            mModemControlPort = null;
            mLineStatusPort = null;
            mModemStatusPort = null;
            mScratchPort = null;

            ReleaseAllPorts();
        }
    }
}
