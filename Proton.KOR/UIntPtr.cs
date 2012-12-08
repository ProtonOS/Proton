namespace System
{
    public unsafe struct UIntPtr
    {
        private void* mValue;

        public UIntPtr(uint value) { mValue = (void*)value; }

        public UIntPtr(ulong value) { mValue = (void*)value; }

        public UIntPtr(void* value) { mValue = value; }

        public static int Size { get { return sizeof(void*); } }

        public uint ToUInt32() { return (uint)mValue; }

        public ulong ToUInt64() { return (ulong)mValue; }

        public void* ToPointer() { return mValue; }

        public override bool Equals(object obj) { return (obj is UIntPtr && ((UIntPtr)obj).mValue == mValue); }

        public override int GetHashCode() { return (int)mValue; }

        public static bool operator ==(UIntPtr a, UIntPtr b) { return a.mValue == b.mValue; }

        public static bool operator !=(UIntPtr a, UIntPtr b) { return a.mValue != b.mValue; }

        public override string ToString()
        {
            if (Size == 4)
            {
                return string.Format("0x{0:x4}", (int)mValue);
            }
            return string.Format("0x{0:x8}", (long)mValue);
        }
    }
}
