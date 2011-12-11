using Proton.IO;
using System;

namespace Proton.Devices
{
    public sealed class ATABus : Device
    {
        private const byte CONTROL_NONE = 0x00;
        private const byte CONTROL_SOFTRESET = 0x04;

        private const byte SELECT_MASTER_IDENTIFY = 0xA0;
        private const byte SELECT_SLAVE_IDENTIFY = 0xB0;

        private const ushort SIGNATURE_PATA = 0x0000;
        private const ushort SIGNATURE_PATAPI = 0xEB14;
        private const ushort SIGNATURE_SATA = 0xC33C;
        private const ushort SIGNATURE_SATAPI = 0x9669;

        private readonly ushort mBasePort;
        private readonly byte mIRQ;
        private Port mDataPort = null;
        private Port mFeaturesPort = null;
        private Port mSectorsPort = null;
        private Port mAddress0Port = null;
        private Port mAddress1Port = null;
        private Port mAddress2Port = null;
        private Port mSelectPort = null;
        private Port mCommandPort = null;
        private Port mControlPort = null;
        private ATADrive mMasterDrive = null;
        private ATADrive mSlaveDrive = null;

        public ATABus(ushort pBasePort, byte pIRQ)
        {
            mBasePort = pBasePort;
            mIRQ = pIRQ;
        }

        public ushort BasePort { get { return mBasePort; } }
        public byte IRQ { get { return mIRQ; } }

        internal Port DataPort { get { return mDataPort; } }
        internal Port FeaturesPort { get { return mFeaturesPort; } }
        internal Port SectorsPort { get { return mSectorsPort; } }
        internal Port Address0Port { get { return mAddress0Port; } }
        internal Port Address1Port { get { return mAddress1Port; } }
        internal Port Address2Port { get { return mAddress2Port; } }
        internal Port SelectPort { get { return mSelectPort; } }
        internal Port CommandPort { get { return mCommandPort; } }
        internal Port ControlPort { get { return mControlPort; } }
        public ATADrive MasterDrive { get { return mMasterDrive; } }
        public ATADrive SlaveDrive { get { return mSlaveDrive; } }

        protected internal override bool OnRegister()
        {
            if (!IsPortAvailable((ushort)(mBasePort + 0)) ||
                !IsPortAvailable((ushort)(mBasePort + 1)) ||
                !IsPortAvailable((ushort)(mBasePort + 2)) ||
                !IsPortAvailable((ushort)(mBasePort + 3)) ||
                !IsPortAvailable((ushort)(mBasePort + 4)) ||
                !IsPortAvailable((ushort)(mBasePort + 5)) ||
                !IsPortAvailable((ushort)(mBasePort + 6)) ||
                !IsPortAvailable((ushort)(mBasePort + 7)) ||
                !IsPortAvailable((ushort)(mBasePort + 0x0206))) return false;

            mDataPort = ClaimPort((ushort)(mBasePort + 0));
            mFeaturesPort = ClaimPort((ushort)(mBasePort + 1));
            mSectorsPort = ClaimPort((ushort)(mBasePort + 2));
            mAddress0Port = ClaimPort((ushort)(mBasePort + 3));
            mAddress1Port = ClaimPort((ushort)(mBasePort + 4));
            mAddress2Port = ClaimPort((ushort)(mBasePort + 5));
            mSelectPort = ClaimPort((ushort)(mBasePort + 6));
            mCommandPort = ClaimPort((ushort)(mBasePort + 7));
            mControlPort = ClaimPort((ushort)(mBasePort + 0x0206));

            DetectDrives();

            return true;
        }

        protected internal override void OnUnregister()
        {
            mDataPort = null;
            mFeaturesPort = null;
            mSectorsPort = null;
            mAddress0Port = null;
            mAddress1Port = null;
            mAddress2Port = null;
            mSelectPort = null;
            mCommandPort = null;
            mControlPort = null;

            ReleaseAllPorts();
        }

        private void DetectDrives()
        {
            mSectorsPort.Byte = 0xDE;
            mAddress0Port.Byte = 0xAD;
            if (mSectorsPort.Byte == 0xDE && mAddress0Port.Byte == 0xAD)
            {
                mControlPort.Byte = CONTROL_SOFTRESET;
                for (byte temp = mControlPort.Byte, count = 4; count > 0; temp = mControlPort.Byte, --count) ;
                mControlPort.Byte = CONTROL_NONE;
                for (byte temp = mControlPort.Byte, count = 4; count > 0; temp = mControlPort.Byte, --count) ;

                mSelectPort.Byte = SELECT_MASTER_IDENTIFY;
                for (byte temp = mControlPort.Byte, count = 4; count > 0; temp = mControlPort.Byte, --count) ;

                ushort signature = (ushort)(mAddress1Port.Byte | (mAddress2Port.Byte << 8));
                switch (signature)
                {
                    case SIGNATURE_PATA:
                        break;
                    case SIGNATURE_PATAPI:
                        break;
                    case SIGNATURE_SATA:
                        break;
                    case SIGNATURE_SATAPI:
                        break;
                    default: break;
                }
            }
        }
    }
}
