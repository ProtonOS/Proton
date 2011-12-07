using Proton.Hardware;
using Proton.IO;
using System;

namespace Proton.Drivers
{
    public sealed class Serial8250 : Serial
    {
        private Port mDataPort = null;
        private Port mInterruptEnablePort = null;
        private Port mFIFOControlPort = null;
        private Port mLineControlPort = null;
        private Port mModemControlPort = null;
        private Port mLineStatusPort = null;
        private Port mModemStatusPort = null;
        private Port mScratchPort = null;

        public Serial8250(ushort pBasePort, byte pIRQ) : base(pBasePort, pIRQ) { }

        public bool IsTransmitEmpty { get { return (mLineStatusPort.Byte & 0x20) != 0; } }

        public override Port DataPort { get { return mDataPort; } }
        public override Port InterruptEnablePort { get { return mInterruptEnablePort; } }
        public override Port FIFOControlPort { get { return mFIFOControlPort; } }
        public override Port LineControlPort { get { return mLineControlPort; } }
        public override Port ModemControlPort { get { return mModemControlPort; } }
        public override Port LineStatusPort { get { return mLineStatusPort; } }
        public override Port ModemStatusPort { get { return mModemStatusPort; } }
        public override Port ScratchPort { get { return mScratchPort; } }

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
