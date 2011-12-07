using System;

namespace Proton.IO
{
    public sealed class Memory
    {
        private readonly uint mAddress;
        private readonly uint mLength;

        public byte this[uint pOffset]
        {
            get
            {
                return GetByte(pOffset);
            }
            set
            {
                SetByte(pOffset, value);
            }
        }

        public Memory(uint pAddress, uint pLength) 
        {
            mAddress = pAddress;
            mLength = pLength; 
        }

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
