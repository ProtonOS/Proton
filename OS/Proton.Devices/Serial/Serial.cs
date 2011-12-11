using Proton.IO;
using System;

namespace Proton.Devices
{
    public abstract class Serial : Device
    {
        private readonly ushort mBasePort;
        private readonly byte mIRQ;
        private Port mDataPort = null;
        private Port mInterruptEnablePort = null;
        private Port mFIFOControlPort = null;
        private Port mLineControlPort = null;
        private Port mModemControlPort = null;
        private Port mLineStatusPort = null;
        private Port mModemStatusPort = null;
        private Port mScratchPort = null;

        protected Serial(ushort pBasePort, byte pIRQ)
        {
            mBasePort = pBasePort;
            mIRQ = pIRQ;
        }

        public ushort BasePort { get { return mBasePort; } }
        public byte IRQ { get { return mIRQ; } }

        public Port DataPort { get { return mDataPort; } }
        public Port InterruptEnablePort { get { return mInterruptEnablePort; } }
        public Port FIFOControlPort { get { return mFIFOControlPort; } }
        public Port LineControlPort { get { return mLineControlPort; } }
        public Port ModemControlPort { get { return mModemControlPort; } }
        public Port LineStatusPort { get { return mLineStatusPort; } }
        public Port ModemStatusPort { get { return mModemStatusPort; } }
        public Port ScratchPort { get { return mScratchPort; } }

        protected internal override bool OnRegister()
        {
            if (!IsPortAvailable((ushort)(mBasePort + 0)) ||
                !IsPortAvailable((ushort)(mBasePort + 1)) ||
                !IsPortAvailable((ushort)(mBasePort + 2)) ||
                !IsPortAvailable((ushort)(mBasePort + 3)) ||
                !IsPortAvailable((ushort)(mBasePort + 4)) ||
                !IsPortAvailable((ushort)(mBasePort + 5)) ||
                !IsPortAvailable((ushort)(mBasePort + 6)) ||
                !IsPortAvailable((ushort)(mBasePort + 7))) return false;

            mDataPort = ClaimPort((ushort)(mBasePort + 0));
            mInterruptEnablePort = ClaimPort((ushort)(mBasePort + 1));
            mFIFOControlPort = ClaimPort((ushort)(mBasePort + 2));
            mLineControlPort = ClaimPort((ushort)(mBasePort + 3));
            mModemControlPort = ClaimPort((ushort)(mBasePort + 4));
            mLineStatusPort = ClaimPort((ushort)(mBasePort + 5));
            mModemStatusPort = ClaimPort((ushort)(mBasePort + 6));
            mScratchPort = ClaimPort((ushort)(mBasePort + 7));

            return true;
        }

        protected internal override void OnUnregister()
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

        public abstract void WriteByte(byte pByte);
    }
}
